#version 330

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
    float t = time * 3.0;

    float flameH =
        ( sin(5.0 * h + t) + sin(2.0 * h + 2.0 * t) + sin(4.0 * h + t / 2.0)) / 3.0;

    if (uv.y < flameH) {

        int index = (int(uv.x * 255.0) + int(time * 4)) % 255; 
        //index = clamp(index, 0, 255);
        
        float fadeFactor = 1.0 - (uv.y / flameH);
        float energy = spectrum[index] * fadeFactor; 

        float r = 0.0;
        float g = 0.0;
        float b = 0.0;

        r = clamp(energy / 20.0, 0.0, 1.0);
        g = clamp((energy - 20.0) / (255.0 - 20.0), 0.0, 1.0);    

        finalColor.r = max(current.r, r);
        finalColor.g = max(current.g, g);
    }

    if (uv.y > flameH + 1.5) {
       if (finalColor.r > 0.02) { finalColor.r -= 0.02; }
       if (finalColor.g > 0.02) { finalColor.g -= 0.02; }
    }

    gl_FragColor = finalColor;
}