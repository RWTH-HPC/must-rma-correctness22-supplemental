/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file DCollectiveMatchRoot.cpp
 *       @see must::DCollectiveMatchRoot.
 *
 *  @date 25.04.2012
 *  @author Fabian Haensel, Tobias Hilbrich, Joachim Protze, Mathias Korepkat
 */

#include "GtiMacros.h"

#include "DCollectiveMatchRoot.h"

using namespace must;

mGET_INSTANCE_FUNCTION(DCollectiveMatchRoot)
mFREE_INSTANCE_FUNCTION(DCollectiveMatchRoot)
mPNMPI_REGISTRATIONPOINT_FUNCTION(DCollectiveMatchRoot)

//=============================
// Constructor
//=============================
DCollectiveMatchRoot::DCollectiveMatchRoot(const char* instanceName)
: DCollectiveMatch<DCollectiveMatchRoot, I_DCollectiveMatchRoot> (instanceName, false)
  {
    //Nothing to do
  }

//=============================
// Destructor
//=============================
DCollectiveMatchRoot::~DCollectiveMatchRoot()
{
    //Nothing to do
}

/*EOF*/
