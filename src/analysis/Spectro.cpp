#include <vector>
#include <string>
#include <sndfile.h>
#include <fftw3.h>
#include <cmath>
#include <algorithm>
#include <omp.h>
#include <iostream>
#include <Spectro.hpp>

namespace Spectro {

    Spectrogram getSpectroFromOgg(std::string file, int accuracy, int height) {

        Spectrogram result;

        // 1. Загружаем аудио
        SF_INFO sfinfo;
        SNDFILE* sndfile = sf_open(file.c_str(), SFM_READ, &sfinfo);

        std::vector<float> samples(sfinfo.frames * sfinfo.channels);
        sf_readf_float(sndfile, samples.data(), sfinfo.frames);
        sf_close(sndfile);

        // Моно
        std::vector<float> mono(sfinfo.frames);
        for (int i = 0; i < sfinfo.frames; i++)
            mono[i] = samples[i * sfinfo.channels];

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