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

    // 3. Спавн "огня" из спектра
    float h = uv.x * 4.0;
    float t = time * 0.05;

    float flameH = 0.8;

    if (uv.y < flameH) {

        int index = (int(uv.x * 255.0) + int(time * 4)) % 255; 
        //index = clamp(index, 0, 255);
        
        float fadeFactor = 1.0 - (uv.y / flameH);
        float energy = spectrum[index] * fadeFactor; 

        float r = 0.0;
        float g = 0.0;
        float b = 0.0;

        r = clamp((energy - 60.0) / (255.0 - 60.0), 0.0, 1.0);  // громкий = красный
        g = clamp((energy - 190.0) / (255.0 - 190.0), 0.0, 1.0);  // ещё громче белый
        b = energy;  // тихий = синий  

        finalColor.r = r;
        finalColor.g = g;
        finalColor.b = g;
    }

    if (uv.y > flameH + 1.0) {
       if (finalColor.r > 0.05) { finalColor.r -= 0.05; }
       if (finalColor.g > 0.05) { finalColor.g -= 0.05; }
    }

    gl_FragColor = finalColor;
}