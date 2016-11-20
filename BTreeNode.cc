#include "BTreeNode.h"
#include <iostream>
#include <stdlib.h>
#include <cstring>


using namespace std;

/*
 * BTLeafNode constructor
 */
BTLeafNode::BTLeafNode()
{
    // initialize buffer with all 0
    memset(buffer, 0, 1024);
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{
    // See BTNonLeafNode::read notes
    RC res = pf.read(pid, buffer);

    return res;

}

/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{
    // See BTNonLeafNode::write notes
    RC res = pf.write(pid, buffer);

    return res;

}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{
  // char count[4];
  // strncpy(count, buffer, 4);
  // int num = *(int*)count;
  // return num;

  int count = 0;
  memcpy(&count, buffer, 4);
  return count;
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */

// Testing functions:


RC BTLeafNode::printBuffer()
{
  char count[4];


  strncpy(count, buffer, 4);


  int num = *(int*)count;
  cout << "There are ";
  cout << num;
  cout << " pairs" << endl;

  for(int i=0; i<num; i++){
    char key[4];
    char pageid[4];
    char sid[4];
    strncpy(pageid, (buffer+8)+(i*12), 4);
    strncpy(sid, (buffer+12)+(i*12), 4);
    strncpy(key, (buffer+16)+(i*12), 4);

    int ikey = *(int*)key;
    int ipageid = *(int*)pageid;
    int isid = *(int*)sid;

    cout << "Pair ";
    cout << i+1 << endl;
    cout << "key: ";
    cout << ikey << endl;
    cout << "pageid: ";
    cout << ipageid << endl;
    cout << "sid: ";
    cout << isid << endl;
    cout << "==============" << endl;
  }

  return 0;
}


// some helpful functions: strncpy(dest,source,num), memcpy(dest,source,num), memmove(dest,source,num).
RC BTLeafNode::insert(int key, const RecordId& rid)
{

  int num = getKeyCount();

  // the leaf node doesnt contain any pair.
  if(num == 0){
    // start inserting at buffer[8]
    // recordid takes 8 bytes
    // key takes 4 bytes
    memcpy(buffer+8, &rid, 8);
    memcpy(buffer+16, &key, 4);

    // update count to 1
    int cnt = 1;
    memcpy(buffer, &cnt, 4);

  }
  // the leaf node already contains pairs
  else{

    // the leaf node is full with 84 pairs, buffer has 1016 bytes and last 8 bytes are left untouched.
    if(num == 84) {
      cout << "Node Full" << endl;
      return RC_NODE_FULL;
    }

    else {

      // traverse the buffer to find the right place to insert
      for(int i = 0; i < num; i++){
        char tmp_key[4];
        strncpy(tmp_key, (buffer+16)+(i*12), 4);
        int ikey = *(int*)tmp_key;

        // find the slot to insert
        if(ikey >= key){

          // move pairs 12 bytes backward
          memmove((buffer+20)+(i*12), (buffer+8)+(i*12), (num-i)*12);

          // start insert
          memcpy((buffer+8)+(i*12), &rid, 8);
          memcpy((buffer+8)+(i*12)+8, &key, 4);

          // update count
          num++;
          memcpy(buffer, &num, 4);
          return 0;
        }
      }

      // if reach this point, the pair should be inserted in the back
      // start insert
      memcpy((buffer+8)+(num*12), &rid, 8);
      memcpy((buffer+8)+(num*12)+8, &key, 4);

      // update count
      num++;
      memcpy(buffer, &num, 4);
    }
  }

  return 0;
}

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid,
                              BTLeafNode& sibling, int& siblingKey)
{
  // this should be 84
  int num = getKeyCount();
  // this should be 42
  int mid = num/2;

  // traverse the buffer to find the right place to insert
  for(int i = 0; i < num; i++){
    char tmp_key[4];
    strncpy(tmp_key, (buffer+16)+(i*12), 4);
    int ikey = *(int*)tmp_key;

    // find the slot to insert
    if(ikey >= key){

      // the pair should be inserted into the sibling node
      if(i >= mid) {

        // update sibling node count
        int sibling_count = 43;
        memcpy(sibling.buffer, &sibling_count, 4);

        // // update sibling node pageid
        // memcpy(sibling.buffer+4, buffer+4, 4);
        //
        // // update current node pageid
        // int pageid = *(int*)&sibling;
        // memcpy(buffer+4, &pageid, 4);

        // update current node count
        int cur_count = 42;
        memcpy(buffer, &cur_count, 4);

        // copy second half to sibling node
        memcpy(sibling.buffer+8, (buffer+8)+(42*12), 42*12);

        // insert the pair into sibling node
        int j = i - mid;
        // move pairs 12 bytes backward
        memmove((sibling.buffer+20)+(j*12), (sibling.buffer+8)+(j*12), (42-j)*12);

        // start insert into sibling node
        memcpy((sibling.buffer+8)+(j*12), &rid, 8);
        memcpy((sibling.buffer+8)+(j*12)+8, &key, 4);

        // get the first key of the sibling node after split
        char first_key[4];
        strncpy(first_key, sibling.buffer+16, 4);
        siblingKey = *(int*)first_key;

        return 0;
      }

      // the pair should be inserted into the current node
      else {
        // update sibling node count
        int sibling_count = 42;
        memcpy(sibling.buffer, &sibling_count, 4);

        // // update sibling node pageid
        // memcpy(sibling.buffer+4, buffer+4, 4);
        //
        // // update current node pageid
        // int pageid = *(int*)&sibling;
        // memcpy(buffer+4, &pageid, 4);

        // update current node count
        int cur_count = 43;
        memcpy(buffer, &cur_count, 4);

        // copy second half to sibling node
        memcpy(sibling.buffer+8, (buffer+8)+(42*12), 42*12);

        // start insert pair into current node
        // move pairs 12 bytes backward
        memmove((buffer+20)+(i*12), (buffer+8)+(i*12), (mid-i)*12);

        // start insert into current node
        memcpy((buffer+8)+(i*12), &rid, 8);
        memcpy((buffer+8)+(i*12)+8, &key, 4);

        // get the first key of the sibling node after split
        char first_key[4];
        strncpy(first_key, sibling.buffer+16, 4);
        siblingKey = *(int*)first_key;

        return 0;
      }
    }
  }

  // if reach this point, the pair should be inserted in the back of the sibling node
  // update sibling node count
  int sibling_count = 43;
  memcpy(sibling.buffer, &sibling_count, 4);

  // // update sibling node pageid
  // memcpy(sibling.buffer+4, buffer+4, 4);
  //
  // // update current node pageid
  // int pageid = *(int*)&sibling;
  // memcpy(buffer+4, &pageid, 4);

  // update current node count
  int cur_count = 42;
  memcpy(buffer, &cur_count, 4);

  // copy second half to sibling node
  memcpy(sibling.buffer+8, (buffer+8)+(42*12), 42*12);

  // start insert
  memcpy((sibling.buffer+8)+(42*12), &rid, 8);
  memcpy((sibling.buffer+8)+(42*12)+8, &key, 4);

  // get the first key of the sibling node after split
  char first_key[4];
  strncpy(first_key, sibling.buffer+16, 4);
  siblingKey = *(int*)first_key;

  return 0;
}

/**
 * If searchKey exists in the node, set eid to the index entry
 * with searchKey and return 0. If not, set eid to the index entry
 * immediately after the largest index key that is smaller than searchKey,
 * and return the error code RC_NO_SUCH_RECORD.
 * Remember that keys inside a B+tree node are always kept sorted.
 * @param searchKey[IN] the key to search for.
 * @param eid[OUT] the index entry number with searchKey or immediately
                   behind the largest key smaller than searchKey.
 * @return 0 if searchKey is found. Otherwise return an error code.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{
  int num = getKeyCount();

  for(int i=0; i<num; i++) {
    char tmp_key[4];
    strncpy(tmp_key, (buffer+16)+(i*12), 4);
    int ikey = *(int*)tmp_key;

    // find the key slot
    if(ikey == searchKey){
      eid = i;
      return 0;
    }
  }

  for(int i=0; i<num; i++) {
    char tmp_key[4];
    strncpy(tmp_key, (buffer+16)+(i*12), 4);
    int ikey = *(int*)tmp_key;

    // find the key slot immediately after the largest index key that is smaller than searchKey
    if(ikey > searchKey){
      eid = i;
      return RC_NO_SUCH_RECORD;
    }
  }

  // if reach this point, the searchKey is larger than any keys in the buffer
  eid = num;

  return RC_NO_SUCH_RECORD;
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{
  int num = getKeyCount();

  // out of bound access
  if(eid >= num){
    return RC_NO_SUCH_RECORD;
  }

  for(int i=0; i<num; i++) {

    // find the entry
    if(i == eid){
      char tmp_key[4];
      strncpy(tmp_key, (buffer+16)+(i*12), 4);
      key = *(int*)tmp_key;

      char tmp_pid[4];
      char tmp_sid[4];
      strncpy(tmp_pid, (buffer+8)+(i*12), 4);
      rid.pid = *(int*)tmp_pid;
      strncpy(tmp_sid, (buffer+8)+(i*12)+4, 4);
      rid.sid = *(int*)tmp_sid;
    }
  }

  return 0;
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node
 */
PageId BTLeafNode::getNextNodePtr()
{
  char pid[4];
  strncpy(pid, buffer+4, 4);
  return *(int*)pid;
}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{
  memcpy(buffer+4, &pid, 4);
  return 0;
}



RC BTNonLeafNode::printBuffer()
{
  char count[4];


  strncpy(count, buffer, 4);


  int num = *(int*)count;
  cout << "There are ";
  cout << num;
  cout << " pairs" << endl;

  for(int i=0; i<num; i++){
    char key[4];
    char pageid[4];
    strncpy(key, (buffer+8)+(i*8), 4);
    strncpy(pageid, (buffer+8)+(i*8)+4, 4);


    int ikey = *(int*)key;
    int ipageid = *(int*)pageid;

    cout << "Pair ";
    cout << i+1 << endl;
    cout << "key: ";
    cout << ikey << endl;
    cout << "pageid: ";
    cout << ipageid << endl;
    cout << "==============" << endl;
  }

  return 0;
}

/*
 * BTNonLeafNode constructor
 */
BTNonLeafNode::BTNonLeafNode()
{
    // initialize buffer with all 0
    memset(buffer, 0, 1024);
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{
    // Notes to self:

    // buffer is the memory buffer defined in BTreeNode.h

    // pf.read implementation found in PageFile.cc
    // pf.read returns 0 on success, an error code otherwise
    RC res = pf.read(pid, buffer);

    // if (res != 0) {
    //     return res;
    // } else {
    //     return 0;
    // }

    return res;  // Lolol I realized I could just do this instead of above...
}

/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{
    // See PageFile.h for description of pf.write
    // and see PageFile.cc for implementation of pf.write

    // pf.write returns 0 on success, error code otherwise
    RC res = pf.write(pid, buffer);

    return res;

}

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{

  char count[4];
  strncpy(count, buffer, 4);
  int num = *(int*)count;
  return num;

}


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTNonLeafNode::insert(int key, PageId pid)
{

  int num = getKeyCount();

  // the leaf node doesnt contain any pair.
  if(num == 0){
    // start inserting at buffer[8]
    // key takes 4 bytes
    // pid takes 4 bytes
    memcpy(buffer+8, &key, 4);
    memcpy(buffer+12, &pid, 4);

    // update count to 1
    int cnt = 1;
    memcpy(buffer, &cnt, 4);

  }
  // the leaf node already contains pairs
  else{

    // the leaf node is full with 127 pairs, buffer has 1024 bytes.
    if(num == 127) {
      cout << "Node Full" << endl;
      return RC_NODE_FULL;
    }

    else {

      // traverse the buffer to find the right place to insert
      for(int i = 0; i < num; i++){
        char tmp_key[4];
        strncpy(tmp_key, (buffer+8)+(i*8), 4);
        int ikey = *(int*)tmp_key;

        // find the slot to insert
        if(ikey >= key){

          // move pairs 8 bytes backward
          memmove((buffer+16)+(i*8), (buffer+8)+(i*8), (num-i)*8);

          // start insert
          memcpy((buffer+8)+(i*8), &key, 4);
          memcpy((buffer+8)+(i*8)+4, &pid, 4);

          // update count
          num++;
          memcpy(buffer, &num, 4);
          return 0;
        }
      }

      // if reach this point, the pair should be inserted in the back
      // start insert
      memcpy((buffer+8)+(num*8), &key, 4);
      memcpy((buffer+8)+(num*8)+4, &pid, 4);

      // update count
      num++;
      memcpy(buffer, &num, 4);
    }
  }

  return 0;
}

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{
  // this should be 127
  int num = getKeyCount();
  // this should be 63
  int mid = num/2;

  // traverse the buffer to find the right place to insert
  for(int i = 0; i < num; i++){
    char tmp_key[4];
    strncpy(tmp_key, (buffer+8)+(i*8), 4);
    int ikey = *(int*)tmp_key;

    // find the slot to insert
    if(ikey >= key){

      // the pair should be inserted into the sibling node
      if(i > mid) {

        // update sibling node count
        int sibling_count = 64;
        memcpy(sibling.buffer, &sibling_count, 4);

        // update sibling node pageid
        memcpy(sibling.buffer+4, (buffer+8)+(mid*8)+4, 4);

        // // update current node pageid
        // int pageid = *(int*)&sibling;
        // memcpy(buffer+4, &pageid, 4);

        // update current node count
        int cur_count = 63;
        memcpy(buffer, &cur_count, 4);

        // copy second half to sibling node
        memcpy(sibling.buffer+8, (buffer+8)+(64*8), 63*8);

        // insert the pair into sibling node
        int j = i - mid - 1;
        // move pairs 8 bytes backward
        memmove((sibling.buffer+8)+(j*8)+8, (sibling.buffer+8)+(j*8), (63-j)*8);

        // start insert into sibling node
        memcpy((sibling.buffer+8)+(j*8), &key, 4);
        memcpy((sibling.buffer+8)+(j*8)+4, &pid, 4);

        // get the mid key
        char mid_key[4];
        strncpy(mid_key, (buffer+8)+(mid*8), 4);
        midKey = *(int*)mid_key;

        return 0;
      }

      // the pair should be inserted into the current node
      else {
        // update sibling node count
        int sibling_count = 63;
        memcpy(sibling.buffer, &sibling_count, 4);

        // update sibling node pageid
        memcpy(sibling.buffer+4, (buffer+8)+(mid*8)+4, 4);

        // // update current node pageid
        // int pageid = *(int*)&sibling;
        // memcpy(buffer+4, &pageid, 4);

        // update current node count
        int cur_count = 64;
        memcpy(buffer, &cur_count, 4);

        // copy second half to sibling node
        memcpy(sibling.buffer+8, (buffer+8)+(64*8), 63*8);

        // get the mid key before inserting
        char mid_key[4];
        strncpy(mid_key, (buffer+8)+(mid*8), 4);
        midKey = *(int*)mid_key;

        if(mid == i) {
          // start insert into current node by overwriting at mid
          memcpy((buffer+8)+(i*8), &key, 4);
          memcpy((buffer+8)+(i*8)+4, &pid, 4);

          return 0;
        }

        // start insert pair into current node
        // move pairs 8 bytes backward
        memmove((buffer+8)+(i*8)+8, (buffer+8)+(i*8), (mid-i+1)*8);

        // start insert into current node
        memcpy((buffer+8)+(i*8), &key, 4);
        memcpy((buffer+8)+(i*8)+4, &pid, 4);

        return 0;
      }
    }
  }

  // if reach this point, the pair should be inserted in the back of the sibling node
  // update sibling node count
  int sibling_count = 64;
  memcpy(sibling.buffer, &sibling_count, 4);

  // update sibling node pageid, this pageid should be the pageid correspond to the mid_key
  memcpy(sibling.buffer+4, (buffer+8)+(mid*8)+4, 4);

  // // update current node pageid
  // int pageid = *(int*)&sibling;
  // memcpy(buffer+4, &pageid, 4);

  // update current node count
  int cur_count = 63;
  memcpy(buffer, &cur_count, 4);

  // copy second half to sibling node
  memcpy(sibling.buffer+8, (buffer+8)+(64*8), 63*8);

  // start insert into sibling node
  memcpy((sibling.buffer+8)+(63*8), &key, 4);
  memcpy((sibling.buffer+8)+(63*8)+4, &pid, 4);

  // get the mid key
  char mid_key[4];
  strncpy(mid_key, (buffer+8)+(mid*8), 4);
  midKey = *(int*)mid_key;

  return 0;
}

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{
  int num = getKeyCount();

  for(int i=0; i<num; i++) {
    char tmp_key[4];
    strncpy(tmp_key, (buffer+8)+(i*8), 4);
    int ikey = *(int*)tmp_key;

    // find the key slot
    if(ikey > searchKey){
      char tmp_pid[4];
      strncpy(tmp_pid, (buffer+4)+(i*8), 4);
      pid = *(int*)tmp_pid;
      return 0;
    }
  }

  // if reach this point, the searchKey is larger than any key in the buffer
  char tmp_pid[4];
  strncpy(tmp_pid, (buffer+4)+(num*8), 4);
  pid = *(int*)tmp_pid;
  return 0;
}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
 RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
 {
     int count = 1;

     // initialize number of pairs to 1
     memcpy(buffer, &count, 4);

     // Note: I think this makes sense, but since insert isn't done yet, don't know if it'll work.
     memcpy(buffer+4, &pid1, 4);

     memcpy(buffer+8, &key, 4);

     memcpy(buffer+12, &pid2, 4);

     // BTNonLeafNode::insert returns 0 on success, error code otherwise
     return 0;

 }

 // Testing functions:
