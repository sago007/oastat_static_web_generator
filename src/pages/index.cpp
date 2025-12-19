#include "pages.hpp"
#include "../common/common.hpp"
#include <ctemplate/template.h>
#include <fstream>
#include <iostream>

const size_t MAX_MAP_LIST = 10;

void write_html_index(cppdb::session& database, const std::string& output_dir) {
	ctemplate::TemplateDictionary index_tpl("templates/index.tpl");
	index_tpl.SetValue("GENERATION_DATE", timestamp_now_as_string(database));

	std::vector<int> player_list = get_top_killers(database, 25);
	populate_player_deaths(database, player_list);
	populate_player_awards(database, player_list);
	populate_player_kills(database, player_list);

	std::vector<OastatGame> games;
	getRecentGames(database, games);

	// Add player statistics
	auto& kills_by_player = get_kills_by_player();
	auto& player_deaths_map = get_player_deaths();
	auto& player_awards_map = get_player_awards();
	auto& player_weapon_kills_map = get_player_weapon_kills();

	for (size_t i = 0; i < player_list.size(); ++i) {
		OastatPlayer p = getPlayer(database, player_list.at(i));
		ctemplate::TemplateDictionary* sub_dict = index_tpl.AddSectionDictionary("PLAYER_LIST");
		sub_dict->SetValue("EVEN_LINE", (i%2)?"1":"0");
		sub_dict->SetValue("PLAYER_NAME", p.nickname);
		sub_dict->SetValue("PLAYER_LAST_SEEN", getTimeStamp(p.lastseen));
		sub_dict->SetValue("PLAYER_KILLS", std::to_string(kills_by_player[p.playerid]));
		sub_dict->SetValue("PLAYER_DEATHS", std::to_string(player_deaths_map[p.playerid]));
		sub_dict->SetValue("PLAYER_IS_BOT", p.isBot);
		sub_dict->SetValue("AWARD_GAUNTLET", std::to_string(player_awards_map[p.playerid][AWARD_GAUNTLET]));
		sub_dict->SetValue("AWARD_IMPRESSIVE", std::to_string(player_awards_map[p.playerid][AWARD_IMPRESSIVE]));
		sub_dict->SetValue("AWARD_EXCELLENT", std::to_string(player_awards_map[p.playerid][AWARD_EXCELLENT]));
		sub_dict->SetValue("AWARD_CAPTURE", std::to_string(player_awards_map[p.playerid][AWARD_CAPTURE]));
		sub_dict->SetValue("AWARD_DEFENCE", std::to_string(player_awards_map[p.playerid][AWARD_DEFENCE]));
		sub_dict->SetValue("AWARD_ASSIST", std::to_string(player_awards_map[p.playerid][AWARD_ASSIST]));
	}

	// Add player weapon statistics
	for (size_t i = 0; i < player_list.size(); ++i) {
		OastatPlayer p = getPlayer(database, player_list.at(i));
		ctemplate::TemplateDictionary* sub_dict = index_tpl.AddSectionDictionary("PLAYER_WEAPON_LIST");
		sub_dict->SetValue("EVEN_LINE", (i%2)?"1":"0");
		sub_dict->SetValue("PLAYER_NAME", p.nickname);
		sub_dict->SetValue("WEAPON_SHOTGUN", std::to_string(player_weapon_kills_map[p.playerid][MOD_SHOTGUN]));
		sub_dict->SetValue("WEAPON_GAUNTLET", std::to_string(player_weapon_kills_map[p.playerid][MOD_GAUNTLET]));
		sub_dict->SetValue("WEAPON_MACHINEGUN", std::to_string(player_weapon_kills_map[p.playerid][MOD_MACHINEGUN]));
		sub_dict->SetValue("WEAPON_GRENADE", std::to_string(player_weapon_kills_map[p.playerid][MOD_GRENADE]+player_weapon_kills_map[p.playerid][MOD_GRENADE_SPLASH]));
		sub_dict->SetValue("WEAPON_ROCKET", std::to_string(player_weapon_kills_map[p.playerid][MOD_ROCKET]+player_weapon_kills_map[p.playerid][MOD_ROCKET_SPLASH]));
		sub_dict->SetValue("WEAPON_PLASMA", std::to_string(player_weapon_kills_map[p.playerid][MOD_PLASMA]+player_weapon_kills_map[p.playerid][MOD_PLASMA_SPLASH]));
		sub_dict->SetValue("WEAPON_RAILGUN", std::to_string(player_weapon_kills_map[p.playerid][MOD_RAILGUN]));
		sub_dict->SetValue("WEAPON_LIGHTNING", std::to_string(player_weapon_kills_map[p.playerid][MOD_LIGHTNING]));
		sub_dict->SetValue("WEAPON_NAILGUN", std::to_string(player_weapon_kills_map[p.playerid][MOD_NAIL]));
		sub_dict->SetValue("WEAPON_CHAINGUN", std::to_string(player_weapon_kills_map[p.playerid][MOD_CHAINGUN]));
		sub_dict->SetValue("WEAPON_BFG", std::to_string(player_weapon_kills_map[p.playerid][MOD_BFG]+player_weapon_kills_map[p.playerid][MOD_BFG_SPLASH]));
		sub_dict->SetValue("WEAPON_TELEFRAG", std::to_string(player_weapon_kills_map[p.playerid][MOD_TELEFRAG]));
		sub_dict->SetValue("WEAPON_FALLING", std::to_string(player_weapon_kills_map[p.playerid][MOD_FALLING]));
	}

	// Add map list
	std::vector<std::string> map_list = get_most_played_maps(database);
	auto& map_infos = get_map_infos();

	for (size_t i = 0; i < map_list.size() && i < MAX_MAP_LIST; ++i) {
		const std::string& mapname = map_list[i];
		const MapInfo& info = map_infos[mapname];
		ctemplate::TemplateDictionary* sub_dict = index_tpl.AddSectionDictionary("MAP_LIST");
		sub_dict->SetValue("EVEN_LINE", (i%2)?"1":"0");
		sub_dict->SetValue("MAP_NAME", mapname);
		sub_dict->SetValue("TIMES_PLAYED", std::to_string(info.times_played));
		sub_dict->SetValue("LAST_PLAYED", getTimeStamp(info.last_played));
	}

	// Add recent games
	for (size_t i = 0; i < games.size(); ++i) {
		const OastatGame& game = games[i];
		ctemplate::TemplateDictionary* sub_dict = index_tpl.AddSectionDictionary("RECENT_GAMES");
		sub_dict->SetValue("EVEN_LINE", (i%2)?"1":"0");
		sub_dict->SetValue("GAMENUMBER", std::to_string(game.gamenumber) );
		sub_dict->SetValue("MAPNAME", game.mapname);
		sub_dict->SetValue("TIME", getTimeStamp(game.time));
		sub_dict->SetValue("SERVERNAME", game.servername);
	}

	// Write output
	std::string output;
	ctemplate::ExpandTemplate("templates/index.tpl", ctemplate::DO_NOT_STRIP, &index_tpl, &output);
	std::ofstream myfile;
	myfile.open(output_dir + "/index.html");
	myfile << output;
	myfile.close();
}

