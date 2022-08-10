/*
 * Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MpiTypeArt.h
 * 	@see MpiTypeArt.
 */

#include "ModuleBase.h"
#include "I_ParallelIdAnalysis.h"
#include "I_LocationAnalysis.h"
#include "I_CreateMessage.h"
#include "I_ArgumentAnalysis.h"
#include "I_DatatypeTrack.h"

#include "I_MpiTypeArt.h"

#include <list>
#include <map>

#ifndef MPITYPEART_BASE_H
#define MPITYPEART_BASE_H

using namespace gti;

namespace must
{
  
  /**
   * Implementation of MPI function call annotation using TSan.
   */
  class MpiTypeArt : public gti::ModuleBase<MpiTypeArt, I_MpiTypeArt>
  {
  public:
    /**
     * Constructor.
     * @param instanceName name of this module instance.
     */
    MpiTypeArt (const char* instanceName);
    
    /**
     * Destructor.
     */
    virtual ~MpiTypeArt (void);
    

    
    /**
     * @see I_MpiTypeArt::checkSendOrRecv
     */
    GTI_ANALYSIS_RETURN checkSendOrRecv (
	MustParallelId pId,
	MustLocationId lId,
	MustAddressType buffer,
	MustDatatypeType datatype,
	int count);
    
    /**
     * @see I_MpiTypeArt::checkSendOrRecvCounts
     */
    GTI_ANALYSIS_RETURN checkSendOrRecvCounts (
	MustParallelId pId,
	MustLocationId lId,
	MustAddressType buffer,
	const int displs[],
	const int counts[],
	MustDatatypeType datatype,
	int commsize);
    
    /**
     * @see I_MpiTypeArt::checkSendOrRecvTypes
     */
    GTI_ANALYSIS_RETURN checkSendOrRecvTypes (
	MustParallelId pId,
	MustLocationId lId,
	MustAddressType buffer,
	const int displs[],
	const int counts[],
	const MustDatatypeType datatypes[],
	int commsize);
    
    void createMessage(
    MustParallelId pId, 
    MustLocationId lId, 
    const std::string& message,
    MustMessageIdNames type);
    
  protected:

    I_ParallelIdAnalysis* myPIdMod;
    I_LocationAnalysis* myLIdMod;
    I_CreateMessage* myLogger;
    I_ArgumentAnalysis* myArgMod;
    I_DatatypeTrack* myDatMod;
    
    
  }; /* class MpiTypeArt */
} /* namespace must */

#endif /* MPITYPEART_BASE_H */
