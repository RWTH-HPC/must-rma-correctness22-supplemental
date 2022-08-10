/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_FinishNotify.h
 *       @see I_FinishNotify.
 *
 *  @date 05.08.2013
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "BaseIds.h"

#ifndef I_FINISHNOTIFY_H
#define I_FINISHNOTIFY_H

/**
 * Class for listeners to finish.
 */
class I_FinishListener
{
public:
    virtual void finish (void) = 0;
};

/**
 * Notifies other modules of an impeding shutdown of the tool.
 *
 * Dependencies (order as listed):
 * - NONE
 *
 */
class I_FinishNotify : public gti::I_Module
{
public:
	/**
	 * Notification of finish.
	 */
    virtual gti::GTI_ANALYSIS_RETURN finish (void) = 0;

    /**
     * Adds a listener.
     */
    virtual void addListener (I_FinishListener *listener) = 0;
};/*class I_FinishNotify*/

#endif /*I_FINISHNOTIFY_H*/
