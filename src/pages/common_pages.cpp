#include "pages.hpp"
#include "../common/common.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

// Global data storage
static std::map<int, int> kills_by_player;
static std::map<int, int> player_deaths;
static std::map<int, std::map<int, int>> player_awards;
static std::map<int, std::map<int, int>> player_weapon_kills;
static std::map<std::string, MapInfo> map_infos;
static std::set<int> generated_games;
static std::set<std::string> generated_maps;
static std::map<int, OastatPlayer> players;

// Accessors for global data
std::map<int, int>& get_kills_by_player() { return kills_by_player; }
std::map<int, int>& get_player_deaths() { return player_deaths; }
std::map<int, std::map<int, int>>& get_player_awards() { return player_awards; }
std::map<int, std::map<int, int>>& get_player_weapon_kills() { return player_weapon_kills; }
std::map<std::string, MapInfo>& get_map_infos() { return map_infos; }
std::set<int>& get_generated_games() { return generated_games; }
std::set<std::string>& get_generated_maps() { return generated_maps; }

// Utility functions
std::string ZeroPadNumber(int num, int size = 2)
{
	std::ostringstream ss;
	ss.clear();
	ss << std::setw(size) << std::setfill('0') << num;
	return ss.str();
}

std::string getTimeStamp(const tm& _datetime)
{
	std::string s = ZeroPadNumber(_datetime.tm_year+1900,4) + "-" + ZeroPadNumber(_datetime.tm_mon+1) + "-"
		+ ZeroPadNumber(_datetime.tm_mday) + " " + ZeroPadNumber(_datetime.tm_hour) + ":"
		+ ZeroPadNumber(_datetime.tm_min) + ":" + ZeroPadNumber(_datetime.tm_sec);
	return s;
}

std::string getWeaponName(int modtype) {
	switch(modtype) {
		case MOD_SHOTGUN: return "Shotgun";
		case MOD_GAUNTLET: return "Gauntlet";
		case MOD_MACHINEGUN: return "Machinegun";
		case MOD_GRENADE:
		case MOD_GRENADE_SPLASH: return "Grenade";
		case MOD_ROCKET:
		case MOD_ROCKET_SPLASH: return "Rocket";
		case MOD_PLASMA:
		case MOD_PLASMA_SPLASH: return "Plasma";
		case MOD_RAILGUN: return "Railgun";
		case MOD_LIGHTNING: return "Lightning";
		case MOD_BFG:
		case MOD_BFG_SPLASH: return "BFG";
		case MOD_NAIL: return "Nailgun";
		case MOD_CHAINGUN: return "Chaingun";
		case MOD_TELEFRAG: return "Telefrag";
		case MOD_FALLING: return "Falling";
		default: return "Other";
	}
}

// Player management functions
OastatPlayer getPlayer(cppdb::session& database, int playerid) {
	OastatPlayer ret;
	if (players.count(playerid)) {
		ret = players[playerid];
		return ret;
	}
	cppdb::statement st = database.prepare("select playerid, lastseen, isbot, model, headmodel, nickname from oastat.oastat_players where playerid = ?");
	st.bind(1, playerid);
	cppdb::result res = st.query();
	if (res.next()) {
		res >> ret.playerid >> ret.lastseen >> ret.isBot >> ret.model >> ret.headmodel >> ret.nickname;
		players[playerid] = ret;
	}
	return ret;
}

void populate_player_deaths(cppdb::session& database, const std::vector<int>& player_ids) {
	std::string sql = "select count(0) from oastat.oastat_kills k where k.attacker <> k.target and target = ?";
	cppdb::statement st = database.prepare(sql);
	for (int player_id : player_ids) {
		st.bind(1, player_id);
		int deaths;
		cppdb::result res = st.query();
		if (res.next()) {
			res >> deaths;
			player_deaths[player_id] = deaths;
		}
	}
}

void populate_player_awards(cppdb::session& database, const std::vector<int>& player_ids) {
	std::string sql = "select award, count(0) c from oastat.oastat_awards where player = ? group by  player, award";
	cppdb::statement st = database.prepare(sql);
	for (int player_id : player_ids) {
		st.bind(1, player_id);
		int award;
		int count;
		cppdb::result res = st.query();
		while (res.next()) {
			res >> award >> count;
			player_awards[player_id][award] = count;
		}
	}
}

void populate_player_kills(cppdb::session& database, const std::vector<int>& player_ids) {
	std::string sql = "select modtype, count(0) c from oastat.oastat_kills where attacker <> target and attacker = ? group by modtype";
	cppdb::statement st = database.prepare(sql);
	for (int player_id : player_ids) {
		st.bind(1, player_id);
		int weapon;
		int count;
		cppdb::result res = st.query();
		while (res.next()) {
			res >> weapon >> count;
			player_weapon_kills[player_id][weapon] = count;
		}
	}
}

std::vector<int> get_top_killers(cppdb::session& database, int count) {
	std::vector<int> ret;
	std::string sql = "select p.playerid, p.isbot b, count(0) c from oastat.oastat_players p, oastat.oastat_kills k where p.playerid = k.attacker and k.attacker <> k.target and p.playerid <> 0 group by p.playerid, p.isbot order by b, c desc limit ?";
	cppdb::statement st = database.prepare(sql);
	st.bind(1, count);
	cppdb::result res = st.query();
	while(res.next()) {
		int player_id;
		int kills;
		std::string isBot;
		res >> player_id >> isBot >> kills;
		kills_by_player[player_id] = kills;
		ret.push_back(player_id);
	}
	return ret;
}

// Map management functions
std::vector<std::string> get_most_played_maps(cppdb::session& database) {
	std::vector<std::string> ret;
	std::string sql = "select mapname, count(0) c, max(time) last_game from oastat.oastat_games group by mapname order by c desc";
	cppdb::statement st = database.prepare(sql);
	cppdb::result res = st.query();
	while(res.next()) {
		std::string mapname;
		MapInfo info;
		res >> mapname >> info.times_played >> info.last_played;
		map_infos[mapname] = info;
		ret.push_back(mapname);
	}
	return ret;
}

