// AudioManager.h
#pragma once
#include <Render.h>
#include <Engine.h>
#include <Audio.h>
#include <Script.h>
#include <string>
#include <vector>

using namespace gce;

struct AudioData
{
    std::string name;
    std::wstring path;
    Category category;
    bool isLoaded;
};

DECLARE_SCRIPT(AudioManager, ScriptFlag::Start | ScriptFlag::Update)

private:
    std::vector<AudioData> audioLibrary;
    int currentIndex = -1;

    std::string walkSound = "WalkSound";
    std::string jumpSound = "JumpSound";
    std::string shootSound = "ShootSound";
    std::string realoadSound = "realoadSound";

    static AudioManager* instance;

public:
    static AudioManager* GetInstance() { return instance; }

    void Start();
    void Update();
    void InitializeAudioLibrary();
    void AddAudioToLibrary(const std::string& name, const std::wstring& relativePath, Category cat);
    void LoadAllSounds();
    void NextSound();
    void PreviousSound();
    void PlayCurrentSound();
    void PauseCurrentSound();
    void ResumeCurrentSound();
    void StopCurrentSound();
    void PlayWalkSound();
    void PlayRealoadSound();
    void PlayJumpSound();
    void PlayShootSound();
    void IncreaseVolume();
    void DecreaseVolume();

    END_SCRIPT