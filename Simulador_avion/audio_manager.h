#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

namespace AudioManager {
    void init();
    void toggleMute();
    bool isMuted();
    void setMuted(bool muted);
    void stop();
}

#endif // AUDIO_MANAGER_H
