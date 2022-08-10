/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_MessageFileReduction.h
 *       Interface for changing the output file for messages
 */

#ifndef MUST_I_MESSAGEFILEREDUCTION_H
#define MUST_I_MESSAGEFILEREDUCTION_H


#include <list>

#include "I_Module.h"
#include "I_Reduction.h"


/**
 * Interface that tries to redirect @ref changeMessageFile events.
 */
class I_MessageFileReduction : public gti::I_Module, public gti::I_Reduction
{
  public:
    /**
     * Entry point for @ref changeMessageFile event.
     *
     * This method receives a new @ref changeMessageFile event and redirects it
     * to the reduction and filter methods.
     *
     *
     * @param fileId              The sequential file ID.
     * @param filename            The new filename to be used.
     * @param len                 Length of @p filename array.
     *
     * @return @see gti::GTI_ANALYSIS_RETURN
     */
    virtual gti::GTI_ANALYSIS_RETURN
    changeMessageFile(size_t fileId, const char *filename, size_t len) = 0;

    /**
     * Entry point for @ref changeMessageFile event.
     *
     * @param filename            The new filename to be used.
     *
     * @return @see gti::GTI_ANALYSIS_RETURN
     */
    virtual gti::GTI_ANALYSIS_RETURN
    changeMessageFileWrapper(const char *filename) = 0;

    /**
     * Filter the @ref changeMessageFile event.
     *
     * This method filters the @ref changeMessageFile event and sends only the
     * first event to the message logger to open the new message log-file.
     *
     *
     * @param fileId              The sequential file ID.
     * @param filename            The new filename to be used.
     * @param len                 Length of @p filename array.
     * @param thisChannel         The channel hitting the reduction.
     * @param outFinishedChannels The channels that already hit the reduction.
     *
     * @return @see gti::GTI_ANALYSIS_RETURN
     */
    virtual gti::GTI_ANALYSIS_RETURN
    filter(size_t fileId,
           const char *filename,
           size_t len,
           gti::I_ChannelId *thisChannel,
           std::list<gti::I_ChannelId *> *outFinishedChannels) = 0;

    /**
     * Reduce the @ref changeMessageFile event.
     *
     * This method reduces the @ref changeMessageFile event and sends an
     * `closeFile` event for the message logger to close the old message
     * log-file, after all processes have switched to the new file.
     *
     *
     * @param fileId              The sequential file ID.
     * @param thisChannel         The channel hitting the reduction.
     * @param outFinishedChannels The channels that already hit the reduction.
     *
     * @return @see gti::GTI_ANALYSIS_RETURN
     */
    virtual gti::GTI_ANALYSIS_RETURN
    reduce(size_t fileId,
           gti::I_ChannelId *thisChannel,
           std::list<gti::I_ChannelId *> *outFinishedChannels) = 0;
};


#endif
