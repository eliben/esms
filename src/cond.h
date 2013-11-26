// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#ifndef COND_H
#define COND_H


#include "cond_condition.h"
#include "cond_action.h"
#include <vector>


using namespace std;


/// An aggregate for conditionals.
///
/// The conditonal instructions ("conds") are given in the
/// teamsheet file for each playing team. Each line of the
/// conditional instructions from the teamsheet is translated
/// to a "cond" object that represents it internally - an
/// action + a list of conditions for this action.
///
/// The typical usage of a cond is: when the teamsheets are
/// read, a list of conds is created for each team. When the
/// simulation starts, on each minute all the conds of each
/// team are checked and if their conditions are satisfied,
/// their action is executed.
///
class cond
{
public:
    /// Create a cond from a line.
    ///
    /// A line has the following structure:
    /// action IF condition, condition, ...
    ///
    /// For example:
    ///
    /// TACTIC P IF SCORE >= 1, MIN >= 80
    ///
    /// Will create a cond with the action "TACTIC P"
    /// and two conditions: "SCORE >= 1" and "MIN >= 80"
    ///
    string create(int team_num_, string line);

    /// Executes the cond.
    ///
    /// A cond execution is: iff all the conditions
    /// are satisfied, execute the action.
    ///
    void test_and_execute(void);

private:
    /// The team number on which the cond is defined.
    ///
    int team_num;

    /// A list of conditions of a cond.
    ///
    vector<cond_condition*> conditions;

    /// The action of a cond.
    ///
    cond_action* action;

    /// Adds a condition to the list of cond's conditions.
    ///
    void add_condition(cond_condition* condition_);

    /// Sets the cond action.
    ///
    void set_action(cond_action* action_);
};



#endif // COND_H
