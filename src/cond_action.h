// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#ifndef COND_ACTION_H
#define COND_ACTION_H

#include <string>


using namespace std;

///////////////
//
// cond_action
//
// The action taken after fulfilled condition(s)
//
// create - creates the action. Returns "" upon success
//          and an error string if the input is illegal
//
// execute - executes the action (called when all conditions
//           for this actions are satisfied)
//
// is_legal_position & is_legal_number - check legality of
//           a position and player's number (from 1 to num_players)
//
// worst_player_on_pos - returns the number of the worst player
//                       on a given position
//
class cond_action
{
public:
    virtual string create(int team_num_, string data) = 0;
    virtual void execute(void) = 0;

    virtual ~cond_action()
    {}
protected:
    int worst_player_on_pos(string fullpos);

	int team_num;
};


// For actions of the type:
//
// TACTIC <new tactic>
//
class cond_action_tactic : public cond_action
{
public:
    virtual string create(int team_num_, string data);
    virtual void execute(void);

    virtual ~cond_action_tactic()
    {}
protected:
    string new_tactic;
};


// For actions of the type:
//
// CHANGEPOS <player> <new position>
//
// <player> may be either a number, a player name 
// or a position.
// If it's a position, we will pick the worse player
// at that position (ie. if it's DF, we pick the
// defender with the lowest tackling)
//
class cond_action_changepos : public cond_action
{
public:
    virtual string create(int team_num_, string data);
    virtual void execute(void);

    virtual ~cond_action_changepos()
    {}
protected:
    string new_pos;
    string player_pos;
    int player_number;
};


// For actions of the type:
//
// SUB <player out> <player in> <new position>
//
// <player out> may be either a number, a player name
// or a position.
// If it's a position, we will pick the worse player
// at that position (ie. if it's DF, we pick the
// defender with the lowest tackling)
//
class cond_action_sub : public cond_action
{
public:
    virtual string create(int team_num_, string data);
    virtual void execute(void);

    virtual ~cond_action_sub()
    {}
protected:
    string new_pos;
    string player_out_pos;
    int player_out_number;
    int player_in_number;
};

#endif // COND_ACTION_H
