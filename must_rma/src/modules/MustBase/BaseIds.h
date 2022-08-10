/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file BaseIds.h
 * 		Header with typedef for the location id.
 *
 * @author Tobias Hilbrich
 * @date 04.01.2011
 */

#include <stdint.h>
#include <string>
#include <utility>

#include "GtiTypes.h"

#ifndef BASEIDS_H
#define BASEIDS_H

/**
 * Type for location ids.
 * These IDs represent a call location in source, which is at least the name of the call and potentially a call stack.
 * In addition, these id's encode an occurrence count that represents how often this call was issued by the application.
 */
typedef uint64_t MustLocationId;

/**
 * Type for parallel ids.
 */
typedef uint64_t MustParallelId;

/**
 * Type for identifing argument indices and names.
 */
typedef int MustArgumentId;

/**
 * Type for RMA call ids.
 */
typedef uint64_t MustRMAId;


/**
 * A stack level, used if available.
 * Used both in I_InitLocationId and I_LocationAnalysis,
 * thus here.
 */
struct MustStackLevelInfo
{
    std::string symName;
    std::string fileModule;
    std::string lineOffset;

    MustStackLevelInfo() = default;

    MustStackLevelInfo(std::string fileModule, std::string symName,
                       std::string lineOffset = "")
        : symName(std::move(symName)), fileModule(std::move(fileModule)),
          lineOffset(std::move(lineOffset)) {}

    inline bool operator==(const MustStackLevelInfo &o) const {
      return (symName == o.symName && lineOffset == o.lineOffset);
    }
    inline bool operator<(const MustStackLevelInfo &o) const {
      if (symName < o.symName)
        return true;
      if (symName != o.symName)
        return false;
      if (lineOffset < o.lineOffset)
        return true;
      return false;
    }
};

/**
 * Enumeration for argument name constants.
 *
 * Important: if you change something here,
 * make sure to reflect this in ArgumentAnalysis.cpp at
 * MUST::ArgumentAnalysis::ArgumentAnalysis.
 */
enum MustArgumentNameIds
{
	MUST_ARGUMENT_COMM = 0,
	MUST_ARGUMENT_BUF,
	MUST_ARGUMENT_COUNT,
	MUST_ARGUMENT_DATATYPE,
	MUST_ARGUMENT_DEST,
	MUST_ARGUMENT_TAG,
	MUST_ARGUMENT_SOURCE,
	MUST_ARGUMENT_STATUS,
	MUST_ARGUMENT_BUFFER,
	MUST_ARGUMENT_SIZE,
	MUST_ARGUMENT_REQUEST,
	MUST_ARGUMENT_FLAG,
	MUST_ARGUMENT_ARRAY_OF_REQUESTS,
	MUST_ARGUMENT_INDEX,
	MUST_ARGUMENT_ARRAY_OF_STATUSES,
	MUST_ARGUMENT_INCOUNT,
	MUST_ARGUMENT_OUTCOUNT,
	MUST_ARGUMENT_ARRAY_OF_INDICES,
	MUST_ARGUMENT_SENDBUF,
	MUST_ARGUMENT_SENDCOUNT,
	MUST_ARGUMENT_SENDTYPE,
	MUST_ARGUMENT_SENDTAG,
	MUST_ARGUMENT_RECVBUF,
	MUST_ARGUMENT_RECVCOUNT,
	MUST_ARGUMENT_RECVTYPE,
	MUST_ARGUMENT_RECVTAG,
	MUST_ARGUMENT_OLDTYPE,
	MUST_ARGUMENT_NEWTYPE,
	MUST_ARGUMENT_BLOCKLENGTH,
	MUST_ARGUMENT_STRIDE,
	MUST_ARGUMENT_ARRAY_OF_BLOCKLENGTHS,
	MUST_ARGUMENT_ARRAY_OF_DISPLACEMENTS,
	MUST_ARGUMENT_ARRAY_OF_TYPES,
	MUST_ARGUMENT_LOCATION,
	MUST_ARGUMENT_ADDRESS,
	MUST_ARGUMENT_EXTENT,
	MUST_ARGUMENT_DISPLACEMENT,
	MUST_ARGUMENT_INBUF,
	MUST_ARGUMENT_OUTBUF,
	MUST_ARGUMENT_OUTSIZE,
	MUST_ARGUMENT_POSITION,
	MUST_ARGUMENT_INSIZE,
	MUST_ARGUMENT_ERRORCODE,
	MUST_ARGUMENT_ORIGIN_ADDR,
	MUST_ARGUMENT_ORIGIN_COUNT,
	MUST_ARGUMENT_ORIGIN_DATATYPE,
	MUST_ARGUMENT_TARGET_RANK,
	MUST_ARGUMENT_TARGET_DISP,
	MUST_ARGUMENT_TARGET_COUNT,
	MUST_ARGUMENT_TARGET_DATATYPE,
	MUST_ARGUMENT_OP,
	MUST_ARGUMENT_WIN,
	MUST_ARGUMENT_ERRORCLASS,
	MUST_ARGUMENT_STRING,
	MUST_ARGUMENT_RECVCOUNTS,
	MUST_ARGUMENT_DISPLS,
	MUST_ARGUMENT_INFO,
	MUST_ARGUMENT_BASEPTR,
	MUST_ARGUMENT_SENDCOUNTS,
	MUST_ARGUMENT_SDISPLS,
	MUST_ARGUMENT_RDISPLS,
	MUST_ARGUMENT_SENDTYPES,
	MUST_ARGUMENT_RECVTYPES,
	MUST_ARGUMENT_KEYVAL,
	MUST_ARGUMENT_ATTRIBUTE_VAL,
	MUST_ARGUMENT_ROOT,
	MUST_ARGUMENT_RANK,
	MUST_ARGUMENT_MAXDIMS,
	MUST_ARGUMENT_COORDS,
	MUST_ARGUMENT_OLD_COMM,
	MUST_ARGUMENT_NDIMS,
	MUST_ARGUMENT_DIMS,
	MUST_ARGUMENT_PERIODS,
	MUST_ARGUMENT_REORDER,
	MUST_ARGUMENT_COMM_CART,
	MUST_ARGUMENT_NEWRANK,
	MUST_ARGUMENT_DIRECTION,
	MUST_ARGUMENT_DISP,
	MUST_ARGUMENT_RANK_SOURCE,
	MUST_ARGUMENT_RANK_DEST,
	MUST_ARGUMENT_REMAIN_DIMS,
	MUST_ARGUMENT_NEW_COMM,
	MUST_ARGUMENT_PORT_NAME,
	MUST_ARGUMENT_NEWCOMM,
	MUST_ARGUMENT_COMM1,
	MUST_ARGUMENT_COMM2,
	MUST_ARGUMENT_RESULT,
	MUST_ARGUMENT_FUNCTION,
	MUST_ARGUMENT_ERRHANDLER,
	MUST_ARGUMENT_COMM_COPY_ATTR_FN,
	MUST_ARGUMENT_COMM_DELETE_ATTR_FN,
	MUST_ARGUMENT_COMM_KEYVAL,
	MUST_ARGUMENT_EXTRA_STATE,
	MUST_ARGUMENT_GROUP,
	MUST_ARGUMENT_ERHANDLER,
	MUST_ARGUMENT_COMM_NAME,
	MUST_ARGUMENT_RESULTLEN,
	MUST_ARGUMENT_PARENT,
	MUST_ARGUMENT_FD,
	MUST_ARGUMENT_INTERCOMM,
	MUST_ARGUMENT_COMMAND,
	MUST_ARGUMENT_ARGV,
	MUST_ARGUMENT_MAXPROCS,
	MUST_ARGUMENT_ARRAY_OF_ERRCODES,
	MUST_ARGUMENT_ARRAY_OF_COMMANDS,
	MUST_ARGUMENT_ARRAY_OF_ARGV,
	MUST_ARGUMENT_ARRAY_OF_MAXPROCS,
	MUST_ARGUMENT_ARRAY_OF_INFO,
	MUST_ARGUMENT_COLOR,
	MUST_ARGUMENT_KEY,
	MUST_ARGUMENT_NNODES,
	MUST_ARGUMENT_FILE,
	MUST_ARGUMENT_FH,
	MUST_ARGUMENT_FILENAME,
	MUST_ARGUMENT_AMODE,
	MUST_ARGUMENT_INFO_USED,
	MUST_ARGUMENT_ETYPE,
	MUST_ARGUMENT_FILETYPE,
	MUST_ARGUMENT_DATAREP,
	MUST_ARGUMENT_OFFSET,
	MUST_ARGUMENT_WHENCE,
	MUST_ARGUMENT_BASE,
	MUST_ARGUMENT_NAME,
	MUST_ARGUMENT_VERSION,
	MUST_ARGUMENT_SUBVERSION,
	MUST_ARGUMENT_COMM_OLD,
	MUST_ARGUMENT_EDGES,
	MUST_ARGUMENT_COMM_GRAPH,
	MUST_ARGUMENT_SOURCES,
	MUST_ARGUMENT_DEGREES,
	MUST_ARGUMENT_DESTINATIONS,
	MUST_ARGUMENT_WEIGHTS,
	MUST_ARGUMENT_COMM_DIST_GRAPH,
	MUST_ARGUMENT_MAXINDICES,
	MUST_ARGUMENT_MAXEDGES,
	MUST_ARGUMENT_NNEIGHBORS,
	MUST_ARGUMENT_MAXNEIGHBORS,
	MUST_ARGUMENT_NEIGHBORS,
	MUST_ARGUMENT_NEDGES,
	MUST_ARGUMENT_QUERY_FN,
	MUST_ARGUMENT_FREE_FN,
	MUST_ARGUMENT_CANCEL_FN,
	MUST_ARGUMENT_GROUP1,
	MUST_ARGUMENT_GROUP2,
	MUST_ARGUMENT_NEWGROUP,
	MUST_ARGUMENT_N,
	MUST_ARGUMENT_RANKS,
	MUST_ARGUMENT_RANGES3,
	MUST_ARGUMENT_RANKS1,
	MUST_ARGUMENT_RANKS2,
	MUST_ARGUMENT_NEWINFO,
	MUST_ARGUMENT_VALUELEN,
	MUST_ARGUMENT_VALUE,
	MUST_ARGUMENT_NKEYS,
	MUST_ARGUMENT_ARGC,
	MUST_ARGUMENT_REQUIRED,
	MUST_ARGUMENT_PROVIDED,
	MUST_ARGUMENT_LOCAL_COMM,
	MUST_ARGUMENT_LOCAL_LEADER,
	MUST_ARGUMENT_BRIDGE_COMM,
	MUST_ARGUMENT_REMOTE_LEADER,
	MUST_ARGUMENT_NEWINTERCOMM,
	MUST_ARGUMENT_NEWINTRACOMM,
	MUST_ARGUMENT_HIGH,
	MUST_ARGUMENT_COPY_FN,
	MUST_ARGUMENT_DELETE_FN,
	MUST_ARGUMENT_SERVICE_NAME,
	MUST_ARGUMENT_COMMUTE,
	MUST_ARGUMENT_LEVEL,
	MUST_ARGUMENT_INOUTBUF,
	MUST_ARGUMENT_READ_CONVERSION_FN,
	MUST_ARGUMENT_WRITE_CONVERSION_FN,
	MUST_ARGUMENT_DTYPE_FILE_EXTENT_FN,
	MUST_ARGUMENT_IBUF,
	MUST_ARGUMENT_C_STATUS,
	MUST_ARGUMENT_F_STATUS,
	MUST_ARGUMENT_TYPE,
	MUST_ARGUMENT_GSIZE_ARRAY,
	MUST_ARGUMENT_DISTRIB_ARRAY,
	MUST_ARGUMENT_DARG_ARRAY,
	MUST_ARGUMENT_PSIZE_ARRAY,
	MUST_ARGUMENT_ORDER,
	MUST_ARGUMENT_P,
	MUST_ARGUMENT_R,
	MUST_ARGUMENT_TYPE_COPY_ATTR_FN,
	MUST_ARGUMENT_TYPE_DELETE_ATTR_FN,
	MUST_ARGUMENT_TYPE_KEYVAL,
	MUST_ARGUMENT_ARRAY_OF_BLOCK_LENGTHS,
	MUST_ARGUMENT_SIZE_ARRAY,
	MUST_ARGUMENT_SUBSIZE_ARRAY,
	MUST_ARGUMENT_START_ARRAY,
	MUST_ARGUMENT_LB,
	MUST_ARGUMENT_MTYPE,
	MUST_ARGUMENT_MAX_INTEGERS,
	MUST_ARGUMENT_MAX_ADDRESSES,
	MUST_ARGUMENT_MAX_DATATYPES,
	MUST_ARGUMENT_ARRAY_OF_INTEGERS,
	MUST_ARGUMENT_ARRAY_OF_ADDRESSES,
	MUST_ARGUMENT_ARRAY_OF_DATATYPES,
	MUST_ARGUMENT_ARRAY_OF_SIZES,
	MUST_ARGUMENT_ARRAY_OF_SUBSIZES,
	MUST_ARGUMENT_ARRAY_OF_STARTS,
	MUST_ARGUMENT_ARRAY_OF_GSIZES,
	MUST_ARGUMENT_ARRAY_OF_DISTRIBS,
	MUST_ARGUMENT_ARRAY_OF_DARGS,
	MUST_ARGUMENT_ARRAY_OF_PSIZES,
	MUST_ARGUMENT_NUM_INTEGERS,
	MUST_ARGUMENT_NUM_ADDRESSES,
	MUST_ARGUMENT_NUM_DATATYPES,
	MUST_ARGUMENT_COMBINER,
	MUST_ARGUMENT_TYPE_NAME,
	MUST_ARGUMENT_TRUE_LB,
	MUST_ARGUMENT_TRUE_EXTENT,
	MUST_ARGUMENT_TYPECLASS,
	MUST_ARGUMENT_ATTR_VAL,
	MUST_ARGUMENT_UB,
	MUST_ARGUMENT_DISP_UNIT,
	MUST_ARGUMENT_WIN_COPY_ATTR_FN,
	MUST_ARGUMENT_WIN_DELETE_ATTR_FN,
	MUST_ARGUMENT_WIN_KEYVAL,
	MUST_ARGUMENT_ASSERT,
	MUST_ARGUMENT_WIN_NAME,
	MUST_ARGUMENT_LOCK_TYPE,
	MUST_ARGUMENT_INDICES,
	MUST_ARGUMENT_RANGES,
	MUST_ARGUMENT_PEER_COMM,
	MUST_ARGUMENT_ARRAY_OF_IDISPLACEMENTS,

	//Keep this as last element, we use it elsewhere ...
	MUST_ARGUMENT_LAST_ARGUMENT
};

#endif /*BASEIDS*/
