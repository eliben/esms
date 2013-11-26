// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#include "cond_utils.h"
#include "tactics.h"
#include "game.h"


extern struct teams team[2];


int player_name_to_number(int team_num, string name)
{
    for (int i = 1; i <= num_players; ++i)
    {
		if (name == team[team_num].player[i].name)
			return i;
	}
	
	return -1;
}


int action_str_to_player_number(int team_num, string str)
{
	char* ptr;
	int player_number = strtol(str.c_str(), &ptr, 10);
	
	// It's a number ? Player number...
	//
	if (!*ptr)
	{
		if (!is_legal_player_number(player_number))
			return -1;
	}
	// Not a number ? So it's probably a name
	//
	else
	{
		player_number = player_name_to_number(team_num, str);
		
		if (player_number < 0)
			return -1;
	}
	
	return player_number;
}


bool is_legal_side(char side)
{
    return ((side == 'L') || (side == 'R') || (side == 'C'));
}


// Position: 3 letters (DFL, AMC, etc.) or GK
//
bool is_legal_position(string position)
{
    if (position == "GK")
        return true;

    if (position.size() != 3)
        return false;

    string raw_position = position.substr(0, 2);
    char side = position[2];

    return tact_manager().position_exists(raw_position) && is_legal_side(side);
}


bool is_legal_sign(string sign)
{
    if (sign == "=" || sign == ">=" || sign == "=>" ||
            sign == ">" || sign == "<=" || sign == "=<" ||
            sign == "<")
        return true;

    return false;
}


// true_sign_expression - given a, b and a sign (as a string)
//                        tells if "a sign b" is true. ie.
//                        (4, 1, ">") is true
//
bool true_sign_expression(int a, int b, string sign)
{
    if ((sign == "=" && a == b) ||
            ((sign == ">=" || sign == "=>") && a >= b) ||
            ((sign == "<=" || sign == "=<") && a <= b) ||
            (sign == "<" && a < b) ||
            (sign == ">" && a > b))
        return true;

    return false;
}


bool is_legal_player_number(int num)
{
    if (num >= 1 && num <= num_players)
        return true;

    return false;
}


bool is_legal_tactic(string tactic)
{
    return tact_manager().tactic_exists(tactic);
}


// Given a full position (like DML), get only
// the position (DM)
//
string fullpos2position(string fullpos)
{
    assert(fullpos.size() == 3);
    return fullpos.substr(0, 2);
}


// Given full position (like DML), get only
// the side (L)
//
char fullpos2side(string fullpos)
{
    assert(fullpos.size() == 3);
    return fullpos[2];
}


// Given a position (DM) and a side (L), returns the
// full position (DML)
//
string pos_and_side2fullpos(string pos, char side)
{
    string fullpos = pos;
    fullpos += side;
    return fullpos;
}
