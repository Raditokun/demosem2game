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

// Satu wave bisa berisi banyak SpawnGroup → beberapa tipe musuh dalam 1 wave.
struct WaveDef {
    std::vector<SpawnGroup> groups;
};

class WaveManager {
public:
    int currentWave;
    int currentGroup;       // kelompok yang sedang di-spawn di dalam wave
    float spawnTimer;
    int spawnedThisGroup;   // sudah spawn berapa di kelompok ini
    bool waveActive;
    std::vector<WaveDef> waves;

    WaveManager();
    void Init();
    void Update(float dt, std::vector<Enemy>& enemies, const std::vector<Vector2>& path);
    void StartNextWave();
    bool AllWavesComplete() const;
    bool IsWaveCleared(const std::vector<Enemy>& enemies) const;
    void Draw() const;
};
