// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#ifndef LEAGUE_TABLE_H
#define LEAGUE_TABLE_H

#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <fstream>
#include <functional>
#include "util.h"

using namespace std;


class league_table
{
public:
    league_table()
    {}

    bool team_exists(string name);

    // adds a new team to the league
    // if such team already exists - it's replaced
    //
    void add_new_team(string name, int played = 0, int won = 0,
                      int drawn = 0, int lost = 0, int goals_for = 0,
                      int goals_against = 0, int goal_difference = 0,
                      int points = 0);

    // adds a new result for a team in terms of how many goals
    // it scored and conceded. recalculates all team data.
    // if such team doesn't exist - it's created
    //
    void add_team_result(string name, int scored = 0, int conceded = 0);

    // Reads a league table file and fills in the teams data
    //
    void read_league_table_file(string filename);

    // Reads a results file and adds the new results to teams data
    //
    void read_results_file(string filename);

    // returns the (properly sorted and formatted) league table as a string
    //
    string dump_league_table(void);

private:
    struct team_data
    {
        team_data()
        {}

        team_data(string name_, int played_ = 0, int won_ = 0, int drawn_ = 0,
                  int lost_ = 0, int goals_for_ = 0, int goals_against_ = 0,
                  int goal_difference_ = 0, int points_ = 0)
                : name(name_), played(played_), won(won_), drawn(drawn_), lost(lost_),
                goals_for(goals_for_), goals_against(goals_against_),
                goal_difference(goal_difference_), points(points_)
        {}

        string name;
        int played;
        int won;
        int drawn;
        int lost;
        int goals_for;
        int goals_against;
        int goal_difference;
        int points;
    };

    friend bool team_data_predicate(team_data data1, team_data data2);

    map<string, team_data> teams;
};


#endif // LEAGUE_TABLE_H
