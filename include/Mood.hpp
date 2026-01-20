class Mood {

public:

	/*! инициализация, вызываемая после конструктора */
	virtual void Init() {};

	/*! вызывать каждый игровой цикл */
	virtual void Cycle(float dt) {};

	/*! при вызове запускает затухание */
	virtual void StartVanish() {};

	virtual ~Mood() = default;
};

// Если в cpp не определили, макрос сюда не совать!
#define EXT_MOOD : public Mood { void Init() override; void Cycle(float dt) override; void StartVanish() override; };

namespace MoodVariants {

	/*! создаст объект по индексу типа */
	Mood* MakeNewMood(int index);

	// 67 классов

	class BlueSky EXT_MOOD
	class NightSky : public Mood {};
	class DaySky : public Mood {};
	class SnowSky : public Mood {};
	class Snowflake : public Mood {};
	class Swirl : public Mood {};
	class Antiswirl : public Mood {};
	class Sunset : public Mood {};
	class Fog : public Mood {};
	class OceanSchematic : public Mood {};
	class Bubbles : public Mood {};
	class BubblesUnderwater : public Mood {};
	class Galaxy : public Mood {};
	class MilkiWay : public Mood {};
	class BlackHole : public Mood {};
	class Supernova : public Mood {};
	class SunSystemSchematic : public Mood {};
	class Asteroids : public Mood {};
	class EarthOrbit : public Mood {};
	class Moon : public Mood {};
	class WindowsXP : public Mood {};
	class Linux : public Mood {};
	class Lamas : public Mood {};
	class Fish : public Mood {};
	class Clouds : public Mood {};
	class StarsSchematic : public Mood {};
	class Candies : public Mood {};
	class WaterAtoms : public Mood {};
	class DiamondAtoms : public Mood {};
	class SovietUnion : public Mood {};
	class Sword : public Mood {};
	class BowAndArrows : public Mood {};
	class Spears : public Mood {};
	class MagicGravProjectiles : public Mood {};
	class MagicGravPlayer : public Mood {};
	class MagicFireballs : public Mood {};
	class MagicProjectileScaler : public Mood {};
	class MagicBarrierScaler : public Mood {};
	class LazerShowStatic : public Mood {};
	class LazerShowMovement : public Mood {};
	class LazerShowRotate : public Mood {};
	class LazerShowRotateMove : public Mood {};
	class BallLightnings : public Mood {};
	class Tesla5balls : public Mood {};
	class TeslaMany : public Mood {};
	class Fire : public Mood {};
	class FireBottom : public Mood {};
	class FireWithFireballs : public Mood {};
	class FireWithScreenDestroy : public Mood {};
	class WaveGraph1 : public Mood {};
	class WaveGraph2 : public Mood {};
	class WaveGraph3 : public Mood {};
	class WaveGraph4 : public Mood {};
	class SpiralForward : public Mood {};
	class SpiralBackward : public Mood {};
	class TileVisualizer : public Mood {};
	class LiquidWaves : public Mood {};
	class InYan : public Mood {};
	class Furmark : public Mood {};
	class FallingDownCalm : public Mood {};
	class FallingDownNormal : public Mood {};
	class FallingDownHard : public Mood {};
	class PoisonGas : public Mood {};
	class SmokeLight : public Mood {};
	class SmokeMedium : public Mood {};
	class SmokeDementors : public Mood {};
	class VHS : public Mood {};
}