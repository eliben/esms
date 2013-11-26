// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#ifndef COND_UTILS_H
#define COND_UTILS_H

#include <vector>
#include <string>
#include <cstdio>

using namespace std;

extern int num_players;

bool is_legal_side(char side);
bool is_legal_player_number(int number);
bool is_legal_sign(string sign);
bool is_legal_tactic(string tactic);
bool is_legal_position(string position);
bool true_sign_expression(int a, int b, string sign);

/// Returns a player number, given his name, or -1 if no
/// such player.
///
int player_name_to_number(int team_num, string name);

/// Returns a player number from a string with either a number or
/// or a name, or -1 if no such player.
///
int action_str_to_player_number(int team_num, string str);

string fullpos2position(string fullpos);
char fullpos2side(string fullpos);
string pos_and_side2fullpos(string pos, char side);

#endif // COND_UTILS_H


