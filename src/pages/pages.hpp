#ifndef PAGES_HPP
#define PAGES_HPP

#include <cppdb/frontend.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <ctime>
#include "../common/common.hpp"

// Struct definitions (that are specific to pages)
struct OastatGame {
	int gamenumber = 0;
	int gametype = 0;
	std::string mapname;
	tm time = {};
	std::string basegame;
	int second = 0;
	std::string servername;
};

struct ScorePoint {
	int playerid = 0;
	int second = 0;
	int score = 0;
};

// Player management
OastatPlayer getPlayer(cppdb::session& database, int playerid);
std::vector<int> get_top_killers(cppdb::session& database, int count);
void populate_player_deaths(cppdb::session& database, const std::vector<int>& player_ids);
void populate_player_awards(cppdb::session& database, const std::vector<int>& player_ids);
void populate_player_kills(cppdb::session& database, const std::vector<int>& player_ids);

// Map management
std::vector<std::string> get_most_played_maps(cppdb::session& database);
void getMapWeaponKills(cppdb::session& database, const std::string& mapname, std::map<int, int>& weapon_kills);
void getMapRecentGames(cppdb::session& database, const std::string& mapname, std::vector<OastatGame>& games);

// Game management
void getRecentGames(cppdb::session& database, std::vector<OastatGame>& games);
void getGameScoreTotal(cppdb::session& database, int gamenumber, std::vector<std::pair<int,int>>& scores);
void getGameScoreProgression(cppdb::session& database, int gamenumber, std::vector<ScorePoint>& progression);

// Page generation functions
void write_html_index(cppdb::session& database, const std::string& output_dir);
void write_html_game(cppdb::session& database, const OastatGame& game, const std::string& output_dir);
void write_html_map(cppdb::session& database, const std::string& mapname, const std::string& output_dir);

// Utility functions
std::string getWeaponName(int modtype);
std::string getTimeStamp(const tm& _datetime);

// Global data accessors
std::map<int, int>& get_kills_by_player();
std::map<int, int>& get_player_deaths();
std::map<int, std::map<int, int>>& get_player_awards();
std::map<int, std::map<int, int>>& get_player_weapon_kills();
std::map<std::string, MapInfo>& get_map_infos();
std::set<int>& get_generated_games();
std::set<std::string>& get_generated_maps();

#endif // PAGES_HPP

