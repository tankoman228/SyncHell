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

    float angleRad = atan(centeredUV.y, centeredUV.x) + time * 0.14;
    float angleDeg = degrees(angleRad);
    if (angleDeg < 0.0) angleDeg += 360.0;

    vec2 center = resolution.xy * 0.5;
    float distPx = distance(gl_FragCoord.xy, center);

    int temp = int(angleDeg * 512.0 / 360.0) % 512;
    int feature = (temp < 256) ? temp : (511 - temp); 

    // Отдельный блок для размазывания previousTexture
     if (true) {
        // Параметры спирали
        float zoom = 0.8 * deltaTime;   // Скорость стягивания к центру
        float blueDecay = 0.95;

        float rotation = 
            0.004 * deltaTime * distPx
            * sqrt(256.0 - spectrum[feature]) / 16.0
            ; // Скорость вращения (в радианах)

        if (distPx <= 300.0) {
            zoom     *= -3.0;
            rotation *= -22.0;
            blueDecay = 0.99;

            if (distPx <= 60.0) {
                zoom *= 60.0 / (distPx + 0.0001);
            }
        }

        // Вычисляем вектор от центра к текущему пикселю
        vec2 dir = uv - 0.5;
        
        // Матрица поворота
        mat2 rotMat = mat2(cos(rotation), -sin(rotation), sin(rotation), cos(rotation));
        
        // Трансформируем координаты: немного уменьшаем масштаб и поворачиваем
        vec2 spiralUV = dir * (1.0 - zoom) * rotMat + 0.5;

        vec4 spr = texture2D(previousTexture, spiralUV);
        spr.r = spr.r;
        spr.g = 0;
        spr.b = spr.b * blueDecay;

        if (spr.r > 0.02 && spr.b < 0.98) {
            spr.r -= 0.02;
            spr.b += 0.02;
        }

        if (spr.b > 0.01 && spr.b < 0.5 && spr.g < 0.99) {
            spr.b -= 0.01;
            spr.r += 0.01;
        }

        // Смешиваем текущий пиксель с пикселем из "прошлого" по спирали
        gl_FragColor = mix(
            texture2D(previousTexture, uv),
            spr,
            0.3); 
    }


    if ( // Спавн круга визуализатора
        fract(angleDeg) > 0.3 && 
        distPx > 300.0 && 
        distPx < spectrum[feature] * spectrum[feature] / 512.0 + 300.0) 
    {
         gl_FragColor = mix(vec4(1.0, 1.0, 1.0, 1.0), texture2D(previousTexture, uv), 0.8);
    }
}