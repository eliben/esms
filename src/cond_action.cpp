// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#include <climits>
#include <iostream>
#include "cond_action.h"
#include "cond_utils.h"
#include "util.h"
#include "game.h"


using namespace std;


extern struct teams team[2];


// returns the number of the worst player
//
// if no players were on the given position at all, returns -1
//
int cond_action::worst_player_on_pos(string fullpos)
{
    string position = fullpos2position(fullpos);

    // The "worst" player is the player with the lowest skill for
    // his position. For example, if position = "DF", we return
    // the number of the defender with the lowest Tackling
    //
    int worst_player_num = -1;
    double worst_player_skill = INT_MAX;

    for (int i = 1; i <= num_players; ++i)
    {
        string player_i_fullpos = pos_and_side2fullpos(team[team_num].player[i].pos,
                                  team[team_num].player[i].side);

        // not interested in inactive players and players on other
        // positions
        //
        if (team[team_num].player[i].active != 1 || fullpos != player_i_fullpos)
            continue;

        if (position == "GK")
        {
            if (team[team_num].player[i].st < worst_player_skill)
            {
                worst_player_num = i;
                worst_player_skill = team[team_num].player[i].st;
            }
        }
        else if (position == "DF")
        {
            if (team[team_num].player[i].tk < worst_player_skill)
            {
                worst_player_num = i;
                worst_player_skill = team[team_num].player[i].tk_contrib;
            }
        }
        else if (position == "MF")
        {
            if (team[team_num].player[i].ps < worst_player_skill)
            {
                worst_player_num = i;
                worst_player_skill = team[team_num].player[i].ps_contrib;
            }
        }
        else if (position == "FW")
        {
            if (team[team_num].player[i].sh < worst_player_skill)
            {
                worst_player_num = i;
                worst_player_skill = team[team_num].player[i].sh_contrib;
            }
        }
        else
        {
            cout << "Internal error, " << __FILE__ << ", line " << __LINE__ << endl;
        }
    }

    return worst_player_num;
}


///////////////////////////////////////////////////////////////////




string cond_action_tactic::create(int team_num_, string data)
{
    team_num = team_num_;

    vector<string> tok = tokenize(data);

    if (tok.size() != 2)
        return "expecting TACTIC <new tactic>";

    if (is_legal_tactic(tok[1]))
        new_tactic = tok[1];
    else
        return "Invalid tactic: " + tok[1];

    return "";
}


void cond_action_tactic::execute(void)
{
    change_tactic(team_num, new_tactic.c_str());
}


///////////////////////////////////////////////////////////////////


string cond_action_changepos::create(int team_num_, string data)
{
    // Note: if a position is specified, it will be put in
    // player_pos. if a number is specified, "" will be put
    // in player_pos, and the number in player_number
    //
    team_num = team_num_;

    vector<string> tok = tokenize(data);

    if (tok.size() != 3)
        return "expecting CHANGEPOS <position / number> <new position>";

    // if a position was specified
    if (is_legal_position(tok[1]))
    {
        player_pos = tok[1];
    }
    else
    {
		player_number = action_str_to_player_number(team_num, tok[1]);
		
		if (player_number < 0)
			return "Invalid player name/number: " + tok[1];

        player_pos = "";
    }

    // now check and record the new position
    if (is_legal_position(tok[2]))
    {
        new_pos = tok[2];
    }
    else
        return "Invalid new position : " + tok[2];

    return "";
}


void cond_action_changepos::execute(void)
{
    // If a position was specified, look for the worst player on that position
    // and changepos him
    //
    if (player_pos != "")
    {
        int num = worst_player_on_pos(player_pos);

        if (num != -1)
            change_position(team_num, num, new_pos);
    }
    // If a number was specified, just changepos this player
    else
    {
        change_position(team_num, player_number, new_pos);
    }
}


///////////////////////////////////////////////////////////////////


string cond_action_sub::create(int team_num_, string data)
{
    // Note: if a position is specified, it will be put in
    // player_out_pos. if a number is specified, "" will be put
    // in player_out_pos, and the number in player_out_number
    //
    team_num = team_num_;

    vector<string> tok = tokenize(data);

    if (tok.size() != 4)
        return "expecting SUB <position / number out> <number in> <new position>";

    // if a position was specified
    if (is_legal_position(tok[1]))
    {
        player_out_pos = tok[1];
    }
    else
    {
		player_out_number = action_str_to_player_number(team_num, tok[1]);
		
		if (player_out_number < 0)
			return "Invalid player name/number : " + tok[1];

        player_out_pos = "";
    }

    // check and record the number of player_in
    player_in_number = action_str_to_player_number(team_num, tok[2]);

    if (player_in_number < 0)
        return "Invalid player name/number : " + tok[2];

    // now check and record the new position
    if (is_legal_position(tok[3]))
    {
        new_pos = tok[3];
    }
    else
        return "Invalid new position : " + tok[3];

    return "";
}


void cond_action_sub::execute(void)
{
    // If a position was specified, look for the worst player on that position
    // and sub
    //
    if (player_out_pos != "")
    {
        int num = worst_player_on_pos(player_out_pos);

        if (num != -1)
            substitute_player(team_num, num, player_in_number, new_pos);
    }
    // If a number was specified, just sub this player
    else
    {
        substitute_player(team_num, player_out_number, player_in_number, new_pos);
    }
}

