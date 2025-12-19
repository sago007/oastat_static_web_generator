#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>
#include <cppdb/frontend.h>

// means of death
typedef enum {
	MOD_UNKNOWN,
	MOD_SHOTGUN,
	MOD_GAUNTLET,
	MOD_MACHINEGUN,
	MOD_GRENADE,
	MOD_GRENADE_SPLASH,
	MOD_ROCKET,
	MOD_ROCKET_SPLASH,
	MOD_PLASMA,
	MOD_PLASMA_SPLASH,
	MOD_RAILGUN,
	MOD_LIGHTNING,
	MOD_BFG,
	MOD_BFG_SPLASH,
	MOD_WATER,
	MOD_SLIME,
	MOD_LAVA,
	MOD_CRUSH,
	MOD_TELEFRAG,
	MOD_FALLING,
	MOD_SUICIDE,
	MOD_TARGET_LASER,
	MOD_TRIGGER_HURT,
	MOD_NAIL,
	MOD_CHAINGUN,
	MOD_PROXIMITY_MINE,
	MOD_KAMIKAZE,
	MOD_JUICED,
	MOD_GRAPPLE
} meansOfDeath_t;

#define AWARD_IMPRESSIVE 2
#define AWARD_EXCELLENT 1
#define AWARD_GAUNTLET 0
#define AWARD_DEFENCE 3
#define AWARD_CAPTURE 4
#define AWARD_ASSIST 5


struct MapInfo {
	int times_played = {};
	tm last_played = {};
};


struct OastatPlayer {
	int playerid = 0;
	std::string nickname;
	tm lastseen;
	std::string isBot = "n";
	std::string model;
	std::string headmodel;
};

std::string timestamp_now_as_string(cppdb::session& database);

#endif // COMMON_HPP
