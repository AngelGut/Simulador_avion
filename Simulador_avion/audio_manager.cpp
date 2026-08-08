#include "audio_manager.h"
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <filesystem>

#pragma comment(lib, "winmm.lib")

namespace {
    bool g_isMuted = false;
    bool g_isPlaying = false;
}

namespace AudioManager {
    void init() {
        if (g_isPlaying) return;

        std::string audioPath = "audio/amphitrite.mp3";
        if (!std::filesystem::exists(audioPath)) {
            if (std::filesystem::exists("../audio/amphitrite.mp3")) {
                audioPath = "../audio/amphitrite.mp3";
            }
        }

        mciSendStringA("close bgm", NULL, 0, NULL);

        std::string openCmd = "open \"" + audioPath + "\" type mpegvideo alias bgm";
        MCIERROR err = mciSendStringA(openCmd.c_str(), NULL, 0, NULL);
        if (err == 0) {
            mciSendStringA("play bgm repeat", NULL, 0, NULL);
            g_isPlaying = true;
            mciSendStringA("setaudio bgm volume to 800", NULL, 0, NULL);
            std::cout << "[Audio] Reproduciendo musica de fondo: " << audioPath << std::endl;
        } else {
            char errText[256] = {0};
            mciGetErrorStringA(err, errText, 256);
            std::cout << "[Audio] Error al iniciar audio (" << errText << "). Ruta: " << audioPath << std::endl;
        }
    }

    void toggleMute() {
        setMuted(!g_isMuted);
    }

    bool isMuted() {
        return g_isMuted;
    }

    void setMuted(bool muted) {
        g_isMuted = muted;
        if (g_isMuted) {
            mciSendStringA("pause bgm", NULL, 0, NULL);
            std::cout << "[Audio] Musica silenciada." << std::endl;
        } else {
            mciSendStringA("play bgm repeat", NULL, 0, NULL);
            std::cout << "[Audio] Musica activada." << std::endl;
        }
    }

    void pauseMenuMusic() {
        mciSendStringA("pause bgm", NULL, 0, NULL);
    }

    void resumeMenuMusic() {
        if (!g_isMuted && g_isPlaying) {
            mciSendStringA("play bgm repeat", NULL, 0, NULL);
        }
    }

    void stop() {
        if (g_isPlaying) {
            mciSendStringA("close bgm", NULL, 0, NULL);
            g_isPlaying = false;
        }
    }
}
