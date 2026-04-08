#include <vector>
#include <string>
#include <sndfile.h>
#include <fftw3.h>
#include <cmath>
#include <algorithm>
#include <omp.h>
#include <iostream>
#include <Analysis/Spectro.hpp>

namespace Spectro {

    void loadRawSound(std::string file, std::vector<float>* waveRaw) {
        if (!waveRaw) {
            throw std::invalid_argument("waveRaw pointer is null");
        }
        
        SF_INFO sfinfo;
        SNDFILE* sndfile = sf_open(file.c_str(), SFM_READ, &sfinfo);
        
        if (!sndfile) {
            throw std::runtime_error("Cannot open file: " + file);
        }
        
        // Проверяем, что частота дискретизации соответствует требуемой
        const int TARGET_SR = 44100;
        
        // Читаем все сэмплы
        std::vector<float> temp(sfinfo.frames * sfinfo.channels);
        sf_readf_float(sndfile, temp.data(), sfinfo.frames);
        sf_close(sndfile);
        
        // Конвертируем в моно, если нужно
        std::vector<float> mono(sfinfo.frames);
        if (sfinfo.channels == 1) {
            mono = temp;
        } else {
            for (int i = 0; i < sfinfo.frames; i++) {
                float sum = 0;
                for (int ch = 0; ch < sfinfo.channels; ch++) {
                    sum += temp[i * sfinfo.channels + ch];
                }
                mono[i] = sum / sfinfo.channels; // Усреднение каналов
            }
        }
        
        // Ресемплинг до 44100 Гц, если нужно
        if (sfinfo.samplerate != TARGET_SR) {
            float ratio = static_cast<float>(TARGET_SR) / sfinfo.samplerate;
            int newLength = static_cast<int>(mono.size() * ratio);
            
            std::vector<float> resampled(newLength);
            
            // Простая линейная интерполяция (для более качественного - использовать библиотеку)
            #pragma omp parallel for
            for (int i = 0; i < newLength; i++) {
                float srcPos = i / ratio;
                int srcIndex = static_cast<int>(srcPos);
                float frac = srcPos - srcIndex;
                
                if (srcIndex + 1 < mono.size()) {
                    resampled[i] = mono[srcIndex] * (1 - frac) + mono[srcIndex + 1] * frac;
                } else {
                    resampled[i] = mono[srcIndex];
                }
            }
            
            mono = std::move(resampled);
        }
        
        // Нормализация (опционально - убеждаемся, что значения в [-1, 1])
        float maxVal = 0.0f;
        for (float val : mono) {
            maxVal = std::max(maxVal, std::abs(val));
        }
        
        if (maxVal > 1.0f) {
            // Если значения превышают 1, нормализуем
            float scale = 1.0f / maxVal;
            #pragma omp parallel for
            for (float& val : mono) {
                val *= scale;
            }
        }
        
        // Масштабируем до [-100, 100] для удобства
        const float SCALE_FACTOR = 100.0f;
        waveRaw->resize(mono.size());
        
        #pragma omp parallel for
        for (size_t i = 0; i < mono.size(); i++) {
            waveRaw->operator[](i) = mono[i] * SCALE_FACTOR;
        }
        
        std::cout << "Loaded: " << mono.size() << " waveRaw at " 
                  << TARGET_SR << " Hz" << std::endl;
    }

    Spectrogram getSpectroFromOgg(std::string file, int accuracy, int height) {

        Spectrogram result;

        // 1. Загружаем аудио
        SF_INFO sfinfo;
        SNDFILE* sndfile = sf_open(file.c_str(), SFM_READ, &sfinfo);

        std::vector<float> waveRaw(sfinfo.frames * sfinfo.channels);
        sf_readf_float(sndfile, waveRaw.data(), sfinfo.frames);
        sf_close(sndfile);

        // Моно
        std::vector<float> mono(sfinfo.frames);
        for (int i = 0; i < sfinfo.frames; i++)
            mono[i] = waveRaw[i * sfinfo.channels];

        // 2. Настройки
        int samplerate = sfinfo.samplerate;
        int hopSize = samplerate / accuracy;
        int fftSize = 1;
        while (fftSize < height * 2) fftSize *= 2;

        // Создаем FFT планы для каждого потока
        std::vector<fftwf_plan> plans;
        std::vector<std::vector<float>> inBuffers;
        std::vector<std::vector<fftwf_complex>> outBuffers;
        
        int maxThreads = omp_get_max_threads();
        
        for (int i = 0; i < maxThreads; i++) {
            inBuffers.emplace_back(fftSize);
            outBuffers.emplace_back(fftSize / 2 + 1);
            plans.push_back(fftwf_plan_dft_r2c_1d(fftSize, 
                inBuffers[i].data(), 
                outBuffers[i].data(), 
                FFTW_MEASURE));
        }

        // 3. Создаем спектрограмму
        int nFrames = (mono.size() - fftSize) / hopSize;
        result.spectro = new float*[nFrames];
        for (int f = 0; f < nFrames; f++) {
            result.spectro[f] = new float[height];
        }
        result.timeLength = nFrames;
        result.heigth = height;

        //diagram.resize(nFrames, nullptr); Ранее было так, но теперь выходной массив создавать надо иначе

        #pragma omp parallel for num_threads(8)
        for (int frame = 0; frame < nFrames; frame++) {
            int threadId = omp_get_thread_num();
            
            int start = frame * hopSize;
            auto& in = inBuffers[threadId];
            auto& out = outBuffers[threadId];
            
            // Копируем окно + окно Хэннинга
            for (int j = 0; j < fftSize; j++) {
                if (start + j < (int)mono.size())
                    in[j] = mono[start + j] * (0.5f - 0.5f * cos(2 * 3.14159f * j / (fftSize - 1)));
                else
                    in[j] = 0.0f;
            }

            fftwf_execute(plans[threadId]);

            // Логарифмическое распределение по высоте
            for (int h = 0; h < height; h++) {
                float frac = (float)h / (height - 1);
                float freq = 20.0f * powf((samplerate / 2.0f) / 20.0f, frac);
                int bin = (int)(freq * fftSize / samplerate);
                
                if (bin < 1) bin = 1;
                if (bin >= fftSize / 2 + 1) bin = fftSize / 2;
                
                float re = out[bin][0];
                float im = out[bin][1];
                float mag = sqrtf(re * re + im * im);
                float db = 20.0f * log10f(mag + 1e-6f);
                
                result.spectro[frame][h] = db;
            }
        }

        // Очистка планов
        for (auto& plan : plans) {
            fftwf_destroy_plan(plan);
        }

        // 4. Нормировка (0–255)
        // Находим min/max
        float minVal = 1e9, maxVal = -1e9;
        for (int i = 0; i < result.timeLength; i++) {
            for (int j = 0; j < height; j++) {
                float v = result.spectro[i][j];

                if (v < minVal) minVal = v;
                if (v > maxVal) maxVal = v;         
            }
        }

        float range = maxVal - minVal;
        if (range > 1e-6f) {
            #pragma omp parallel for num_threads(8)
            for (int i = 0; i < result.timeLength; i++) {
                for (int j = 0; j < height; j++) {
                    float* v = &result.spectro[i][j];
                    *v = (*v - minVal) / range * 255.0f;
                }
            }
        }

        std::cout << result.timeLength << " Width of result spectro \n";

        return result;
    }
}