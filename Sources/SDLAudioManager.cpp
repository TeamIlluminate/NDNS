#include "SDLAudioManager.h"
#include "SoundProcessing.h"
#include "Settings.h"
#include "SettingsFields.h"

SDLAudioManager::SDLAudioManager()
{
    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioInit(SDL_GetAudioDriver(0));
    SDL_zero(AUDIO_SPEC);
    AUDIO_SPEC.freq = 48000;
    AUDIO_SPEC.format = AUDIO_S16;
    AUDIO_SPEC.channels = 1;
    AUDIO_SPEC.samples = AUDIO_BUF;
    AUDIO_SPEC.callback = nullptr;
}

SDLAudioManager::~SDLAudioManager()
{
    if (input != -1)
        SDL_CloseAudioDevice(input);
    if (output != -1)
        SDL_CloseAudioDevice(output);
    
    SDL_CloseAudio();
}

void SDLAudioManager::InitProcessors()
{
    auto controller = Settings::Get().GetField(S_VOLUME_IN);
    inputProcessors.push_back(std::make_shared<Volume>(std::bind(&VolumeControl::GetVolume, static_cast<VolumeControl *>(controller.get()))));

    controller = Settings::Get().GetField(S_VOLUME_OUT);
    outputProcessors.push_back(std::make_shared<Volume>(std::bind(&VolumeControl::GetVolume, static_cast<VolumeControl *>(controller.get()))));
}

bool SDLAudioManager::SetupInput(const char *deviceName)
{
    if (input != -1)
    {
        SDL_CloseAudioDevice(input);
        input == -1;
    }
    SDL_AudioSpec have;
    input = SDL_OpenAudioDevice(deviceName, 1, &AUDIO_SPEC, &have, 0);
    if (AUDIO_SPEC.format != have.format)
    {
        std::cout << "Error when opening input audio" << std::endl;
        return false;
    }
    SDL_PauseAudioDevice(input, 1);
    SDL_ClearQueuedAudio(input);
    return true;
}

bool SDLAudioManager::SetupOutput(const char *deviceName)
{
    if (output != -1)
    {
        SDL_CloseAudioDevice(output);
        output == -1;
    }
    SDL_AudioSpec have;
    output = SDL_OpenAudioDevice(deviceName, 0, &AUDIO_SPEC, &have, 0);
    if (AUDIO_SPEC.format != have.format)
    {
        std::cout << "Error when opening output audio" << std::endl;
        return false;
    }
    SDL_PauseAudioDevice(output, 1);
    SDL_ClearQueuedAudio(output);
    return true;
}

bool SDLAudioManager::RecordAudio(int16 *data)
{
    auto size = SDL_DequeueAudio(input, data, AUDIO_BUF * 2);

    if (SDL_GetQueuedAudioSize(input) > AUDIO_BUF * 4)
    {
        SDL_ClearQueuedAudio(input);
    }

    if (size == 0)
        return false;

    for (auto processor : inputProcessors)
    {
        processor->ProcessSound(data, AUDIO_BUF);
    }

    return true;
}

void SDLAudioManager::PlayAudio(Sint16 *data)
{
    if (data != NULL)
    {
        for (auto processor : outputProcessors)
        {
            processor->ProcessSound(data, AUDIO_BUF);
        }

        if (SDL_GetQueuedAudioSize(output) > AUDIO_BUF * 4)
        {
            SDL_ClearQueuedAudio(output);
        }

        SDL_QueueAudio(output, data, AUDIO_BUF * 2);
    }
}

void SDLAudioManager::Start()
{
    SDL_PauseAudioDevice(output, 0);
    SDL_PauseAudioDevice(input, 0);
    SDL_ClearQueuedAudio(input);
    SDL_ClearQueuedAudio(output);
}

void SDLAudioManager::Stop()
{
    SDL_PauseAudioDevice(output, 1);
    SDL_PauseAudioDevice(input, 1);
}