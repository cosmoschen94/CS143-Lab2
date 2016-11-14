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

using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
    // need to initialize treeHeight
    treeHeight = 1; // toDo: is this correct?

    rootPid = -1;
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

    RC res = pf.open(indexname, mode);
    if (res != 0 ) return res;
    // might have to add more here
    return 0;
}

/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
    RC res = pf.close();
    if (res != 0 ) return res;
    // might have to add more here
    return 0;
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
RC BTreeIndex::insert(int key, const RecordId& rid)
{
    // we'll need to change treeHeight accordingly
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
    return recursive_locate(searchkey, cursor, 1, rootPid);
    //return 0;
}

// ToDo: not sure if pid should be a PageId or PageId&

RC recursive_locate(int searchKey, IndexCursor& cursor, int height, PageId& pid) {
    // if height reaches treeHeight, then we're looking at a leaf node
    if (height == treeHeight) {
        // toDo: do stuff
    } else {
        // if here, then we're still looking through nonleaf nodes

        // need to compare the key at cursor with searchKey
        // if we find it, then move to the corresponding pid and search from there
        // do this with btnonleafnode.read(pid, buffer)
        // ToDo: not sure if buffer is pf in this case
        BTNonLeafNode n;
        RC res = n.read(pid, pf);

        if (res != 0 ) return -1; // ToDo: which error code to return here? RC_NO_SUCH_RECORD?

        res = n.locateChildPtr(searchKey, pid);

        if (res != 0) return -1; // ToDo: which error code to return here?

        // have to increment height
        // pid is now the next pid found by locateChildPtr 
        return recursive_locate(searchKey, cursor, height+1, pid)
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
    return 0;
}
