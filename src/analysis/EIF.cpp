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

float DeepEnviroment[256 * 32];  // "среда" внутренних рецепторов
float PendulumEnviroment[256];   // необработанное значение маятников

// веса для рецепторов
float PendulumStrengths[256];
float DeepEnviromentHardness[256 * 32];
float DeepEnviromentResistance[256 * 32];

/// <summary>
/// В переменные OUT выведет энергию с рецепторов
/// 
/// Важно, чтобы сырая волна имела разрешение 
/// 44.1 КГЦ, т.е. секунда звука это 44 100 элементов массива
/// или 44 100 т.е. 176 КБ на секунду в ОЗУ
/// </summary>
void EIF::Cycle(std::vector<float> waveRaw, int waveIndexEnd, int& waveIndex, float deltaTimeSec) {

    // TODO: Внутренний рецептор (код калька с C# неизвестно как работавшая)
    for (int& i = waveIndex; i < waveIndexEnd; i++)
    {
        DeepEnviroment[0] = waveRaw[i]; // волна входит в среду

        for (int j = std::min(256 * 32 - 2, i + 1); j >= 0; j--)
        {
            // Вправо идёт, меняясь согласно жёсткости и сопротивлению среды
            DeepEnviroment[j + 1] =
                (DeepEnviroment[j] * DeepEnviromentHardness[j] / DeepEnviromentHardness[j + 1] 
                    + DeepEnviroment[j + 1] * DeepEnviromentResistance[j])

                / (DeepEnviromentResistance[j] + 1)
                * 0.999f;

            OUT_Deep[j] += std::abs(DeepEnviroment[j + 1]); // 
        }
    }

    for (int i = 0; i < 256; i++) {
        OUT_Pendulum[i] *= 1;
        OUT_Deep[i] *= 1; // TODO: затухание
    }
}

/// <summary>
/// Генерирует синусоиду заданной длины для конкретного номера рецептора.
/// </summary>
/// <param name="length">Количество семплов (44100 = 1 секунда)</param>
/// <param name="noteIndex">Номер рецептора (0-255)</param>
std::vector<float> EIF::GetWaveForNote(int length, int noteIndex) {
    std::vector<float> waveRaw(length);

    const double minFreq = 20.0;    // Минимальная частота (индекс 0)
    const double maxFreq = 20000.0;  // Максимальная частота (индекс 255)
    const double sampleRate = 44100;
    
    // Логарифмический шаг: каждая октава умножает частоту на 2
    // Всего октав от 20 до 20000: log2(20000/20) = log2(1000) ≈ 10 октав
    // Формула: f = minFreq * (maxFreq/minFreq)^(index/255)
    double frequency = minFreq * std::pow(maxFreq / minFreq, noteIndex / 255.0);
    
    for (int i = 0; i < length; ++i) {
        waveRaw[i] = (float)std::sin(2.0 * M_PI * frequency * i / sampleRate);
    }

    return waveRaw;
}

void EIF::InitParams() {
	// TODO: генерировать ДО РЕ МИ ФА СО ЛЯ СИ для разных октав по частотам и подгонять под них каждый
	// из рецепторов, проверяя через Cycle
}