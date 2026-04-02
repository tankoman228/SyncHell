#version 120

uniform sampler2D mask;

// цветовая гамма уровня
uniform vec4 ColorMain;         // доб. по голубому из маски
uniform vec4 ColorSecondary;    // доб. по красному из маски
uniform vec4 ColorAdditional;   // доб. по зелёному из маски

uniform vec4 projectileColor;   // цветокорр из самой частицы (её личный цвет, если маска чёрная, только он)
uniform float spectroValue;     // значение ноты от 0 до 1, для домножения при добавлении

void main()
{
    vec4 maskColor = texture2D(mask, gl_TexCoord[0].xy / 4);

    // берём текущий
    vec4 maskResult = vec4(0);

    // Итого маска должна покраситься в нужный цвет по вот этому вот
    maskResult += ColorAdditional * maskColor.g * spectroValue;
    maskResult += ColorSecondary  * maskColor.r * spectroValue;
    maskResult += ColorMain       * maskColor.b * spectroValue;

    // При тишине или чёрной маске - цвет оригинал
    gl_FragColor = clamp(projectileColor + maskResult, 0.0, 1.0); 
}