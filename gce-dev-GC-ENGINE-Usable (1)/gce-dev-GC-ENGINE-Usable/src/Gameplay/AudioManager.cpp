#include "AudioManager.h"

using namespace gce;

AudioManager* AudioManager::instance = nullptr;

void AudioManager::Start()
{
    instance = this;
    InitializeAudioLibrary();
    LoadAllSounds();
    AudioUse::SetMasterVolume(50);

    if (!audioLibrary.empty())
    {
        currentIndex = 0;
        if (audioLibrary[currentIndex].category == Category::MUSIC)
        {
            AudioUse::Play(audioLibrary[currentIndex].name.c_str(), true);
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

    //AddAudioToLibrary(walkSound, L"res/Exemple/DROSHEKALIMO.mp3", Category::SFX);
   // AddAudioToLibrary(jumpSound, L"res/Exemple/DROSHEKALIMO.mp3", Category::SFX);
    //AddAudioToLibrary(shootSound, L"res/Exemple/DROSHEKALIMO.mp3", Category::SFX);
    //AddAudioToLibrary(realoadSound, L"res/Exemple/DROSHEKALIMO.mp3", Category::SFX);
}

void AudioManager::AddAudioToLibrary(const std::string& name, const std::wstring& relativePath, Category cat)
{
    AudioData data;
    data.name = name;
    data.path = std::wstring(WRES_PATH) + relativePath;
    data.category = cat;
    data.isLoaded = false;
    audioLibrary.push_back(data);
}

void AudioManager::LoadAllSounds()
{
    for (auto& audio : audioLibrary)
    {
        AudioUse::LoadSound(audio.name.c_str(), audio.path.c_str());
        AudioUse::SetAudioCategory(audio.name.c_str(), audio.category);
        audio.isLoaded = true;
    }
}

void AudioManager::NextSound()
{
    if (audioLibrary.empty()) return;
    StopCurrentSound();
    currentIndex = (currentIndex + 1) % audioLibrary.size();
    PlayCurrentSound();
}

void AudioManager::PreviousSound()
{
    if (audioLibrary.empty()) return;
    StopCurrentSound();
    currentIndex--;
    if (currentIndex < 0)
        currentIndex = audioLibrary.size() - 1;
    PlayCurrentSound();
}

void AudioManager::PlayCurrentSound()
{
    if (currentIndex >= 0 && currentIndex < audioLibrary.size())
    {
        const AudioData audio = audioLibrary[currentIndex];
        bool loop = (audio.category == Category::MUSIC);
        AudioUse::Stop(audio.name.c_str());
        AudioUse::Play(audio.name.c_str(), loop);
    }
}

void AudioManager::PauseCurrentSound()
{
    if (currentIndex >= 0 && currentIndex < audioLibrary.size())
        AudioUse::Pause(audioLibrary[currentIndex].name.c_str());
}

void AudioManager::ResumeCurrentSound()
{
    if (currentIndex >= 0 && currentIndex < audioLibrary.size())
        AudioUse::Resume(audioLibrary[currentIndex].name.c_str());
}

void AudioManager::StopCurrentSound()
{
    if (currentIndex >= 0 && currentIndex < audioLibrary.size())
        AudioUse::Stop(audioLibrary[currentIndex].name.c_str());
}

void AudioManager::PlayWalkSound()
{
    AudioUse::Play(walkSound.c_str(), false);
}

void AudioManager::PlayRealoadSound()
{
    AudioUse::Play(realoadSound.c_str(), false);
}

void AudioManager::PlayJumpSound()
{
    AudioUse::Play(jumpSound.c_str(), false);
}

void AudioManager::PlayShootSound()
{
    AudioUse::Play(shootSound.c_str(), false);
}

void AudioManager::IncreaseVolume()
{
    if (currentIndex >= 0 && currentIndex < audioLibrary.size())
    {
        const AudioData audio = audioLibrary[currentIndex];
        AudioUse::SetVolumeOfCategory(audio.category, 80);
    }
}

void AudioManager::DecreaseVolume()
{
    if (currentIndex >= 0 && currentIndex < audioLibrary.size())
    {
        const AudioData audio = audioLibrary[currentIndex];
        AudioUse::SetVolumeOfCategory(audio.category, 20);
    }
}