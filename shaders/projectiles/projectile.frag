#version 330 core
// Обязательно core, SFML съест

uniform sampler2D backgroundTexture;
uniform vec4 projectileColor;
uniform vec2 screenSize; // <-- Добавляем размер экрана

out vec4 FragColor; // <-- SFML в 330 core требует явный out

void main()
{
    // gl_FragCoord.xy — это координаты пикселя на экране (от 0 до width/height)
    // Переводим их в UV (от 0 до 1) для текстуры фона
    vec2 screenUV = gl_FragCoord.xy / screenSize;
    
    // Берём цвет фона в ЭТОЙ точке экрана
    vec4 bg = texture(backgroundTexture, screenUV);
    
    // Инвертируем
    vec4 inverted = vec4(1.0 - bg.rgb, bg.a);
    
    // Смешиваем с цветом пули
    FragColor = mix(projectileColor, inverted, 0.9);
}