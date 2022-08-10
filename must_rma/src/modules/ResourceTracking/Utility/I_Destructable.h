/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_Destructable.h
 *       @see I_Destructable.
 *
 *  @date 27.06.2011
 *  @author Joachim Protze
 */

#ifndef I_DESTRUCTABLE_H
#define I_DESTRUCTABLE_H

namespace must
{
    /**
     * Interface to make persistent handle storage objects destructable
     */
    class I_Destructable
    {
    public:
        /**
         * Erases the information.
         *
         * @return true if this erased the last use of this persistent information, this information should not be used
         * by the user.
         */
        virtual bool erase (void) = 0;

        /**
         * Allows to copy this persistent information.
         * I.e. the persistent info can now be copied
         * and each of the copies needs to call erase
         * respectively.
         * @return true if successful, false otherwise.
         */
        virtual bool copy (void) = 0;
    };
}

#endif /* I_DESTRUCTABLE_H */
