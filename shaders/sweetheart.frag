#version 330

// --- SYNC HELL BASIC BG SHADER PARAMETERS --- (если редактировать, то у всех сразу)
uniform sampler2D previousTexture; 
uniform float spectrum[256];    
uniform float spectrumSum[256]; // <---- TODO: использоваь
uniform vec2 resolution;
uniform float deltaTime;
uniform float time;

void main() {
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    
    vec2 aspect = vec2(resolution.x / resolution.y, 1.0);
    vec2 pos = uv * 2.0 - 1.0;
    pos *= aspect;
    
    float scale = 0.6;
    float x = pos.x / scale;
    float y = pos.y / scale * 1.1 + 0.5;
    
    // Исправленная формула сердца с учетом знака x
    float x2 = x * x;
    
    // Безопасное вычисление x^(2/3) для отрицательных x
    float x23 = x >= 0.0 ? pow(x, 2.0 / 3.0) : pow(-x, 2.0 / 3.0);
    
    float heart = x2 + pow(y - x23, 2);
    
    // Небольшое сглаживание для лучшего результата
    float heartMask = 1.0 - smoothstep(0.98, 1.02, heart);

    if (heart <= 1.0) {

        // Расстояние от центра
        float radius = length(pos);

        // Угол с учетом времени
        float angle = atan(pos.y, pos.x);

        // Спираль: угол + радиус, умноженный на коэффициент (чем дальше от центра, тем больше смещение)
        // Коэффициент radius задает плотность витков спирали
        float spiral = angle + radius * 25.0 + time * 0.7;

        int feature = int(spiral / (2.0 * 3.14159) * 256.0) % 256;

        vec3 heartColor = mix(vec3(1.0), vec3(1.0, 0.4, 0.7), clamp((spectrum[feature] - 170.0) / 85.0, 0.0, 1.0));
        heartColor = texture(previousTexture, uv).rgb * 0.8 + heartColor * 0.2;
        
        gl_FragColor = vec4(heartColor, 1);
    }
    else {
        // Масштабирование прошлого кадра с мягким смешиванием
        float scale = 1.0 + deltaTime * 0.7; // коэффициент увеличения
        vec2 center = vec2(0.5, 0.5); // центр масштабирования

        // Вычисляем UV координаты с масштабированием от центра
        vec2 scaledUv = center + (uv - center) / scale;

        // Проверяем, не вышли ли за границы текстуры
        if (scaledUv.x >= 0.0 && scaledUv.x <= 1.0 && 
            scaledUv.y >= 0.0 && scaledUv.y <= 1.0) {
            
            // Получаем цвет из масштабированной области
            vec3 scaledColor = texture(previousTexture, scaledUv).rgb;
            
            // Получаем оригинальный цвет
            vec3 originalColor = texture(previousTexture, uv).rgb;
            
            // Смешиваем масштабированное с оригиналом для мягкости
            float mixFactor = 0.6; // чем выше, тем больше масштабированного
            vec3 blended = mix(originalColor, scaledColor, mixFactor);
            
            // Усиливаем синий канал
            blended.b = max(blended.b, blended.r);
            blended.g = min(blended.g, 0.8);

            gl_FragColor = vec4(blended, 1.0);
        } else {
            // Если вышли за границы - используем оригинальный цвет
            vec3 originalColor = texture(previousTexture, uv).rgb;
            originalColor.b = max(originalColor.b, originalColor.r);
            gl_FragColor = vec4(originalColor, 1.0);
        }
    }
}