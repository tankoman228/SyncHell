#include <vector>
#include <cmath>
#include <iostream>
#include <Analysis/Splitter.hpp>

struct Biquad {
    float b0, b1, b2, a1, a2;
    float z1 = 0, z2 = 0;

    float process(float in) {
        float out = in * b0 + z1;
        z1 = in * b1 + z2 - a1 * out;
        z2 = in * b2 - a2 * out;
        return out;
    }

    void setLowPass(float freq, float sampleRate) {
        float w0 = 2 * M_PI * freq / sampleRate;
        float cosW0 = cos(w0);
        float alpha = sin(w0) / sqrt(2.0f);
        float a0 = 1 + alpha;
        b0 = (1 - cosW0) / 2 / a0;
        b1 = (1 - cosW0) / a0;
        b2 = (1 - cosW0) / 2 / a0;
        a1 = -2 * cosW0 / a0;
        a2 = (1 - alpha) / a0;
    }

    // Новый метод: честная отсечка низов
    void setHighPass(float freq, float sampleRate) {
        float w0 = 2 * M_PI * freq / sampleRate;
        float cosW0 = cos(w0);
        float alpha = sin(w0) / sqrt(2.0f);
        float a0 = 1 + alpha;
        b0 = (1 + cosW0) / 2 / a0;
        b1 = -(1 + cosW0) / a0;
        b2 = (1 + cosW0) / 2 / a0;
        a1 = -2 * cosW0 / a0;
        a2 = (1 - alpha) / a0;
    }
};

struct Splitter {
    Biquad lp1, lp2, hp1, hp2;

    void setup(float freq, float sampleRate) {
        lp1.setLowPass(freq, sampleRate);
        lp2.setLowPass(freq, sampleRate);
        hp1.setHighPass(freq, sampleRate);
        hp2.setHighPass(freq, sampleRate);
    }

    // Возвращает {то что ниже частоты, то что выше частоты}
    std::pair<float, float> process(float in) {
        // Двойной проход (4-й порядок) дает крутой срез и чистую фазу
        float low = lp1.process(lp2.process(in));
        float high = hp1.process(hp2.process(in));
        return { low, high };
    }
};

// TODO: сделать, чтобы оно выдавало 5 векторов дорожек, прям делило
void processEqualizer(std::vector<float>& rawSound) {
    float sampleRate = 44100.0f;

    // Создаем цепочку разделителей
    Splitter s1, s2, s3, s4;
    s1.setup(150.0f, sampleRate);
    s2.setup(600.0f, sampleRate);
    s3.setup(2000.0f, sampleRate);
    s4.setup(7000.0f, sampleRate);

    double totalDiff = 0;

    for (size_t i = 0; i < rawSound.size(); ++i) {
        float original = rawSound[i];

        // 1. Последовательно делим спектр
        // Отделяем саб-бас, остаток кидаем дальше
        auto res1 = s1.process(original);
        float b1 = res1.first;           // 0 - 150 Гц

        // Из того, что выше 150, выделяем 150-600
        auto res2 = s2.process(res1.second);
        float b2 = res2.first;           // 150 - 600 Гц

        // Из того, что выше 600, выделяем 600-2000
        auto res3 = s3.process(res2.second);
        float b3 = res3.first;           // 600 - 2000 Гц

        // Из того, что выше 2000, выделяем 2000-7000 и остаток
        auto res4 = s4.process(res3.second);
        float b4 = res4.first;           // 2000 - 7000 Гц
        float b5 = res4.second;          // 7000+ Гц

        totalDiff += std::abs(original - (b1 + b2 + b3 + b4 + b5));

        // 2. Применяем усиление (теперь в b3 нет грязи от b1)
        b1 *= 0.2f;
        b2 *= 0.5f;
        b3 *= 1.9f;
        b4 *= 2.2f;
        b5 *= 1.8f;

        // 3. Сборка (сумма будет чистой благодаря фазовой согласованности HP/LP)
        rawSound[i] = b1 + b2 + b3 + b4 + b5;
    }

    std::cout << "Total diff: " << totalDiff << "\n";
    std::cout << "Avg diff:   " << totalDiff / rawSound.size() << "\n";
}
