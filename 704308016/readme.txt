This is how we divided up our work for part D:
Siyu Chen  chensiyuwilliam@hotmail.com
I implemented and improved BTreeIndex::insert() and worked on most of the debugging related to the B+ Tree when we tested on the given datasets.

My implementation for BTreeIndex::insert() had a crucial problem when tuples were inserted into the B+ Tree.

The problem was if a leaf node overflowed, the new sibling node's next pointer was never set.

It should actually be set to the pid of current leaf node.

For example,

Given leaf node 1 -> leaf node 2, after insertion of a tuple caused leaf node 1 overflow again,

my old implementation would do the following:

leaf node 1 -> leaf node 3  [no pointer here]  leaf node 2.

The new sibling node 3 would lose the next pointer to leaf node 2.

This happened because the new sibling node's next pointer was never set.

But my latest implementation fixes this problem.


Roland Zeng  roland.zeng@gmail.com
I implemented SqlEngine::select() and SqlEngine::load() and worked on different test cases.

The majority of my contributions was working on SQLEngine, adding different checks for different query combinations.
We have a lot of signals (boolean true/false) for asserting if we have min or max values for keys and values, and this determines
1) if we even need a b+ tree (NE-only or Value-only queries don't require a b+ tree)
2) the search constraints for searching through the b+ trees
