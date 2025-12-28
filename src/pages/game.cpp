#include "pages.hpp"
#include "../common/common.hpp"
#include <ctemplate/template.h>
#include <fstream>
#include <sstream>
#include <set>

static void addPlayerToTemplateDict(ctemplate::TemplateDictionary* dict, const OastatPlayer& player) {
	dict->SetValue("PLAYER_ID", std::to_string(player.playerid));
	dict->SetValue("PLAYER_HEADMODEL", player.headmodel);
	dict->SetValue("PLAYER_ISBOT", player.isBot);
	dict->SetValue("PLAYER_LASTSEEN", getTimeStamp(player.lastseen));
	dict->SetValue("PLAYER_MODEL", player.model);
	dict->SetValue("PLAYER_NICKNAME", player.nickname);
}

void write_html_game(cppdb::session& database, const OastatGame& game, const std::string& output_dir) {
	// Skip if already generated
	auto& generated_games = get_generated_games();
	if (generated_games.count(game.gamenumber)) {
		return;
	}
	generated_games.insert(game.gamenumber);

	ctemplate::TemplateDictionary game_tpl("templates/game.tpl");
	game_tpl.SetValue("GENERATION_DATE", timestamp_now_as_string(database));
	game_tpl.SetValue("GAME_NUMBER", std::to_string(game.gamenumber));
	game_tpl.SetValue("GAME_MAP", game.mapname);
	game_tpl.SetValue("GAME_SERVERNAME", game.servername);

	std::vector<std::pair<int,int>> scores;
	getGameScoreTotal(database, game.gamenumber, scores);
	for (size_t i = 0; i < scores.size(); ++i) {
		ctemplate::TemplateDictionary* sub_dict = game_tpl.AddSectionDictionary("SCORES_LIST");
		const OastatPlayer& p = getPlayer(database, scores.at(i).first);
		sub_dict->SetValue("ID", std::to_string(scores.at(i).first));
		sub_dict->SetValue("SCORE", std::to_string(scores.at(i).second));
		addPlayerToTemplateDict(sub_dict, p);
	}

	// Get score progression for chart
	std::vector<ScorePoint> progression;
	getGameScoreProgression(database, game.gamenumber, progression);

	// Build JSON data for D3.js chart
	// Group by player to create series
	std::map<int, std::vector<ScorePoint>> playerSeries;
	for (const auto& point : progression) {
		playerSeries[point.playerid].push_back(point);
	}

	// Generate JSON for each player's score progression
	std::stringstream json_data;
	json_data << "[";
	bool first_player = true;
	for (const auto& series : playerSeries) {
		if (!first_player) json_data << ",";
		first_player = false;

		const OastatPlayer& p = getPlayer(database, series.first);
		json_data << "{\"player\":\"" << p.nickname << "\",\"playerid\":" << series.first << ",\"data\":[";

		bool first_point = true;
		for (const auto& point : series.second) {
			if (!first_point) json_data << ",";
			first_point = false;
			json_data << "{\"second\":" << point.second << ",\"score\":" << point.score << "}";
		}
		json_data << "]}";
	}
	json_data << "]";

	// Write JSON to separate file
	std::ofstream json_file;
	json_file.open(output_dir + "/game/" + std::to_string(game.gamenumber) + ".json");
	json_file << json_data.str();
	json_file.close();

	// Get kill matrix
	std::map<std::pair<int,int>, int> kill_matrix;
	std::vector<int> matrix_players;
	getGameKillMatrix(database, game.gamenumber, kill_matrix, matrix_players);

	// Add kill matrix to template
	if (!matrix_players.empty()) {
		game_tpl.ShowSection("HAS_KILL_MATRIX");

		// Add header row with player names
		for (int playerid : matrix_players) {
			ctemplate::TemplateDictionary* header_dict = game_tpl.AddSectionDictionary("MATRIX_HEADER");
			const OastatPlayer& p = getPlayer(database, playerid);
			header_dict->SetValue("PLAYER_NICKNAME", p.nickname);
		}

		// Add data rows
		for (int killer : matrix_players) {
			ctemplate::TemplateDictionary* row_dict = game_tpl.AddSectionDictionary("MATRIX_ROWS");
			const OastatPlayer& killer_player = getPlayer(database, killer);
			row_dict->SetValue("KILLER_NICKNAME", killer_player.nickname);
			for (int victim : matrix_players) {
				ctemplate::TemplateDictionary* cell_dict = row_dict->AddSectionDictionary("MATRIX_CELLS");
				auto key = std::make_pair(killer, victim);
				int kills = kill_matrix.count(key) ? kill_matrix[key] : 0;
				cell_dict->SetValue("KILL_COUNT", std::to_string(kills));
				if (kills > 0) {
					cell_dict->ShowSection("HAS_KILLS");
				}
			}
		}
	}

	std::string output;
	ctemplate::ExpandTemplate("templates/game.tpl", ctemplate::DO_NOT_STRIP, &game_tpl, &output);
	std::ofstream myfile;
	myfile.open(output_dir + "/game/" + std::to_string(game.gamenumber) + ".html");
	myfile << output;
	myfile.close();
}

void getRecentGames(cppdb::session& database, std::vector<OastatGame>& games) {
	std::string sql = "select gamenumber, gametype, mapname, time, basegame, second, servername from oastat.oastat_games order by gamenumber desc limit 10";
	cppdb::statement st = database.prepare(sql);
	cppdb::result res = st.query();
	while(res.next()) {
		OastatGame game;
		res >> game.gamenumber >> game.gametype >> game.mapname >> game.time >> game.basegame >> game.second >> game.servername;
		games.push_back(game);
	}
}

void getGameScoreTotal(cppdb::session& database, int gamenumber, std::vector<std::pair<int,int>>& scores) {
	std::string sql = "SELECT player, score FROM oastat.oastat_points p1 WHERE gamenumber = ? AND second = (SELECT MAX(second) FROM oastat.oastat_points p2 WHERE p2.player = p1.player AND p2.gamenumber = ?) ORDER BY score DESC";
	cppdb::statement st = database.prepare(sql);
	st.bind(1, gamenumber);
	st.bind(2, gamenumber);
	cppdb::result res = st.query();
	while(res.next()) {
		int playerid;
		int score;
		res >> playerid >> score;
		scores.push_back(std::pair<int,int>(playerid, score));
	}
}

void getGameScoreProgression(cppdb::session& database, int gamenumber, std::vector<ScorePoint>& progression) {
	std::string sql = "SELECT player, `second` as the_second, score FROM oastat.oastat_points where gamenumber = ? ORDER BY the_second, player";
	cppdb::statement st = database.prepare(sql);
	st.bind(1, gamenumber);
	cppdb::result res = st.query();
	while(res.next()) {
		ScorePoint point;
		res >> point.playerid >> point.second >> point.score;
		progression.push_back(point);
	}
}

void getGameKillMatrix(cppdb::session& database, int gamenumber, std::map<std::pair<int,int>, int>& matrix, std::vector<int>& players) {
	// Get all players in the game
	std::set<int> player_set;
	std::string sql_players = "SELECT DISTINCT player FROM oastat.oastat_points WHERE gamenumber = ?";
	cppdb::statement st_players = database.prepare(sql_players);
	st_players.bind(1, gamenumber);
	cppdb::result res_players = st_players.query();
	while(res_players.next()) {
		int playerid;
		res_players >> playerid;
		player_set.insert(playerid);
	}

	// Convert to vector for ordered iteration
	players.assign(player_set.begin(), player_set.end());

	// Get kill data
	std::string sql = "SELECT attacker, target, COUNT(0) as kills FROM oastat.oastat_kills WHERE gamenumber = ? AND attacker <> target GROUP BY attacker, target";
	cppdb::statement st = database.prepare(sql);
	st.bind(1, gamenumber);
	cppdb::result res = st.query();
	while(res.next()) {
		int attacker, target, kills;
		res >> attacker >> target >> kills;
		matrix[std::make_pair(attacker, target)] = kills;
	}
}

