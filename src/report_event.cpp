// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#include "report_event.h"


string report_event_goal::get_event(void)
{
    string event = player + " (" + team + ") " + minute + "'\n";

    return event;
}


string report_event_penalty::get_event(void)
{
    string event = player + " (" + team + ") " + minute + "' pen\n";

    return event;
}


string report_event_red_card::get_event(void)
{
    string event = "Red: " + player + " (" + team + ") " + minute + "'\n";

    return event;
}


string report_event_injury::get_event(void)
{
    string event = "Inj: " + player + " (" + team + ") " + minute + "'\n";

    return event;
}
