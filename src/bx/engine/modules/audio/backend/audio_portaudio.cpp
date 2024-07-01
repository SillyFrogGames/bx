#include "bx/engine/modules/audio.hpp"

#include "bx/engine/core/macros.hpp"

#include <stdio.h>
#include <math.h>
#include <portaudio.h>

#define AUDIO_MAX_CLIPS_PER_CHANNEL (100)

static PaStream* g_stream = nullptr;

struct ChannelImpl
{
    f32 volume = 1.0f;
    AudioHandle audios[AUDIO_MAX_CLIPS_PER_CHANNEL];
};

struct AudioImpl
{
    List<f32> samples;
    SizeType currentIndex = 0;
};

static List<ChannelImpl> g_channels;
static List<AudioImpl> g_audios;

static int AudioCallback(
    const void* inputBuffer, void* outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData)
{
    auto* out = static_cast<f32*>(outputBuffer);

    for (u64 i = 0; i < framesPerBuffer; ++i)
    {
        f32 mix = 0.0f;
        for (const auto& channel : g_channels)
        {
            for (SizeType a = 0; a < AUDIO_MAX_CLIPS_PER_CHANNEL; ++a)
            {
                AudioHandle audio = channel.audios[a];
                if (audio == AUDIO_INVALID_HANDLE)
                    continue;

                auto& audioData = g_audios[audio];
                if (audioData.currentIndex < audioData.samples.size())
                {
                    mix += audioData.samples[audioData.currentIndex++] * channel.volume;
                }
            }
        }
        *out++ = mix;
    }

    return paContinue;
}

AudioHandle Audio::GetDefaultChannel()
{
    return 0;
}

void Audio::CreateChannel(const ChannelInfo& info)
{
}

void Audio::DestroyChannel(const AudioHandle channel)
{
}

void Audio::SetChannelVolume(const AudioHandle channel, f32 volume)
{
}

void Audio::CreateAudio(const AudioInfo& info)
{
}

void Audio::DestroyAudio(const AudioHandle audio)
{
}

void Audio::PlayAudio(const AudioHandle channel, const AudioHandle audio)
{
}

void Audio::StopAudio(const AudioHandle channel, const AudioHandle audio)
{
}

bool Audio::Initialize()
{
    PaError err = Pa_Initialize();
    if (err != paNoError)
    {
        BX_LOGE("Failed to initialize PortAudio: {}", Pa_GetErrorText(err));
        return false;
    }

    err = Pa_OpenDefaultStream(
        &g_stream,
        0,          // no input channels
        1,          // mono output
        paFloat32,  // 32-bit floating point output
        44100,      // sample rate
        256,        // frames per buffer
        AudioCallback,
        0);// &data);

    if (err != paNoError)
    {
        BX_LOGE("PortAudio failed to open default stream: {}", Pa_GetErrorText(err));
        return false;
    }

    err = Pa_StartStream(g_stream);
    if (err != paNoError)
    {
        BX_LOGE("PortAudio failed to start stream: {}", Pa_GetErrorText(err));
        return false;
    }

    return true;
}

void Audio::Reload()
{
}

void Audio::Shutdown()
{
    PaError err = Pa_StopStream(g_stream);
    if (err != paNoError)
    {
        BX_LOGE("PortAudio failed to stop stream: ", Pa_GetErrorText(err));
    }

    err = Pa_CloseStream(g_stream);
    if (err != paNoError)
    {
        BX_LOGE("PortAudio failed to close stream: ", Pa_GetErrorText(err));
    }

    Pa_Terminate();
}