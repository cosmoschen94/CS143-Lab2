
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeNode.h"
#include "RecordFile.h"
#include "PageFile.h"
#include <cstdio>


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

  test.insertAndSplit(40, record, test_sibling, sibling_key);

  test_sibling.printBuffer();

  // test.printBuffer();

  return 0;
}
