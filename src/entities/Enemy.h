#ifndef ENEMY_H
#define ENEMY_H

#include <string>

struct Enemy {
	std::string name;
	int hp;
	int damage;
	int damageReduction;
	int weakenTurns;

	// abilities??

	Enemy(const std::string& n, int h, int d)
		: name(n), hp(h), damage(d), damageReduction(0), weakenTurns(0) {}
};

#endif