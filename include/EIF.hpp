#pragma once

#include <vector>

// EIF - Ear Imitation Features
// Имитация человеческого уха на звуке (ну, уже не особо уха, значения менее сырые)

#ifndef EIF_DEF
#define EIF_DEF 1

namespace EIF {

	/// <summary>
	/// маятниковые рецепторы
	/// </summary>
	extern float OUT_Etalon[256];

	/// <summary>
	/// внутренние рецепторы
	/// </summary>
	extern float OUT_Deep[256];


	/// <summary>
	/// В переменные OUT выведет энергию с рецепторов
	/// 
	/// Важно, чтобы сырая волна имела разрешение 
	/// 44.1 КГЦ, т.е. секунда звука это 44 100 элементов массива
	/// или 44 100 т.е. 176 КБ на секунду в ОЗУ
	/// </summary>
	void Cycle(std::vector<float>* waveRaw, int waveIndexStart, int waveIndexEnd);

	/// <summary>
	/// Инициализация всех весов для рецепторов
	/// </summary>
	void InitParams();

	void ClearOutput();

	/// <summary>
	/// Генерирует синусоиду заданной длины для конкретного номера рецептора.
	/// </summary>
	/// <param name="length">Количество семплов (44100 = 1 секунда)</param>
	/// <param name="noteIndex">Номер рецептора (0-255)</param>
	std::vector<float> GetWaveForNote(int length, int noteIndex);
}

#endif