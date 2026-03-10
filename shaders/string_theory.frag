#version 330

// --- SYNC HELL BASIC BG SHADER PARAMETERS ---
uniform sampler2D previousTexture; 
uniform float spectrum[256];      
uniform vec2 resolution;
uniform float deltaTime;
uniform float time;

// для шумоподавления
float denoise(float value) {
    return max(value - 190.0, 0.0) / 512.0; 
}

void main() {
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    vec3 color = vec3(0.0);
    
    // Параметры сетки струн
    int numStrings = 16;
    int paramsPerString = 16;
    float thickness = 0.005; // Толщина линии

    for (int i = 0; i < numStrings; i++) {
        // Вычисляем базовую высоту струны (распределяем по вертикали)
        float baseY = (float(i) + 0.5) / float(numStrings);
        
        // Считаем сумму синусоид на основе 16 параметров спектра
        float wave = 0.0;
        for (int j = 0; j < paramsPerString; j++) {
            int index = i * paramsPerString + j;
            float amp = denoise(spectrum[index]);
            float x = uv.x * ((j * 5.0) + 1.0) + j * -2.0;

            // Сумма синусоид: частота растет с j, фаза зависит от времени
            wave += sin(x + time * -1.5 * sqrt(j)) * amp * 0.013 * sqrt(sqrt(j + 1.0));
        }

        // Вычисляем расстояние от текущего пикселя до кривой струны
        float dist = abs(uv.y - (baseY + wave));
        
        // Рисуем струну (мягкое сглаживание)
        float brightness = smoothstep(thickness, 0.0, dist);
        
        // Добавляем немного цвета (градиент от индекса струны)
        color += brightness * vec3(0.5 + 0.5 * cos(time + float(i) * 0.2), 0.8, 1.0);
    }

    // Смешивание с предыдущим кадром (Motion Blur / Anti-flicker)
    vec4 prevColor = texture(previousTexture, uv);
    vec3 finalColor = mix(color, prevColor.rgb, 0.3);

    gl_FragColor = vec4(finalColor, 1.0);
}