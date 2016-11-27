/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <queue>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeIndex.h"

using namespace std;

// external functions and variables for load file and sql command parsing
extern FILE* sqlin;
int sqlparse(void);


RC SqlEngine::run(FILE* commandline)
{
  fprintf(stdout, "Bruinbase> ");

  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
               // SqlParser.y by bison (bison is GNU equivalent of yacc)

  return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
    RecordFile rf;   // RecordFile containing the table
    RecordId   rid;  // record cursor for table scanning

    RC     rc;
    int    key;
    string value;
    int    count;
    int    diff;

    // open the table file
    if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
      fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
      return rc;
    }

    // attempt to open the corresponding index file
    BTreeIndex b;
    IndexCursor c; // need this for b.locate later on
    if ( b.open(table + ".idx", 'r') == 0) {
        puts("Associated index file does exist!");
        rid.pid = 0;
        rid.sid = 0;

        bool condition_equal = false;
        int condition_equal_val = -999;

        bool condition_min = false;
        int condition_min_val = 0;

        bool condition_max = false;
        int condition_max_val = -999;

        //int absolute_smallest_key = 0;

        int tmp;
        bool canTerminate = false;
        count = 0;
        for (unsigned i = 0; i < cond.size(); i++) {
            if (cond[i].attr == 1) {
                tmp = atoi(cond[i].value);
                // if (absolute_smallest_key == -999) {
                //     absolute_smallest_key = tmp;
                // } else {
                //     absolute_smallest_key = min(tmp, absolute_smallest_key);
                // }

                switch (cond[i].comp) {
                    case SelCond::EQ:
                    if (condition_equal && condition_equal_val != atoi(cond[i].value)) {
                        puts("two unique key equal statements. terminate now");
                        canTerminate = true;
                        goto skip_to_end;
                    }
                    condition_equal = true;
                    condition_equal_val = atoi(cond[i].value);
                    break;

                    case SelCond::NE:
                    break;

                    case SelCond::GT:
                    condition_min = true;
                    // logic:
                    // if we already seen key>3, then condition_min_val = 4
                    // if next we run into key > 5, then tmp = 6
                    // new condition_min_val = max(4,6)
                    tmp = atoi(cond[i].value) + 1;
                    condition_min_val = max(tmp, condition_min_val);
                    break;

                    case SelCond::LT:
                    condition_max = true;
                    // logic:
                    // if we already seen key < 5, then condition_max_val = 4
                    // if next we run into key < 3, then tmp = 2
                    // new condition_max_val = min(4,2)
                    tmp = atoi(cond[i].value) - 1;
                    if (condition_max_val == -999) {
                        condition_max_val = tmp;
                    } else {
                        condition_max_val = min(tmp, condition_max_val);
                    }
                    break;

                    case SelCond::GE:
                    condition_min = true;
                    tmp = atoi(cond[i].value);
                    condition_min_val = max(tmp, condition_min_val);
                    break;

                    case SelCond::LE:
                    condition_max = true;
                    tmp = atoi(cond[i].value);
                    if (condition_max_val == -999) {
                        condition_max_val = tmp;
                    } else {
                        condition_max_val = min(tmp, condition_max_val);
                    }
                    break;

                } //end switch cond[i].comp
            } //end if(cond[i].attr == 1)

        } //end for

        //Initialization!


        cout << "condition_equal: " << condition_equal << endl;
        cout << "condition_min: " << condition_min << endl;
        cout << "condition_max: " << condition_max << endl;
        if (condition_equal) {
            puts("init condition equal");
            cout << "condition_equal_val: " << condition_equal_val << endl;
            if (b.locate(condition_equal_val, c)<0) {
                puts("err locating");
            }
            if (b.readForward(c, key, rid)<0) {
                puts("err readForward");
            }
            if (rf.read(rid, key, value)<0) {
                puts("err read");
            }
            diff = key - condition_equal_val;
            if (diff != 0) {
                puts("no match");
            } else if (condition_equal && condition_max && condition_equal_val > condition_max_val) {
                puts("conflict 1");
            } else if (condition_equal && condition_min && condition_equal_val < condition_min_val) {
                puts("conflict 2");
            } else {
                count = 1;
                printHelper(attr, key, value);
            }

            canTerminate = true;

        } else if (condition_min) {
            puts("init condition min");
            b.locate(condition_min_val, c);
        } else {
            puts ("init condition 0");
            b.locate(0, c);
        }

        b.locate(max(0, condition_min_val), c);
        cout << "condition_min_val: " << condition_min_val << endl;
        cout << "condition_max_val: " << condition_max_val << endl;
        cout << "\n-----\n" << endl;

        while (b.readForward(c, key, rid) == 0 && canTerminate == false) {
            if ((rc = rf.read(rid, key, value)) < 0) {
                cout << "read err" << endl;
            }
            // for "key < 1000" type constraints we can check immediately
            if (key > condition_max_val && condition_max_val != -999) {
                puts("key exceeds max limits - skipping.");
                goto skip_printing;
            }

            int diff;

            for (unsigned i = 0; i < cond.size(); i++) {
                if (cond[i].attr == 1) {
                    //puts("comparing keys");
                    //cout << "key value: " << cond[i].value << endl;
                    // key = 321  cond[i].value = 100
                    // diff = key - cond[i].value --> diff > 0
                    diff = key - atoi(cond[i].value);
                } else if (cond[i].attr == 2) {
                    //puts("comparing values");
                    diff = strcmp(value.c_str(), cond[i].value);
                }

                if (cond[i].comp == SelCond::EQ && diff != 0) {
                    // this activates for "value=___" queries
                    puts("condition EQ - but not this tuple. skip");
                    goto skip_printing;
                }

                if (cond[i].comp == SelCond::NE && diff == 0) {
                    puts("condition NE - skip");
                    // if we say key <> 3, and we find that key = 3 at the current cursor,
                    // then don't print anything
                    goto skip_printing;
                }

                if (cond[i].comp == SelCond::GT && diff <= 0) {
                    puts("condition GT - skip");
                    goto skip_printing;
                }

                if (cond[i].comp == SelCond::GE && diff < 0) {
                    puts("condition GE - skip");
                    goto skip_printing;
                }

                if (cond[i].comp == SelCond::LT && diff >= 0) {
                    puts("condition LT - skip");
                    goto skip_printing;
                }

                if (cond[i].comp == SelCond::LE && diff > 0) {
                    puts("condition LE - skip");
                    goto skip_printing;
                }

            } //end for

            count++;
            printHelper(attr, key, value);

            skip_printing:
                cout << "";

        }
        skip_to_end:
            cout << "";

  } else {
      puts("Associated index file does not exist. Proceeding with skeleton code implementation.");

      // scan the table file from the beginning
      rid.pid = rid.sid = 0;
      count = 0;
      while (rid < rf.endRid()) {
        // read the tuple
        if ((rc = rf.read(rid, key, value)) < 0) {
          fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
          goto exit_select;
        }

        // check the conditions on the tuple
        for (unsigned i = 0; i < cond.size(); i++) {
          // compute the difference between the tuple value and the condition value
          switch (cond[i].attr) {
          case 1:
    	diff = key - atoi(cond[i].value);
    	break;
          case 2:
    	diff = strcmp(value.c_str(), cond[i].value);
    	break;
          }

          // skip the tuple if any condition is not met
          switch (cond[i].comp) {
          case SelCond::EQ:
    	if (diff != 0) goto next_tuple;
    	break;
          case SelCond::NE:
    	if (diff == 0) goto next_tuple;
    	break;
          case SelCond::GT:
    	if (diff <= 0) goto next_tuple;
    	break;
          case SelCond::LT:
    	if (diff >= 0) goto next_tuple;
    	break;
          case SelCond::GE:
    	if (diff < 0) goto next_tuple;
    	break;
          case SelCond::LE:
    	if (diff > 0) goto next_tuple;
    	break;
          }
        }

        // the condition is met for the tuple.
        // increase matching tuple counter
        count++;

        // print the tuple
        switch (attr) {
        case 1:  // SELECT key
          fprintf(stdout, "%d\n", key);
          break;
        case 2:  // SELECT value
          fprintf(stdout, "%s\n", value.c_str());
          break;
        case 3:  // SELECT *
          fprintf(stdout, "%d '%s'\n", key, value.c_str());
          break;
        }

        // move to the next tuple
        next_tuple:
        ++rid;
      }

  } //end else from opening index - todo: is this in the right area?

  // print matching tuple count if "select count(*)"
  if (attr == 4) {
    fprintf(stdout, "%d\n", count);
  }
  rc = 0;

  // close the table file and return
  exit_select:
  rf.close();
  return rc;
}

RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
  RecordFile rf;
  RecordId rid;
  string line;
  int key;
  string value;
  BTreeIndex b;


  if (rf.open(table + ".tbl", 'w') != 0) {
      puts("Error opening RecordFile");
      return -1;
  }

  // Part D stuff
    if (index) {
        b.open(table + ".idx", 'w');
    }



  // http://stackoverflow.com/questions/7868936/read-file-line-by-line
  ifstream infile(loadfile.c_str());
  if ( !infile.good() ) {
      puts("File does not exist");
      return -1;
  }

  // debugging purpose
  int gdb = 0;
  while (getline(infile, line)) {
    int tryParsing = parseLoadLine(line, key, value);
    int tryAppending = rf.append(key, value, rid);
    if (tryParsing < 0) {
        puts("Error parsing line in file");
        return -1;
    }
    if (tryAppending < 0) {
        puts("Error appending line to RecordFile");
        return -1;
    }

    // Part D: if index == true then try inserting into BTreeIndex
    // BTreeIndex::insert(int key, const RecordId& rid)
    int tryInserting;
    if (index) {
        tryInserting = b.insert(key, rid);
        if (tryInserting < 0) {
            puts("Error inserting to BTreeIndex");
            return -1;
        }
    }

    gdb++;

  }

  if (index) {
    b.close();
  }
  puts("Successfully wrote all tuples to RecordFile");

  infile.close();

  // Close RecordFile
  rf.close();
  /*
  if (index) {
      // part D: if the third parameter index is set to true, Bruinbase creates the corresponding B+tree index on the key column of the table. The index file should be named 'tblname.idx' where tblname is the name of the table, and created in the current working directory. Essentially, you have to change the load function, such that if index is true, for every tuple inserted into the table, you obtain RecordId of the inserted tuple, and insert a corresponding (key, RecordId) pair to the B+tree of the table.
      BTreeIndex b;

      // BTreeIndex::open(const string& indexname, char mode)
      b.open(table + ".idx", "w");

      while(getline(infile, line)) {
        int tryParsing = parseLoadLine(line, key, value);
        int tryAppending = rf.append(key, value, rid);
        if (tryParsing < 0) {
            puts("Error parsing line in file");
            return -1;
        }
        if (tryAppending)


      }

      b.close();
  }
  */
  return 0;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;

    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }

    // get the integer key value
    key = atoi(s);

    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');

    // if there is nothing left, set the value to empty string
    if (c == 0) {
        value.erase();
        return 0;
    }

    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }

    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }

    return 0;
}

void SqlEngine::printHelper(int& attr, int& key, string& value) {
    // attr = 1: key
    // attr = 2: value
    // attr = 3: * (print both columns)
    // attr = 4: count(*) (not handled here)
    switch (attr) {
        case 1:
          fprintf(stdout, "%d\n", key);
          break;
        case 2:
          fprintf(stdout, "%s\n", value.c_str());
          break;
        case 3:
          fprintf(stdout, "%d '%s'\n", key, value.c_str());
          break;
    }
}
