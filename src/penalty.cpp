// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#include "game.h"
#include "comment.h"


/* Variables used from @esms.c */
extern struct teams team[2];
extern FILE* comm;


/* maximum of 11 PK takers is possible for each team */
struct playerstruct PenaltyTaker[2][11];
int KickTakers[2]; /* num of penalty kick takers available for each team */
int PenScore[2] = {0};  /* penalties scored by teams 0 and 1 */


void RunPenaltyShootout(void)
{
    int nTeam, nPenaltyNum;    /* used in the main penalties loop */

    fprintf(comm, "\n%s\n", the_commentary().rand_comment("PENALTYSHOOTOUT").c_str());

    AssignPenaltyTakers();

    /* Main penalties loop 
       Each team takes 5 penalties (the shootout will stop if
       it will be obvious that one team won at some stage before
       the end, ie. one team leading 4-1 etc.) 
    */
    for (nPenaltyNum = 0; nPenaltyNum < 5; nPenaltyNum++)
    {
	for (nTeam = 0; nTeam <= 1; nTeam++)
	{
	    TakePenalty(nTeam, nPenaltyNum);
	    
	    /* Special condition for stopping the PKs 
	       Algorithm: if after a certain kick, the amount of kicks 
	       left for team A is less than the score B - A            
	       there is no point to proceed with the PKs, as           
	       team A loses anyway.                                    
	    */
	    if (((nTeam == 0) && ((GoalDiff() > 5 - nPenaltyNum)
				  || (-GoalDiff() > 4 - nPenaltyNum)))
		|| ((nTeam == 1) && ((GoalDiff() > 4 - nPenaltyNum)
				     || (-GoalDiff() > 4 - nPenaltyNum))))
		goto stopPKs;
	}
    }

  stopPKs:

    /* If there is still a draw after 5 penalties by each team,
       both teams will take one penalty at a time until one team
       will lead (after an even amount of shots was taken by each team).
    */
    if (GoalDiff() == 0)
    {
	/* Flag indicating when penalties can be stopped */
	int GameDecided = 0;

	/* Until the game is decided (one team leads after both teams
	   took the same amount of shots)
	*/
	while (!GameDecided)
	{
	    for (nTeam = 0; nTeam <= 1; nTeam++)
	    {
		TakePenalty(nTeam, nPenaltyNum);
	    }

	    /* Check if the shootout was decided */
	    if (GoalDiff() != 0)
	    {
		GameDecided = 1;
	    }
	    else
	    {
		GameDecided = 0;
	      
		/* prepare next players in each team */
		if (nPenaltyNum == KickTakers[0]-1)
		    nPenaltyNum = 0;
		else
		    nPenaltyNum++;
	    }
	}
    }

    if (GoalDiff() > 0)
	fprintf(comm, "\n%s", the_commentary().rand_comment("WONPENALTYSHOOTOUT", team[0].name).c_str());
    else
	fprintf(comm, "\n%s", the_commentary().rand_comment("WONPENALTYSHOOTOUT", team[1].name).c_str());
}

/* Returns the goal difference (negative if team 1 leads)
*/
int GoalDiff(void)
{
    return (PenScore[0] - PenScore[1]);
}


/* Assigns the players with the best Sh value (and who is active, means
** not injured and not suspended, and wasn't substituted) to be the penalty
** kick takers.
*/
void AssignPenaltyTakers()
{
    int nTeam, nTaker, nPlayer;      /* counters to loop through the teams */
    int max = 0, index = 0;          /* used for finding the best shooters */
    
    KickTakers[0] = KickTakers[1] = 0;

    for (nTeam = 0; nTeam <= 1; nTeam++)
    {
	for (nPlayer = 1; nPlayer <= num_players; nPlayer++)
	{
	    if (team[nTeam].player[nPlayer].active == 1)
		KickTakers[nTeam]++;
	}
    }

    /* each team will have an even number of PK takers */
    KickTakers[0] = KickTakers[1] = (KickTakers[0] > KickTakers[1] ? KickTakers[1] : KickTakers[0]);
    
    for (nTeam = 0; nTeam <= 1; nTeam++)
    {
	for (nTaker = 0; nTaker < KickTakers[nTeam]; nTaker++)     /* for each penalty taker */
	{
	    for (nPlayer = 1; nPlayer <= num_players; nPlayer++)
	    {
		if ((team[nTeam].player[nPlayer].sh > max) && (team[nTeam].player[nPlayer].active == 1))
		{
		    max = team[nTeam].player[nPlayer].sh;
		    index = nPlayer;
		}
	    }

	    /* copy the stats of the suitable player to PenaltyTaker */
	    strcpy(PenaltyTaker[nTeam][nTaker].name, team[nTeam].player[index].name);
	    PenaltyTaker[nTeam][nTaker].sh = team[nTeam].player[index].sh;
	    /* set active = 0, making sure that this player won't be chosen again */
	    team[nTeam].player[index].active = 0;

	    max = index = 0;
	}
    }
}


/* This function takes care of the penalties taken 
*/
void TakePenalty(int nTeam, int nPenaltyNum)
{
    fprintf(comm, "\n%s", the_commentary().rand_comment("PENALTY", PenaltyTaker[nTeam][nPenaltyNum].name).c_str());

    /* checking if a goal was scored */
    if (randomp(8000 + PenaltyTaker[nTeam][nPenaltyNum].sh*100 -
		team[!nTeam].player[team[!nTeam].current_gk].st*100))
    {
	fprintf(comm, "%s", the_commentary().rand_comment("GOAL").c_str());;

	PenScore[nTeam]++;
	fprintf(comm, "\n          ...  %s %d-%d %s...", team[0].name, PenScore[0],
		PenScore[1],  team[1].name);
    }
    else
    {
	int rnd = my_random(10);
	if (rnd < 5)
	    fprintf(comm, "%s", the_commentary().rand_comment("SAVE", 
							team[!nTeam].player[team[!nTeam].current_gk].name).c_str());
	else
	    fprintf(comm, "%s", the_commentary().rand_comment("OFFTARGET", 
							team[!nTeam].player[team[!nTeam].current_gk].name).c_str());
    }
}


