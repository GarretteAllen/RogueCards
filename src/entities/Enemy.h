#ifndef ENEMY_H
#define ENEMY_H

#include <string>

struct Enemy {
    std::string name;
    int hp;
    int damage;
    int damageReduction;
    int weakenTurns;
    int poisonDamage;
    int poisonTurns;
    int wetTurns;
    bool frozen;

    Enemy(const std::string& n, int h, int d)
        : name(n), hp(h), damage(d), damageReduction(0), weakenTurns(0),
        poisonDamage(0), poisonTurns(0), wetTurns(0), frozen(false) {}
};

#endif