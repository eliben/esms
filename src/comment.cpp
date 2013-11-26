// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <cstdarg>
#include <algorithm>

#include "comment.h"
#include "util.h"

using namespace std;


// get a reference to a static commentary (a singleton)
//
commentary& the_commentary(void)
{
    static commentary tcfg;
    return tcfg;
}


// Initializes commentary data, reading it from the language.dat file
//
void commentary::init_commentary(string language_file)
{
    ifstream infile(language_file.c_str());

    if (!infile)
        die("Failed to open language.dat");

    string line;

    // Read language.dat line by line, updating the
    // commentary database
    //
    while(getline(infile, line))
    {
        string event, comment;
        string::size_type index1, index2;

        // Locate the event
        //
        index1 = line.find_first_of("[");
        index2 = line.find_first_of("]");
        
        if ((index1 == string::npos) || (index2 == string::npos))
            continue;

        event = line.substr(index1 + 1, index2 - index1 - 1);

        // Locate the commentary
        //
        index1 = line.find_first_of("{");
        index2 = line.find_first_of("}");

        if ((index1 == string::npos) || (index2 == string::npos))
            continue;

        comment = line.substr(index1 + 1, index2 - index1 - 1);

        // Add line to the commentary database 
        //
        comm_data[event].push_back(comment);
    }
}


string commentary::rand_comment(const char* event, ...)
{
    va_list arglist;
    va_start(arglist, event);

    // How many commentary lines of this type are available ?
    //
    string str_event = event;
    int num_of_choices = comm_data[str_event].size();

    if (num_of_choices < 1)
        die("No commentary choices found for event %s", event);

    // Pick one of the possible commentaries randomly
    //
    int choice_num = rand() % num_of_choices;
    string comm_format = comm_data[str_event][choice_num];

    char* buf = new char[4096];
    vsprintf(buf, comm_format.c_str(), arglist);

    string ret;

    // Convert the '\n's in the string into "real" newlines
    // which are recognized by the printing functions
    //
    char* s1 = buf;

    for (; *s1; ++s1)
    {
        if ((*s1 == '\\') && (*(s1+1) == 'n'))
        {
            ret += '\n';
            ++s1;
        }
        else
            ret += *s1;
    }

    delete [] buf;
    return ret;
}


