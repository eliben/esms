// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
//
#include <cstdio>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include "config.h"
#include "util.h"


// get a reference to a static config (a singleton)
//
config& the_config()
{
    static config tcfg;
    return tcfg;
}


void config::load_config_file(string filename)
{
    ifstream infile(filename.c_str());

    // Can't use the die facility here, because it
    // tries to look for WAIT_FOR_EXIT (via a call to EXIT)
    // and it hasn't yet been loaded
    //
    if (!infile)
    {
        cout << "Failed to open config file " << filename << endl;
        cout << "\nPress Enter" << endl;
        getchar();
        MY_EXIT(2);
    }

    // Clear previous config records
    //
    config_map.clear();

    string line;
    bool abbr_mode = false;

    while(getline(infile, line))
    {
        // Preprocess line - erase whitespace
        //
        line.erase(remove
                   (line.begin(), line.end(), ' '), line.end());
        line.erase(remove
                   (line.begin(), line.end(), '\t'), line.end());
        line.erase(remove
                   (line.begin(), line.end(), '\r'), line.end());

        vector<string> tokens = tokenize(line, "=");

        // Incorrect input is ignored
        //
        if (tokens.size() == 1)
        {
            // backward compatibility - AARGH
            //
            if (tokens[0] == "Abbreviations:" || tokens[0] == "Abbrevations:")
                abbr_mode = true;

            continue;
        }
        else if (tokens.size() == 2)
        {
            if (abbr_mode)
            {
                // abbreviations - precede by "abbr_"
                //
                tokens[0] = "abbr_" + tokens[0];
            }
            else
            {
                // others - transform to uppercase
                //
                int (*pf)(int) = toupper;
                transform(tokens[0].begin(), tokens[0].end(), tokens[0].begin(), pf);
            }
        }
        else
            continue;

        config_map[tokens[0]] = tokens[1];
    }

    return;
}


void config::set_config_value(string key, string value)
{
    config_map[key] = value;
}


string config::get_config_value(string key)
{
    if (config_map.find(key) == config_map.end())
        return "";
    else
        return config_map[key];
}


int config::get_int_config(string key, int dflt)
{
    if (config_map.find(key) == config_map.end())
        return dflt;
    else
        return atoi(config_map[key].c_str());
}

