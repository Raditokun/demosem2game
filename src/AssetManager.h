#pragma once
#include <raylib.h>
#include <map>
#include <string>

//AssetManager untuk memuat den mengelola asset dengan key string


class AssetManager {
public:
    AssetManager();
    ~AssetManager();

    
    void Load(const std::string& key, const std::string& filepath);
    //Load("enemy", "assets/enemy.png")
    Texture2D* Get(const std::string& key);
    
    bool Has(const std::string& key) const;
    void UnloadAll();
    static void LoadSoundAsset(const std::string& key, const std::string& path);
    static Sound GetSound(const std::string& key);

  
    static Texture2D* GetTextureStatic(const std::string& key);

private:
    std::map<std::string, Texture2D> textures;
    static std::map<std::string, Sound> sounds;
    static AssetManager* instance;  // di-set oleh konstruktor
};
