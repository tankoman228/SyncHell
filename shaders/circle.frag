#version 330

// --- SYNC HELL BASIC BG SHADER PARAMETERS --- (если редактировать, то у всех сразу)
uniform sampler2D previousTexture; 
uniform float spectrum[256]; // от 0 до 255 (нормализованные фичи звука)      
uniform vec2 resolution;
uniform float deltaTime;
uniform float time;


void main() { 
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    vec2 texel = 1.0 / resolution;

    vec2 centeredUV = uv * 2.0 - 1.0;
    centeredUV.x *= resolution.x / resolution.y;

    float angleRad = atan(centeredUV.y, centeredUV.x) + time * 0.03;
    float angleDeg = degrees(angleRad);
    if (angleDeg < 0.0) angleDeg += 360.0;

    vec2 center = resolution.xy * 0.5;
    float distPx = distance(gl_FragCoord.xy, center);

    int temp = int(angleDeg * 512.0 / 360.0) % 512;
    int feature = (temp < 256) ? temp : (511 - temp); 

    if (true) {
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
            blended.r = 0;

            gl_FragColor = vec4(blended, 1.0);
        } else {
            // Если вышли за границы - используем оригинальный цвет
            vec3 originalColor = texture(previousTexture, uv).rgb;
            originalColor.b = max(originalColor.b, originalColor.r);
            gl_FragColor = vec4(originalColor, 1.0);
        }
    }


    if ( // Спавн круга визуализатора
        fract(angleDeg) > 0.2 && 
        distPx > 300.0 && 
        distPx < spectrum[feature] * spectrum[feature] / 512.0 + 300.0) 
    {
         gl_FragColor = mix(vec4(1.0, 1.0, 1.0, 1.0), texture2D(previousTexture, uv), 0.6);
    }
}