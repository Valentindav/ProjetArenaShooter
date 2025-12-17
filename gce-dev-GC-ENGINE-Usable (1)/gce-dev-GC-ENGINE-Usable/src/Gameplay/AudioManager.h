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
    std::string m_name;
    std::wstring m_path;
    Category m_category;
    bool m_isLoaded;
};

DECLARE_SCRIPT(AudioManager, ScriptFlag::Start | ScriptFlag::Update)

private:
    std::vector<AudioData> m_audioLibrary;
    int m_currentIndex = -1;

    std::string m_walkSound = "WalkSound";
    std::string m_jumpSound = "JumpSound";
    std::string m_shootSound = "ShootSound";
    std::string m_realoadSound = "realoadSound";

    static AudioManager* m_instance;

public:
    static AudioManager* GetInstance() { return m_instance; }

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