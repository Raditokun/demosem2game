#pragma once
#include <raylib.h>



class AssetManager; // deklarasi maju

class Hero {
public:
    
    Vector2 position;       // posisi dunia (di-set ke titik jalur terakhir)
    float   radius;         // radius collision / visual
    int   maxHP;
    int   currentHP;

    
    int   currentUltCharge; // terisi dari kill musuh
    int   maxUltCharge;     // 100 = terisi penuh, siap ditembakkan
    float ultDuration;      // berapa lama visual laser tampil di layar
    float ultActiveTimer;   // menghitung MUNDUR selama visual laser tampil
    float ultDamage;        // damage sekali tembak saat fire
    float ultRadius;        // radius area-of-effect (tak dipakai kini, disimpan untuk kompatibilitas)
    bool  isUltFiring;      // true pada SATU frame saat damage diterapkan

    
    float pulseTimer;       // timer animasi internal (legacy, dipakai untuk pulse UI)
    float animTimer;        // timer frame sprite sheet
    int   currentFrame;     // frame animasi idle saat ini (0-4)
    static constexpr float FRAME_TIME = 0.15f;
    static constexpr int   IDLE_FRAMES = 5;
    static constexpr int   SPRITE_SIZE = 64;

    
    float ultAnimTimer;     // timer frame sprite ult
    int   currentUltFrame;  // frame animasi ult saat ini (mulai di 9, loop 2→0)
    static constexpr float ULT_FRAME_TIME = 0.05f;
    static constexpr int   ULT_TOTAL_FRAMES = 10;

    
    Hero();
    void Init(Vector2 basePos);
    void Update(float dt, bool isWaveActive);
    void Draw(AssetManager* assets = nullptr) const;

    
    bool  IsUltReady() const;
    bool  IsUltActive() const;      // true selama visual laser tampil
    void  FireUltimate();           // pemicu sekali: set isUltFiring, kuras charge
    void  AddUltCharge(int amount); // dipanggil saat kill musuh
    float GetUltChargePercent() const; // 0.0 = kosong, 1.0 = penuh

    
    void TakeDamage(int amount);
    bool IsAlive() const;
};
