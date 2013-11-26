// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#include <vector>
#include <cstdlib>
#include "cond_condition.h"
#include "cond_utils.h"
#include "util.h"
#include "game.h"



///////////////////////////////////////////////////////////////////


string cond_condition_minute::create(int team_num_, string data)
{
    team_num = team_num_;

    vector<string> tok = tokenize(data);

    // Data should be of the form "MIN <sign> <minute>"
    if (tok.size() != 3)
        return "A sign and minute should follow MIN";

    if (is_legal_sign(tok[1]))
        sign = tok[1];
    else
        return "Invalid sign: " + tok[1];

    char* ptr;
    int int_minute = strtol(tok[2].c_str(), &ptr, 10);

    if (ptr == tok[2].c_str() || int_minute < 1 || int_minute > 90)
    {
        return "Invalid minute: " + tok[2];
    }

    cond_minute = int_minute;

    return "";
}


bool cond_condition_minute::is_true(void)
{
    extern int minute;  // from esms.cpp

    return true_sign_expression(minute, cond_minute, sign);
}


///////////////////////////////////////////////////////////////////


string cond_condition_score::create(int team_num_, string data)
{
    team_num = team_num_;

    vector<string> tok = tokenize(data);

    // Data should be of the form "SCORE <sign> <score>"
    if (tok.size() != 3)
        return "A sign and score should follow SCORE";

    if (is_legal_sign(tok[1]))
        sign = tok[1];
    else
        return "Invalid sign: " + tok[1];

    char* ptr;
    int int_score = strtol(tok[2].c_str(), &ptr, 10);

    if (ptr == tok[2].c_str())
    {
        return "Invalid score: " + tok[2];
    }

    cond_score = int_score;

    return "";
}


bool cond_condition_score::is_true(void)
{
    extern int score_diff;  // from esms.cpp

    return true_sign_expression(score_diff, cond_score, sign);
}


///////////////////////////////////////////////////////////////////


string cond_condition_yellow::create(int team_num_, string data)
{
    // Note: if a position is specified, it means any player
    // on that position. Otherwise, an exact player number is
    // given, and the player_pos is set to ""
    //
    team_num = team_num_;

    vector<string> tok = tokenize(data);

    // Data should be of the form "YELLOW <player num / position>"
    if (tok.size() != 2)
        return "A player number / position should follow YELLOW";

    if (is_legal_position(tok[1]))
    {
        player_pos = tok[1];
    }
    else
    {
		player_num = action_str_to_player_number(team_num, tok[1]);
		
		if (player_num < 0)
			return "Invalid player name/number: " + tok[1];

        player_pos = "";
    }

    return "";
}


bool cond_condition_yellow::is_true(void)
{
    extern int yellow_carded[2];
    extern struct teams team[2];

    // Were any yellow cards given to our team on this minute ?
    if (yellow_carded[team_num] == -1)
        return false;

    // If a position was specified ...
    if (player_pos != "")
    {
        if (player_pos == team[team_num].player[yellow_carded[team_num]].pos)
            return true;
    }
    else
    {
        if (yellow_carded[team_num] == player_num)
            return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////


string cond_condition_red::create(int team_num_, string data)
{
    // Note: if a position is specified, it means any player
    // on that position. Otherwise, an exact player number is
    // given, and the player_pos is set to ""
    //
    team_num = team_num_;

    vector<string> tok = tokenize(data);

    // Data should be of the form "RED <player num / position>"
    if (tok.size() != 2)
        return "A player number / position should follow RED";

    if (is_legal_position(tok[1]))
    {
        player_pos = tok[1];
    }
    else
    {
		player_num = action_str_to_player_number(team_num, tok[1]);
		
		if (player_num < 0)
			return "Invalid player name/number: " + tok[1];

        player_pos = "";
    }

    return "";
}


bool cond_condition_red::is_true(void)
{
    extern int red_carded[2];
    extern struct teams team[2];

    // Were any red cards given to our team on this minute ?
    if (red_carded[team_num] == -1)
        return false;

    // If a position was specified ...
    if (player_pos != "")
    {
        if (player_pos == team[team_num].player[red_carded[team_num]].pos)
            return true;
    }
    else
    {
        if (red_carded[team_num] == player_num)
            return true;
    }

    return false;
}


///////////////////////////////////////////////////////////////////

string cond_condition_inj::create(int team_num_, string data)
{
    // Note: if a position is specified, it means any player
    // on that position. Otherwise, an exact player number is
    // given, and the player_pos is set to ""
    //
    team_num = team_num_;

    vector<string> tok = tokenize(data);

    // Data should be of the form "INJ <player num / position>"
    if (tok.size() != 2)
        return "A player number / position should follow INJ";

    if (is_legal_position(tok[1]))
    {
        player_pos = tok[1];
    }
    else
    {
		player_num = action_str_to_player_number(team_num, tok[1]);
		
		if (player_num < 0)
			return "Invalid player name/number: " + tok[1];

        player_pos = "";
    }

    return "";
}


bool cond_condition_inj::is_true(void)
{
    extern int injured_ind[2];
    extern struct teams team[2];

    // Did anyone get injured on this minute ?
    if (injured_ind[team_num] == -1)
        return false;

    // If a position was specified ...
    if (player_pos != "")
    {
        if (player_pos == team[team_num].player[injured_ind[team_num]].pos)
            return true;
    }
    else
    {
        if (injured_ind[team_num] == player_num)
            return true;
    }

    return false;
}
