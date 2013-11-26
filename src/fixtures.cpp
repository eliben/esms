// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
////////////////////////////////////////////////////////////////////////////
//
// This program will create a fixtures list for 2 or more teams 
//
// Algorithm:
// 
// For example, with 8 teams the first week is:
// 
// 1 - 2
// 3 - 4
// 5 - 6
// 7 - 8 
//
// Then leave #1 in place and rotate the rest clockwise,
// 1 - 3
// 5 - 2
// 7 - 4
// 8 - 6
//
// and so on...
// 1 - 5
// 7 - 3
// 8 - 2
// 6 - 4
//
// If the amount of teams is odd, a dummy team will be added and all
// it's games will be deleted in the end, so some team will miss a 
// game each week.
//
////////////////////////////////////////////////////////////////////////////
 

#include <fstream>
#include <iostream>
#include <algorithm>
#include "util.h"
#include "anyoption.h"


using namespace std;


bool waitflag = true;
const string DUMMY_TEAMNAME = "DUMMY DUMMY DUMMY DUMMY HOPE IT WONT APPEAR !!!!";


int main(int argc, char** argv)
{
    // handling/parsing command line arguments
    //
    AnyOption* opt = new AnyOption();
    opt->noPOSIX();

    opt->setFlag("no_wait_on_exit");
    opt->setFlag("help");
    opt->setOption("teams_file");
    opt->processCommandArgs(argc, argv);    

    if (opt->getFlag("no_wait_on_exit"))
	waitflag = false;

    if (opt->getValue("help"))
    {
	cout << 
	    "Supply this program with a file\n"
	    "containing a list of teams.\n"
	    "The file can be specified with\n"
	    "the --team_file option. Without\n"
	    "this option, it looks in teams.txt\n\n";

	MY_EXIT(0);
    }

    string teams_filename = "teams.txt";

    if (opt->getValue("teams_file"))
	teams_filename = opt->getValue("teams_file");

    // First, the teams' names are read from the input file.
    // These serve only to count how many teams are there,
    // and later to be printed to the output file. The
    // fixtures generator itself denotes teams by numbers
    // which are converted to actual team names only when
    // the fixtures are printed.
    //
    ifstream teams_file(teams_filename.c_str());

    if (!teams_file)
	die("Unable to open %s\n(use --help if you're using this program for a first time)",
	    teams_filename.c_str());

    string line;
    vector<string> teams;

    // Read the input file, create a vector of teams
    //
    while (getline(teams_file, line))
    {
	if (is_only_whitespace(line))
	    continue;

	vector<string> tokens = tokenize(line);
	
	string team_name = tokens[0];
	
	for (unsigned i = 1; i < tokens.size(); ++i)
	    team_name += " " + tokens[i];

	teams.push_back(team_name);
    }

    unsigned num_teams = teams.size();

    if (num_teams < 2)
	die ("Two teams or more are needed for a league !");

    // Insert a dummy team if there is an odd amount of teams
    // Note: from here on, the amount of teams in the league
    // is even.
    //
    if (num_teams % 2 == 1)
    {
	teams.push_back(DUMMY_TEAMNAME);
	++num_teams;
    }

    // Initialize the games vector
    // 
    unsigned num_weeks_in_round = num_teams - 1;
    vector<string> empty_round(num_teams);
    vector<vector<string> > games(num_weeks_in_round, empty_round);

    // Initialize 1st week (1st vs. 2nd, 3rd vs. 4th, etc...)
    //
    for (unsigned i = 0; i < num_teams; ++i)
    {
	games[0][i] = teams[i];
    }

    // Create a round of games
    //
    if (num_teams > 2)
    {
	for (unsigned week_n = 0; week_n < num_weeks_in_round - 1; ++week_n)
	{
	    // Each week is built from the previous week, using the 
	    // algorithm described at the top of this file.
	    //
	    for (unsigned team_n = 1; team_n < num_teams - 1; ++team_n)
	    {
		if (team_n % 2 == 1)
		    games[week_n + 1][team_n + 2] = games[week_n][team_n];
		else
		    games[week_n + 1][team_n - 2] = games[week_n][team_n];
	    }

	    // Special rotation around the first team (which doesn't move)
	    //
	    games[week_n + 1][0] = games[week_n][0];
	    games[week_n + 1][1] = games[week_n][2];
	    games[week_n + 1][num_teams - 2] = games[week_n][num_teams - 1];
	}
    }

    // Calibrate home/away so that every team playes home-away-home-away...
    // (very approximately: better for large leagues, worse for small ones).
    //
    // This is done by swapping all teams' home/away every other week.
    //
    for (unsigned week_n = 1; week_n < num_weeks_in_round; week_n += 2)
	for (unsigned team_n = 0; team_n < num_teams; team_n += 2)
	    swap(games[week_n][team_n], games[week_n][team_n + 1]);

    ofstream fx_file("fixtures.txt");

    if (fx_file)
    {
	// Now print the fixtures to fixtures.txt
	// First, "games" is printed as is. Then, it's printed again, with
	// home/away reversed (second round). The dummy team is removed.
	// 
	for (unsigned week_n = 0; week_n < num_weeks_in_round * 2; ++week_n)
	{
	    fx_file << week_n + 1 << ".\n\n";
	
	    for (unsigned team_n = 0; team_n < num_teams; team_n += 2)
	    {
		string home_team = (week_n < num_weeks_in_round) ? 
		    games[week_n][team_n] : games[week_n - num_weeks_in_round][team_n + 1];
		string away_team = (week_n < num_weeks_in_round) ? 
		    games[week_n][team_n + 1] : games[week_n - num_weeks_in_round][team_n];

		if (home_team != DUMMY_TEAMNAME && away_team != DUMMY_TEAMNAME)
		    fx_file << home_team << " - " << away_team << endl;
	    }

	    fx_file << endl;
	}

	cout << "fixtures.txt generated\n";
    }
    else
	die("Error creating fixtures.txt !");
	
    MY_EXIT(0);
    return 0;
}
