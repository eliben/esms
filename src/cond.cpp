// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#include "cond.h"
#include "util.h"


string cond::create(int team_num_, string line)
{
    team_num = team_num_;

    str_index idx = line.find("IF");

    if (idx == string::npos)
        return "IF statement not found";

    // Everything on the left hand side of the IF is the action
    // and on the right hand side is the list of conditions

    string action_str = line.substr(0, idx);
    string conditions_str = line.substr(idx + 2);

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // First, deal with the action
    //
    // Just find out which action it is, and then handle control
    // to the appropriate cond_action (which will do its own
    // error checking)
    //

    vector<string> tok = tokenize(action_str);

    if (tok.size() == 0)
        return "Badly formed condition";

    cond_action* action;
    string msg = "";

    // Decide which action it is, and create an appropriate object
    //
    if (tok[0] == "TACTIC")
    {
        action = new cond_action_tactic;
        msg = action->create(team_num, action_str);
    }
    else if (tok[0] == "SUB")
    {
        action = new cond_action_sub;
        msg = action->create(team_num, action_str);
    }
    else if (tok[0] == "CHANGEPOS")
    {
        action = new cond_action_changepos;
        msg = action->create(team_num, action_str);
    }
    else
    {
        msg = "Unknown action specified: " + tok[0];
        action = 0;
    }

    if (msg != "")
        return msg;

    // If we're here, the action is legal, and we can set it
    // as this cond's action
    set_action(action);
    //   cout << "Set action: " << action_str << endl;

    // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Now, deal with the condition(s)
    //
    // Conditions are separated by commas

    vector<string> conds = tokenize(conditions_str, ",\n");

    // For each condition: decide which condition it is and
    // create an appropriate cond_condition object

    for (vector<string>::iterator cond_iter = conds.begin();
            cond_iter != conds.end(); ++cond_iter)
    {
        cond_condition* condition;
        tok = tokenize(*cond_iter);
        msg = "";

        if (tok[0] == "MIN")
        {
            condition = new cond_condition_minute;
            msg = condition->create(team_num, *cond_iter);
        }
        else if (tok[0] == "SCORE")
        {
            condition = new cond_condition_score;
            msg = condition->create(team_num, *cond_iter);
        }
        else if (tok[0] == "YELLOW")
        {
            condition = new cond_condition_yellow;
            msg = condition->create(team_num, *cond_iter);
        }
        else if (tok[0] == "RED")
        {
            condition = new cond_condition_red;
            msg = condition->create(team_num, *cond_iter);
        }
        else if (tok[0] == "INJ")
        {
            condition = new cond_condition_inj;
            msg = condition->create(team_num, *cond_iter);
        }
        else
        {
            msg = "Unknown condition specified: " + tok[0];
            condition = 0;
        }

        if (msg != "")
            return msg;

        // It looks OK, so add the condition to this cond
        //
        add_condition(condition);
        //      cout << "Added condition: " << *cond_iter << endl;
    }

    return "";
}


void cond::test_and_execute(void)
{
    vector<cond_condition*>::iterator iter;
    bool all_true = true;

    // If all conditions are true, execute the action
    //
    for (iter = conditions.begin(); iter != conditions.end(); ++iter)
    {
        if (!(*iter)->is_true())
            all_true = false;
    }

    if (all_true)
        action->execute();
}


void cond::add_condition(cond_condition* condition_)
{
    conditions.push_back(condition_);
}


void cond::set_action(cond_action* action_)
{
    action = action_;
}
