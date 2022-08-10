/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_TrackBase.h
 *       @see I_TrackBase.
 *
 *  @date 12.06.2017
 *  @author Joachim Protze
 */

#ifndef I_TRACKBASE_H
#define I_TRACKBASE_H

namespace must
{
    /**
     * Interface of common basic functions in tracking modules
     */
    template<typename FULL_INFO> //Type of full info, maintained with pointers
    class I_TrackBase
    {
    public:
        /**
         * Test whether a handle is predefined.
         * @return true if provided info is for a predefined handle.
         */
        virtual bool isPredefined (FULL_INFO * info) = 0;
    };
}

#endif /* I_TRACKBASE_H */
