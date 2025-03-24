#ifndef CARD_EFFECT_H
#define CARD_EFFECT_H

#include <string>

enum class CardEffectType {
	None,
	Armor,
	Heal,
	MultiStrike,
	Weaken,
	Poison,
	Thorns,
};

struct CardEffect {
	CardEffectType type;
	int value;
	int count; 
	CardEffect(CardEffectType t = CardEffectType::None, int v = 0, int c = 1) : type(t), value(v), count(c) {}
};

#endif