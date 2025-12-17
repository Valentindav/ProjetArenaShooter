#include "AudioManager.h"

using namespace gce;

AudioManager* AudioManager::m_instance = nullptr;

void AudioManager::Start()
{
    m_instance = this;
    InitializeAudioLibrary();
    LoadAllSounds();
    AudioUse::SetMasterVolume(50);

    if (!m_audioLibrary.empty())
    {
        m_currentIndex = 0;
        if (m_audioLibrary[m_currentIndex].m_category == Category::MUSIC)
        {
            AudioUse::Play(m_audioLibrary[m_currentIndex].m_name.c_str(), true);
        }
        else
        {
            PlayCurrentSound();
        }
    }
}

void AudioManager::Update()
{
    if (GetKeyDown(Keyboard::RIGHT_ARROW))
        NextSound();

    if (GetKeyDown(Keyboard::LEFT_ARROW))
        PreviousSound();

    if (GetKeyDown(Keyboard::C))
        PlayCurrentSound();

    if (GetKeyDown(Keyboard::V))
        PauseCurrentSound();

    if (GetKeyDown(Keyboard::B))
        ResumeCurrentSound();

    if (GetKeyDown(Keyboard::X))
        StopCurrentSound();

    if (GetKeyDown(Keyboard::UP_ARROW))
        IncreaseVolume();

    if (GetKeyDown(Keyboard::DOWN_ARROW))
        DecreaseVolume();
}

void AudioManager::InitializeAudioLibrary()
{
    AddAudioToLibrary("Music1", L"res/Exemple/Music.mp3", Category::MUSIC);
    AddAudioToLibrary("Son1", L"res/Exemple/DROSHEKALIMO.mp3", Category::SFX);

    //AddAudioToLibrary(m_walkSound, L"res/Exemple/DROSHEKALIMO.mp3", Category::SFX);
   // AddAudioToLibrary(m_jumpSound, L"res/Exemple/DROSHEKALIMO.mp3", Category::SFX);
    //AddAudioToLibrary(m_shootSound, L"res/Exemple/DROSHEKALIMO.mp3", Category::SFX);
    //AddAudioToLibrary(m_realoadSound, L"res/Exemple/DROSHEKALIMO.mp3", Category::SFX);
}

void AudioManager::AddAudioToLibrary(const std::string& name, const std::wstring& relativePath, Category cat)
{
    AudioData data;
    data.m_name = name;
    data.m_path = std::wstring(WRES_PATH) + relativePath;
    data.m_category = cat;
    data.m_isLoaded = false;
    m_audioLibrary.push_back(data);
}

void AudioManager::LoadAllSounds()
{
    for (auto& audio : m_audioLibrary)
    {
        AudioUse::LoadSound(audio.m_name.c_str(), audio.m_path.c_str());
        AudioUse::SetAudioCategory(audio.m_name.c_str(), audio.m_category);
        audio.m_isLoaded = true;
    }
}

void AudioManager::NextSound()
{
    if (m_audioLibrary.empty()) return;
    StopCurrentSound();
    m_currentIndex = (m_currentIndex + 1) % m_audioLibrary.size();
    PlayCurrentSound();
}

void AudioManager::PreviousSound()
{
    if (m_audioLibrary.empty()) return;
    StopCurrentSound();
    m_currentIndex--;
    if (m_currentIndex < 0)
        m_currentIndex = m_audioLibrary.size() - 1;
    PlayCurrentSound();
}

void AudioManager::PlayCurrentSound()
{
    if (m_currentIndex >= 0 && m_currentIndex < m_audioLibrary.size())
    {
        const AudioData audio = m_audioLibrary[m_currentIndex];
        bool loop = (audio.m_category == Category::MUSIC);
        AudioUse::Stop(audio.m_name.c_str());
        AudioUse::Play(audio.m_name.c_str(), loop);
    }
}

void AudioManager::PauseCurrentSound()
{
    if (m_currentIndex >= 0 && m_currentIndex < m_audioLibrary.size())
        AudioUse::Pause(m_audioLibrary[m_currentIndex].m_name.c_str());
}

void AudioManager::ResumeCurrentSound()
{
    if (m_currentIndex >= 0 && m_currentIndex < m_audioLibrary.size())
        AudioUse::Resume(m_audioLibrary[m_currentIndex].m_name.c_str());
}

void AudioManager::StopCurrentSound()
{
    if (m_currentIndex >= 0 && m_currentIndex < m_audioLibrary.size())
        AudioUse::Stop(m_audioLibrary[m_currentIndex].m_name.c_str());
}

void AudioManager::PlayWalkSound()
{
    AudioUse::Play(m_walkSound.c_str(), false);
}

void AudioManager::PlayRealoadSound()
{
    AudioUse::Play(m_realoadSound.c_str(), false);
}

void AudioManager::PlayJumpSound()
{
    AudioUse::Play(m_jumpSound.c_str(), false);
}

void AudioManager::PlayShootSound()
{
    AudioUse::Play(m_shootSound.c_str(), false);
}

void AudioManager::IncreaseVolume()
{
    if (m_currentIndex >= 0 && m_currentIndex < m_audioLibrary.size())
    {
        const AudioData audio = m_audioLibrary[m_currentIndex];
        AudioUse::SetVolumeOfCategory(audio.m_category, 80);
    }
}

void AudioManager::DecreaseVolume()
{
    if (m_currentIndex >= 0 && m_currentIndex < m_audioLibrary.size())
    {
        const AudioData audio = m_audioLibrary[m_currentIndex];
        AudioUse::SetVolumeOfCategory(audio.m_category, 20);
    }
}