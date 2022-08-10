/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file I_MessageLogger.h
 *       @see I_MessageLogger.
 *
 *  @date 20.01.2011
 *  @author Tobias Hilbrich
 */

#include "I_Module.h"
#include "GtiEnums.h"
#include "I_CreateMessage.h"
#include "I_ChannelId.h"

#include "BaseIds.h"

#ifndef I_MESSAGELOGGER_H
#define I_MESSAGELOGGER_H

/**
 * Interface for logging messages.
 * Implementations may logg messages however they like.
 */
class I_MessageLogger : public gti::I_Module
{
public:
	/**
	 * Logs a new message.
	 * @param msgId unique identifier for this type of message, e.g. 7 for the "Buffer not allocated message".
	 * @param int true if this has an associated location (global if not).
	 * @param pId parallel Id.
	 * @param lId location id.
	 * @param msgType type of the message (error/warning/...) @see MustMessageType.
	 * @param text message text.
	 * @param textLen length of the text.
	 * @param numReferences number of extra location references used in the text.
	 * @param refPIds parallel ids of the extra locations, array of size numReferences.
	 * @param refLIds location ids of the extra locations, array of size numReferences.
	 * @return see gti::GTI_ANALYSIS_RETURN.
	 */
    virtual gti::GTI_ANALYSIS_RETURN log (
    		int msgId,
    		int hasLocation,
    		uint64_t pId,
    		uint64_t lId,
    		size_t fileId,
    		int msgType,
    		char *text,
    		int textLen,
    		int numReferences,
    		uint64_t* refPIds,
    		uint64_t* refLIds
    		) = 0;

    /**
     * Logs a new reduced (stride representation) message.
     * @param msgId unique identifier for this type of message, e.g. 7 for the "Buffer not allocated message".
     * @param pId parallel Id.
     * @param lId location id.
     * @param startRank first rank of the strided representation.
     * @param stride of the strided representation.
     * @param count of the strided representation.
     * @param msgType type of the message (error/warning/...) @see MustMessageType.
     * @param text message text.
     * @param textLen length of the text.
     * @param numReferences number of extra location references used in the text.
     * @param refPIds parallel ids of the extra locations, array of size numReferences.
     * @param refLIds location ids of the extra locations, array of size numReferences.
     * @return see gti::GTI_ANALYSIS_RETURN.
     */
    virtual gti::GTI_ANALYSIS_RETURN logStrided (
            int msgId,
            uint64_t pId,
            uint64_t lId,
            size_t fileId,
            int startRank,
            int stride,
            int count,
            int msgType,
            char *text,
            int textLen,
            int numReferences,
            uint64_t* refPIds,
            uint64_t* refLIds
    ) = 0;

    /**
     * Open a new message logfile.
     *
     * Each message logger has a default filename, where messages get written
     * into. However, an application might want to change this filename to write
     * future messages into another file. Therefore, the application simply
     * needs to call `GTI_ChangeMessageFile("the-new-filename")`.
     *
     * @note Currently, only the HTML MessageLogger supports this function.
     *
     * @note The filename is interpreted relative to the path given by
     *        getBaseOutputDir(), which is given via MUST option output-dir.
     *
     * @param fileId   The sequential file ID.
     * @param filename The new filename to be used.
     * @param len      Length of @p filename array.
     */
    virtual void openFile(size_t fileId, const char *filename, size_t len) {};

    /**
     * Close an old message logfile.
     *
     * After all processes have switched the message logfile, this function will
     * be called to close the old file handle.
     *
     *
     * @param fileId The fileId to be closed.
     */
    virtual void closeFile(size_t fileId) {};

}; /*class I_MessageLogger*/

/**
 * Copy of I_MessageLogger, with the difference that these interfaces here use an additional
 * channel id.
 */
class I_MessageLoggerCId : public gti::I_Module
{
public:
    /**
     * Logs a new message.
     * @see I_MessageLogger::log
     */
    virtual gti::GTI_ANALYSIS_RETURN log (
            int msgId,
            int hasLocation,
            uint64_t pId,
            uint64_t lId,
            size_t fileId,
            int msgType,
            char *text,
            int textLen,
            int numReferences,
            uint64_t* refPIds,
            uint64_t* refLIds,
            gti::I_ChannelId *cId
            ) = 0;

    /**
     * Logs a new reduced (stride representation) message.
     * @see I_MessageLogger::logStrided
     */
    virtual gti::GTI_ANALYSIS_RETURN logStrided (
            int msgId,
            uint64_t pId,
            uint64_t lId,
            size_t fileId,
            int startRank,
            int stride,
            int count,
            int msgType,
            char *text,
            int textLen,
            int numReferences,
            uint64_t* refPIds,
            uint64_t* refLIds,
            gti::I_ChannelId *cId
    ) = 0;

    /**
     * @see I_MessageLogger::openFile
     */
    virtual void openFile(size_t fileId, const char *filename, size_t len) {};

    /**
     * @see I_MessageLogger::closeFile
     */
    virtual void closeFile(size_t fileId) {};

}; /*class I_MessageLoggerCId*/


#endif /*I_MESSAGELOGGER_H*/
