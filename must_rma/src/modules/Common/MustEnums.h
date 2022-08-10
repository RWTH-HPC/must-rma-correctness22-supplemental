/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MustEnums.h
 *       Global enumerations used for MUST.
 *
 *  @date 21.01.2010
 *  @author Tobias Hilbrich
 */

#include "MustTypes.h"
#ifndef MUSTENUMS_H
#define MUSTENUMS_H

namespace must
{
    /**
     * Enumeration for arc types.
     */
    enum ArcType
    {
        ARC_AND = 0,
        ARC_OR
    };

    /**
     * Enumeration of all collective communications.
     * IMPORTANT: these serve both for the blocking and non-blocking
     *            versions of the collectives. E.g., MUST_COLL_GATHER
     *            is used for both MPI_Gather and MPI_Igather.
     *            The two will differentiate in the fact that they may
     *            or may not have a request.
     */
    enum MustCollCommType
    {
        MUST_COLL_GATHER = 1,
        MUST_COLL_GATHERV,
        MUST_COLL_REDUCE,
        MUST_COLL_BCAST,
        MUST_COLL_SCATTER,
        MUST_COLL_SCATTERV,
        MUST_COLL_ALLGATHER,
        MUST_COLL_ALLGATHERV,
        MUST_COLL_ALLTOALL,
        MUST_COLL_ALLTOALLV,
        MUST_COLL_ALLTOALLW,
        MUST_COLL_ALLREDUCE,
        MUST_COLL_REDUCE_SCATTER,
        MUST_COLL_REDUCE_SCATTER_BLOCK,
        MUST_COLL_SCAN,
        MUST_COLL_EXSCAN,
        MUST_COLL_BARRIER,
        MUST_COLL_CART_CREATE,
        MUST_COLL_CART_SUB,
        MUST_COLL_COMM_CREATE,
        MUST_COLL_COMM_DUP,
        MUST_COLL_COMM_FREE,
        MUST_COLL_COMM_SPLIT,
        MUST_COLL_FINALIZE,
        MUST_COLL_GRAPH_CREATE,
        MUST_COLL_INTERCOMM_CREATE,
        MUST_COLL_INTERCOMM_MERGE,
        MUST_COLL_WIN_ALLOCATE,
        MUST_COLL_WIN_ALLOCATE_SHARED,
        MUST_COLL_WIN_CREATE,
        MUST_COLL_WIN_CREATE_DYNAMIC,
        MUST_COLL_WIN_FENCE,
        MUST_COLL_WIN_FREE,
        MUST_COLL_UNKNOWN /*Used for invalid initialization and queriny the size of the enum*/
    };

    enum MustSendMode
	{
		MUST_BUFFERED_SEND = 0,
		MUST_READY_SEND = 1,
		MUST_SYNCHRONIZED_SEND = 2,
		MUST_STANDARD_SEND = 3,
		MUST_UNKNOWN_SEND
	};

    // clang-format off
	// Used for enum MustMessageIdNames
    #define FOREACH_MUST_ERRORS(macro) \
        macro (MUST_ERROR_INTEGER_NEGATIVE) \
        macro (MUST_ERROR_INTEGER_ZERO) \
        macro (MUST_ERROR_INTEGER_NEGATIVE_ARRAY) \
        macro (MUST_ERROR_INTEGER_ENTRY_GREATER_OR_EQUAL) \
        macro (MUST_ERROR_INTEGER_NEGATIVE_NOT_PROC_NULL_ANY_SOURCE) \
        macro (MUST_ERROR_INTEGER_NEGATIVE_NOT_PROC_NULL) \
        macro (MUST_ERROR_INTEGER_NEGATIVE_NOT_PROC_NULL_ARRAY) \
        macro (MUST_ERROR_INTEGER_NEGATIVE_PROC_NULL_ANY_SOURCE) \
        macro (MUST_ERROR_INTEGER_NEGATIVE_UNDEFINED) \
        macro (MUST_ERROR_INTEGER_NOT_WITHIN_ZERO_TAG_UB) \
        macro (MUST_ERROR_INTEGER_NOT_WITHIN_ZERO_TAG_UB_ANY_TAG) \
        macro (MUST_ERROR_INTEGER_GREATER_COMM_SIZE) \
        macro (MUST_ERROR_INTEGER_GREATER_EQUAL_COMM_SIZE) \
        macro (MUST_ERROR_INTEGER_PRODUCT_GREATER_COMM_SIZE) \
        macro (MUST_ERROR_GROUP_RANGE_RANK) \
        macro (MUST_ERROR_GROUP_RANGE_STRIDE) \
        macro (MUST_ERROR_REQUEST_ACTIVE) \
        macro (MUST_ERROR_REQUEST_ACTIVE_ARRAY) \
        macro (MUST_ERROR_REQUEST_NOT_KNOWN) \
        macro (MUST_ERROR_REQUEST_NOT_KNOWN_ARRAY) \
        macro (MUST_ERROR_REQUEST_NULL) \
        macro (MUST_ERROR_REQUEST_NULL_ARRAY) \
        macro (MUST_ERROR_REQUEST_PERSISTENT_BUT_INACTIVE) \
        macro (MUST_ERROR_COMM_UNKNWOWN) \
        macro (MUST_ERROR_COMM_NULL) \
        macro (MUST_ERROR_NOT_CART_COMM) \
        macro (MUST_ERROR_NOT_GRAPH_COMM) \
        macro (MUST_ERROR_INTER_COMM) \
        macro (MUST_ERROR_INTER_COMM_MPI1) \
        macro (MUST_ERROR_ROOT_NOT_IN_COMM) \
        macro (MUST_ERROR_PREDEFINED_COMM) \
        macro (MUST_ERROR_NOT_INTER_COMM) \
        macro (MUST_ERROR_POINTER_NULL) \
        macro (MUST_ERROR_LEAK_COMM) \
        macro (MUST_ERROR_LEAK_DATATYPE) \
        macro (MUST_ERROR_LEAK_REQUEST) \
        macro (MUST_ERROR_LEAK_GROUP) \
        macro (MUST_ERROR_LEAK_ERR) \
        macro (MUST_ERROR_LEAK_KEYVAL) \
        macro (MUST_ERROR_LEAK_OP) \
        macro (MUST_ERROR_DIRECTION_GREATER_NDIMS) \
        macro (MUST_ERROR_DATATYPE_NULL) \
        macro (MUST_ERROR_DATATYPE_UNKNOWN) \
        macro (MUST_ERROR_DATATYPE_NOT_COMMITED) \
        macro (MUST_ERROR_GROUP_NULL) \
        macro (MUST_ERROR_GROUP_UNKNOWN) \
        macro (MUST_ERROR_INTEGER_GREATER_GROUP_SIZE) \
        macro (MUST_ERROR_INTEGER_DUPLICATION_ARRAY) \
        macro (MUST_ERROR_INTEGER_GREATER_GROUP_SIZE_ARRAY) \
        macro (MUST_ERROR_INTEGER_DUPLICATION_ARRAY_TRIPLET) \
        macro (MUST_ERROR_RANK_FROM_RANGES_NOT_IN_GROUP) \
        macro (MUST_ERROR_OPERATION_PREDEFINED) \
        macro (MUST_ERROR_OPERATION_UNKNOWN) \
        macro (MUST_ERROR_OPERATION_NULL) \
        macro (MUST_ERROR_POINTER_NULL_NOT_BOTTOM) \
        macro (MUST_ERROR_POINTER_NULL_COMM_SIZE) \
        macro (MUST_ERROR_POINTER_NULL_COMM_SIZE_ARRAY) \
        macro (MUST_ERROR_POINTER_NULL_COMM_SIZE_ARRAY_AT_INDEX) \
        macro (MUST_ERROR_MPI_IN_PLACE_USED) \
        macro (MUST_ERROR_SELFOVERLAPPED) \
        macro (MUST_ERROR_OVERLAPPED_SEND) \
        macro (MUST_ERROR_OVERLAPPED_RECV) \
        macro (MUST_ERROR_POINTER_NULL_STATUS_IGNORE) \
        macro (MUST_ERROR_TYPEMATCH_INTERNAL_NOTYPE) \
        macro (MUST_ERROR_TYPEMATCH_INTERNAL_TYPESIG) \
        macro (MUST_ERROR_TYPEMATCH_MISMATCH) \
        macro (MUST_ERROR_TYPEMATCH_MISMATCH_BYTE) \
        macro (MUST_ERROR_TYPEMATCH_LENGTH) \
        macro (MUST_ERROR_TYPEMATCH_ALIGNMENT) \
        macro (MUST_ERROR_MESSAGE_LOST) \
        macro (MUST_ERROR_COLLECTIVE_CALL_MISMATCH) \
        macro (MUST_ERROR_COLLECTIVE_OP_MISMATCH) \
        macro (MUST_ERROR_COLLECTIVE_ROOT_MISMATCH) \
        macro (MUST_ERROR_COLLECTIVE_BLOCKING_NONBLOCKING_MISMATCH) \
        macro (MUST_ERROR_DEADLOCK) \
        macro (MUST_ERROR_BUFFER_REATTACH) \
        macro (MUST_ERROR_BUFFER_NOATTACHED) \
        macro (MUST_ERROR_COUNTS_ARRAYS_DIFFER) \
        macro (MUST_ERROR_MPI_MULTIPLE_THREADS) \
        macro (MUST_ERROR_UNSUPPORTED) \
        macro (MUST_ERROR_OPENMP) \
        macro (MUST_LAST_ERROR) 

   #define FOREACH_MUST_WARNING(macro) \
        macro(MUST_WARNING_INTEGER_ZERO) \
        macro(MUST_WARNING_INTEGER_ZERO_ARRAY) \
        macro (MUST_WARNING_INTEGER_NOT_ONE_OR_ZERO) \
        macro (MUST_WARNING_INTEGER_NOT_ONE_OR_ZERO_ARRAY) \
        macro (MUST_WARNING_INTEGER_HIGH_BUT_LESS_TAG_UB) \
        macro (MUST_WARNING_INTEGER_PRODUCT_LESS_COMM_SIZE) \
        macro (MUST_WARNING_INTER_COMM) \
        macro (MUST_WARNING_REQUEST_ACTIVE_RECV) \
        macro (MUST_WARNING_REQUEST_CANCELED) \
        macro (MUST_WARNING_REQUEST_NULL) \
        macro (MUST_WARNING_REQUEST_NULL_OR_INACTIVE_ARRAY) \
        macro (MUST_WARNING_REQUEST_INACTIVE) \
        macro (MUST_WARNING_NOT_CART_COMM) \
        macro (MUST_WARNING_INTER_COMM_MPI2) \
        macro (MUST_WARNING_COMM_NULL) \
        macro (MUST_WARNING_MAXDIMS_GREATER_NDIMS) \
        macro (MUST_WARNING_MAXNEIGHBORS_TO_SMALL) \
        macro (MUST_WARNING_MAXINDICES_TO_SMALL) \
        macro (MUST_WARNING_MAXEDGES_TO_SMALL) \
        macro (MUST_WARNING_DATATYPE_PREDEFINED) \
        macro (MUST_WARNING_DATATYPE_COMMITED) \
        macro (MUST_WARNING_DATATYPE_BAD_ALIGNMENT) \
        macro (MUST_WARNING_IF_EMPTY) \
        macro (MUST_WARNING_GROUP_NULL) \
        macro (MUST_WARNING_POINTER_NULL) \
        macro (MUST_WARNING_SELFOVERLAPPED) \
        macro (MUST_WARNING_BUFFER_OUTSIZED) \
        macro (MUST_WARNING_THREADLEVEL) \
        macro (MUST_WARNING_DATARACE) \
        macro (MUST_LAST_WARNING) 
        
    #define FOREACH_MUST_INFO(macro) \
        macro (MUST_INFO_FREE_NONPERSISTENT_REQUEST) \
        macro (MUST_INFO_MISSING_WC_SOURCE_EXPLORATION) \
        macro (MUST_INFO_MISSING_WC_SOURCE_EXPLORATION_STATISTICS) \
        macro (MUST_INFO_ENFORCED_WC_SOURCE_DECISION) \
        

    enum MustMessageIdNames
    {
        //Success
        MUST_MESSAGE_NO_ERROR = 0,
#define enum_entry_macro(name) name,
        //Errors
        FOREACH_MUST_ERRORS(enum_entry_macro)
        //Warnings
        FOREACH_MUST_WARNING(enum_entry_macro)
        //Informations
        FOREACH_MUST_INFO(enum_entry_macro)

#undef enum_entry_macro

        MUST_INFO_UNIMPLEMENTED_FEATURE,

        //Last entry
        MUST_LAST_MESSAGE_ID_NAME

    };
    // clang-format off

}

/**
    * Enumeration of all predefined datatypes.
    */
enum MustMpiDatatypePredefined
{
    //Elementary C
    MUST_MPI_CHAR = 0,
    MUST_MPI_SHORT,
    MUST_MPI_INT,
    MUST_MPI_LONG,
    MUST_MPI_UNSIGNED_CHAR,
    MUST_MPI_UNSIGNED_SHORT,
    MUST_MPI_UNSIGNED,
    MUST_MPI_UNSIGNED_LONG,
    MUST_MPI_FLOAT,
    MUST_MPI_DOUBLE,
    MUST_MPI_LONG_DOUBLE,

    //Elementary C & Fortran
    MUST_MPI_BYTE,
    MUST_MPI_PACKED,

    //Elementary Fortran
    MUST_MPI_INTEGER,
    MUST_MPI_REAL,
    MUST_MPI_DOUBLE_PRECISION,
    MUST_MPI_COMPLEX,
    MUST_MPI_LOGICAL,
    MUST_MPI_CHARACTER,

    //Reduction types C
    MUST_MPI_FLOAT_INT,
    MUST_MPI_DOUBLE_INT,
    MUST_MPI_LONG_INT,
    MUST_MPI_2INT,
    MUST_MPI_SHORT_INT,
    MUST_MPI_LONG_DOUBLE_INT,

    //Reduction types Fortran
    MUST_MPI_2REAL,
    MUST_MPI_2DOUBLE_PRECISION,
    MUST_MPI_2INTEGER,
    MUST_MPI_2COMPLEX,
    MUST_MPI_2DOUBLE_COMPLEX,

    //Optional C
    MUST_MPI_LONG_LONG_INT,
    MUST_MPI_LONG_LONG,
    MUST_MPI_UNSIGNED_LONG_LONG,
    MUST_MPI_WCHAR,
    MUST_MPI_SIGNED_CHAR,

    // MPI Types
    MUST_MPI_AINT,
    MUST_MPI_OFFSET,
    MUST_MPI_COUNT,
    
    //Optional C++
    MUST_MPI_CXX_BOOL,
    MUST_MPI_CXX_FLOAT_COMPLEX,
    MUST_MPI_CXX_DOUBLE_COMPLEX,
    MUST_MPI_CXX_LONG_DOUBLE_COMPLEX,

    //Optional Fortran
    MUST_MPI_INTEGER1,
    MUST_MPI_INTEGER2,
    MUST_MPI_INTEGER4,
    MUST_MPI_INTEGER8,
    MUST_MPI_INTEGER16,
    MUST_MPI_REAL2,
    MUST_MPI_REAL4,
    MUST_MPI_REAL8,
    MUST_MPI_REAL16,
    MUST_MPI_DOUBLE_COMPLEX,
    MUST_MPI_COMPLEX8,
    MUST_MPI_COMPLEX16,
    MUST_MPI_COMPLEX32,
    MUST_MPI_LOGICAL1,
    MUST_MPI_LOGICAL2,
    MUST_MPI_LOGICAL4,
    MUST_MPI_LOGICAL8,
    MUST_MPI_LOGICAL16,

    //Bound markers
    MUST_MPI_UB,
    MUST_MPI_LB,

    //Elementary C (MPI-2.2)
    MUST_MPI_C_BOOL,
    MUST_MPI_INT8_T,
    MUST_MPI_INT16_T,
    MUST_MPI_INT32_T,
    MUST_MPI_INT64_T,
    MUST_MPI_UINT8_T,
    MUST_MPI_UINT16_T,
    MUST_MPI_UINT32_T,
    MUST_MPI_UINT64_T,
    MUST_MPI_C_COMPLEX,
    MUST_MPI_C_FLOAT_COMPLEX,
    MUST_MPI_C_DOUBLE_COMPLEX,
    MUST_MPI_C_LONG_DOUBLE_COMPLEX,


    MUST_MPI_DATATYPE_UNKNOWN
};

/**
    * Enumeration of all (derived) datatype classes.
    */
enum MustMpiDatatypeClass
{
    MUST_TYPE_BASE,
    MUST_TYPE_CONTIGUOUS,
    MUST_TYPE_VECTOR,
    MUST_TYPE_HVECTOR,
    MUST_TYPE_INDEXED,
    MUST_TYPE_HINDEXED,
    MUST_TYPE_INDEXED_BLOCK,
    MUST_TYPE_STRUCT,
    MUST_TYPE_RESIZED,
    MUST_TYPE_SUBARRAY,
    MUST_TYPE_DARRAY,
    MUST_TYPE_UNKNOWN
};

/**
    * Enumeration of all predefined communicators.
    */
enum MustMpiCommPredefined
{
    MUST_MPI_COMM_WORLD = 0,
    MUST_MPI_COMM_SELF,

    MUST_MPI_COMM_UNKNOWN
};

/**
    * Enumeration of all predefined errs.
    */
enum MustMpiErrPredefined
{
    MUST_MPI_ERRORS_ARE_FATAL =0,
    MUST_MPI_ERRORS_RETURN,

    MUST_MPI_ERRORS_UNKNOWN
};

/**
    * Enumeration of all predefined groups.
    * Not used for values returned to the user as we only have one of them,
    * we still need some enum for internal use.
    */
enum MustMpiGroupPredefined
{
    MUST_MPI_GROUP_EMPTY =0,

    MUST_MPI_GROUP_UNKNOWN
};

/**
    * Enumeration of all predefined keyvals.
    */
enum MustMpiKeyvalPredefined
{
    MUST_MPI_KEY_TAG_UB =0,
    MUST_MPI_KEY_IO,
    MUST_MPI_KEY_HOST,
    MUST_MPI_KEY_WTIME_IS_GLOBAL,

    MUST_MPI_KEY_UNKNOWN
};

/**
    * Enumeration of all predefined ops.
    */
enum MustMpiOpPredefined
{
    MUST_MPI_OP_MAX =0,
    MUST_MPI_OP_MIN,
    MUST_MPI_OP_SUM,
    MUST_MPI_OP_PROD,
    MUST_MPI_OP_LAND,
    MUST_MPI_OP_BAND,
    MUST_MPI_OP_LOR,
    MUST_MPI_OP_BOR,
    MUST_MPI_OP_LXOR,
    MUST_MPI_OP_BXOR,
    MUST_MPI_OP_MAXLOC,
    MUST_MPI_OP_MINLOC,

    MUST_MPI_OP_UNKNOWN
};

/**
 * Return state when processing an operation:
 * - PROCESSING_SUCCESS: successful processing, the
 *       operation can be removed from the queue
 * - PROCESSING_ERROR: some critical error occurred
 * - PROCESSING_REEXECUTE: the operation discovered
 *       that it can't be processed at the moment (and
 *       updated I_OperationReordering accordingly). This
 *       causes the operation to stay in the queues and be
 *       processed once its newly set block or suspension
 *       is removed.
 */
enum PROCESSING_RETURN
{
    PROCESSING_SUCCESS = 0,
    PROCESSING_ERROR,
    PROCESSING_REEXECUTE
};

#endif /*MUSTENUMS_H*/
