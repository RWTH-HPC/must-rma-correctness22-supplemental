/*
 * Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_MpiTypeArt.h
 * 	@see I_MpiTypeArt.
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "BaseIds.h"
#include "I_ChannelId.h"
#include "MustTypes.h"

#include "StridedBlock.h"

#ifndef I_MPITYPEART_H
#define I_MPITYPEART_H

/**
 * Checks the underlying data type using TypeArt.
 */
class I_MpiTypeArt : public gti::I_Module
{
public:

		
	/**
         * Checks for overlaps of requests where isSend is set to true.
         * @param pId parallel context
         * @param lId location id of context.
         * @param datatype to get references for.
         * @param buffer address of transfer buffer.
         * @param count number of repetitions.
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN checkSendOrRecv (
                MustParallelId pId,
                MustLocationId lId,
                MustAddressType buffer,
                MustDatatypeType datatype,
                int count
		) = 0;
		
	/**
         * Checks whether a request overlaps memory regions spanned by open requests.
         * @param pId parallel context
         * @param lId location id of context.
         * @param buffer address of transfer buffer.
         * @param displs displacements for each block.
         * @param counts lengths of each block.
         * @param datatype to get references for.
         * @param commsize number of ranks.
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN checkSendOrRecvCounts (
                MustParallelId pId,
                MustLocationId lId,
                MustAddressType buffer,
                const int displs[],
                const int counts[],
                MustDatatypeType datatype,
                int commsize
                ) = 0;
		
	/**
         * Checks whether a request overlaps memory regions spanned by open requests.
         * @param pId parallel context
         * @param lId location id of context.
         * @param buffer address of transfer buffer.
         * @param displs displacements for each block.
         * @param counts lengths of each block.
         * @param datatype to get references for.
         * @param commsize number of ranks.
         * @return see gti::GTI_ANALYSIS_RETURN.
         */
        virtual gti::GTI_ANALYSIS_RETURN checkSendOrRecvTypes (
                MustParallelId pId,
                MustLocationId lId,
                MustAddressType buffer,
                const int displs[],
                const int counts[],
                const MustDatatypeType datatypes[],
                int commsize
                ) = 0;
		
	

  }; /* class I_MpiTypeArt */

#endif /* I_MPITYPEART_H */
