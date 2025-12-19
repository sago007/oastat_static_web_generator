#include "pages.hpp"
#include "../common/common.hpp"
#include <ctemplate/template.h>
#include <fstream>
#include <algorithm>
#include <set>

static bool compareWeaponKillsDesc(const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
	return a.second > b.second;
}

void write_html_map(cppdb::session& database, const std::string& mapname, const std::string& output_dir) {
	// Skip if already generated
	auto& generated_maps = get_generated_maps();
	if (generated_maps.count(mapname)) {
		return;
	}
	generated_maps.insert(mapname);

	ctemplate::TemplateDictionary map_tpl("templates/map.tpl");
	map_tpl.SetValue("GENERATION_DATE", timestamp_now_as_string(database));
	map_tpl.SetValue("MAP_NAME", mapname);

	// Get weapon kills for this map
	std::map<int, int> weapon_kills;
	getMapWeaponKills(database, mapname, weapon_kills);

	// Combine splash damage with base weapon damage
	std::map<std::string, int> combined_weapon_kills;
	combined_weapon_kills["Shotgun"] = weapon_kills[MOD_SHOTGUN];
	combined_weapon_kills["Gauntlet"] = weapon_kills[MOD_GAUNTLET];
	combined_weapon_kills["Machinegun"] = weapon_kills[MOD_MACHINEGUN];
	combined_weapon_kills["Grenade"] = weapon_kills[MOD_GRENADE] + weapon_kills[MOD_GRENADE_SPLASH];
	combined_weapon_kills["Rocket"] = weapon_kills[MOD_ROCKET] + weapon_kills[MOD_ROCKET_SPLASH];
	combined_weapon_kills["Plasma"] = weapon_kills[MOD_PLASMA] + weapon_kills[MOD_PLASMA_SPLASH];
	combined_weapon_kills["Railgun"] = weapon_kills[MOD_RAILGUN];
	combined_weapon_kills["Lightning"] = weapon_kills[MOD_LIGHTNING];
	combined_weapon_kills["Nailgun"] = weapon_kills[MOD_NAIL];
	combined_weapon_kills["Chaingun"] = weapon_kills[MOD_CHAINGUN];
	combined_weapon_kills["BFG"] = weapon_kills[MOD_BFG] + weapon_kills[MOD_BFG_SPLASH];
	combined_weapon_kills["Telefrag"] = weapon_kills[MOD_TELEFRAG];
	combined_weapon_kills["Falling"] = weapon_kills[MOD_FALLING];

	// Sort weapons by kill count (descending)
	std::vector<std::pair<std::string, int>> sorted_weapons;
	for (const auto& wk : combined_weapon_kills) {
		if (wk.second > 0) {
			sorted_weapons.push_back(wk);
		}
	}
	std::stable_sort(sorted_weapons.begin(), sorted_weapons.end(), compareWeaponKillsDesc);

	for (const auto& wk : sorted_weapons) {
		ctemplate::TemplateDictionary* sub_dict = map_tpl.AddSectionDictionary("WEAPON_KILLS");
		sub_dict->SetValue("WEAPON_NAME", wk.first);
		sub_dict->SetValue("KILL_COUNT", std::to_string(wk.second));
	}

	// Get recent matches for this map
	std::vector<OastatGame> games;
	getMapRecentGames(database, mapname, games);
	for (size_t i = 0; i < games.size(); ++i) {
		const OastatGame& game = games[i];
		ctemplate::TemplateDictionary* sub_dict = map_tpl.AddSectionDictionary("RECENT_MATCHES");
		sub_dict->SetValue("GAMENUMBER", std::to_string(game.gamenumber));
		sub_dict->SetValue("TIME", getTimeStamp(game.time));
		sub_dict->SetValue("SERVERNAME", game.servername);
	}

	std::string output;
	ctemplate::ExpandTemplate("templates/map.tpl", ctemplate::DO_NOT_STRIP, &map_tpl, &output);
	std::ofstream myfile;
	myfile.open(output_dir + "/map/" + mapname + ".html");
	myfile << output;
	myfile.close();
}

void getMapWeaponKills(cppdb::session& database, const std::string& mapname, std::map<int, int>& weapon_kills) {
	std::string sql = "select k.modtype, count(0) c from oastat.oastat_kills k, oastat.oastat_games g "
		"where g.gamenumber = k.gamenumber and g.mapname = ? and k.attacker <> k.target "
		"group by k.modtype order by c desc";
	cppdb::statement st = database.prepare(sql);
	st.bind(1, mapname);
	cppdb::result res = st.query();
	while(res.next()) {
		int modtype;
		int count;
		res >> modtype >> count;
		weapon_kills[modtype] = count;
	}
}

void getMapRecentGames(cppdb::session& database, const std::string& mapname, std::vector<OastatGame>& games) {
	std::string sql = "select gamenumber, gametype, mapname, time, basegame, second, servername "
		"from oastat.oastat_games where mapname = ? order by gamenumber desc limit 10";
	cppdb::statement st = database.prepare(sql);
	st.bind(1, mapname);
	cppdb::result res = st.query();
	while(res.next()) {
		OastatGame game;
		res >> game.gamenumber >> game.gametype >> game.mapname >> game.time >> game.basegame >> game.second >> game.servername;
		games.push_back(game);
	}
}

