#pragma once

#ifndef SpectroH_define
#define SpectroH_define

#include <string>
#include <vector>

namespace Spectro {
    
    struct Spectrogram {
        int timeLength = 0, heigth = 0; // координаты перепуаны верно. Первая координата время, ширина, а высота это частоты
        float **spectro = nullptr;

        void FreeMemory() {

            if (timeLength <= 0 || spectro == nullptr) return;

            for (int i = 0; i < timeLength; i++) {
                delete spectro[i];
            }
            delete spectro;
            spectro = nullptr;
            timeLength = 0;
            heigth = 0;
        }
    };

    void loadRawSound(std::string file, std::vector<float>* waveRaw);

    Spectrogram getSpectroFromOgg(std::string file, int accuracy, int height);
}

#endif // !SpectroH_define