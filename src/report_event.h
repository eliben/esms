// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#ifndef REPORT_EVENT_H
#define REPORT_EVENT_H

#include <string>


using namespace std;


class report_event
{
    public:
	report_event(string player_, string team_, string minute_) 
	   : player(player_), team(team_), minute(minute_) {}

	virtual string get_event(void) = 0;

	virtual ~report_event() {};
    protected:
	report_event();

	string player;
	string team;
	string minute;
};


class report_event_goal : public report_event
{
    public:
	report_event_goal(string player_, string team_, string minute_)
	    : report_event(player_, team_, minute_) {}

	virtual string get_event(void);

	virtual ~report_event_goal() {};
};


class report_event_penalty : public report_event
{
    public:
	report_event_penalty(string player_, string team_, string minute_)
	    : report_event(player_, team_, minute_) {}

	virtual string get_event(void);

	virtual ~report_event_penalty() {};
};


class report_event_red_card : public report_event
{
    public:
	report_event_red_card(string player_, string team_, string minute_)
	    : report_event(player_, team_, minute_) {}

	virtual string get_event(void);

	virtual ~report_event_red_card() {};
};



class report_event_injury : public report_event
{
    public:
	report_event_injury(string player_, string team_, string minute_)
	    : report_event(player_, team_, minute_) {}

	virtual string get_event(void);

	virtual ~report_event_injury() {};
};


#endif // REPORT_EVENT_H
