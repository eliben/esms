// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
//
#ifndef CONFIG_H
#define CONFIG_H


#include <map>
#include <string>

using namespace std;


///////////////////////
//
// config
//
// A configuration map, read from a config file (league.dat)
//
// Implemented as a Singleton. I.e. only instance exists
// at any given time, and should be accessed via the
// the_config() function. For this purporse, all config's
// constructing options are private and the_config is a
// friend returning a reference to a static config object
//
// load_config_file - loads a configuration map from a
//                    given file
//
// get_config_value - returns a value associated with a key,
//                    or "" if the key doesn't exist
//
// get_int_config - returns an integer value associated with
//                  a key (must have numeric value), or a
//                  default if the key doesn't exist
class config
{
public:
    void load_config_file(string filename);
    string get_config_value(string key);
    void set_config_value(string key, string value);
    int get_int_config(string key, int dflt);

    friend config& the_config();
private:
    config()
    {}
    config(const config& rhs);
    config& operator= (const config& rhs);

    map<string, string> config_map;
};


config& the_config();


#endif // CONFIG_H

