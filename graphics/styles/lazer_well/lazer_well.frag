#version 330

// --- SYNC HELL BASIC BG SHADER PARAMETERS ---
uniform sampler2D previousTexture;
uniform float spectrum[256]; // от 0 до 255
uniform float spectrumSum[256]; // <---- TODO: использоваь
uniform vec2 resolution;
uniform float deltaTime;
uniform float time;

void main() {
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    
    // Только затухание предыдущего кадра
    vec4 prevColor = texture2D(previousTexture, uv);
    vec4 finalColor = prevColor * 0.96;
    
    // Расчет спирали
    vec2 centerPixel = resolution * 0.5;
    vec2 dir = gl_FragCoord.xy - centerPixel;
    float dist = length(dir);
    float angle = atan(dir.y, dir.x);
    
    // Параметры спирали
    float spiralTightness = 90.0; // Расстояние между витками в пикселях
    float spiralThickness = 23.0; // Толщина линии
    
    // Добавляем вращение спирали с течением времени
    // Увеличиваем угол пропорционально времени для вращения
    float rotationSpeed = -0.7; // скорость вращения (оборотов в секунду)
    float rotatedAngle = angle + time * rotationSpeed * 2.0 * 3.14159;
    
    // Приводим угол к диапазону от 0 до 2pi
    float normAngle = rotatedAngle + 3.14159; // от 0 до 2pi
    normAngle = mod(normAngle, 2.0 * 3.14159);
    
    // Находим ближайший виток спирали
    // Для данного угла, радиус спирали должен быть: r = spiralTightness * (normAngle/(2pi) + n)
    float turns = floor(dist / spiralTightness); // количество полных оборотов
    
    // Вычисляем ожидаемый радиус для этого угла на разных витках
    float expectedR1 = spiralTightness * (normAngle/(2.0 * 3.14159) + turns);
    float expectedR2 = spiralTightness * (normAngle/(2.0 * 3.14159) + turns + 1.0);
    float expectedR3 = spiralTightness * (normAngle/(2.0 * 3.14159) + turns - 1.0);
    
    // Находим минимальное расстояние до любого витка
    float distToSpiral = min(abs(dist - expectedR1), 
                          min(abs(dist - expectedR2), abs(dist - expectedR3)));
    
    // Сглаженная интенсивность спирали
    float spiralIntensity = 1.0 - smoothstep(0.0, spiralThickness, distToSpiral);
    
    // Расчет затухания яркости к центру
    // От 300 до 500 пикселей - плавное затухание, до 300 пикселей - ноль
    float fadeDistance = 600.0; // расстояние, на котором начинается затухание
    float zeroDistance = 300.0;  // расстояние, на котором яркость становится нулевой
    
    float distanceFade = 1.0;
    if (dist < zeroDistance) {
        distanceFade = 0.0; // до 300 пикселей - ноль
    } else if (dist < fadeDistance) {
        // плавное затухание от 300 до 500 пикселей
        distanceFade = (dist - zeroDistance) / (fadeDistance - zeroDistance);
    }
    
    // Применяем затухание к интенсивности спирали
    spiralIntensity *= distanceFade;
    
    // Цвет от спектра
    float normAngleColor = (angle + 3.14159) / (2.0 * 3.14159);
    int section = (int(normAngleColor * 256.0) + int(dist*1.3)) % 256;
    float energy = spectrum[section] - 150.0;
    if (energy < 0.0) {
        gl_FragColor = finalColor;
    }
    else {
        float intensity = energy / 255.0;
        
        // Бирюзовый
        vec3 spiralColor = vec3(spectrum[section % 64] * intensity / 2024.0, intensity, intensity);
        
        // Добавляем спираль с учетом затухания
        finalColor.rgb += spiralColor * spiralIntensity * 2.0;
        
        gl_FragColor = clamp(finalColor, 0.0, 1.0);
    }
    
}