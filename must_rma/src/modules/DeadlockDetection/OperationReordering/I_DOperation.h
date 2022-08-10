/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_DOperation.h
 *       @see MUST::I_DOperation.
 *
 *  @date 24.01.2012
 *  @author Tobias Hilbrich, Joachim Protze, Mathias Korepkat, Fabian Haensel
 */

#include "GtiEnums.h"
#include "MustEnums.h"
#include <iostream>

#ifndef I_DOPERATION_H
#define I_DOPERATION_H

using namespace gti;

namespace must
{


    /**
     * Interface for all operations in distributed processing.
     */
    class I_DOperation
    {
    public:
        /**
         * Virtual destructor needed.
         */
        virtual ~I_DOperation () {};

        /**
         * Called when a operation should be processed.
         * @param rank to which the operation belongs.
         * @return GTI_SUCCESS if successful.
         */
        virtual PROCESSING_RETURN process (int rank) = 0;

        /**
         * Called when human readable information on
         * an operation shall be printed. Primary use case
         * is for debugging.
         * @param out stream to print into.
         * @return GTI_SUCCESS if successful.
         */
        virtual GTI_RETURN print (std::ostream &out) = 0;
    };
} /*namespace must*/

#endif /*I_DOPERATION_H*/
