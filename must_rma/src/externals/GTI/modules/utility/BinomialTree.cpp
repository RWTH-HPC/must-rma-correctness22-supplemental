#include <cmath>
#include <cstddef>

#ifndef PERMUT_RANK
#define PERMUT_RANK(rank, root, N) ((rank + N - root) % N)
#endif

namespace BinomialTree {
unsigned int treeDepth(size_t groupSize) {
    return std::ceil(std::log2(groupSize));
}

unsigned int treeHeight(size_t groupSize) {
    return std::ceil(std::log2(groupSize));
}

size_t nodeHeight(size_t groupSize, int rank, int root) {
    size_t k, height = 0;
    bool active = true;
    for (k = 0; k < treeHeight(groupSize); k++) {
        if (active) {
            if ((PERMUT_RANK(rank, root, groupSize) >> k) & 1)
                active = false;
            else if (PERMUT_RANK(rank, root, groupSize) + (1 << k) < groupSize)
                height += 1;
        }
    }
    return height;
}

size_t nodeDepth(size_t groupSize, int rank, int root) {
    return treeHeight(groupSize) - nodeHeight(groupSize, rank, root);
}

size_t nodeClockSize(size_t numTotalProcs, size_t groupSize, int rank,
                                          int root) {
    return (rank == root) ? numTotalProcs
                          : (1 << nodeHeight(groupSize, rank, root)) + (numTotalProcs - groupSize);
}

size_t posForClockSize(size_t groupSize, int localId, int remoteLocalId,
                                     int localRootId) {
    //    size_t pos = 1, i;
    //    for (i = 0; i < clockSize - compSize; i++)
    //        pos += i;
    //
    //    return pos;
    return (PERMUT_RANK(remoteLocalId, localRootId, groupSize) -
            PERMUT_RANK(localId, localRootId, groupSize));
}

void listWaitingNodes(int* ranks, size_t groupSize, int rank, int root, int* nodeList,
                                    size_t) {
    int tmpPow;
    bool active = true;
    for (unsigned int k = 0, j = 0; k < treeHeight(groupSize); k++) {
        if (active) {
            tmpPow = 1 << k;
            if ((PERMUT_RANK(rank, root, groupSize) >> k) & 1)
                active = false;
            else if (PERMUT_RANK(rank, root, groupSize) + tmpPow < groupSize)
                nodeList[j++] = ranks[PERMUT_RANK(rank, root, groupSize) + tmpPow];
        }
    }
}
} /*namespace BinomialTree*/
