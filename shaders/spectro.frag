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

    vec4 down       = texture2D(previousTexture, uv + vec2(0.0, -texel.y));
    vec4 finalColor = down;

    if (uv.y < 0.003) {

        int index = (int(uv.x * 256.0)) % 256; 
        //index = clamp(index, 0, 255);
        
        float energy = spectrum[index]; 

        float r = 0.0;
        float g = 0.0;
        float b = 0.0;

        r = clamp((energy - 100.0) / (255.0 - 100.0), 0.0, 1.0);  // громкий = красный
        g = clamp((energy - 250.0) / (255.0 - 250.0), 0.0, 1.0);  // ещё громче белый
        b = clamp(energy / 16.0, 0.0, 1.0);  // тихий = синий  

        // + тень, чтобы экран не забеляло
        finalColor.r = r / 1.0;
        finalColor.g = g / 1.0;
        finalColor.b = b / 1.0;
    }

    gl_FragColor = finalColor;
}