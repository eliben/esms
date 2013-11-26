// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#include "league_table.h"


bool team_data_predicate(league_table::team_data data1, league_table::team_data data2)
{
    if (data1.points != data2.points)
        return data1.points > data2.points;
    else
    {
        if (data1.goal_difference != data2.goal_difference)
            return data1.goal_difference > data2.goal_difference;
        else
            return data1.goals_for > data2.goals_for;
    }
}


bool league_table::team_exists(string name)
{
    return (teams.find(name) != teams.end());
}


void league_table::read_league_table_file(string filename)
{
    ifstream table_in(filename.c_str());

    // The file doesn't have to exist (if it doesn't, a new table is
    // created). But if it exists, it must be in correct format
    //
    if (table_in)
    {
        string line;

        // skip header
        //
        getline(table_in, line);
        getline(table_in, line);

        while (getline(table_in, line))
        {
            if (is_only_whitespace(line))
                continue;

            vector<string> tokens = tokenize(line);

            // The structure of a line must be:
            //
            // PLACE TEAMNAME+ PL W D L GF GA GD PTS
            //
            // TEAMNAME may be multiple tokens, so we count from the
            // end ! The first token is PLACE, the last 8 tokens are
            // as specified, and everything between the first and
            // the last 8 is the team name.
            //
            // Note: when the team name is restructured from the
            // tokens, each token is separated by one space
            //
            unsigned num_tokens = tokens.size();

            if (num_tokens < 10)
                die("The following line in %s has too few tokens:\n%s", filename.c_str(), line.c_str());

            int points = str_atoi(tokens[num_tokens - 1]);
            int goal_difference = str_atoi(tokens[num_tokens - 2]);
            int goals_against = str_atoi(tokens[num_tokens - 3]);
            int goals_for = str_atoi(tokens[num_tokens - 4]);
            int lost = str_atoi(tokens[num_tokens - 5]);
            int drawn = str_atoi(tokens[num_tokens - 6]);
            int won = str_atoi(tokens[num_tokens - 7]);
            int played = str_atoi(tokens[num_tokens - 8]);

            string name = tokens[1];

            for (unsigned i = 2; i <= num_tokens - 9; ++i)
                name += " " + tokens[i];

            add_new_team(name, played, won, drawn, lost, goals_for, goals_against,
                         goal_difference, points);
        }
    }
}


void league_table::read_results_file(string filename)
{
    ifstream results_in(filename.c_str());

    if (!results_in)
        die("Unable to open results file %s", filename.c_str());

    string line;

    while (getline(results_in, line))
    {
        if (is_only_whitespace(line))
            continue;

        vector<string> tokens = tokenize(line);

        // A reports file consists not only of results,
        // but also from scorers, injured players, etc.
        // The results must be extracted and taken into account. The
        // rest must be ignored. The results lines look as follows:
        //
        // TEAMNAME1+ SCORE - SCORE TEAMNAME2+
        //
        // Where both TEAMNAMEs can be several tokens long.
        //
        // Note: incorrectly formed results lines will be ignored.
        // The reports file is machine-generated, so this should not
        // be a problem in real situations.
        //

        if (tokens.size() < 5)
            continue;

        unsigned dash_index = 0;

        for (vector<string>::const_iterator i = tokens.begin() + 1; i != tokens.end() - 1; ++i)
        {
            string score_1 = *(i - 1);
            string dash = *i;
            string score_2 = *(i + 1);

            if (is_number(score_1) && dash == "-" && is_number(score_2))
                dash_index = i - tokens.begin();
            else
                continue;
        }

        if (dash_index == 0)
            continue;

        // If we're here, dash_index is the token number of the dash in a correctly
        // formed result line.
        //
        int score_1 = str_atoi(tokens[dash_index - 1]);
        int score_2 = str_atoi(tokens[dash_index + 1]);

        string name_1 = tokens[0];

        for (unsigned i = 1; i <= dash_index - 2; ++i)
            name_1 += " " + tokens[i];

        string name_2 = tokens[dash_index + 2];

        for (unsigned i = dash_index + 3; i < tokens.size(); ++i)
            name_2 += " " + tokens[i];

        add_team_result(name_1, score_1, score_2);
        add_team_result(name_2, score_2, score_1);
    }
}


// adds a new team to the league
// if such team already exists - it's replaced
//
void league_table::add_new_team(string name, int played, int won,
                                int drawn, int lost, int goals_for,
                                int goals_against, int goal_difference,
                                int points)
{
    team_data new_team_data(name, played, won, drawn, lost, goals_for,
                            goals_against, goal_difference, points);

    teams[name] = new_team_data;
}


// adds a new result for a team in terms of how many goals
// it scored and conceded. recalculates all team data.
// if such team doesn't exist - it's created
//
void league_table::add_team_result(string name, int scored, int conceded)
{
    team_data data(name);

    if (team_exists(name))
        data = teams[name];

    data.played++;

    if (scored > conceded)
    {
        data.won++;
        data.points += 3;
    }
    else if (scored < conceded)
    {
        data.lost++;
    }
    else
    {
        data.drawn++;
        data.points++;
    }

    data.goals_for += scored;
    data.goals_against += conceded;
    data.goal_difference = data.goals_for - data.goals_against;

    teams[name] = data;
}


string league_table::dump_league_table(void)
{
    string ret;
    vector<team_data> sorted_teams;

    for (map<string, team_data>::const_iterator i = teams.begin();
            i != teams.end(); ++i)
    {
        sorted_teams.push_back(i->second);
    }

    sort(sorted_teams.begin(), sorted_teams.end(), team_data_predicate);

    // print header
    //
    ret += "Pl   Team                    P    W   D   L    GF   GA   GD   Pts\n";
    ret += "-----------------------------------------------------------------\n";

    // print teams
    //
    for (vector<team_data>::const_iterator i = sorted_teams.begin();
            i != sorted_teams.end(); ++i)
    {
        string line = format_str("%-4d %-21s %3d %4d %3d %3d %5d %4d %4d %5d\n",
                                 (i - sorted_teams.begin()) + 1, // place
                                 i->name.c_str(),
                                 i->played,
                                 i->won,
                                 i->drawn,
                                 i->lost,
                                 i->goals_for,
                                 i->goals_against,
                                 i->goal_difference,
                                 i->points);

        ret += line;
    }

    return ret;
}
