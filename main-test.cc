
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
  RecordId record;
  record.pid = 2;
  record.sid = 3;
  test.initializeBuffer();
  test.printBuffer();
  test.insert(1, record);
  test.printBuffer();
  return 0;
}
