#version 330

// --- SYNC HELL BASIC BG SHADER PARAMETERS --- (если редактировать, то у всех сразу)
uniform sampler2D previousTexture; 
uniform float spectrum[256]; // от 0 до 255 (нормализованные фичи звука)      
uniform vec2 resolution;
uniform float deltaTime;
uniform float time;

float MULTIPLEXER_ITEM(int i) {
    float s = spectrum[i];
    if (s <= 230.0f) return 0.1f;
    
    // Линейный рост от 0.1 до ? в диапазоне s от ? до 255
    return 0.1f + (s - 230.0f) * (1.2f - 0.1f) / (255.0f - 230.0f);
}

float MULTIPLEXER_SUM(int step_) { 

    // begin от 0 до 8. Задача: пройтись
    float s = 0; float count = 0;

    int end = min(256, int((step_ + 1.0) * 40));

    for (int i = step_ * 29; i < end; i++) {
        s += MULTIPLEXER_ITEM(i); 
        count += 1;
    }

    return s / count / 6.0 * float(step_ + 2);
}

float MULTIPLEXER_CACHE[9];
float MULTIPLEXER(int i) {
    return MULTIPLEXER_CACHE[i];
}


void mainImage(out vec4 fragColor, in vec2 fragCoord) {

    float zoom = 0.9; 

    vec2 uv = (fragCoord - 0.5 * resolution.xy) / resolution.y * zoom;
    uv += vec4(-0.1, 0.1, 0.0, 0.0).xy;  // Сдвиг, чтобы рассмотреть детали спиралей

    vec2 c = vec2(-0.8, 0.156);  // Julia Constant

    vec2 z = uv; 
    float iter = 0.0;
    const float max_iter = 120.0; // Чем выше, тем четче края

    vec2 c_mod = c + vec2(cos(time * 0.25) * 2.0, sin(time * 0.5) * 2.0) * 0.01;

    // 3. ЦИКЛ С ПРОВЕРКОЙ ГРАНИЦЫ
    for(float i = 0.0; i < max_iter; i++) {
  
        z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c_mod;

        if(length(z) > 4.0) break;
        iter++;
    }

    // 4. СТАТИЧНАЯ КИСЛОТНАЯ РАСКРАСКА (No Time, No Black)
    float dist = length(z);
    
    // Плавный индекс (двойной логарифм)
    float smooth_iter = iter - log2(log2(max(dist, 1.05))) + 4.0;

    vec3 finalCol;

    // Используем дробную часть координат Z для создания текстуры "Ловушки Орбиты"
    vec2 gridUV = fract(z * 8.0); 
    float pattern = sin(gridUV.x * 6.28) * cos(gridUV.y * 6.28);
    pattern = step(0.0, pattern); // Делаем четкие "шашечки" или жесткие переходы


    vec3 rainbow = 0.5 + 0.5 * cos(smooth_iter * 2.5 + vec3(0.0, 2.1 * MULTIPLEXER(0), 4.2  * MULTIPLEXER(1)));
    float grain = fract(sin(dot(z, vec2(12.9898 * MULTIPLEXER(2), 78.233 * MULTIPLEXER(3)))) * 43758.5453 * MULTIPLEXER(4));
    finalCol = mix(rainbow, vec3(pattern), 0.4);
    finalCol += 0.2 * cos(smooth_iter * 20.0 + vec3(1 * MULTIPLEXER(5), 3 * MULTIPLEXER(6), 5 * MULTIPLEXER(7)));

    if(iter >= max_iter - 1.0) {
        finalCol *= MULTIPLEXER(8);
    } 

    // Фиксируем яркость
    fragColor = vec4(clamp(finalCol, 0.0, 1.0), 1.0);
}

void main() {

    for (int i = 0; i < 9; i++) {
        MULTIPLEXER_CACHE[i] = MULTIPLEXER_SUM(i);
    }

    vec4 color = vec4(0,0,0,1);

    vec2 uv = gl_FragCoord.xy / resolution.xy;
    vec4 current = texture2D(previousTexture, uv);

    mainImage(color, gl_FragCoord.xy);
    
    gl_FragColor = mix(color, current, 0.2);
}