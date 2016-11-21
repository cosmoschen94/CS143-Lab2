
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeNode.h"
#include "RecordFile.h"
#include "PageFile.h"
#include "BTreeIndex.h"
#include <cstdio>
#include <iostream>

using namespace std;

int main()
{
    BTreeIndex b;

    b.open("testing", 'w');

    // 85 is the number of tuples that will cause leaf overflow
    // 5419 is the number of tuples that will cause nonleaf overflow, which will cause a new root to be created

    int tuples = 5419;

    //populate B+ Tree
    for(int i = 0; i < tuples; i++){
      RecordId r;
      r.pid = i;
      r.sid = i;

      RC res = b.insert(i,r);
      if (res != 0) {
          cout << i << endl;
          puts("error inserting");
          //return 0;

      } else {
          //puts("success inserting");
      }
    }

    // RecordId temp;
    // temp.pid = 83;
    // temp.sid = 83;
    // b.insert(83,temp);


    b.printBTree();

    b.close();

    // IndexCursor cursor;
    //
    // b.locate(1, cursor);
    //
    // cout << cursor.pid << endl;
    // cout << cursor.eid << endl;
    //
    // int key;
    // RecordId rid;
    // b.readForward(cursor, key, rid);
    //
    // cout << cursor.pid << endl;
    // cout << cursor.eid << endl;
    // cout << key << endl;
    // cout << rid.pid << endl;
    // cout << rid.sid << endl;
    //
    // b.readForward(cursor, key, rid);
    //
    // cout << cursor.pid << endl;
    // cout << cursor.eid << endl;
    // cout << key << endl;
    // cout << rid.pid << endl;
    // cout << rid.sid << endl;



  // BTLeafNode test;
  // BTLeafNode test_sibling;
  // int sibling_key;
  // RecordId record;
  // record.pid = 2;
  // record.sid = 3;
  // test.initializeBuffer();
  // test_sibling.initializeBuffer();
  // test.printBuffer();
  //
  // for(int i=0; i<84; i++){
  //   test.insert(i, record);
  // }
  //
  // test_sibling.printBuffer();


  /**
   * test BTLeafNode::locate()
   */

  // int eid = 0;
  // test.locate(1,eid);
  // cout << "Before insert and split, the eid is: ";
  // cout << eid << endl;
  //
  // test.insertAndSplit(42, record, test_sibling, sibling_key);
  //
  // test.locate(42,eid);
  // cout << "After insert and split, the eid is: ";
  // cout << eid << endl;

  /**
   * test BTLeafNode::readEntry()
   */
  // int key = 0;
  // RecordId rid;
  // rid.pid = 3;
  // rid.sid = 5;
  // test.insert(100, rid);
  //
  // RecordId rec;
  // test.locate(42,eid);
  // test.readEntry(eid,key,rec);
  //
  // cout << "The key is: ";
  // cout << key << endl;
  // cout << "The pid is: ";
  // cout << rec.pid << endl;
  // cout << "The sid is: ";
  // cout << rec.sid << endl;

  /**
   * test BTLeafNode::getNextNodePtr()
   */
  // cout << "Before the insert, the next node ptr is: ";
  // cout << test.getNextNodePtr() << endl;
  // test.insertAndSplit(42, record, test_sibling, sibling_key);
  // cout << "After the insert, the next node ptr is: ";
  // cout << test.getNextNodePtr() << endl;

  /**
   * test BTLeafNode::setNextNodePtr()
   */

  // test.setNextNodePtr(42);
  // cout << "After set, the next node ptr is: ";
  // cout << test.getNextNodePtr() << endl;


  /**
   * test BTNonLeafNode::insert()
   */
  //
  // BTNonLeafNode testNonLeaf;
  //
  // testNonLeaf.initializeBuffer();
  //
  // for(int i=0; i<127; i++){
  //   testNonLeaf.insert(i, 1);
  // }
  //
  // testNonLeaf.printBuffer();
  //
  // /**
  //  * test BTNonLeafNode::insertAndSplit()
  //  */
  //
  // BTNonLeafNode testNonLeaf_sibling;
  // testNonLeaf_sibling.initializeBuffer();
  //
  // int mid_key = 0;
  // testNonLeaf.insertAndSplit(62, 2, testNonLeaf_sibling, mid_key);
  //
  // testNonLeaf.printBuffer();
  //
  // cout << "The mid key is: ";
  // cout << mid_key << endl;
  //


  //nonLeafNodeInsert();

  return 0;
}

// void nonLeafNodeInsert() {
//     cout << "\nTesting NonLeafNode::insert\n" << endl;
//     BTNonLeafNode test;
//     test.initializeBuffer();
//
//     cout << "Key count before insert: " << test.getKeyCount() << endl;
//     PageId pid = 1;
//     test.insert(0,pid);
//
//     cout << "Key count after insert: " << test.getKeyCount() << endl;
//
//     cout << "\nDone testing NonLeafNode::insert\n" << endl;
// }
