#pragma once
#include <vector>
#include "Enemy.h"

// Satu kelompok spawn: sejumlah musuh dengan SATU tipe.
struct SpawnGroup {
    int count;
    float interval;
    EnemyType type;
    float hpMult;
};

// Mode urutan spawn dalam satu wave.
enum class SpawnOrder {
    SEQUENTIAL,  // kelompok keluar gantian: kelompok 1 habis dulu, baru kelompok 2
    MIXED        // semua kelompok keluar paralel → tipe musuh kecampur
};

// Satu wave bisa berisi banyak SpawnGroup
struct WaveDef {
    std::vector<SpawnGroup> groups;
    SpawnOrder order = SpawnOrder::SEQUENTIAL;  // default: gantian
};

class WaveManager {
public:
    int currentWave;
    bool waveActive;
    std::vector<WaveDef> waves;

    // State runtime per-kelompok (di-resize tiap StartNextWave)
    std::vector<int>   groupSpawned;  // sudah spawn berapa per kelompok
    std::vector<float> groupTimer;    // timer mundur per kelompok

    WaveManager();
    void Init();
    void Update(float dt, std::vector<Enemy>& enemies, const std::vector<Vector2>& path);
    void StartNextWave();
    bool AllWavesComplete() const;
    bool IsWaveCleared(const std::vector<Enemy>& enemies) const;
    void Draw() const;
};
