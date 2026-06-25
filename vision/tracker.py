import cv2
import mediapipe as mp
import socket
import math

mp_hands_mod = mp.solutions.hands
mp_draw_mod  = mp.solutions.drawing_utils


EMA_ALPHA       = 0.5 #smotthing
CLICK_THRESHOLD = 0.05
UDP_IP          = "127.0.0.1"
UDP_PORT        = 5005
DEBUG_INTERVAL  = 10 #log only


FINGER_TIPS = {"thumb": 4, "index": 8, "middle": 12, "ring": 16, "pinky": 20}
FINGER_PIPS = {            "index": 6, "middle": 10, "ring": 14, "pinky": 18}


def count_extended_fingers(lm, actual_hand_label):
    extended = set()

    # cek apakah jari ke arah mana
    for name in ("index", "middle", "ring", "pinky"):
        if lm[FINGER_TIPS[name]].y < lm[FINGER_PIPS[name]].y:
            extended.add(name)

    # Jempol
    thumb_tip_x = lm[4].x
    thumb_ip_x  = lm[3].x
    if actual_hand_label == "Right":
        
        if thumb_tip_x < thumb_ip_x:
            extended.add("thumb")
    else: 
        if thumb_tip_x > thumb_ip_x:
            extended.add("thumb")

    return extended, len(extended)



cap  = cv2.VideoCapture(0)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

hands = mp_hands_mod.Hands(
    max_num_hands=2,
    min_detection_confidence=0.7,
    min_tracking_confidence=0.5,
)

smoothed_x = 0.5
smoothed_y = 0.5
frame_count = 0



while cap.isOpened():
    ok, frame = cap.read()
    if not ok:
        break

    frame  = cv2.flip(frame, 1) 
    rgb    = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    result = hands.process(rgb)

    # Nilai default tiap frame
    is_clicking = 0
    start_wave  = 0
    upgrade     = 0
    sell        = 0
    ult         = 0
    right_hand_present = 0   # 1 kalau tangan kanan terlihat

    if result.multi_hand_landmarks and result.multi_handedness:
        for hand_lms, handedness in zip(result.multi_hand_landmarks,
                                        result.multi_handedness):
            mp_label = handedness.classification[0].label  # "Left" atau "Right"
            actual_label = "Right" if mp_label == "Left" else "Left"

            lm = hand_lms.landmark
            extended, n_ext = count_extended_fingers(lm, actual_label)

            if actual_label == "Right":
                # Tangan kiri
                right_hand_present = 1
                # Kursor + klik 
                tx, ty = lm[4].x, lm[4].y  # ujung jempol
                ix, iy = lm[8].x, lm[8].y  # ujung telunjuk
                dist = math.sqrt((ix - tx) ** 2 + (iy - ty) ** 2)
                is_clicking = 1 if dist < CLICK_THRESHOLD else 0

                #EMA
                smoothed_x = EMA_ALPHA * ix + (1.0 - EMA_ALPHA) * smoothed_x
                smoothed_y = EMA_ALPHA * iy + (1.0 - EMA_ALPHA) * smoothed_y

                
                # Peace sign: telunjuk + tengah saja.
                if n_ext == 2 and extended == {"index", "middle"}:
                    start_wave = 1
                # Kepalan: tidak ada jari terbuka.
                if n_ext == 0:
                    ult = 1

            else:  # Tangan kanan
                
                # telunjuk saja.
                if n_ext == 1 and "index" in extended:
                    upgrade = 1
                # Telapak terbuka: kelima jari.
                if n_ext == 5:
                    sell = 1

            mp_draw_mod.draw_landmarks(frame, hand_lms, mp_hands_mod.HAND_CONNECTIONS)

    # Paket 8 field: X,Y + 5 aksi + hand-present
    packet = (f"{smoothed_x:.4f},{smoothed_y:.4f},"
              f"{is_clicking},{start_wave},{upgrade},{sell},{ult},{right_hand_present}")
    sock.sendto(packet.encode(), (UDP_IP, UDP_PORT))

    frame_count += 1
    if frame_count % DEBUG_INTERVAL == 0:
        print(f"[UDP] {packet}")

    cv2.imshow("Hand Tracker -- press q to quit", frame)
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

cap.release()
cv2.destroyAllWindows()
sock.close()
print("[tracker] Exited cleanly.")
