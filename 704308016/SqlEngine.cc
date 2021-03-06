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
// #include <queue>
#include <climits>
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
        // puts("Associated index file does exist!");
        rid.pid = 0;
        rid.sid = 0;

        bool condition_equal = false;
        int condition_equal_val = -999;

        bool condition_min = false;
        // should be INT_MIN not 0 because searchKey can be negative
        int condition_min_val = INT_MIN;

        bool condition_max = false;
        int condition_max_val = -999;

        bool only_value = true;

        //int absolute_smallest_key = 0;
        bool has_equals_value = false;
        string equals_value = "";

        bool has_max_value = false;
        bool has_le = false;
        string max_value = "";

        bool has_min_value = false;
        bool has_ge = false;
        string min_value = "";

        int tmp;
        bool canTerminate = false;
        count = 0;

        // Query with only NE condition, regardless of key or value attribute
        // should avoid B+ Tree index lookup and jump to default sequenctial lookup
        if (cond.size() == 1 && cond[0].comp == SelCond::NE) goto Default_lookup;

        for (unsigned i = 0; i < cond.size(); i++) {
            if (cond[i].attr == 1) {
                only_value = false;
                tmp = atoi(cond[i].value);
                // if (absolute_smallest_key == -999) {
                //     absolute_smallest_key = tmp;
                // } else {
                //     absolute_smallest_key = min(tmp, absolute_smallest_key);
                // }

                switch (cond[i].comp) {
                    case SelCond::EQ:
                    if (condition_equal && condition_equal_val != atoi(cond[i].value)) {
                        // puts("two unique key equal statements. terminate now");
                        canTerminate = true;
                        goto skip_to_end;
                    }
                    condition_equal = true;
                    condition_equal_val = atoi(cond[i].value);
                    break;

                    case SelCond::NE:
                    // what happens here?
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
            if (cond[i].attr == 2) {
                string tmpStr = cond[i].value;
                if (cond[i].comp == SelCond::EQ) {
                    if(has_equals_value && equals_value != "") {
                        // puts("value equals conflict");
                        goto skip_to_end;
                    } else {
                        // puts("value equals condition");
                        has_equals_value = true;
                        equals_value = cond[i].value;
                    }
                }
                if (cond[i].comp == SelCond::GT) {
                    has_min_value = true;
                    if (min_value == "") {
                        min_value = tmpStr;
                    } else {
                        min_value = max(tmpStr, min_value);
                    }
                }
                if (cond[i].comp == SelCond::LT) {
                    has_max_value = true;
                    if (max_value == "") {
                        max_value = tmpStr;
                    } else {
                        max_value = min(tmpStr, max_value);
                    }
                }
                if (cond[i].comp == SelCond::GE) {
                    has_min_value = true;
                    has_ge = true;
                    if (min_value == "") {
                        min_value = tmpStr;
                    } else {
                        min_value = max(tmpStr, min_value);
                    }
                }
                if (cond[i].comp == SelCond::LE) {
                    has_max_value = true;
                    has_le = true;
                    if (max_value == "") {
                        max_value = tmpStr;
                    } else {
                        max_value = min(tmpStr, max_value);
                    }
                }

            }
        } //end for

        //Initialization!
        if (only_value && attr != 4){
          // avoid B+ tree if the query is only for value
          // puts("Only value query, skip to default sequential");
          goto Default_lookup;
        }

        // cout << "condition_equal: " << condition_equal << endl;
        // cout << "condition_min: " << condition_min << endl;
        // cout << "condition_max: " << condition_max << endl;
        if (condition_equal) {
            // puts("init condition equal");
            // cout << "condition_equal_val: " << condition_equal_val << endl;
            if (b.locate(condition_equal_val, c)<0) {
                // puts("err locating");
            }
            if (b.readForward(c, key, rid)<0) {
                // puts("err readForward");
            }

            // move this section into printHelper() to reduce page read
            if (rf.read(rid, key, value)<0) {
                // puts("err read");
            }

            diff = key - condition_equal_val;
            if (diff != 0) {
                // puts("no match");
            } else if (condition_equal && condition_max && condition_equal_val > condition_max_val) {
                // puts("conflict 1");
            } else if (condition_equal && condition_min && condition_equal_val < condition_min_val) {
                // puts("conflict 2");
            } else if (has_le && strcmp(value.c_str(), max_value.c_str()) > 0) {
                // puts("conflict 3");
            } else if (has_ge && strcmp(value.c_str(), min_value.c_str()) < 0) {
                // puts("conflict 4");
            } else if (has_equals_value && strcmp(value.c_str(), equals_value.c_str()) != 0) {
                // puts("conflict 5");
            } else if (!has_le && has_max_value && strcmp(value.c_str(), max_value.c_str()) >= 0) {
                // puts("conflict 6");
            } else if (!has_ge && has_min_value && strcmp(value.c_str(), min_value.c_str()) <= 0) {
                // puts("conflict 7");
            } else {
                count = 1;
                //printHelper(rf, rid, attr, key, value);
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

            canTerminate = true;

        } else if (condition_min) {
            // puts("init condition min");
            b.locate(condition_min_val, c);
        } else {
            // puts ("init condition 0");

            // should be INT_MIN not 0 because searchKey can be negative
            b.locate(INT_MIN, c);
        }

        // should be INT_MIN not 0 because searchKey can be negative
        condition_min_val = max(INT_MIN, condition_min_val);
        b.locate(condition_min_val, c);
        // cout << "condition_min_val: " << condition_min_val << endl;
        // cout << "condition_max_val: " << condition_max_val << endl;
        // cout << "\n-----\n" << endl;

        while (b.readForward(c, key, rid) == 0 && canTerminate == false) {

            // for "key < 1000" type constraints we can check immediately
            if (key > condition_max_val && condition_max_val != -999) {
                // puts("key exceeds max limits - skipping.");
                //goto skip_printing;
                goto skip_to_end;
            }

            if(attr != 4){
              if ((rc = rf.read(rid, key, value)) < 0) {
                  // cout << "read err" << endl;
              }
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
                //cout << "diff: " << diff << endl;
                if (cond[i].comp == SelCond::EQ && diff != 0) {
                    // this activates for "value=___" queries
                    // puts("condition EQ - but not this tuple. skip");
                    goto skip_printing;
                }

                if (cond[i].comp == SelCond::NE && diff == 0) {
                    // puts("condition NE - skip");
                    // if we say key <> 3, and we find that key = 3 at the current cursor,
                    // then don't print anything
                    goto skip_printing;
                }

                if (cond[i].comp == SelCond::GT && diff <= 0) {
                    // puts("condition GT - skip");
                    goto skip_printing;
                }

                if (cond[i].comp == SelCond::GE && diff < 0) {
                    // puts("condition GE - skip");
                    goto skip_printing;
                }

                if (cond[i].comp == SelCond::LT && diff >= 0) {
                    // puts("condition LT - skip");
                    goto skip_printing;
                }

                if (cond[i].comp == SelCond::LE && diff > 0) {
                    // puts("condition LE - skip");
                    goto skip_printing;
                }

            } //end for

            count++;

            if(attr == 4){
              goto skip_printing;
            }

            printHelper2(attr, key, value);

            skip_printing:
                // cout << "";
                int placeholder = 1;

        }
        skip_to_end:
            // cout << "";
            int placeholder2 = 1;

  } else {
      // puts("Associated index file does not exist. Proceeding with skeleton code implementation.");

      Default_lookup:
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
      } //while

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
      // puts("Error opening RecordFile");
      return -1;
  }

  // Part D stuff
    if (index) {
        b.open(table + ".idx", 'w');
    }



  // http://stackoverflow.com/questions/7868936/read-file-line-by-line
  ifstream infile(loadfile.c_str());
  if ( !infile.good() ) {
      // puts("File does not exist");
      return -1;
  }

  // debugging purpose
  // int gdb = 0;
  while (getline(infile, line)) {
    int tryParsing = parseLoadLine(line, key, value);
    int tryAppending = rf.append(key, value, rid);
    if (tryParsing < 0) {
        // puts("Error parsing line in file");
        return -1;
    }
    if (tryAppending < 0) {
        // puts("Error appending line to RecordFile");
        return -1;
    }

    // Part D: if index == true then try inserting into BTreeIndex
    // BTreeIndex::insert(int key, const RecordId& rid)
    int tryInserting;
    if (index) {
        tryInserting = b.insert(key, rid);
        if (tryInserting < 0) {
            // puts("Error inserting to BTreeIndex");
            return -1;
        }
    }

    // gdb++;

  }

  if (index) {

    // testing function
    // b.printBTree();

    b.close();
  }
  // puts("Successfully wrote all tuples to RecordFile");

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

void SqlEngine::printHelper(RecordFile rf, RecordId rid, int& attr, int& key, string& value) {
    // attr = 1: key
    // attr = 2: value
    // attr = 3: * (print both columns)
    // attr = 4: count(*) (not handled here)

    if(attr == 4){
      return;
    }

    RC rc;
    // move this code section here to reduce page read
    if ((rc = rf.read(rid, key, value)) < 0) {
        // cout << "read err" << endl;
    }

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

void SqlEngine::printHelper2(int& attr, int& key, string& value) {
    // attr = 1: key
    // attr = 2: value
    // attr = 3: * (print both columns)
    // attr = 4: count(*) (not handled here)

    if(attr == 4){
      return;
    }

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
