/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include "BTreeIndex.h"
#include "BTreeNode.h"
#include <stdio.h>
#include <cstring>

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    rootPid = -1;
    treeHeight = 0;

    // initialize buffer with all 0
    memset(buffer, 0, 1024);

    // update buffer
    memcpy(buffer, &rootPid, 4);
    memcpy(buffer+4, &treeHeight, 4);
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
RC BTreeIndex::open(const string& indexname, char mode)
{

    RC result = pf.open(indexname, mode);

    if(result){
      // error occurs
      return result;
    }

    // endPid is 0, the pageFile is empty
    if(pf.endPid() == 0){
        rootPid = -1;
        treeHeight = 0;
        // update buffer
        memcpy(buffer, &rootPid, 4);
        memcpy(buffer+4, &treeHeight, 4);
        RC result = pf.write(0, buffer);
        if(result){
          // error occurs
          return result;
        }
        return 0;
    }

    // pageFile is not empty
    else {
        result = pf.read(0, buffer);
        if(result) {
          // error occurs
          return result;
        }
        int temp_rootpid, temp_treeHeight;
        memcpy(&temp_rootpid, buffer, 4);
        memcpy(&temp_treeHeight, buffer+4, 4);

        if(temp_rootpid != 0 && temp_treeHeight != 0){
          rootPid = temp_rootpid;
          treeHeight = temp_treeHeight;
        }
    }


    return 0;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
    memcpy(buffer, &rootPid, 4);
    memcpy(buffer, &treeHeight, 4);

    RC res = pf.write(0, buffer);
    if (res != 0 ) return res;
    // ToDo: might have to add more here
    return pf.close();
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{

    if(key < 0){
      return RC_INVALID_ATTRIBUTE;
    }

    // the tree is empty
    if(rootPid == -1) {

      BTLeafNode l;
      l.insert(key, rid);

      if(pf.endPid() == 0) {rootPid = 1;}
      else {rootPid = pf.endPid();}

      treeHeight = 1;

      // update buffer
      memcpy(buffer, &rootPid, 4);
      memcpy(buffer+4, &treeHeight, 4);

      // update buffer to pageFile
      RC result = pf.write(0, buffer);
      if(result){
        // error occurs
        puts("err in pf.write(0,buffer)");
        return result;
      }

      // update treeNode buffer to pageFile
      result = l.write(rootPid,pf);
      if(result){
        // error occurs
        puts("err in l.write(pf.endPid(),pf)");
        return result;
      }

      return 0;
    }

    // the tree is not empty
    bool leafNodeOverflow = false;
    bool nonLeafNodeOverflow = false;
    int siblingKey = -1;
    int siblingPid = -1;
    return recursive_insert(key, rid, 1, rootPid, treeHeight, leafNodeOverflow, nonLeafNodeOverflow, siblingKey, siblingPid);

}


RC BTreeIndex::recursive_insert(int key, const RecordId& rid, int height, PageId& pid, int treeHeight, bool& leafNodeOverflow, bool& nonLeafNodeOverflow, int& siblingKey, int& siblingPid)
{
    // at LeafNode level
    if(height == treeHeight){
      BTLeafNode l;

      // read buffer of current node in PageFile
      RC result = l.read(pid, pf);
      if(result){
        // error occurs
        return result;
      }

      // insert success
      if(!l.insert(key, rid)){
        result = l.write(pid,pf);
        if(result){
          // error occurs
          return result;
        }
        return 0;
      }

      // leafNode overflow
      else{
        BTLeafNode sibling;
        // update leafNodeOverflow flag so that when this level return, nonLeafNode level will know leafNode has overflow
        leafNodeOverflow = true;

        // before insertAndSplit we need to set nextNodePtr
        siblingPid = pf.endPid();
        l.setNextNodePtr(siblingPid);

        // insert and split success
        if(!l.insertAndSplit(key, rid, sibling, siblingKey)){
          // write l buffer to pageFile with pageId pid
          result = l.write(pid,pf);
          if(result){
            return result;
          }

          // write sibling buffer to pageFile with pageId siblingPid
          result = sibling.write(siblingPid, pf);
          if(result){
            return result;
          }

          // already at rootNode level
          if (height == 1){
            // create a new root
            BTNonLeafNode newRoot;
            int newRootPid = pf.endPid();

            // initialize new root with current node pid and siblingPid before write to pageFile
            newRoot.initializeRoot(pid, siblingKey, siblingPid);

            // write new root buffer to pageFile with pageId newRootPid
            result = newRoot.write(newRootPid,pf);
            if(result){
              return result;
            }

            // update rootPid and treeHeight
            rootPid = newRootPid;
            treeHeight = treeHeight + 1;
            // update buffer to pageFile with pageId 0
            memcpy(buffer, &rootPid, 4);
            memcpy(buffer+4, &treeHeight, 4);
            result = pf.write(0, buffer);
            if(result){
              // error occurs
              return result;
            }
          }

          return 0;
        }

      }
    }


    // at nonLeafNode level
    else{

      BTNonLeafNode nl;
      RC result = nl.read(pid, pf);
      if(result){
        // error occurs
        return result;
      }

      PageId recursive_pid;
      nl.locateChildPtr(key, recursive_pid);

      // if success
      result = recursive_insert(key, rid, height+1, recursive_pid, treeHeight, leafNodeOverflow, nonLeafNodeOverflow, siblingKey, siblingPid);
      if(result){
        // error occurs
        return result;
      }

      // lower level leafNode overflow
      if(leafNodeOverflow) {
        BTNonLeafNode nl;
        RC result = nl.read(pid, pf);
        if(result){
          // error occurs
          return result;
        }

        // insert success
        if(!nl.insert(siblingKey, siblingPid)){
          result = nl.write(pid,pf);
          if(result){
            return result;
          }
          leafNodeOverflow = false;
          return 0;
        }

        // current nonLeafNode overflow
        else{
          BTNonLeafNode sibling;
          // update nonLeafNodeOverflow flag so that when this level return, upper level nonLeafNode will know lower level nonLeafNode has overflow
          nonLeafNodeOverflow = true;
          leafNodeOverflow = false;

          // insert and split success
          if(!nl.insertAndSplit(key, pid, sibling, siblingKey)){
            result = nl.write(pid,pf);
            if(result){
              return result;
            }

            siblingPid = pf.endPid();
            result = sibling.write(siblingPid, pf);
            if(result){
              return result;
            }

            // at root level
            if(height == 1){

              // create a new root
              BTNonLeafNode newRoot;
              int newRootPid = pf.endPid();
              // insert success
              if(!newRoot.insert(siblingKey, siblingPid)){
                result = newRoot.write(newRootPid,pf);
                if(result){
                  return result;
                }

                rootPid = newRootPid;
                treeHeight = treeHeight + 1;
                // update buffer
                memcpy(buffer, &rootPid, 4);
                memcpy(buffer+4, &treeHeight, 4);
                result = pf.write(0, buffer);
                if(result){
                  // error occurs
                  return result;
                }
                return 0;
              }

            }
            return 0;
          }
        }
      }

      // lower level nonLeafNode overflow
      else if(nonLeafNodeOverflow) {
        BTNonLeafNode nl;
        RC result = nl.read(pid, pf);
        if(result){
          // error occurs
          return result;
        }

        // insert success
        if(!nl.insert(siblingKey, siblingPid)){
          result = nl.write(pid,pf);
          if(result){
            return result;
          }
          nonLeafNodeOverflow = false;
          return 0;
        }

        // current nonLeafNode overflow
        else{
          BTNonLeafNode sibling;
          // update overflow flag so that when this level return, upper level nonLeafNode will know lower level nonLeafNode has overflow
          nonLeafNodeOverflow = true;
          leafNodeOverflow = false;

          // insert and split success
          if(!nl.insertAndSplit(key, pid, sibling, siblingKey)){
            result = nl.write(pid,pf);
            if(result){
              return result;
            }

            siblingPid = pf.endPid();
            result = sibling.write(siblingPid, pf);
            if(result){
              return result;
            }

            // at root level
            if(height == 1){

              // create a new root
              BTNonLeafNode newRoot;
              int newRootPid = pf.endPid();

              // initialize new root with current node pid and siblingPid before write to pageFile
              newRoot.initializeRoot(pid, siblingKey, siblingPid);

              // write new root buffer to pageFile with pageId newRootPid
              result = newRoot.write(newRootPid,pf);
              if(result){
                return result;
              }

              // update rootPid and treeHeight
              rootPid = newRootPid;
              treeHeight = treeHeight + 1;
              // update buffer to pageFile with pageId 0
              memcpy(buffer, &rootPid, 4);
              memcpy(buffer+4, &treeHeight, 4);
              result = pf.write(0, buffer);
              if(result){
                // error occurs
                return result;
              }

            }
            return 0;
          }
        }
      }

      // lower level no overflow
      else {
          return 0;
      }
    }

    return 0;
}

/**
 * Run the standard B+Tree key search algorithm and identify the
 * leaf node where searchKey may exist. If an index entry with
 * searchKey exists in the leaf node, set IndexCursor to its location
 * (i.e., IndexCursor.pid = PageId of the leaf node, and
 * IndexCursor.eid = the searchKey index entry number.) and return 0.
 * If not, set IndexCursor.pid = PageId of the leaf node and
 * IndexCursor.eid = the index entry immediately after the largest
 * index key that is smaller than searchKey, and return the error
 * code RC_NO_SUCH_RECORD.
 * Using the returned "IndexCursor", you will have to call readForward()
 * to retrieve the actual (key, rid) pair from the index.
 * @param key[IN] the key to find
 * @param cursor[OUT] the cursor pointing to the index entry with
 *                    searchKey or immediately behind the largest key
 *                    smaller than searchKey.
 * @return 0 if searchKey is found. Othewise an error code
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
    // initial height = 1
    // intial pid = pid of root
    // toDO: set pid of root in constructor

    // obtain rootPid and treeHeight from buffer
    int temp_rootpid, temp_treeHeight;
    memcpy(&temp_rootpid, buffer, 4);
    memcpy(&temp_treeHeight, buffer+4, 4);
    rootPid = temp_rootpid;
    treeHeight = temp_treeHeight;

    return recursive_locate(searchKey, cursor, 1, rootPid, treeHeight);
    //return 0;
}

// ToDo: not sure if pid should be a PageId or PageId&

RC BTreeIndex::recursive_locate(int searchKey, IndexCursor& cursor, int height, PageId& pid, int treeHeight) {
    BTNonLeafNode n;
    BTLeafNode l;
    RC res;
    //PageFile pf;
    // if height reaches treeHeight, then we're looking at a leaf node
    if (height == treeHeight) {
        int eid;

        res = l.read(pid, pf);
        if (res != 0) return RC_NO_SUCH_RECORD; // see bruinbase.h for RC_NO_SUCH_RECORD


        res = l.locate(searchKey, eid); // eid instead of pid in this case: see BTLeafNode::read implementation
        if (res != 0 ) return RC_NO_SUCH_RECORD; // toDo: find what error code to return

        // IndexCursor has two parameters: pid and eid (see BTreeIndex.h)
        cursor.pid = pid;
        cursor.eid = eid;

        return 0;

    } else {
        // if here, then we're still looking through nonleaf nodes

        // need to compare the current key with searchKey
        // if we find it, then move to the corresponding pid and search from there
        // do this with btnonleafnode.read(pid, buffer)

        res = n.read(pid, pf);

        if (res != 0 ) return RC_NO_SUCH_RECORD;

        res = n.locateChildPtr(searchKey, pid);

        if (res != 0) return RC_NO_SUCH_RECORD; // ToDo: which error code to return here?

        // have to increment height
        // pid is now the next pid found by locateChildPtr
        return recursive_locate(searchKey, cursor, height+1, pid, treeHeight);
    }

}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
    BTLeafNode l;
    RC res = l.read(cursor.pid, pf);

    if (res != 0) return res;

    // BTreeNode.cc: BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
    res = l.readEntry(cursor.eid, key, rid);

    if (res != 0) return res;

    // Step forward
    PageId nextPid = cursor.pid;
    int nextEid = cursor.eid;

    // Increase eid. if it hits the end as defined by getKeyCount,
    // then set eid to 0 and move to the next pid
    if (nextEid + 1 >= l.getKeyCount()) {
        nextEid = 0;
        nextPid = l.getNextNodePtr();
    } else {
        nextEid++;
    }

    // set the new values of pid and eid
    cursor.pid = nextPid;
    cursor.eid = nextEid;

    return 0;

}
