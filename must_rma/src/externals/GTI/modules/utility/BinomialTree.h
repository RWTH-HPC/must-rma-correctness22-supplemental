#include <stddef.h>

#ifndef BINOMIALTREE_H
#define BINOMIALTREE_H

namespace BinomialTree {
#define PERMUT_RANK(rank, root, N) ((rank + N - root) % N)
/* Distance from root to leaves. */
extern unsigned int treeDepth(size_t groupSize);
/* Distance from root to leaves. */
extern unsigned int treeHeight(size_t groupSize);
/* Distance to leaves for certain rank. (Equals number of clocks to receive.) */
extern size_t nodeHeight(size_t groupSize, int rank, int root);
/* Distance to root for certain rank. */
extern size_t nodeDepth(size_t groupSize, int rank, int root);
extern size_t nodeClockSize(size_t numTotalProcs, size_t groupSize, int rank, int root);
extern size_t posForClockSize(size_t groupSize, int localId, int remoteLocalId, int localRootId);
extern void listWaitingNodes(int* ranks, size_t groupSize, int rank, int root, int* nodeList,
                             size_t);
} // namespace BinomialTree
#endif
