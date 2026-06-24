# UML Class Diagram — demosem2game

Tower-defense game (C++ / raylib) dengan kontrol *hand-tracking* (Python MediaPipe → UDP).

```mermaid
classDiagram
    direction LR

    class Game {
        +GameState state
        +int currency
        +int& playerHealth
        +GridNode grid[ROWS][COLS]
        +vector~Enemy~ enemies
        +vector~Projectile~ projectiles
        +vector~Vector2~ pathPoints
        +Camera2D camera
        +Music bgMusic
        +Init()
        +Update(dt, InputState)
        +Draw() const
        +Shutdown()
        -HandleInput(in)
        -UpdateProjectiles(dt)
        -CheckEnemyReachedBase()
        -UpdateShop(in)
        -UpdateDrafting(in)
    }

    class GridNode {
        +int row, col
        +Vector2 worldPos
        +bool isPathTile
        +vector~Tower~ towerStack
        +CanPlaceTower(tier) bool
        +PlaceTower(Tower)
        +SellTopTower() int
        +UpdateAll(dt, enemies, proj)
        +CalculateSynergy()
        +UpgradeTopTower(currency) bool
    }

    class Tower {
        +TowerType type
        +int baseTier
        +int fieldLevel
        +float synergyMultiplier
        +TowerStats baseStats
        +TowerStats effectiveStats
        +Vector2 position
        +Update(dt, enemies, proj)
        +Draw(yOffset) const
        +Upgrade()
        +CanUpgrade() bool
        -FindTarget(enemies) Enemy*
        -Shoot(target, proj)
    }

    class Enemy {
        +EnemyType type
        +Vector2 position
        +float hp, maxHp, speed, radius
        +int reward, pathIndex
        +bool alive
        +float slowTimer, slowFactor
        +Update(dt, path)
        +Draw(assets) const
        +TakeDamage(dmg)
        +ApplySlow(factor, dur)
        +ReachedEnd(path) bool
    }

    class Projectile {
        +Vector2 position, direction
        +float damage, speed, lifetime
        +bool active, isFreeze
        +TowerType sourceType
        +Color color
        +Update(dt)
        +Draw(assets) const
        +CheckCollision(Enemy&) bool
    }

    class Hero {
        +Vector2 position
        +int maxHP, currentHP
        +int currentUltCharge, maxUltCharge
        +float ultDamage, ultDuration
        +bool isUltFiring
        +Init(basePos)
        +Update(dt, isWaveActive)
        +FireUltimate()
        +AddUltCharge(amount)
        +IsUltReady() bool
        +TakeDamage(amount)
        +IsAlive() bool
    }

    class DeckManager {
        +vector~Card~ pool
        +vector~int~ draftPicks
        +vector~Card~ hand
        +int selectedHandIndex
        +int maxHandSize
        +set~int~ ownedTiers
        +UpdateDrafting(in)
        +FinalizeDraft()
        +UpdatePlaying(in)
        +AddCardToHand(CardDef)
        +SellCard(slot) int
        +UpgradeCapacity()
        +OwnsType(type, tier) bool
    }

    class Card {
        +CardDef def
        +bool selected
        +bool draftSelected
        +DrawInHand(rect, assets) const
        +DrawInDraft(rect, assets) const
        +GetPlacementCost() int
    }

    class WaveManager {
        +int currentWave
        +float spawnTimer
        +bool waveActive
        +vector~WaveDef~ waves
        +Init()
        +Update(dt, enemies, path)
        +StartNextWave()
        +AllWavesComplete() bool
        +IsWaveCleared(enemies) bool
    }

    class ShopManager {
        +vector~ShopItem~ stock
        +bool capacityUpgradeSold
        +bool isOpen
        +GenerateStock()
        +UpdateShop(currency, deck, in) bool
        +DrawShop(currency, deck, assets) const
        +MeetsPrerequisite(item, deck)$ bool
        +ShouldOpenShop(waveIdx)$ bool
    }

    class AssetManager {
        -map~string,Texture2D~ textures
        -map~string,Sound~ sounds$
        +Load(key, filepath)
        +Get(key) Texture2D*
        +Has(key) bool
        +GetSound(key)$ Sound
        +GetTextureStatic(key)$ Texture2D*
    }

    class UDPReceiver {
        -unsigned long long sock_
        -float x_, y_
        -bool handPresent_, clickDown_
        +Poll() bool
        +NormalizedX() float
        +ClickPressed() bool
        +StartWavePressed() bool
        +UltPressed() bool
        +IsAlive() bool
    }

    class InputState {
        <<struct>>
        +Vector2 cursor
        +bool clickPressed
        +bool clickReleased
        +bool rightClickPressed
        +bool udpAlive
        +bool startWavePressed
        +bool upgradePressed
        +bool sellPressed
        +bool ultPressed
    }

    class CardDef {
        <<struct>>
        +int cardId
        +TowerType towerType
        +int baseTier
    }

    %% ---- Komposisi (Game memiliki semua subsistem) ----
    Game *-- DeckManager
    Game *-- WaveManager
    Game *-- ShopManager
    Game *-- Hero
    Game *-- AssetManager
    Game *-- "ROWS*COLS" GridNode
    Game o-- "*" Enemy
    Game o-- "*" Projectile

    GridNode *-- "*" Tower
    DeckManager *-- "*" Card
    Card *-- CardDef
    ShopManager ..> DeckManager : reads ownership
    WaveManager ..> Enemy : spawns
    Tower ..> Enemy : targets
    Tower ..> Projectile : creates
    Projectile ..> Enemy : collides / damages
    Tower ..> AssetManager : sprites
    Enemy ..> AssetManager : sprites

    %% ---- Input pipeline ----
    UDPReceiver ..> InputState : fills
    InputState ..> Game : drives Update()
```

## Catatan relasi

| Relasi | Arti |
|--------|------|
| `*--` (komposisi) | `Game` memiliki & mengelola lifecycle subsistem; `GridNode` memiliki `towerStack` |
| `o--` (agregasi) | koleksi `enemies` / `projectiles` yang dinamis |
| `..>` (dependensi) | satu kelas memakai/membaca kelas lain tanpa memilikinya |

## Alur utama
1. **`main.cpp`** → `UDPReceiver::Poll()` membaca gesture tangan dari Python → mengisi `InputState`.
2. `Game::Update()` mendelegasikan ke subsistem sesuai `GameState` (MAIN_MENU → DRAFTING → PLAYING ↔ SHOP → GAME_OVER/VICTORY).
3. **PLAYING**: `WaveManager` spawn `Enemy` → `GridNode`/`Tower` menembak (`Projectile`) → `Hero` ultimate → reward menambah `currency`.
4. Tiap `SHOP_WAVE_INTERVAL` wave → `ShopManager` menjual kartu T2/T3 (dengan aturan prasyarat tier dari `DeckManager`).
