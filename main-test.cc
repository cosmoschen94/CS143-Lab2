
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeNode.h"
#include "RecordFile.h"
#include "PageFile.h"
#include <cstdio>
#include <iostream>

using namespace std;


int main()
{
  // run the SQL engine taking user commands from standard input (console).
  BTLeafNode test;
  BTLeafNode test_sibling;
  int sibling_key;
  RecordId record;
  record.pid = 2;
  record.sid = 3;
  test.initializeBuffer();
  test_sibling.initializeBuffer();
  test.printBuffer();

  for(int i=0; i<84; i++){
    test.insert(i, record);
  }

  //test.printBuffer();

  //test.insert(100,record);

  test_sibling.printBuffer();

  int eid = 0;
  test.locate(1,eid);
  cout << "Before insert and split, the eid is: ";
  cout << eid << endl;

  test.insertAndSplit(42, record, test_sibling, sibling_key);

  test.locate(42,eid);
  cout << "After insert and split, the eid is: ";
  cout << eid << endl;

  //test_sibling.printBuffer();

  // test.printBuffer();

  return 0;
}
