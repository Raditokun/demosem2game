#pragma once
#include <vector>
#include <set>
#include "Card.h"

class AssetManager;
struct InputState;


class DeckManager {
public:
    //Pool Draft
    std::vector<Card> pool;
    std::vector<int>  draftPicks;       // indeks ke pool
    bool draftComplete;

    //(PLAYING)
    std::vector<Card> hand;
    int selectedHandIndex;

   
    int   maxHandSize;                  
    static constexpr int STARTING_HAND_CAP = 3;
    static constexpr int MAX_HAND_CAP      = 5;

    //dependensi tier
    // towerType * 10 + baseTier
    std::set<int> ownedTiers;

    
    DeckManager();
    void InitPool();

    //  Drafting (hanya T1)
    void UpdateDrafting(const InputState& in);
    void DrawDrafting(AssetManager* assets = nullptr) const;
    bool IsDraftReady() const;
    void FinalizeDraft();
    bool IsTierAllowedInDraft(int tier) const;  // return true hanya untuk T1

    // Playing 
    void UpdatePlaying(const InputState& in);
    void DrawPlaying(AssetManager* assets = nullptr) const;
    Card* GetSelectedCard();
    void DeselectAll();
    bool HasSelection() const;

    // shop
    bool CanAddCard() const;                    // hand.size() < maxHandSize
    void AddCardToHand(const CardDef& def);     // push ke tangan + daftarkan kepemilikan
    void UpgradeCapacity();                     // maxHandSize++, dibatasi di MAX_HAND_CAP
    bool CanUpgradeCapacity() const;            // maxHandSize < MAX_HAND_CAP
    int  SellCard(int slotIndex);               // hapus kartu, return harga jual (T1=$10, T2=$40, T3=$90)

    
    void RegisterOwnership(TowerType type, int tier);
    bool OwnsType(TowerType type, int tier) const;

private:
    static int EncodeOwnership(TowerType type, int tier);
};
