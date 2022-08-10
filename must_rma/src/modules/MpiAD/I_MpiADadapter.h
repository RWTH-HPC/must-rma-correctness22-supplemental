/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef I_MUST_MpiAD_adapter_H
#define I_MUST_MpiAD_adapter_H


#include "ModuleBase.h"


namespace must
{
/**
 * Interface for the MpiAD tools interface adapter.
 *
 * The MpiAD tools interface adapter needs to be loaded by both MUST and the
 * MpiAD runtime. Therefore, a minimal interface is required, to be loaded by
 * the weaver.
 */
class I_MpiADadapter : public gti::I_Module
{
  public:
    /**
     * Notify the analysis about the MPI stack being shutdown.
     *
     * This method get's triggert in the MPI_Finalize event and should inform
     * the analysis to shutdown the MpiAD environment.
     */
    virtual void finish() = 0;
};
} // namespace must

#endif
