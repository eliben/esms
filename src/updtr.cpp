// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
//
#include "updtr.h"
#include "rosterplayer.h"
#include "anyoption.h"
#include "config.h"
#include "comment.h"
#include "util.h"
#include "league_table.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <functional>
#include <algorithm>

using namespace std;


// whether there is a wait on exit
//
bool waitflag = true;


// These reports are filled in by the various updating functions,
// and printed to one file in the end
//
vector<string> skill_change_report;
vector<string> injury_report;
vector<string> suspension_report;
vector<string> stats_report;
vector<string> leaders_report;
vector<string> table_report;


int main(int argc, char* argv[])
{
    srand(time(NULL));

    // handling/parsing command line arguments
    //
    AnyOption* opt = new AnyOption();
    opt->noPOSIX();

    opt->setFlag("no_wait_on_exit");
    opt->processCommandArgs(argc, argv);

    if (opt->getFlag("no_wait_on_exit"))
        waitflag = false;

    int option = 0;

    if (opt->getArgc() == 1)
    {
        if (atoi(opt->getArgv(0)) == 0)
            die("Illegal input\n");

        option = atoi(opt->getArgv(0));
    }
    else
    {
        cout <<
        "What would you like to do ?\n\n"
		"Weekly updates:\n\n"
        "1)  Update rosters (100% fitness recovery)\n"
		"2)  Update rosters (50% fitness recovery)\n"
        "3)  Decrease injuries\n"
        "4)  Decrease suspensions\n"
        "5)  Update league table\n"
        "6)  Decrease suspensions + update rosters (50% of fitness gain)\n"
        "7)  Decrease suspensions, injuries + update rosters (50% fitness recovery), league table\n"
        "8)  Decrease suspensions, injuries + update rosters (100% fitness recovery), league table\n\n"
		"End-of-season updates:\n\n"
		"9)  Increase all players' age by one\n"
		"10) Reset all player stats\n"
		"11) Reset all player stats, including injuries\n"
		"12) Reset all player stats, including injuries and suspensions\n\n"
        "Enter your choice -> ";

        string reply;
        getline(cin, reply);

        if (!is_number(reply))
            die("Expecting a number");

        option = str_atoi(reply);
        cout << endl;
    }

    the_commentary().init_commentary("language.dat");
    the_config().load_config_file("league.dat");

    // Now do the job...
    //
    switch (option)
    {
    case 1:
        update_rosters();
        recover_fitness(false);
		generate_leaders();
        break;
    case 2:
        update_rosters();
	    recover_fitness(true);
		generate_leaders();
        break;
    case 3:
        decrease_suspensions_injuries(INJURIES);
        break;
    case 4:
        decrease_suspensions_injuries(SUSPENSIONS);
        break;
    case 5:
        update_league_table();
        break;
    case 6:
        decrease_suspensions_injuries(SUSPENSIONS);
        update_rosters();
		recover_fitness(true);
        generate_leaders();
        break;
    case 7:
        decrease_suspensions_injuries(SUSPENSIONS | INJURIES);
        update_rosters();
		recover_fitness(true);
        generate_leaders();
        update_league_table();
        break;
    case 8:
        decrease_suspensions_injuries(SUSPENSIONS | INJURIES);
        update_rosters();
		recover_fitness(false);
        generate_leaders();
        update_league_table();
        break;
	case 9:
		increase_ages();
		break;
	case 10:
		reset_stats();
		break;
	case 11:
		reset_stats(INJURIES);
		break;
	case 12:
		reset_stats(INJURIES | SUSPENSIONS);
		break;
    default:
        die("Illegal option %d", option);
    }

    // Now all the generated reports are printed to a single summary
    // file
    //
    ofstream sf("updtr_summary.txt");

    if (!injury_report.empty())
        print_elements(sf, injury_report, "\n", "\n\nInjuries:\n---------\n\n");

    if (!suspension_report.empty())
        print_elements(sf, suspension_report, "\n", "\n\nSuspensions:\n------------\n\n");

    if (!skill_change_report.empty())
        print_elements(sf, skill_change_report, "\n", "\n\nSkill changes:\n--------------\n\n");

    if (!stats_report.empty())
        print_elements(sf, stats_report, "\n", "\n\n");

    if (!table_report.empty())
        print_elements(sf, table_report, "\n", "\n\nTable:\n------\n\n");

    if (!leaders_report.empty())
        print_elements(sf, leaders_report, "\n");

    MY_EXIT(0);
    return 0;
}


bool is_stats_header_line(string line)
{
    vector<string> toks = tokenize(line, " \t");

    if (toks.size() >= 3 && toks[0] == "<<<" && toks[toks.size() - 1] == ">>>")
        return true;
    else
        return false;
}


// Fills in the vectors with stats from filename
//
void get_players_game_stats(string stats_filename, vector<player_game_stats>& home_team,
                            vector<player_game_stats>& away_team)
{
    ifstream file(stats_filename.c_str());

    if (!file)
        die("Failed to open file %s\n", stats_filename.c_str());

    int dp_for_yellow = the_config().get_int_config("DP_FOR_YELLOW", 4);
    int dp_for_red = the_config().get_int_config("DP_FOR_RED", 10);
    int num_subs = the_config().get_int_config("NUM_SUBS", 7);
    int num_players = 11 + num_subs;
    string line;
    int team_count = 0;

    while (getline(file, line))
    {
        if (is_stats_header_line(line))
        {
            ++team_count;
            vector<player_game_stats> team;

            // get the following empty line and lines with column headers
            //
            getline(file, line);
            getline(file, line);
            getline(file, line);

            for (int i = 0; i < num_players; ++i)
            {
                getline(file, line);
                vector<string> tokens = tokenize(line);

                if (tokens.size() != 24)
                    die("Illegal stats line in file %s:\n%s\n", stats_filename.c_str(), line.c_str());

                player_game_stats player;
                player.name = tokens[0];
                player.pos = tokens[1];
                player.minutes = str_atoi(tokens[9]);
                player.games = (player.minutes > 0) ? 1 : 0;
                player.saves = str_atoi(tokens[10]);
                player.tackles = str_atoi(tokens[11]);
                player.keypasses = str_atoi(tokens[12]);
                player.assists = str_atoi(tokens[13]);
                player.shots = str_atoi(tokens[14]);
                player.goals = str_atoi(tokens[15]);
                player.yellow = str_atoi(tokens[16]);
                player.red = str_atoi(tokens[17]);
                player.injured = str_atoi(tokens[18]);
                player.st_ab = str_atoi(tokens[19]);
                player.tk_ab = str_atoi(tokens[20]);
                player.ps_ab = str_atoi(tokens[21]);
                player.sh_ab = str_atoi(tokens[22]);
                player.fitness = str_atoi(tokens[23]);
                player.dp = dp_for_red * player.red + dp_for_yellow * player.yellow;

                team.push_back(player);
            }


            if (team_count == 1)
                home_team = team;
            else
                away_team = team;
        }
    }
}


// Finds a player by name in a roster
//
RosterPlayerIterator get_player_by_name_from_roster(string name, RosterPlayerArray& players)
{
    for (RosterPlayerIterator player = players.begin(); player != players.end(); ++player)
    {
		if (player->name == name)
			return player;
    }

    return players.end();
}


// Handles a skill change as a result of the ability crossing a threshold
//
// Given:
//   - player name, team name, skill name (for printing to report)
//   - ab_points, skill - the ability and skill affected. In case of a skill change,
//     these values are modified by the function
//
void handle_skill_change(string player_name, string team_name, string skill_name, int& ab_points, int& skill)
{
    // Increase ?
    if (ab_points >= 1000)
    {
        ab_points -= 700;
        skill++;
        skill_change_report.push_back(the_commentary().rand_comment("UPDTR_SKILL_INCREASE",
                                      player_name.c_str(),
                                      team_name.c_str(),
                                      skill_name.c_str()));
    }
    // Decrease ?
    else if (ab_points < 0)
    {
        ab_points += 300;
        skill--;
        skill_change_report.push_back(the_commentary().rand_comment("UPDTR_SKILL_DECREASE",
                                      player_name.c_str(),
                                      team_name.c_str(),
                                      skill_name.c_str()));
    }

    return;
}


bool perf_predicate(const pair<string, int>& left, const pair<string, int>& right)
{
    return left.second > right.second;
}


int calc_perf_points(int goals, int shots, int tackles, int saves, int assists, int keypasses, int dp)
{
    return goals * 9 + shots + tackles * 6 + saves * 3 + assists * 7 + keypasses * 4 - dp * 2;
}


string make_header(string header_name)
{
    string line(header_name.length() + 4, '-');
    string ret = line + "\n  " + header_name + "\n" + line + "\n";

    return ret;
}


// Goes over stats.dir and updates the rosters of all teams with
// stats from the listed games.
//
void update_rosters()
{
    // fetch some configs
    //
    int max_inj = the_config().get_int_config("MAX_INJURY_LENGTH", 9);
    int suspension_margin = the_config().get_int_config("SUSPENSION_MARGIN", 10);
    int num_subs = the_config().get_int_config("NUM_SUBS", 7);
    int num_players = 11 + num_subs;

    ifstream dir_file("stats.dir");

    if (!dir_file)
        die("Failed to open file stats.dir\n");

    string line;

    map<string, int> weekly_stats;
    weekly_stats["goals"] = weekly_stats["goals_DF"] = weekly_stats["goals_DM"] = weekly_stats["goals_MF"] =
		weekly_stats["goals_AM"] = weekly_stats["goals_FW"] = weekly_stats["assists"] =
		weekly_stats["assists_DF"] = weekly_stats["assists_DM"] = weekly_stats["assists_MF"] =
		weekly_stats["assists_DM"] = weekly_stats["assists_FW"] = weekly_stats["yellows"] =
		weekly_stats["reds"] = weekly_stats["injuries"] = 0;

    vector<pair<string, int> > weekly_performers;

    // For each line in the stats.dir file (that is, for each game stats to
    // update), open the relevant rosters and update the relevant players.
    //
    while (getline(dir_file, line))
    {
        // delete spaces
        line.erase(remove
                   (line.begin(), line.end(), ' '), line.end());

        vector<string> parts = tokenize(line, "_");

        if (parts.size() != 2)
            die("Illegal stats file name %s in stats.dir\n", line.c_str());

        string team_name[2];
        team_name[0] = parts[0];
        team_name[1] = parts[1].substr(0, parts[1].find_first_of("."));

        vector<player_game_stats> stats_teams[2];
        get_players_game_stats(line, stats_teams[0], stats_teams[1]);

        if (stats_teams[0].size() != unsigned(num_players))
            die("Expected %d players of %s in stats file %s\n",
                num_players, team_name[0].c_str(), line.c_str());

        if (stats_teams[1].size() != unsigned(num_players))
            die("Expected %d players of %s in stats file %s\n",
                num_players, team_name[1].c_str(), line.c_str());

        for (int team_n = 0; team_n <= 1; ++team_n)
        {
			RosterPlayerArray players;
            string roster_name = team_name[team_n] + ".txt";

            string msg = read_roster_players(roster_name, players);
			
			if (msg != "")
			{
				cerr << "Roster update error: " << msg << endl;
				break;
			}

            // For each player in the stats: look it up in the roster, and
            // update everything
            //
            for (int player_n = 0; player_n < num_players; ++player_n)
            {
				RosterPlayerIterator player = get_player_by_name_from_roster(stats_teams[team_n][player_n].name, players);

                if (player == players.end())
                    die("Player %s (from %s) not found in roster %s\n",
                        stats_teams[team_n][player_n].name.c_str(), line.c_str(), roster_name.c_str());

                // Add all simple stats
                //
                player->games += stats_teams[team_n][player_n].games;
                player->saves += stats_teams[team_n][player_n].saves;
                player->tackles += stats_teams[team_n][player_n].tackles;
                player->keypasses += stats_teams[team_n][player_n].keypasses;
                player->shots += stats_teams[team_n][player_n].shots;
                player->goals += stats_teams[team_n][player_n].goals;
                player->assists += stats_teams[team_n][player_n].assists;
                player->st_ab += stats_teams[team_n][player_n].st_ab;
                player->tk_ab += stats_teams[team_n][player_n].tk_ab;
                player->ps_ab += stats_teams[team_n][player_n].ps_ab;
                player->sh_ab += stats_teams[team_n][player_n].sh_ab;

                // Take care of skill increases and decreases
                //
                handle_skill_change(player->name, team_name[team_n], "St",
                                    player->st_ab, player->st);
                handle_skill_change(player->name, team_name[team_n], "Tk",
                                    player->tk_ab, player->tk);
                handle_skill_change(player->name, team_name[team_n], "Ps",
                                    player->ps_ab, player->ps);
                handle_skill_change(player->name, team_name[team_n], "Sh",
                                    player->sh_ab, player->sh);

                // Take care of DP and suspensions
                //
                // A suspension takes place if after the update, a player's
                // DP crossed some factor of suspension_margin. Then, the length of
                // the suspension is this factor.
                //
                // For example:
                //
                // A player's DP before the game was 18, and he got 3 DP during
                // the game, and suspension_margin = 10. His total DP now is 21, so
                // he crossed a factor (crossed = was below it prior to the update,
                // and is above it after the update). Then, his suspension period
                // is 2 (since it's int(DP/suspension_margin).
                //
                int dp_after_update = player->dp + stats_teams[team_n][player_n].dp;

                // Note: relying on C++'s division of integers --> integral part
                //
                if ((player->dp / suspension_margin) < (dp_after_update / suspension_margin))
                {
                    player->suspension = dp_after_update / suspension_margin;

                    string comm_line;

                    if (player->suspension == 1)
                        comm_line = the_commentary().rand_comment("UPDTR_SUSPENDED_1",
                                    player->name.c_str(),
                                    team_name[team_n].c_str());
                    else
                        comm_line = the_commentary().rand_comment("UPDTR_SUSPENDED_N",
                                    player->name.c_str(),
                                    team_name[team_n].c_str(),
                                    player->suspension);

                    suspension_report.push_back(comm_line);
                }

                player->dp = dp_after_update;

                // Take care of injuries
                //
                if (stats_teams[team_n][player_n].injured)
                {
                    player->injury = my_random(my_random(max_inj + 1) + 1);
                    string comm_line;

                    if (player->injury == 0)
                        comm_line = the_commentary().rand_comment("UPDTR_INJURY_NONE",
                                    player->name.c_str(),
                                    team_name[team_n].c_str());
                    else if (player->injury == 1)
                        comm_line = the_commentary().rand_comment("UPDTR_INJURY_1",
                                    player->name.c_str(),
                                    team_name[team_n].c_str());
                    else if (player->injury <= 4)
                        comm_line = the_commentary().rand_comment("UPDTR_INJURY_LIGHT",
                                    player->name.c_str(),
                                    team_name[team_n].c_str(),
                                    player->injury);
                    else
                        comm_line = the_commentary().rand_comment("UPDTR_INJURY_HARD",
                                    player->name.c_str(),
                                    team_name[team_n].c_str(),
                                    player->injury);

                    injury_report.push_back(comm_line);
                }

                // Take care of fitness
                //
                player->fitness = stats_teams[team_n][player_n].fitness;

                // Generate weekly statistics
                //
                weekly_stats["goals"] += stats_teams[team_n][player_n].goals;
                weekly_stats["assists"] += stats_teams[team_n][player_n].assists;
                weekly_stats["yellows"] += stats_teams[team_n][player_n].yellow;
                weekly_stats["reds"] += stats_teams[team_n][player_n].red;
                weekly_stats["injuries"] += stats_teams[team_n][player_n].injured;

                if (stats_teams[team_n][player_n].pos != "GK")
                {
                    string only_position = stats_teams[team_n][player_n].pos.substr(0, 2);

                    weekly_stats["goals_" + only_position] += stats_teams[team_n][player_n].goals;
                    weekly_stats["assists_" + only_position] += stats_teams[team_n][player_n].assists;
                }

                string name_and_team = stats_teams[team_n][player_n].name + " (" + team_name[team_n] + ")";

                int perf_points = calc_perf_points(stats_teams[team_n][player_n].goals,
                                                   stats_teams[team_n][player_n].shots,
                                                   stats_teams[team_n][player_n].tackles,
                                                   stats_teams[team_n][player_n].saves,
                                                   stats_teams[team_n][player_n].assists,
                                                   stats_teams[team_n][player_n].keypasses,
                                                   stats_teams[team_n][player_n].dp);

                weekly_performers.push_back(make_pair(name_and_team, perf_points));
            }

            write_roster_players(roster_name, players);
        }

        cout << "Rosters updated with stats " << line << endl;
    }

    stats_report.push_back(make_header("Round summary"));
    stats_report.push_back(format_str("Goals:        %3d  (DFs - %d, DMs - %d, MFs - %d, AMs - %d, FWs - %d)",
                                      weekly_stats["goals"], weekly_stats["goals_DF"],
                                      weekly_stats["goals_DM"], weekly_stats["goals_MF"],
                                      weekly_stats["goals_AM"], weekly_stats["goals_FW"]));
    stats_report.push_back(format_str("Assists:      %3d  (DFs - %d, DMs - %d, MFs - %d, AMs - %d, FWs - %d)",
                                      weekly_stats["assists"], weekly_stats["assists_DF"],
                                      weekly_stats["assists_DM"], weekly_stats["assists_MF"],
                                      weekly_stats["assists_AM"], weekly_stats["assists_FW"]));
    stats_report.push_back(format_str("Yellow cards: %3d", weekly_stats["yellows"]));
    stats_report.push_back(format_str("Red cards:    %3d", weekly_stats["reds"]));
    stats_report.push_back(format_str("Injuries:     %3d", weekly_stats["injuries"]));

    sort(weekly_performers.begin(), weekly_performers.end(), perf_predicate);

    stats_report.push_back("\nTop performers:\n");

    for (vector<pair<string, int> >::const_iterator it = weekly_performers.begin();
            it != weekly_performers.end(); ++it)
    {
        if (it - weekly_performers.begin() > 10)
            break;

        stats_report.push_back(format_str("%-20s  %d", it->first.c_str(), it->second));
    }
}


// Goes over all players in all teams listed in teams.dir and "transforms" them using
// the provided transformer_proc
//
void transform_all_players(void (*transformer_proc)(RosterPlayerIterator&, string, void*), void* arg)
{
    ifstream dir_file("teams.dir");

    if (!dir_file)
        die("Failed to open file teams.dir\n");

    string line;
	
    while (getline(dir_file, line))
    {
        // delete spaces
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        string roster_name = line;
        string team_name = roster_name.substr(0, roster_name.find_first_of("."));

		RosterPlayerArray players;
		string msg = read_roster_players(roster_name, players);
	
		if (msg != "")
		{
			cerr << "Error reading roster " << team_name << ": " << msg << endl;
			continue;
		}

        for (RosterPlayerIterator player = players.begin(); player != players.end(); ++player)
        {
			transformer_proc(player, team_name, arg);
        }

        write_roster_players(roster_name, players);
	}
}


void transformer_recover_fitness(RosterPlayerIterator& player, string team_name, void* arg)
{
	bool* half = (bool*) arg;
	
	int gain = the_config().get_int_config("UPDTR_FITNESS_GAIN", 20);
	if (*half) gain /= 2;
	
	player->fitness += gain;
	
	if (player->fitness > 100)
		player->fitness = 100;
}


void recover_fitness(bool half)
{
	transform_all_players(transformer_recover_fitness, &half);
	
    cout << "Fitness recovered (" << (half ? "50" : "100") << "%)\n";
}


void transformer_increase_ages(RosterPlayerIterator& player, string team_name, void*)
{
	player->age += 1;
}


void increase_ages()
{
	transform_all_players(transformer_increase_ages, 0);
	cout << "Ages increased\n";
}


void transformer_reset_stats(RosterPlayerIterator& player, string team_name, void* arg)
{
	player->games = player->saves = player->tackles = player->keypasses = player->shots = player->goals = player->assists = player->dp = 0;
	player->fitness = 100;
	
	unsigned* inj_sus_flag = (unsigned*) arg;
	
	if (*inj_sus_flag & INJURIES)
		player->injury = 0;
	
	if (*inj_sus_flag & SUSPENSIONS)
		player->suspension = 0;
}


void reset_stats(unsigned inj_sus_flag)
{
	transform_all_players(transformer_reset_stats, &inj_sus_flag);
	cout << "Stats reset\n";
	
	if (inj_sus_flag & INJURIES)
		cout << "Injuries reset\n";
	
	if (inj_sus_flag & SUSPENSIONS)
		cout << "Suspensions reset\n";
}


void transformer_decrease_sus_inj(RosterPlayerIterator& player, string team_name, void* arg)
{
	unsigned* inj_sus_flag = (unsigned*) arg;
	
	if (*inj_sus_flag & SUSPENSIONS)
	{
		// those with 0 will be decreased to -1, hence they will generate
		// no report on "coming back".
		//
		player->suspension--;

		if (player->suspension == 0)
			suspension_report.push_back(the_commentary().rand_comment("UPDTR_END_SUSPENSION",
										player->name.c_str(),
										team_name.c_str()));
		else if (player->suspension < 0)
			player->suspension = 0;
	}

	if (*inj_sus_flag & INJURIES)
	{
		player->injury--;

		if (player->injury == 0)
		{
			injury_report.push_back(the_commentary().rand_comment("UPDTR_END_INJURY",
									player->name.c_str(),
									team_name.c_str()));

			player->fitness = the_config().get_int_config("UPDTR_FITNESS_AFTER_INJURY", 80);
		}
		else if (player->injury < 0)
			player->injury = 0;
	}
}


void decrease_suspensions_injuries(unsigned inj_sus_flag)
{
	transform_all_players(transformer_decrease_sus_inj, &inj_sus_flag);
	
	if (inj_sus_flag & INJURIES)
		cout << "Injuries decreased\n";
	
	if (inj_sus_flag & SUSPENSIONS)
		cout << "Suspensions decreased\n";
}


void make_leaders_report(const vector<player_stat>& list, string heading, string stat)
{
    leaders_report.push_back("\n\n" + heading + ":");
    leaders_report.push_back(format_str("\nName                 Games    %s\n"
                                        "---------------------------------", stat.c_str()));

    for (int i = 0; i < min<int>(15, list.size()); ++i)
    {
        string name_and_team = list[i].name + " (" + list[i].team_name + ")";
        int the_stat =
            (stat == "Gls") ? list[i].goals :
            (stat == "Ass") ? list[i].assists :
            (stat == "DPs") ? list[i].dp : list[i].perf_points;

        leaders_report.push_back(format_str("%-20s %5d  %5d", name_and_team.c_str(),
                                            list[i].games, the_stat));
    }
}


bool leaders_predicate_goals(const player_stat& p1, const player_stat& p2)
{
    return (p1.goals > p2.goals) || (p1.goals == p2.goals && p1.games < p2.games);
}


bool leaders_predicate_assists(const player_stat& p1, const player_stat& p2)
{
    return (p1.assists > p2.assists) || (p1.assists == p2.assists && p1.games < p2.games);
}


bool leaders_predicate_dps(const player_stat& p1, const player_stat& p2)
{
    return (p1.dp > p2.dp) || (p1.dp == p2.dp && p1.games < p2.games);
}


bool leaders_predicate_perf(const player_stat& p1, const player_stat& p2)
{
    return (p1.perf_points > p2.perf_points) || (p1.perf_points == p2.perf_points && p1.games < p2.games);
}


void generate_leaders(void)
{
    ifstream dir_file("teams.dir");

    if (!dir_file)
        die("Failed to open file teams.dir\n");

    vector<player_stat> stat_players;

    string line;

    while (getline(dir_file, line))
    {
        // delete spaces
        line.erase(remove
                   (line.begin(), line.end(), ' '), line.end());
        string roster_name = line;
        string team_name = roster_name.substr(0, roster_name.find_first_of("."));

		RosterPlayerArray players;
		string msg = read_roster_players(roster_name, players);
	
		if (msg != "")
		{
			cerr << "Error in leaders generation: " << msg << endl;
			continue;
		}

        for (RosterPlayerIterator player = players.begin(); player != players.end(); ++player)
        {
            int perf_points = calc_perf_points(player->goals,
                                               player->shots,
                                               player->tackles,
                                               player->saves,
                                               player->assists,
                                               player->keypasses,
                                               player->dp);

            stat_players.push_back(player_stat(player->name,
                                               team_name,
                                               player->games,
                                               player->goals,
                                               player->assists,
                                               player->dp,
                                               perf_points));

            if (is_only_whitespace(player->name))
                cout << "ALARM";
        }
    }

    sort(stat_players.begin(), stat_players.end(), leaders_predicate_goals);
    make_leaders_report(stat_players, "Scorers", "Gls");
    sort(stat_players.begin(), stat_players.end(), leaders_predicate_perf);
    make_leaders_report(stat_players, "Performers", "Pts");
    sort(stat_players.begin(), stat_players.end(), leaders_predicate_assists);
    make_leaders_report(stat_players, "Assisters", "Ass");
    sort(stat_players.begin(), stat_players.end(), leaders_predicate_dps);
    make_leaders_report(stat_players, "Disciplinary points", "DPs");

    cout << "Leaders generated\n";
}


void update_league_table(void)
{
    league_table table;

    table.read_league_table_file("table.txt");
    table.read_results_file("reports.txt");

    string table_text = table.dump_league_table();
    table_report.push_back(table_text);

    ofstream tf("table.txt");

    if (tf)
    {
        tf << table_text << endl;
        cout << "Table file table.txt updated" << endl;
    }
    else
        cout << "Something went wrong updating table.txt" << endl;
}


// suckz, but the needs of updtr are trivial.
//
int my_random(int n)
{
    return rand() % n;
}

