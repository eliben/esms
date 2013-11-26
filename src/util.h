// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
//
#ifndef UTIL_H
#define UTIL_H


#include <vector>
#include <string>
#include <cstdio>
#include <fstream>


using namespace std;


typedef std::string::size_type str_index;


vector<string> tokenize(string str, string delims = " \t\r\n");
bool is_only_whitespace(string str);
int str_atoi(string str);
bool is_number(string str);
void die(const char *fmt, ...);
string format_str(const char* format, ...);
void MY_EXIT(int status);


// Prints all elements in a STL container.
// (useful for debugging).
//
// stream - where to print
// coll - the container
// delim - delimiter (between elements), a space by default
// prelude - an optional string to be printed before the elements
//
template <class T>
void print_elements(ofstream& stream, const T& coll, string delim = " ", string prelude = "")
{
    typename T::const_iterator pos;

    stream << prelude;

    for (pos = coll.begin(); pos != coll.end(); ++pos)
        stream << *pos << delim;

    stream << endl;
}

#endif // UTIL_H
