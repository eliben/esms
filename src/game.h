// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#ifndef GAME_H
#define GAME_H


/* Header files used by the main program */
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cctype>
#include <string>
#include <cstring>
#include <cerrno>
#include <climits>

using namespace std;


#include "rosterplayer.h"
#include "teamsheet_reader.h"
#include "cond.h"


extern int num_players;


/* Bookings control */
#define YELLOW 1
#define RED 2

const unsigned CHAR_BUF_LEN = 256;


// Represents a player during the simulation
//
struct playerstruct
{
	char name[CHAR_BUF_LEN];

	// contains the 2-char position (w/o side)
	char pos[3];

	// 1-char side (L, R, C)
	char side;

	char pref_side[CHAR_BUF_LEN];
	int st;
	int tk;
	int ps;
	int sh;
	int ag;
	int stamina;
	int injury;
	int suspension;

	// we don't want to calculate it every time...
	bool likes_left;
	bool likes_right;
	bool likes_center;

	// These are used only in the game running phase
	double tk_contrib;
	double ps_contrib;
	double sh_contrib;
	double nominal_fatigue_per_minute;
	double fatigue;
	int injured;     // 0 - no; 1 - yes (For the updater)

	int active;      // Status: 0 - unavailable; 1 - playing  2 - available for substitution

	// final stats
	int minutes;
	int shots;
	int goals;
	int saves;
	int tackles;
	int keypasses;
	int assists;
	int fouls;
	int yellowcards;
	int redcards;

	// Auxiliary, used for AB calculation
	//
	int shots_on;
	int shots_off;
	int conceded;

	int st_ab;     /* The ability change of the player in the game */
	int tk_ab;
	int ps_ab;
	int sh_ab;
};


// Represents a team during simulation
//
struct teams
{
	int score;
	int finalshots_on, finalshots_off, finalfouls;
	int substitutions; /* Number of substitutions made by a team */
	int injuries;
	int aggression;
	double shot_prob;

	double team_tackling;
	double team_passing;
	double team_shooting;

	char name[CHAR_BUF_LEN];
	char fullname[CHAR_BUF_LEN];
	char tactic[2];

	// If this is -1, the team has no preselected PK taker (the best shooter will
	// take the penalties). Otherwise, this is the number of the PK taker as 
	// specified in the teamsheet.
	//
	int penalty_taker;
	
	int current_gk;         
	struct playerstruct player[25];      
	
	RosterPlayerArray roster_players;

	vector<cond*> conds;
};


/**** Declaration of functions in use (from file esms.cpp) ***
*/
void init_teams_data(teamsheet_reader teamsheet[2]);
void ensure_no_duplicate_names(void);
void read_conditionals(teamsheet_reader teamsheet[2]);
void print_starting_tactics(void);
void calc_team_contributions_total(int a);
void calc_aggression(int a);
void calc_player_contributions(int a,int b);
void recalculate_teams_data(void);
void substitute_player(int a, int out, int in, string newpos);
void change_tactic(int a, const char* newtct);
void change_position(int a, int b, string newpos);
void calc_shotprob(int a);
void random_injury(int a);
void if_shot(int a);
int  if_ontarget(int a, int b);
int  if_goal(int a, int b);
int  is_goal_cancelled(void);
void if_foul(int a);
void bookings(int a, int b, int card_color);
void send_off(int a, int b);
void calc_ability(void);
int  randomp(int p);
unsigned  my_random(int n);
void clean_inj_card_indicators(void);
void update_players_minute_count(void);
void add_team_stats_total(void);
void print_final_stats(void);      
void create_stats_file(string);   
void update_reports_file(string); 
void cond_error(int team_num, int line, string msg);
void check_conditionals(int team_num);

int who_got_assist(int team, int assister);

// For the who_did_it function
//
enum DID_WHAT {DID_SHOT, DID_FOUL, DID_TACKLE, DID_ASSIST};

int who_did_it(int team, DID_WHAT event);

/* Functions implemented in penalty.cpp
*/
int GoalDiff(void);
void RunPenaltyShootout(void);
void AssignPenaltyTakers(void);
void TakePenalty(int, int);


#endif /* GAME_H */


