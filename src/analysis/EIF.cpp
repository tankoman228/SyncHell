#include "EIF.hpp"
#include <iostream>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/// <summary>
/// эталонные сравнения по частотам
/// </summary>
float EIF::OUT_Etalon[256] = {0}; // extern

/// <summary>
/// внутренние рецепторы уха
/// </summary>
float EIF::OUT_Deep[256] = { 0 };  // extern

double DeepEnviroment[256 * 32];  // "среда" внутренних рецепторов

// Константы симуляции
const double minFreq = 20.0;     // Минимальная частота (индекс 0)
const double maxFreq = 17000.0;  // Максимальная частота (индекс 255)
const double sampleRate = 44100;
const double movingMaxReduce = 0.99986459733; // Корень от: 100 000 * X ^ 44 100 = 256

// веса для рецепторов
std::vector<float> Etalons;

// Квадратурные состояния (I/Q накопление)
double Etalon_I[256];
double Etalon_Q[256];

// Коэффициенты рекуррентного генератора
double Etalon_cos_w[256];
double Etalon_sin_w[256];

// Текущие фазовые состояния генератора
double Etalon_cos_state[256];
double Etalon_sin_state[256];

// максимальные значения совпадений с эталоном, нужны для приведения сигнала в нужный вид, скользящий максимум крч
double Etalon_max_cor[256];

// Нормализация амплитуды входа (адаптивная)
double InputEnergy[256] = { 1e-9 };

double Etalon_decay_per_sample[256];

// модель конкретрно для пружинных маятников, F - сила, i - номер маятника
inline void EtalonTick(float F, int i)
{
    // --- обновляем энергию входа (для нормализации громкости) ---
    InputEnergy[i] = InputEnergy[i] * Etalon_decay_per_sample[i] + F * F * (1.0 - Etalon_decay_per_sample[i]);

    // --- квадратурная корреляция ---
    Etalon_I[i] = Etalon_I[i] * Etalon_decay_per_sample[i] + F * Etalon_cos_state[i];
    Etalon_Q[i] = Etalon_Q[i] * Etalon_decay_per_sample[i] + F * Etalon_sin_state[i];

    // --- вычисляем мощность (фаза-инвариантно) ---
    double power = Etalon_I[i] * Etalon_I[i] + Etalon_Q[i] * Etalon_Q[i];

    // --- нормализация относительно громкости входа ---
    double norm = power / (InputEnergy[i] + 1e-9);

    // нелинейность
    // размывает double value = std::log1p(norm * 10.0);  
    double value = std::pow(norm, 0.2);

    // получение сигнала
    Etalon_max_cor[i] = std::max(Etalon_max_cor[i], value); // скользящий максимум
    EIF::OUT_Etalon[i] = std::max(EIF::OUT_Etalon[i], float(value / Etalon_max_cor[i] * 255.0)); 
    EIF::OUT_Etalon[i] *= 0.999;

    Etalon_max_cor[i] *= 0.999999;
    Etalon_max_cor[i] = std::min(Etalon_max_cor[i], 8.0);

    // --- рекуррентное обновление генератора фазы ---
    double new_cos = Etalon_cos_state[i] * Etalon_cos_w[i] -
                     Etalon_sin_state[i] * Etalon_sin_w[i];

    double new_sin = Etalon_sin_state[i] * Etalon_cos_w[i] +
                     Etalon_cos_state[i] * Etalon_sin_w[i];

    Etalon_cos_state[i] = new_cos;
    Etalon_sin_state[i] = new_sin;
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
            EtalonTick(F, i);   // тик физики
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
        
        double omega = 2.0 * M_PI * frequency / sampleRate;

        Etalon_cos_w[i] = std::cos(omega);
        Etalon_sin_w[i] = std::sin(omega);

        // стартовая фаза
        Etalon_cos_state[i] = 1.0;
        Etalon_sin_state[i] = 0.0;

        Etalon_I[i] = 0.0;
        Etalon_Q[i] = 0.0;

        Etalon_max_cor[i] = 32;

        double timeConstant_ms = 30.0; // Базовая постоянная времени в мс
        double samplesPerPeriod = sampleRate / frequency;
        // Чем ниже частота, тем больше периодов нужно для того же времени
        double decayPerSample = exp(-1.0 / (timeConstant_ms * 0.001 * sampleRate));
        Etalon_decay_per_sample[i] = decayPerSample;
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
    
    for (int i = 0; i < length; i++) {
        waveRaw[i] = (float)std::sin(2.0 * M_PI * frequency * i / sampleRate);
    }

    return waveRaw;
}

void EIF::ClearOutput() {
    for (int i = 0; i < 256; i++) {
        OUT_Deep[i] = 0;
        OUT_Etalon[i] = 0;
    }

    for (int i = 0; i < 256 * 32; i++) { 
        DeepEnviroment[i] = 0;
    }
}