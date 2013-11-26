// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
//
#ifndef UPDTR_H
#define UPDTR_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <vector>
#include <string>
#include "rosterplayer.h"

using namespace std;


// For stats
//
struct player_game_stats
{
    string name;
    string pos;

    int minutes;
    int games;
    int shots;
    int goals;
    int saves;
    int tackles;
    int keypasses;
    int assists;
    int yellow;
    int red;
    int dp;
    int injured;
    int suspended;
    int st_ab;
    int tk_ab;
    int ps_ab;
    int sh_ab;
    int fitness;
};


// for leaders generation
//
struct player_stat
{
    player_stat(string name_, string team_name_, int games_, int goals_,
                int assists_, int dp_, int perf_points_)
            :
            name(name_), team_name(team_name_), games(games_), goals(goals_),
            assists(assists_), dp(dp_), perf_points(perf_points_)
    {}

    string name;
    string team_name;
    int games;
    int goals;
    int assists;
    int dp;
    int perf_points;
};


void transform_all_players(void (*transformer_proc)(RosterPlayerIterator&, string, void*));
void increase_ages();
void reset_stats(unsigned inj_sus_flag = 0);
void recover_fitness(bool half);
void decrease_suspensions_injuries(unsigned inj_sus_flag = 0);
void generate_leaders();
void update_rosters();
void update_league_table();
int my_random(int n);
void get_players_game_stats(string stats_filename, vector<player_game_stats>& home_team,
                            vector<player_game_stats>& away_team);

const unsigned SUSPENSIONS = 1;
const unsigned INJURIES = 2;


#endif /* UPTDR_H */
