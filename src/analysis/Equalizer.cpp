#include "Equalizer.hpp"


struct Biquad {
    float b0, b1, b2, a1, a2;
    float z1 = 0, z2 = 0;

    // Обработка одного сэмпла
    float process(float in) {
        float out = in * b0 + z1;
        z1 = in * b1 + z2 - a1 * out;
        z2 = in * b2 - a2 * out;
        return out;
    }

    // Настройка коэффициентов для Low-Pass (Низкие частоты)
    void setLowPass(float freq, float sampleRate) {
        float w0 = 2 * M_PI * freq / sampleRate;
        float cosW0 = cos(w0);
        float alpha = sin(w0) / sqrt(2.0f); // Q = 0.707 (Butterworth)
        float a0 = 1 + alpha;
        b0 = (1 - cosW0) / 2 / a0;
        b1 = (1 - cosW0) / a0;
        b2 = (1 - cosW0) / 2 / a0;
        a1 = -2 * cosW0 / a0;
        a2 = (1 - alpha) / a0;
    }
};


void processEqualizer(std::vector<float>& rawSound) {
    float sampleRate = 44100.0f;

    // Четыре фильтра, чтобы разделить на 5 зон
    Biquad lp1, lp2, lp3, lp4;
    lp1.setLowPass(150.0f, sampleRate);  // Саб-бас
    lp2.setLowPass(600.0f, sampleRate);  // Низкие-средние
    lp3.setLowPass(2000.0f, sampleRate);  // Высокие-средние
    lp4.setLowPass(7000.0f, sampleRate);  // Верха

    double totalDiff = 0; // double для точности при больших суммах

    for (size_t i = 0; i < rawSound.size(); ++i) {
        float original = rawSound[i];

        // 1. Раскладываем на слои (как матрешку)
        float b1 = lp1.process(original);          // 0 - 150
        float rest1 = original - b1;               // Остаток выше 150

        float b2 = lp2.process(rest1);             // 150 - 600
        float rest2 = rest1 - b2;                  // Остаток выше 600

        float b3 = lp3.process(rest2);             // 600 - 2500
        float rest3 = rest2 - b3;                  // Остаток выше 2500

        float b4 = lp4.process(rest3);             // 2500 - 8000
        float b5 = rest3 - b4;                     // Все что выше 8000 (остаток)

        // Проверка: сумма b1+b2+b3+b4+b5 в точности равна original
        totalDiff += std::abs(original - (b1 + b2 + b3 + b4 + b5));

        // 2. Твои коэффициенты усиления (Gain)
        b1 *= 0.3f;
        b2 *= 0.5f;
        b3 *= 1.9f;
        b4 *= 2.2f;
        b5 *= 1.8f;

        // 3. Собираем обратно
        rawSound[i] = b1 + b2 + b3 + b4 + b5;
    }

    std::cout << "Total diff: " << totalDiff << "\n";
    std::cout << "Avg diff:   " << totalDiff / rawSound.size() << "\n";
}