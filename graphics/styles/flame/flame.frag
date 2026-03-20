#version 330

// --- SYNC HELL BASIC BG SHADER PARAMETERS --- (если редактировать, то у всех сразу)
uniform sampler2D previousTexture; 
uniform float spectrum[256]; // от 0 до 255 (нормализованные фичи звука)     
uniform float spectrumSum[256]; // <---- TODO: использоваь
uniform vec2 resolution;
uniform float deltaTime;
uniform float time;


void main() {
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    vec2 texel = 1.0 / resolution;

    // Усреднение по соседям. Медленное изменение
    vec4 current = texture2D(previousTexture, uv);

    vec4 top     = texture2D(previousTexture, uv + vec2(0.0, texel.y));
    vec4 down    = texture2D(previousTexture, uv + vec2(0.0, -texel.y));
    vec4 left    = texture2D(previousTexture, uv + vec2(-texel.x, -texel.y));
    vec4 right   = texture2D(previousTexture, uv + vec2(texel.x, -texel.y));
    
    float wind = (sin((time + uv.y) / 0.1) + 1) * 10.0; // от 0 до 20
    vec4 blurred = 
        (
            down * 20.0                           // вес 20
            + left * wind + right * (20.0 - wind) // вес до 20        
            + current                             // вес 1     
            + top * 0.03                          // вес 0.03     
        ) 
        / 41.04;
    vec4 finalColor = blurred;


    float flameH = 0.95;
    if (uv.y < flameH) {

        // 1. Плавная выборка из спектра по горизонтали
        float spectrumPos = uv.x * 256.0;
        
        int index1 = int(floor(spectrumPos)) % 256;
        int index2 = int(ceil(spectrumPos)) % 256;

        // Плавная интерполяция между соседними значениями спектра
        float spectrumValue = mix(spectrum[index1], spectrum[index2], fract(spectrumPos)); // 0..255

        // Шумоподавление
        spectrumValue = (spectrumValue - 80.0) / 175.0 * 255.0;

        // Нормализуем энергию (0..1)
        float energy = spectrumValue / 255.0;
        
        // Вычисляем высоту пламени для этой энергии
        // flameH - максимальная высота (при energy = 1.0)
        flameH = mix(energy * flameH, flameH, 0.9);
        
        // Проверяем, попадает ли текущий пиксель в область пламени
        // uv.y отсчитывается от 0 внизу до flameH вверху
        if (uv.y < flameH) {
            // Пересчитываем t относительно высоты этого языка
            float t = 1.0 - uv.y / flameH; // 1 внизу, 0 на вершине этого языка
            
            // Форма языка (можно регулировать степень)
            t = pow(t, 2.0); // Более низкие степени дают более пологие языки
            
            // Яркость с учётом энергии
            float brightness = energy * t;
            
            // Цвет на основе яркости
            float r = clamp(brightness * 5.0, 0.0, 1.0);
            float g = clamp((brightness * 255.0 - 20.0) / (255.0 - 20.0), 0.0, 1.0);
            
            finalColor.r = mix(max(finalColor.r, r), finalColor.r, 0.5);
            finalColor.g = mix(max(finalColor.g, g), finalColor.g, 0.5);           
        }
    }
        
    if (finalColor.r > 1.0 / 255.0) { finalColor.r -= 1.0 / 255.0; }
    if (finalColor.g > 1.0 / 255.0) { finalColor.g -= 1.0 / 255.0; } 

    finalColor.b = finalColor.b * 0.99;

    gl_FragColor = finalColor;
}