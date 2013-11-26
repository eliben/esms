// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#ifndef TEAMSHEET_READER_H
#define TEAMSHEET_READER_H

#include <deque>
#include <string>

using namespace std;


/// Abstracts a teamsheet file
///
class teamsheet_reader
{
public:
	teamsheet_reader();
	string read_teamsheet(const string& teamsheet_name);

	bool end_of_teamsheet();

	/// Returns the current line, removing it from the store (the next grab/peek
	/// will access the next line).
	///
	string grab_line();

	/// Returns the current line without removing it (the next grab/peek will
	/// access the same line).
	///
	string peek_line();

private:
	deque<string> file_lines;
};



#endif // TEAMSHEET_READER_H
