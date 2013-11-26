// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#ifndef TACTICS_H
#define TACTICS_H


#include <map>
#include <cassert>
#include <string>


using namespace std;

typedef pair<string, string> str_pair;
typedef map<str_pair, double > mult_matrix_t;


///////////////////////
//
// tactics_manager
//
// Handles players' positions and team tactics, and 
// their multipliers for probability calculations. First, init is 
// called and all information is read from a file. Then,
// the desired multipliers are accessed using get_mult
//
// Implemented as a Singleton
//
class tactics_manager
{
    public:
	void init(const string filename);
	double get_mult(const string tactic, const string opp_tactic, 
			const string pos, const string skill);

	bool tactic_exists(const string tactic);
	bool position_exists(const string position);
	bool skill_exists(const string skill);

	string get_tactic_full_name(string name)
	{
	    assert(tactic_exists(name));
	    return tactic_full_name[name];
	}

	const vector<string>& get_positions_names(void)
	{
	    return positions_names;
	}

	friend tactics_manager& tact_manager();

    private:
	tactics_manager() {}
	tactics_manager(const tactics_manager& rhs);
	tactics_manager& operator= (const tactics_manager& rhs);
	
	// Aux methods
	//
	void set_multipliers(void);
	void ensure_no_uninits(void);

	// full names
	map<string, string> tactic_full_name;
	
	// tactics
	vector<string> tactics_names;

	// positions
	vector<string> positions_names;

	// skills
	vector<string> skills_names;
	
	// Holds the lines of MULT and BONUS, sorted with MULTs first
	vector<string> mult_lines;
	
	// The main data structure - holds all the multipliers
	map<str_pair, mult_matrix_t> tact_matrix;
};


// Get the singleton instance
//
tactics_manager& tact_manager();


bool mult_lines_predicate(string s1, string s2);

#endif // TACTICS_H
