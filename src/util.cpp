// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
//
#include "util.h"
#include "config.h"
#include <cctype>
#include <cstdarg>
#include <functional>
#include <algorithm>

using namespace std;


extern bool waitflag;


// Extract a vector of tokens from a string (str) delimited by delims
//
vector<string> tokenize(string str, string delims)
{
    string::size_type start_index, end_index;
    vector<string> ret;

    // Skip leading delimiters, to get to the first token
    start_index = str.find_first_not_of(delims);

    // While found a beginning of a new token
    //
    while (start_index != string::npos)
    {
        // Find the end of this token
        end_index = str.find_first_of(delims, start_index);

        // If this is the end of the string
        if (end_index == string::npos)
            end_index = str.length();

        ret.push_back(str.substr(start_index, end_index - start_index));

        // Find beginning of the next token
        start_index = str.find_first_not_of(delims, end_index);
    }

    return ret;
}


// True if the given string consists only of whitespace
//
bool is_only_whitespace(string str)
{
    if (str.find_first_not_of(" \t\n\r") == string::npos)
        return true;

    return false;
}


// Printf an error message and exit
void die(const char* fmt, ...)
{
    va_list args;

    fflush(stdout);
    fprintf(stderr, "Error: ");

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
    MY_EXIT(2);
}


// Works like sprintf, but returns the resulting string in a
// memory-safe manner
//
string format_str(const char* format, ...)
{
    va_list arglist;
    va_start(arglist, format);
    char* buf = new char[4096];

#ifdef WIN32
	vsprintf_s(buf, 4095, format, arglist);
#else
    vsprintf(buf, format, arglist);
#endif

    string ret = buf;
    delete [] buf;
    return ret;
}


int str_atoi(string str)
{
    return atoi(str.c_str());
}


bool is_number(string str)
{
    if (find_if(str.begin(), str.end(), not1(ptr_fun(::isdigit))) != str.end())
        return false;

    return true;
}


void MY_EXIT(int rc)
{
    if (waitflag)
    {
        printf("\nPress Enter\n");
        getchar();
    }

    exit(rc);
}



