/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MsgLoggerHtml.h
 *       @see MUST::MsgLoggerHtml.
 *
 *  @date 20.01.2011
 *  @author Tobias Hilbrich
 */

#include "I_LocationAnalysis.h"
#include "I_MessageLogger.h"
#include "I_ParallelIdAnalysis.h"
#include "ModuleBase.h"
#include "MustEnums.h"
#include "MustDefines.h"

#include <fstream>
#include <map>

#ifndef MSGLOGGERHTML_H
#define MSGLOGGERHTML_H

using namespace gti;

namespace must
{
    /**
     * Implementation of I_MessageLogger that writes
     * an HTML file.
     */
    class MsgLoggerHtml : public gti::ModuleBase<MsgLoggerHtml, I_MessageLogger>
    {
    public:
            /**
             * Constructor.
             * @param instanceName name of this module instance.
             */
            MsgLoggerHtml (const char* instanceName);

            /**
             * Destructor.
             */
            virtual ~MsgLoggerHtml (void);

            /**
             * @see I_MessageLogger::log.
             */
            GTI_ANALYSIS_RETURN log (
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
            );

            /**
             * @see I_MessageLogger::logStrided.
             */
            GTI_ANALYSIS_RETURN logStrided (
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
            );

            // map to switch between enum and string
#define map_entry_macro(name) {name, #name},
            std::map<MustMessageIdNames, std::string> MustErrorId2String{
                {MUST_MESSAGE_NO_ERROR, "MUST_MESSAGE_NO_ERROR"},
                // Errors
                FOREACH_MUST_ERRORS(map_entry_macro)
                // Warnings
                FOREACH_MUST_WARNING(map_entry_macro)
                // Informations
                FOREACH_MUST_INFO(map_entry_macro){MUST_LAST_MESSAGE_ID_NAME,
                                                   "MUST_LAST_MESSAGE_ID_NAME"}};

#undef map_entry_macro

            /**
             * @see I_MessageLogger::openFile
             */
            void openFile(size_t fileId, const char *filename, size_t len);

            /**
             * @see I_MessageLogger::closeFile
             */
            void closeFile(size_t fileId);

    protected:
        /**
         * Wrapper class for custom ofstream.
         *
         * This class enhances the default @ref std::ofstream with the filename
         * of a specific stream, so it can be used from different positions in
         * the @ref MsgLoggerHtml class.
         */
        class htmlStream : public std::ofstream
        {
            public:
                /**
                 * @see std::ofstream::ofstream
                 */
                htmlStream() : std::ofstream()
                {
                }

                /**
                 * @see std::ofstream::ofstream
                 *
                 * In addition to opening a new @p filename, the filename will
                 * be stored in  a private variable.
                 */
                htmlStream(const std::string &filename)
                    : std::ofstream(std::string{std::string(getBaseOutputDir()) + "/" + filename}), myFilename(filename)
                {
                }

                /**
                 * @return The filename of this stream.
                 */
                const std::string &filename() const
                {
                    return myFilename;
                }

            private:
                /**
                 * The filename of this stream.
                 */
                std::string myFilename;
        };

            /**
             * Prints the trailer to the file that closes the
             * HTML document.
             * @param finalNotes Print a message (or not) indicating proper and intended tool shut down
             */
            void printTrailer (std::ofstream& out, bool finalNotes = false);

            /**
             * Prints the header for the HTML document.
             */
            void printHeader (std::ofstream& out);

            /**
             * Prints a location.
             * @param pId of location
             * @param lId of location
             */
            void printLocation (std::ofstream& out, MustParallelId pId, MustLocationId lId);

            /**
             * Helper that prints an occurrence count nicely formated to fit into our regular outputs.
             * @param out stream to print into.
             * @param lId location id that stores the occurrences count
             */
            void printOccurenceCount (std::ostream& out, MustLocationId lId);

            I_ParallelIdAnalysis *myPIdModule;
            I_LocationAnalysis *myLIdModule;

            /**
             * A map of open file handles.
             *
             * This map maps the fileId of open log-files to its corresponding
             * file stream.
             */
            std::map<size_t, htmlStream> fileHandles;

            bool myLineEven;

            bool myLoggedWarnError;
    }; /*class MsgLoggerHtml */
} /*namespace MUST*/

#endif /*MSGLOGGERHTML_H*/
