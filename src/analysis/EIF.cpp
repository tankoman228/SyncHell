#include "EIF.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/// <summary>
/// внешние рецепторы уха
/// </summary>
float OUT_Pendulum[256] = {0}; // extern

/// <summary>
/// внутренние рецепторы уха
/// </summary>
float OUT_Deep[256] = { 0 };  // extern

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
std::vector<float> GetWaveForNote(int length, int noteIndex) {
    std::vector<float> waveRaw(length);

    const double startFreq = 20.0;  // Базовая частота для 0-го индекса
    const double step = 1.0 / 36.0; // Шаг: 1/36 октавы (так как 3 шага на полутон, а в октаве 12 полутонов)
    const double sampleRate = 44100.0;

    // Вычисляем частоту для текущего индекса: f = f0 * 2^(index * step)
    double frequency = startFreq * std::pow(2.0, noteIndex * step);

    for (int i = 0; i < length; ++i) {
        // Генерируем стандартный синус
        waveRaw[i] = (float)std::sin(2.0 * M_PI * frequency * i / sampleRate);
    }

    return waveRaw;
}

void EIF::InitParams() {
	// TODO: генерировать ДО РЕ МИ ФА СО ЛЯ СИ для разных октав по частотам и подгонять под них каждый
	// из рецепторов, проверяя через Cycle
}