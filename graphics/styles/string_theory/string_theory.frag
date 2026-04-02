#version 330

// --- SYNC HELL BASIC BG SHADER PARAMETERS ---
uniform sampler2D previousTexture; 
uniform float spectrum[256];   
uniform float spectrumSum[256]; // <---- TODO: использоваь
uniform vec2 resolution;
uniform float deltaTime;
uniform float time;

void main() {
    vec2 uv = gl_FragCoord.xy / resolution.xy;
    
    // Предрасчёт констант
    const int numStrings = 16;
    const int paramsPerString = 16;
    const float thickness = 0.005;
    const float invNumStrings = 1.0 / 16.0; // Предрасчёт
    
    vec3 color = vec3(0.0);
    
    // Оптимизация: проходим только ближайшие струны
    float nearestY = uv.y * 16.0;
    int startIdx = max(0, int(nearestY) - 1);
    int endIdx = min(numStrings, startIdx + 3);
    
    for (int i = startIdx; i < endIdx; i++) {
        float baseY = (float(i) + 0.5) * invNumStrings;
        
        // Оптимизация: предрасчёт базового оффсета для индексов
        int baseIndex = i * paramsPerString;
        
        // Оптимизация: развернуть цикл по j (если paramsPerString фиксирован)
        float wave = 0.0;
        float x = uv.x;
        
        // Ручная развёртка для 16 параметров (частичный пример)
        for (int j = 0; j < paramsPerString; j++) {

            int idx = baseIndex + j;
            float amp = max(0.0, spectrum[idx] - 80.0) * 0.02124; 
            if (amp <= 0.001) continue;
            
            float phase = x * (1.0 + float(j)) * 9.0 + time * -0.35 * (4.0 + float(j));
            wave += sin(phase) * amp * 0.0011 * sin(spectrumSum[idx] * 30.0);
        }
        
        float dist = abs(uv.y - (baseY + wave));
        if (dist < thickness) {
            float brightness = 1.0 - (dist / thickness);
            color += brightness * vec3(0.5 + 0.5 * cos(time + float(i) * 0.2), 0.8, 1.0);
        }
    }
    
    // Оптимизация: меньше смешивания с предыдущим кадром
    vec4 prevColor = texture(previousTexture, uv);
    gl_FragColor = vec4(mix(color, prevColor.rgb, 0.2), 1.0);
}