// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#ifndef COND_CONDITION_H
#define COND_CONDITION_H

#include <string>


using namespace std;


///////////////////////
//
// cond_condition
//
// A condition for some action to be taken
//
// create - creates the condition. Returns "" upon success
//          and an error string if the input is illegal
//
// is_true - is the condition true ? ran on each minute by
//           the game loop
//
class cond_condition
{
public:
    virtual string create(int team_num_, string data) = 0;
    virtual bool is_true(void) = 0;

    virtual ~cond_condition()
    {}
protected:

    int team_num;
};


// For conditions of the type:
//
// MIN <sign> <minute>
//
class cond_condition_minute : public cond_condition
{
public:
    virtual string create(int team_num_, string data);
    virtual bool is_true(void);

    virtual ~cond_condition_minute()
    {}
protected:
    int cond_minute;
    string sign;
};


// For conditions of the type:
//
// SCORE <sign> <score>
//
class cond_condition_score : public cond_condition
{
public:
    virtual string create(int team_num_, string data);
    virtual bool is_true(void);

    virtual ~cond_condition_score()
    {}
protected:
    int cond_score;
    string sign;
};


// For conditions of the type:
//
// INJ <player name/num>
//
class cond_condition_inj : public cond_condition
{
public:
    virtual string create(int team_num_, string data);
    virtual bool is_true(void);

    virtual ~cond_condition_inj()
    {}
protected:
    int player_num;
    string player_pos;
};


// For conditions of the type:
//
// YELLOW <player name/num>
//
class cond_condition_yellow : public cond_condition
{
public:
    virtual string create(int team_num_, string data);
    virtual bool is_true(void);

    virtual ~cond_condition_yellow()
    {}
protected:
    int player_num;
    string player_pos;
};


// For conditions of the type:
//
// RED <player name/num>
//
class cond_condition_red : public cond_condition
{
public:
    virtual string create(int team_num_, string data);
    virtual bool is_true(void);

    virtual ~cond_condition_red()
    {}
protected:
    int player_num;
    string player_pos;
};


#endif // COND_CONDITION_H
