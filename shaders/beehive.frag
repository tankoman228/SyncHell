#version 330

// --- SYNC HELL BASIC BG SHADER PARAMETERS --- (если редактировать, то у всех сразу)
uniform sampler2D previousTexture; 
uniform float spectrum[256];      
uniform vec2 resolution;
uniform float deltaTime;
uniform float time;

const vec2 r = vec2(1.0, 1.7320508); // Отношение сторон в правильном шестиугольнике (1, sqrt(3))
const vec2 h = r * 0.5;

void main() {
    vec2 uv = gl_FragCoord.xy / resolution.y;
    float scale = 22.0; 
    vec2 p = uv * scale;
    float aspect = resolution.x / resolution.y;
    float gridWidth = floor((aspect * scale) / r.x); 

    // 1. Разбиваем пространство на виртуальные ячейки
    vec2 a = mod(p, r) - h;
    vec2 b = mod(p - h, r) - h;
    
    // 2. Выбираем ближайший центр
    vec2 g = dot(a, a) < dot(b, b) ? a : b;
    vec2 id = p - g; 

    int col = int(floor(id.x / r.x + 0.5));
    int row = int(floor(id.y / r.y + 0.5));
    int flatIndex = col + row * int(gridWidth);

     // 3. Интенсивность
    float cellHash = fract(sin(dot(floor(id * 10.0), vec2(12.9898, 7.233))) * 43758.5453);
    int index = int(cellHash * 255.0);
    float intensity = max(spectrum[flatIndex % 256] - 160.0, 60.0) / (256.0 - 160.0); // Возвращено как было

    // 4. Расчет маски соты
    vec2 absG = abs(g);
    float d = max(absG.x, dot(absG, normalize(r))); 
    float mask = smoothstep(0.5, 0.48, d); 
    
    // 5. Отрисовка с вариацией оттенка и градиентом
    // Смещение тона на основе индекса
    vec3 baseGold = vec3(1.0, 0.8, 0.2);
    float hueShift = fract(float(index) * 0.05); 
    vec3 cellColor = mix(baseGold, vec3(1.0, 0.4, 0.1), hueShift);

    // Внутренний мягкий градиент (от центра к краям соты)
    float grad = 1.0 - d * 0.5;
    vec3 finalRGB = cellColor * mask * intensity * grad;
    
    // Диффузия
    vec4 prev = texture(previousTexture, gl_FragCoord.xy / resolution.xy);    
    gl_FragColor = mix(vec4(finalRGB, 1.0), prev, 0.94);
}
