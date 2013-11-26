// ESMS - Electronic Soccer Management Simulatorifstream infile(filename.c_str());
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
//
#include "teamsheet_reader.h"
#include "util.h"
#include <fstream>


teamsheet_reader::teamsheet_reader()
{
}


string teamsheet_reader::read_teamsheet(const string& teamsheet_name)
{
	ifstream infile(teamsheet_name.c_str());
	
	if (!infile)
		return "Failed to open teamsheet " + teamsheet_name;
	
	file_lines.clear();
	string line;
	
	while(getline(infile, line))
	{
		if (!is_only_whitespace(line))
			file_lines.push_back(line);
	}

	return "";
}


bool teamsheet_reader::end_of_teamsheet()
{
	return file_lines.empty();
}


string teamsheet_reader::grab_line()
{
	if (end_of_teamsheet())
		return "";
	else
	{
		string front_line = file_lines.front();
		file_lines.pop_front();
		return front_line;
	}
}


string teamsheet_reader::peek_line()
{
	if (end_of_teamsheet())
		return "";
	else
		return file_lines.front();
}
