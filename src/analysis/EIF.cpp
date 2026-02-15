#include "EIF.hpp"
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

// веса для рецепторов
double PendulumK[256];
double DeepEnviromentHardness[256 * 32];
double DeepEnviromentResistance[256 * 32];

// Константы симуляции
const double minFreq = 20.0;     // Минимальная частота (индекс 0)
const double maxFreq = 20000.0;  // Максимальная частота (индекс 255)
const double sampleRate = 44100;
const double R = (1.0 - 5.0 / sampleRate); // чуть ниже 1

/// <summary>
/// В переменные OUT выведет энергию с рецепторов
/// 
/// Важно, чтобы сырая волна имела разрешение 
/// 44.1 КГЦ, т.е. секунда звука это 44 100 элементов массива
/// или 44 100 т.е. 176 КБ на секунду в ОЗУ
/// </summary>
void EIF::Cycle(std::vector<float>* waveRaw, int waveIndexStart, int waveIndexEnd) {

    // TODO: сделать их
    for (int waveI = waveIndexStart; waveI < waveIndexEnd; waveI++) {
        
        // Импульс, полученный от волны в этот период
        float F = waveRaw->operator[](waveI);

        // V(i)’ = F(t) - L(i) * k(i) 
        // L(i)’ = V(i)’
        // + трение
        // Пендельная модель
        for (int i = 0; i < 256; i++) {
            PendulumV[i] += F - PendulumL[i] * PendulumK[i];
            PendulumV[i] *= R;
            PendulumL[i] += PendulumV[i];

            // Получение энергии в итоге
            OUT_Pendulum[i] = std::max(float(PendulumL[i]), OUT_Pendulum[i]);
            //OUT_Pendulum[i] = std::max(255.0f, OUT_Pendulum[i]);
            OUT_Pendulum[i] *= R;
        }
    }

    for (int i = 0; i < 256; i++) {
        OUT_Deep[i] *= 1; // TODO: модель внутренних
    }
}

void EIF::InitParams() {
	// TODO: подсчитать веса для всей модели
    for (int i = 0; i < 256; i++) {

        double frequency = minFreq * std::pow(maxFreq / minFreq, double(i) / 255.0);

        // PendulumK пропорционален квадрату частоты (как жесткость пружины)
        // Берем scale = 1.0/sampleRate^2 для грубой нормализации
        double scale = 1.0 / (sampleRate * sampleRate);
        PendulumK[i] = frequency * frequency * scale * 4.0 * M_PI * M_PI;
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
    }

    for (int i = 0; i < 256 * 32; i++) { 
        DeepEnviroment[i] = 0;
    }
}