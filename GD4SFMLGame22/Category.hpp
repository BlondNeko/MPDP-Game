#pragma once
//SceneNode category used to dispatch commands
namespace Category
{
	enum Type
	{
		kNone = 0,
		kScene = 1 << 0,
		kPlayerAircraft = 1 << 1,
		kAlliedAircraft = 1 << 2,
		kEnemyAircraft = 1 << 3,
		kPickup = 1 << 4,
		kAlliedProjectile = 1 << 5,
		kEnemyProjectile = 1 << 6,
		kParticleSystem = 1 << 7,
		kSoundEffect = 1 << 8,
		kObstacle = 1 << 9,
		kPlayer1 = 1 << 10,
		kPlayer2 = 1 << 11,

		kAircraft = kPlayerAircraft | kAlliedAircraft | kEnemyAircraft | kPlayer1 | kPlayer2,
		kProjectile = kAlliedProjectile | kEnemyProjectile,
	};
}