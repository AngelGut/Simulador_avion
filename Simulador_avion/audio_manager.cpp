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

    void playEngineSound(int planeIndex) {
        std::string planeId = "";
        switch (planeIndex) {
        case 0: planeId = "a-10_thunderbolt_ii"; break;
        case 1: planeId = "b-24_liberator"; break;
        case 2: planeId = "boeing-787-_dreamliner"; break;
        case 3: planeId = "mig_29_9-13"; break;
        default: return;
        }

        mciSendStringA("close engine_sfx", NULL, 0, NULL);

        std::string path = "audio/" + planeId + ".mp3";
        if (!std::filesystem::exists(path)) {
            if (std::filesystem::exists("../audio/" + planeId + ".mp3")) {
                path = "../audio/" + planeId + ".mp3";
            }
        }

        if (!std::filesystem::exists(path)) {
            std::cerr << "[Audio ERROR] No se encontro el archivo de audio para " << planeId << ": " << path << std::endl;
            return;
        }

        std::string openCmd = "open \"" + path + "\" type mpegvideo alias engine_sfx";
        MCIERROR err = mciSendStringA(openCmd.c_str(), NULL, 0, NULL);
        if (err == 0) {
            if (g_isMuted) {
                mciSendStringA("setaudio engine_sfx off", NULL, 0, NULL);
            }
            mciSendStringA("play engine_sfx", NULL, 0, NULL);
            std::cout << "[Audio] Reproduciendo sonido de motor para " << planeId << ": " << path << std::endl;
        } else {
            std::cerr << "[Audio ERROR] Fallo al reproducir sonido MCI: " << path << " (Error " << err << ")" << std::endl;
        }
    }

    void stop() {
        if (g_isPlaying) {
            mciSendStringA("close bgm", NULL, 0, NULL);
            g_isPlaying = false;
        }
    }
}
