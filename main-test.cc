
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeNode.h"
#include "RecordFile.h"
#include "PageFile.h"
#include <cstdio>
#include <iostream>

using namespace std;

void nonLeafNodeInsert();

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

  nonLeafNodeInsert();

  return 0;
}

void nonLeafNodeInsert() {
    cout << "\nTesting NonLeafNode::insert\n" << endl;
    BTNonLeafNode test;
    test.initializeBuffer();

    cout << "Key count before insert: " << test.getKeyCount() << endl;
    PageId pid = 1;
    test.insert(0,pid);

    cout << "Key count after insert: " << test.getKeyCount() << endl;

    cout << "\nDone testing NonLeafNode::insert\n" << endl;
}
