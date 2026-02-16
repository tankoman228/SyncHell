#include "EIF.hpp"
#include <iostream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/// <summary>
/// внешние рецепторы уха
/// </summary>
float EIF::OUT_Pendulum[256] = {0}; // extern

/// <summary>
/// внутренние рецепторы уха
/// </summary>
float EIF::OUT_Deep[256] = { 0 };  // extern

double DeepEnviroment[256 * 32];  // "среда" внутренних рецепторов
double PendulumL[256]; // отклонение маятника  
double PendulumV[256]; // скорость маятника

// Константы симуляции
const double minFreq = 20.0;     // Минимальная частота (индекс 0)
const double maxFreq = 20000.0;  // Максимальная частота (индекс 255)
const double sampleRate = 44100;
const double movingMaxReduce = 0.99986459733; // Корень от: 100 000 * X ^ 44 100 = 256

// веса для рецепторов
double PendulumK[256];
double PendulumR[256] = { 1.0 - 25.0 / sampleRate };
double DeepEnviromentHardness[256 * 32];
double DeepEnviromentResistance[256 * 32];

// Скользящее максимальное для приведения
double PendulumLMovingMax[256]; 


// модель конкретрно для пружинных маятников, F - сила, i - номер маятника
inline void PendulumTick(float F, int i) {

    // V(i)’ = F(t) - L(i) * k(i) 
    // L(i)’ = V(i)’
    // + трение и корректировка
    // Пендельная модель

    PendulumV[i] += F - PendulumL[i] * PendulumK[i];
    PendulumV[i] *= PendulumR[i];
    PendulumL[i] += PendulumV[i];

    PendulumL[i] *= 0.9992; // Доп. трение (лекарство от тинитуса)
    if (std::abs(PendulumL[i]) > 7000) PendulumL[i] *= 0.7;

    // Получение ненормализованной энергии (адекватно для сравнений, но не для логики игры)
    EIF::OUT_Pendulum[i] = std::max(float(PendulumL[i]), EIF::OUT_Pendulum[i]);
    EIF::OUT_Pendulum[i] *= 0.9;

    if (EIF::OUT_Pendulum[i] == NAN || EIF::OUT_Pendulum[i] == INFINITY || EIF::OUT_Pendulum[i] == -INFINITY) {
        EIF::OUT_Pendulum[i] = 255;
        //std::cout << i << " had NAN or INF error!\n";
    }
}

// Нормализация выходного значения. Нужно для процесса игры, но не каллибровки (иначе сырые веса быстро уйдут в INF) (только для цикла)
inline void PendulumNormalize(int i) {
    PendulumLMovingMax[i] = std::max(std::abs(PendulumL[i]), PendulumLMovingMax[i]);
    EIF::OUT_Pendulum[i] = EIF::OUT_Pendulum[i] / PendulumLMovingMax[i] * 254.9999f; 

    if (PendulumLMovingMax[i] > 32) // слишком малый максимум = чувствительность к любому шороху
        PendulumLMovingMax[i] *= movingMaxReduce; // очень малое число, но при накоплении за секунду значительно сбросит 
}

/// <summary>
/// В переменные OUT выведет энергию с рецепторов
/// 
/// Важно, чтобы сырая волна имела разрешение 
/// 44.1 КГЦ, т.е. секунда звука это 44 100 элементов массива
/// или 44 100 т.е. 176 КБ на секунду в ОЗУ
/// </summary>
void EIF::Cycle(std::vector<float>* waveRaw, int waveIndexStart, int waveIndexEnd) {

    for (int waveI = waveIndexStart; waveI < waveIndexEnd; waveI++) {
        
        // Импульс, полученный от волны в этот период
        float F = waveRaw->operator[](waveI);

        for (int i = 0; i < 256; i++) {
            PendulumTick(F, i);   // тик физики
            PendulumNormalize(i); // критически важно, такт работы нормализатора, без него в игру будут попадать числа не от 0 до 255!
        }
    }

    for (int i = 0; i < 256; i++) {
        OUT_Deep[i] *= 1; // TODO: модель внутренних
    }
}

void EIF::InitParams() {

    EIF::ClearOutput();
    for (int i = 0; i < 256; i++) {
 
        double frequency = minFreq * std::pow(maxFreq / minFreq, double(i) / 255.0);

        // PendulumK пропорционален квадрату частоты (как жесткость пружины)
        // Берем scale = 1.0/sampleRate^2 для грубой нормализации
        double scale = 1.0 / (sampleRate * sampleRate);
        PendulumK[i] = frequency * frequency * scale * 4.0 * M_PI * M_PI;

        // Теперь надо поподгонять. Суть в том, чтобы повысить трение так, чтобы целевая частота
        // НЕ привела к увеличению амплитуды до бесконечности, а только к 255

        float outAvg = 0;
        int k = 0;
        float F;
        int good = 0;
        int periodInTicks = 1.0 / frequency * sampleRate + 1;

        // Симулируем маятник до тех пор, пока не подгоним нужную R
        while (true)
        {
            F = 200.0 * std::sin(2.0 * M_PI * frequency * k / sampleRate) / std::sqrt(frequency) * ((i + 1.0) / 100.0); // хз почему, но большие частоты становятся убер чувствительными
            PendulumTick(F, i);
            outAvg = OUT_Pendulum[i] * 0.01 + outAvg * 0.99;
            k++;

            if (k % periodInTicks == 0) { 
                if (outAvg > 600) {
                    // Приближаем к нулю, слишком опасные колебания, риск выхода за границы double
                    PendulumR[i] *= 0.9999;
                    good = 0;
                }
                else if (outAvg < 50) {
                    // Приближаем к единице, риск полного зануления
                    PendulumR[i] = 0.8999999999 + PendulumR[i] * 0.1; // первое значимаость единицы в среднем, второе значимость  
                    good = 0;  
                }
                else good++;
            
                //std::cout << i << "\t" << PendulumR[i] << "\t" << OUT_Pendulum[i] << "\n";

                if (good > 4) {
                    std::cout << i << " решено за колебаний " << k / periodInTicks << " " << PendulumR[i] << '\n';
                    break;
                } // если прошло N колебаний с нужным значением 
                if (k / periodInTicks > 900000) { 
                    if (i > 0) PendulumR[i] = PendulumR[i - 1]; 
                    std::cout << i << " НЕ УДАЛОСЬ РЕШИТЬ, ПО УМОЛЧАНИЮ: " << PendulumR[i] << '\n';
                    break; 
                }
            }
        }

        
    }
}

/// <summary>
/// Генерирует синусоиду заданной длины для конкретного номера рецептора.
/// </summary>
/// <param name="length">Количество семплов (44100 = 1 секунда)</param>
/// <param name="noteIndex">Номер рецептора (0-255)</param>
std::vector<float> EIF::GetWaveForNote(int length, int noteIndex) {
    std::vector<float> waveRaw(length);
    
    // Логарифмический шаг: каждая октава умножает частоту на 2
    // Всего октав от 20 до 20000: log2(20000/20) = log2(1000) ≈ 10 октав
    // Формула: f = minFreq * (maxFreq/minFreq)^(index/255)
    double frequency = minFreq * std::pow(maxFreq / minFreq, noteIndex / 255.0);
    
    for (int i = 0; i < length; ++i) {
        waveRaw[i] = (float)std::sin(2.0 * M_PI * frequency * i / sampleRate);
    }

    return waveRaw;
}

void EIF::ClearOutput() {
    for (int i = 0; i < 256; i++) {
        OUT_Deep[i] = 0;
        OUT_Pendulum[i] = 0;
        PendulumL[i] = 0;
        PendulumV[i] = 0;
        PendulumLMovingMax[i] = 9000; // он быстро упадёт, но это даст время игроку и сделает переход плавнее
    }

    for (int i = 0; i < 256 * 32; i++) { 
        DeepEnviroment[i] = 0;
    }
}