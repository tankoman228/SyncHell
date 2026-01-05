__kernel void process(
    __global const float* input,
    __global float* output,
    const int spectroHeight,
    const int spectroWidth,
    const int chunkSize
) {
    int gid = get_global_id(0); 
    int offset = gid * 256;

    int readOffset = chunkSize * spectroHeight * gid; // Чанки хранятся последовательно. Линейный размер чанка это его ширина на высоту
    
    // последние фичи, почему float, см. посл. фичу
    float volume = 0; // максимальный пиксель
    float ribsH = 0;  // число горизонтальных рёбер (резкие перепады)
    float ribsV = 0;  // число вертикальных рёбер (резкие перепады)

    float FeaturesSum[12]; // Сумма для всех фич. Сделал бы среднее, но разницы найн, нормализации по фичам и так нет

    // Считаем размеры каждой полосы, i это номер полосы
    int rowSegmentHeight = spectroHeight / 20;
    for (int i = 0; i < 20; i++) {
        int rowSegmentStart = rowSegmentHeight * i;
        int rowSegmentEnd = rowSegmentStart + rowSegmentHeight;

        float min = 9999;
        float max = 0;
        float delta_positive = 0;
        float delta_negative = 0;
        float prevValue = 0;
        float sumEnergyMax = 0; // вспомогательная переменная
        float energy = 0;

        // Пики
        float dominateFirstValue = 0;
        float dominateSecondValue = 0;
        int dominateFirstIndex = 0;
        int dominateSecondIndex = 0;

        float prevFreqEnergy = 0; // для условия, что доминирующая находится в экстремме энергия(частоты), см производные крч 

        // Чешем полосы частот. Внутри каждой частоты слева направо считаем дельты
        for (int freq = rowSegmentStart; freq < rowSegmentEnd; freq++) { // По полосе чанка
            float freqEnergy = 0;

            int indexOfFreq = freq - rowSegmentStart;
            
            for (int time = 0; time < chunkSize; time++) { // По колонке внутри полосы чанка
                
                // Элемент из спектрограммы это частота, плюс высота на момент в чанке. Т.е. линейно оффсет по частоте, по времени домножать на высоту
                float inp = input[readOffset + freq + spectroHeight * time]; 

                sumEnergyMax += 255;

                freqEnergy += inp;

                if (inp < min) { min = inp; }
                if (inp > max) { max = inp; }

                float delta = prevValue - inp;
                if (delta * delta > 20) {

                    ribsV++; // изменился рисунок вертикально, т.е. шла частота слева направо и изменилась

                    if (delta > 0) {
                        delta_positive += delta;
                    }
                    else {
                        delta_negative -= delta;
                    }
                    prevValue = inp;
                }
            }

            // Только в экстремме смотрим. Т.е. если у нас пик графика, максимум будет искать только в следующем пике. Это частоты
            if (freqEnergy > prevFreqEnergy) {
                // Если энергия больше, чем у первой, тогда она займёт место первой. 
                if (freqEnergy > dominateFirstValue) {
                    dominateFirstValue = freqEnergy;
                    dominateFirstIndex = indexOfFreq;
                }
                else if (freqEnergy > dominateSecondValue) { // Иначе попытается занять место 2-й, 3-й...
                    dominateSecondValue = freqEnergy;
                    dominateSecondIndex = indexOfFreq;
                }           
            }

            // итерируем остатки
            energy += freqEnergy;
            prevFreqEnergy = freqEnergy;
        }
              
        float waveLineared = 0; // линеаризованная часть волны, нужна для фич, определяющих форму волны (надеюсь, не слишком криво)
        
        // для фич, считающиеся по форме волны
        float maxDerived = 0;
        float minDerived = 99e99;
        float avgDerived = 0;

        float maxDerived2 = 0; // производные второго порядка
        float minDerived2 = 99e99;
        float avgDerived2 = 0;

        float derived = 0;
        float prevWave = 0; 
        float prevDerived = 0; 

        // Чешем частоты. Внутри каждой временной точки
        for (int time = 0; time < chunkSize; time++) { // По полосе чанка

            waveLineared = 0;
            float prevValue = 0;

            for (int freq = rowSegmentStart; freq < rowSegmentEnd; freq++) { // По колонке внутри полосы чанка
                
                float inp = input[readOffset + freq + spectroHeight * time]; 
                waveLineared += inp;

                if (volume < inp) {
                    volume = inp;
                }
                
                float delta = prevValue - inp;
                if (delta * delta > 20) {
                    ribsH++; // ребро горизонтально, на участке времени в зависимости от частоты... 
                }

                prevValue = inp;
            }

            // первая производная
            if (time > 0) {
                derived = waveLineared - prevWave;
                if (derived > maxDerived) {
                    maxDerived = derived;
                }
                else if (derived < minDerived) {
                    minDerived = derived;
                }
                avgDerived += derived;
            }

            // вторая производная
            if (time > 1) {
                float derived2 = derived - prevDerived;
                if (derived2 > maxDerived2) {
                    maxDerived2 = derived2;
                }
                else if (derived2 < minDerived2) {
                    minDerived2 = derived2;
                }
                avgDerived2 += derived2;
            }

            prevWave = waveLineared;
            prevDerived = derived;
        }
        avgDerived /= chunkSize - 1;
        avgDerived2 /= chunkSize - 2;

        float sharpness[4];
        sharpness[0] = maxDerived - avgDerived;
        sharpness[1] = avgDerived - minDerived;
        sharpness[2] = maxDerived2 - avgDerived2;
        sharpness[3] = avgDerived2 - minDerived2;


        // Общие фичи для ряда, offset, (i + ?) это номер фичи
        output[offset + i] = energy / sumEnergyMax;
        output[offset + i + 20] = (min + max) / 2;
        output[offset + i + 40] = delta_positive;
        output[offset + i + 60] = delta_negative;
        output[offset + i + 80] = dominateFirstValue;
        output[offset + i + 100] = dominateSecondValue;
        output[offset + i + 120] = dominateFirstIndex;
        output[offset + i + 140] = dominateSecondIndex;

        output[offset + i + 160] = sharpness[0];
        output[offset + i + 180] = sharpness[1];
        output[offset + i + 200] = sharpness[2];
        output[offset + i + 220] = sharpness[3];

        FeaturesSum[0] += fabs(energy / sumEnergyMax);
        FeaturesSum[1] += fabs((min + max) / 2);
        FeaturesSum[2] += fabs(delta_positive);
        FeaturesSum[3] += fabs(delta_negative);
        FeaturesSum[4] += fabs(dominateFirstValue);
        FeaturesSum[5] += fabs(dominateSecondValue);
        FeaturesSum[6] += (dominateFirstIndex);
        FeaturesSum[7] += (dominateSecondIndex);
        FeaturesSum[8] += fabs(sharpness[0]);
        FeaturesSum[9] += fabs(sharpness[1]);
        FeaturesSum[10] += fabs(sharpness[2]);
        FeaturesSum[11] += fabs(sharpness[3]);
    }

    for (int i = 0; i < 12; i++) {
        output[offset + 240 + i] = FeaturesSum[i];
    }

    output[offset + 252] = volume;
    output[offset + 253] = ribsH;
    output[offset + 254] = ribsV;
    output[offset + 255] = ribsV / (ribsH + 1);

    // диапазон    описание                                         значения в песне Beast In Black

    // от  0 до 19 энергия (громкость?)                             0-1
    // от 20 до 39 разброс                                          0-240
    // от 40 до 59 дельта усиления                                  0-3900
    // от 60 до 79 дельта ослабления                                0-4000
    // от 80 до 99 энергия 1-й доминирующей частоты                 0-4000
    // от 100 энергия 2-й доминирующей частоты                      0-3800
    // от 120 индекс первой доминирующей                            0-7
    // от 140 индекс второй доминирующей                            0-18
    // от 160 резкость волны по производной                         0-1362
    // от 180 альт. резкость волны по производной                   0-1362
    // от 200 резкость волны по второй производной                  0-2568
    // от 220 альт. резкость волны по второй производной            0-13000
    // от 240 до 251 суммы фич по всем полосам                      дофига
    // 252 - обобщённая громкость                                   0-255
    // 253 - число горизонтальных рёбер (резкие перепады)           0-2222
    // 254 - число вертикальных рёбер (резкие перепады)             0-3923
    // 255 - отношение числа вертикальных к (горизонтальным + 1)    0-15.8

    // На практике почти всегда нули: 140-145, 120-123, 100-105 (низкие частоты менее разнообразны)
}