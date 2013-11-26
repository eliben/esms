// ESMS - Electronic Soccer Management Simulator
// Copyright (C) <1998-2005>  Eli Bendersky
//
// This program is free software, licensed with the GPL (www.fsf.org)
// 
#include <cstdlib>
#include "league_table.h"
#include "anyoption.h"


// wait on exit ?
//
bool waitflag = true;


//
// This is just a simple, independent driver for league_table
//

int main(int argc, char* argv[])
{
    // handling/parsing command line arguments
    //
    AnyOption* opt = new AnyOption();
    opt->noPOSIX();

    opt->setOption("work_dir");
    opt->setFlag("no_wait_on_exit");
    opt->setOption("table_file");
    opt->setOption("results_file");

    opt->processCommandArgs(argc, argv);

    string work_dir;

    if (opt->getFlag("no_wait_on_exit"))
        waitflag = false;

    if (opt->getValue("work_dir"))
        work_dir = opt->getValue("work_dir");
    else
        work_dir = "";

    string table_file;

    if (opt->getValue("table_file"))
        table_file = work_dir + opt->getValue("table_file");
    else
        table_file = work_dir + "table.txt";

    string results_file;

    if (opt->getValue("results_file"))
        results_file = work_dir + opt->getValue("results_file");
    else
        results_file = work_dir + "reports.txt";

    league_table table;

    cout << results_file << endl;

    table.read_league_table_file(table_file);
    table.read_results_file(results_file);

    string table_text = table.dump_league_table();

    ofstream tf(table_file.c_str());

    if (tf)
    {
        tf << table_text << endl;
        cout << "Table file " << table_file << " updated" << endl;
    }
    else
        die("Something went wrong opening %s for writing !", table_file.c_str());

    MY_EXIT(0);
    return 0;
}
