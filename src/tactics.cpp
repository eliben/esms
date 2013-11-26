// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#include <vector>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <cassert>

#include "tactics.h"
#include "util.h"


// Un-initialized multiplier
const double UNINIT = -1.0;


// get a reference to a static tactics_manager (a singleton)
//
tactics_manager& tact_manager()
{
    static tactics_manager tmng;
    return tmng;
}


void tactics_manager::init(const string filename)
{
    ifstream tactfile(filename.c_str());

    if (!tactfile)
	die("Failed to open tactics.dat");

    string line;

    // The TACTICS line should come before all MULT and BONUS
    // line: 
    // - When a TACTICS line is read, this flag is turned on
    // - When an other line is read and the flag is not on,
    // an error is reported
    //
    bool found_tactics_line = false;

    // --1-- Read the tactics file and initialize tact_matrix
    //
    // At this stage, incorrect input (which may include user comments)
    // is ignored
    //
    while(getline(tactfile, line))
    {
	vector<string> tokens = tokenize(line);

	if (tokens.size() == 0)
	    continue;

	// Declaring available tactics. Names of tactics expected
	//
	if (tokens[0] == "TACTIC")
	{
	    found_tactics_line = true;

	    if (tokens.size() < 2 || tokens.size() > 3)
		die("Illegal TACTIC declaration in %s", filename.c_str());

	    tactics_names.push_back(tokens[1]);

	    if (tokens.size() == 3)
	    {
		string fullname = tokens[2];
		replace(fullname.begin(), fullname.end(), '_', ' ');
		tactic_full_name[tokens[1]] = fullname;
	    }
	    else
	    {
		tactic_full_name[tokens[1]] = tokens[1];
	    }
	}
	// Defining multipliers 
	//
	else if (tokens[0] == "MULT")
	{
	    if (!found_tactics_line)
		die("TACTIC declarations must come first in %s", filename.c_str());

	    mult_lines.push_back(line);
	}
	// Defining bonuses
	//
	else if (tokens[0] == "BONUS")
	{
	    if (!found_tactics_line)
		die("TACTICS declaration must come first in %s", filename.c_str());

	    mult_lines.push_back(line);
	}
	// ignore anything else
	else
	    continue;
    }

    // Initialize skills and positions
    // This is pre-defined (for now)
    //
    positions_names.push_back("DF");
    positions_names.push_back("DM");
    positions_names.push_back("MF");
    positions_names.push_back("AM");
    positions_names.push_back("FW");
    skills_names.push_back("TK");
    skills_names.push_back("PS");
    skills_names.push_back("SH");

    // --2-- Initialize the tact_matrix
    // For all pairs of tactics, set all multipliers to UNINIT
    //

    // First make sure that the tactics names are unique
    //
    sort(tactics_names.begin(), tactics_names.end());
    tactics_names.erase(unique(tactics_names.begin(), tactics_names.end()), tactics_names.end());

    vector<string>::const_iterator i, j;    
    mult_matrix_t mult_matrix;

    // Initialize a mult_matrix full of UNINITs for all positions
    // and skills
    //
    for (i = positions_names.begin(); i != positions_names.end(); ++i)
	for (j = skills_names.begin(); j != skills_names.end(); ++j)
	    mult_matrix[str_pair(*i, *j)] = UNINIT;
  
    // For all pairs of tactics, initialize each mult_matrix
    // with a mult_matrix of UNINITs
    //
    for (i = tactics_names.begin(); i != tactics_names.end(); ++i)
	for (j = tactics_names.begin(); j != tactics_names.end(); ++j)
	    tact_matrix[str_pair(*i, *j)] = mult_matrix;

    // Sort mult_lines - all MULTs must come before all BONUSes
    //
    sort(mult_lines.begin(), mult_lines.end(), mult_lines_predicate);

    // --3-- Set the multipliers from mult_lines
    // 
    set_multipliers();

    // --4-- Make sure all multipliers were initialized (the user
    // may have forgot to specify some of the multipliers in 
    // tactics.dat)
    //
    ensure_no_uninits();
}


bool tactics_manager::tactic_exists(const string tactic)
{
    return find(tactics_names.begin(), tactics_names.end(), tactic) != tactics_names.end();
}


bool tactics_manager::position_exists(const string position)
{
    return find(positions_names.begin(), positions_names.end(), position) != positions_names.end();
}


bool tactics_manager::skill_exists(const string skill)
{
    return find(skills_names.begin(), skills_names.end(), skill) != skills_names.end();
}


// Parses mult_lines and sets multipliers in tact_matrix
//
void tactics_manager::set_multipliers(void)
{
    vector<string>::const_iterator line_i;

    // For each of mult_lines
    //
    for (line_i = mult_lines.begin(); line_i != mult_lines.end(); ++line_i)
    {
	vector<string> tokens = tokenize(*line_i);

	// Handle a MULT line
	//
	// MULT <tactic> <pos> <skill> <multiplier>
        //
	if (tokens.size() == 5 && tokens[0] == "MULT")
	{
	    if (!tactic_exists(tokens[1])) 
		die("In tactics file: In line:\n%s\n%s - tactic doesn't exist", 
			  line_i->c_str(), tokens[1].c_str());

	    if (!position_exists(tokens[2]))
		die("In tactics file: In line:\n%s\n%s - position doesn't exist", 
			  line_i->c_str(), tokens[2].c_str());

	    if (!skill_exists(tokens[3]))
		die("In tactics file: In line:\n%s\n%s - skill doesn't exist", 
			  line_i->c_str(), tokens[3].c_str());
	    
	    char* ptr;
	    double value = strtod(tokens[4].c_str(), &ptr);

	    // not a legal double ?
	    if (*ptr)
		die("In tactics file, illegal value in line:\n%s", line_i->c_str());

	    // As this is a MULT, assign the value against every tactic
	    //
	    for (vector<string>::const_iterator tact_i = tactics_names.begin(); 
		 tact_i != tactics_names.end(); ++tact_i)
	    {
		(tact_matrix[str_pair(tokens[1], *tact_i)])[str_pair(tokens[2], tokens[3])] = value;
	    }
	}
	// Handle a BONUS line
	//
	// BONUS <tactic> <opp_tactic> <pos> <skill> <bonus>
	//
	else if (tokens.size() == 6 && tokens[0] == "BONUS")
	{
	    if (!tactic_exists(tokens[1])) 
		die("In tactics file: In line:\n%s\n%s - tactic doesn't exist", 
			  line_i->c_str(), tokens[1].c_str());

	    if (!tactic_exists(tokens[2])) 
		die("In tactics file: In line:\n%s\n%s - tactic doesn't exist", 
			  line_i->c_str(), tokens[2].c_str());

	    if (!position_exists(tokens[3]))
		die("In tactics file: In line:\n%s\n%s - position doesn't exist", 
			  line_i->c_str(), tokens[3].c_str());

	    if (!skill_exists(tokens[4]))
		die("In tactics file: In line:\n%s\n%s - skill doesn't exist", 
			  line_i->c_str(), tokens[4].c_str());
	
	    char* ptr;
	    double value = strtod(tokens[5].c_str(), &ptr);

	    // not a legal double ?
	    if (*ptr)
		die("In tactics file, illegal value in line:\n%s", line_i->c_str());

	    // As this is a BONUS, add the value to the designated value 
	    // Don't add if the multiplier hasn't been set. It is an error
	    // that will be caught later
	    //
	    if ((tact_matrix[str_pair(tokens[1], tokens[2])])[str_pair(tokens[3], tokens[4])] != UNINIT)
		(tact_matrix[str_pair(tokens[1], tokens[2])])[str_pair(tokens[3], tokens[4])] += value;
	}
	// Else it can only be a MULT or BONUS with an illegal amount of arguments,
	// because lines that aren't MULT or BONUS were filtered out earlier
	//
	else
	    die("In tactics file, wrong number of arguments in line:\n%s", line_i->c_str());
    }
}


void tactics_manager::ensure_no_uninits(void)
{
    string error = "";

    vector<string>::const_iterator tact_i, tact_j, pos_i, skill_i;

    // Go over all tactic-opp_tactic-pos-skill combination
    // Quad vector iterators loop - FUN !!
    //
    for (tact_i = tactics_names.begin(); tact_i != tactics_names.end(); ++tact_i)
	for (tact_j = tactics_names.begin(); tact_j != tactics_names.end(); ++tact_j)
	    for (pos_i = positions_names.begin(); pos_i != positions_names.end(); ++ pos_i)
		for (skill_i = skills_names.begin(); skill_i != skills_names.end(); ++skill_i)
		{
		    if ((tact_matrix[str_pair(*tact_i, *tact_j)])[str_pair(*pos_i, *skill_i)] == UNINIT)
		    {
			error += *tact_i + " " + *tact_j + " " + *pos_i + " " + *skill_i + "\n";
		    }
		}

    if (error != "")
	die("In tactics file, the following multipliers are missing:\n%100s", error.c_str());
}


double tactics_manager::get_mult(const string tactic, const string opp_tactic, 
				 const string pos, const string skill)
{
    double mult = (tact_matrix[str_pair(tactic, opp_tactic)])[str_pair(pos, skill)];

    assert(tactic_exists(tactic));
    assert(tactic_exists(opp_tactic));
    assert(position_exists(pos));
    assert(skill_exists(skill));
    assert(mult != UNINIT);

    return mult;
}


// Predicate for sorting
//
// MULT lines are "smaller" than (should come before) BONUS lines
//
// Assumption: a line is either MULT or BONUS, other lines
// are filtered away before reaching this function
//
bool mult_lines_predicate(string s1, string s2)
{
    string::size_type is1mult = s1.find("MULT");
    string::size_type is2bonus = s2.find("BONUS");

    if (is1mult != string::npos && is2bonus != string::npos)
	return true;
    else
	return false;
} 

