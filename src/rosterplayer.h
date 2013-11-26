// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#ifndef ROSTERPLAYER_H_DEFINED
#define ROSTERPLAYER_H_DEFINED

#include <string>
#include <vector>
using namespace std;


/// Represents player information as read from a roster
///
struct RosterPlayer
{
    string name;
    string team;
    string nationality;
    string pref_side;
    int age;

    int st;
    int tk;
    int ps;
    int sh;
    int ag;
    int stamina;

    int st_ab;
    int tk_ab;
    int ps_ab;
    int sh_ab;

    int games;
    int saves;
    int tackles;
    int keypasses;
    int shots;
    int goals;
    int assists;
    int dp;

    int injury;
    int suspension;
    int fitness;
};

const unsigned NUM_COLUMNS_IN_ROSTER = 25;

typedef vector<RosterPlayer> RosterPlayerArray;
typedef vector<RosterPlayer>::iterator RosterPlayerIterator;
typedef vector<RosterPlayer>::const_iterator RosterPlayerConstIterator;


/// Reads a roster into the vector of RosterPlayers. Uses push_back on the vector, without
/// clearing it.
/// Returns "" on success, and an error message if something went wrong.
///
string read_roster_players(string roster_filename, RosterPlayerArray& players_arr);

/// Writes a vector of RosterPlayers into a roster.
/// Returns "" on success, and an error message if something went wrong.
///
string write_roster_players(string roster_filename, const RosterPlayerArray& players_arr);



#endif // ROSTERPLAYER_H_DEFINED


