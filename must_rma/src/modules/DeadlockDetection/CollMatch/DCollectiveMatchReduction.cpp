/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file DCollectiveMatchReduction.cpp
 *       @see must::DCollectiveMatchReduction.
 *
 *  @date 25.04.2012
 *  @author Fabian Haensel, Tobias Hilbrich, Joachim Protze, Mathias Korepkat
 */

#include "GtiMacros.h"

#include "DCollectiveMatchReduction.h"

using namespace must;

mGET_INSTANCE_FUNCTION(DCollectiveMatchReduction)
mFREE_INSTANCE_FUNCTION(DCollectiveMatchReduction)
mPNMPI_REGISTRATIONPOINT_FUNCTION(DCollectiveMatchReduction)

//=============================
// Constructor
//=============================
DCollectiveMatchReduction::DCollectiveMatchReduction(const char* instanceName)
: DCollectiveMatch<DCollectiveMatchReduction, I_DCollectiveMatchReduction> (instanceName, true)
  {
    //Nothing to do
  }

//=============================
// Destructor
//=============================
DCollectiveMatchReduction::~DCollectiveMatchReduction()
{
    //Nothing to do
}

/*EOF*/
