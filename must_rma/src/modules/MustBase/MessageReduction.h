/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file MessageReduction.h
 *       @see MUST::MessageReduction.
 *
 *  @date 05.08.2013
 *  @author Tobias Hilbrich
 */

#include "ModuleBase.h"
#include "I_ParallelIdAnalysis.h"
#include "I_LocationAnalysis.h"
#include "I_FinishNotify.h"
#include "BaseApi.h"

#include "I_MessageReduction.h"
#include "MessageAnalysis.h"

#ifndef MESSAGEREDUCTION_H
#define MESSAGEREDUCTION_H

using namespace gti;

namespace must
{
    /**
     * Storage class for message reduction
     */
    class MessageRepresentation
    {
    protected:
        int myMsgId;
        MustParallelId myPId;
        MustLocationId myLId;
        size_t myFileId;
        std::string myCallName;
        int myMsgType;
        std::string myText;
        int myNumReferences;
        MustParallelId *myRefPIds;
        MustLocationId *myRefLIds;

		MessageAnalysis *myAnalyser;
		size_t myMatchIndex;
		std::vector<std::string> myMatch;

        std::map<int, std::pair<int, int> > myStrides; //Maps startRank, to (stride,count)

    public:

        /**
         * Copy constructor.
         */
        MessageRepresentation( const MessageRepresentation& other);

        /*
         * Creates a representation from the given information.
         * Copies the arrays into own memory.
         */
        MessageRepresentation (
                int msgId,
                uint64_t pIdRef,
                uint64_t lIdRef,
                size_t fileId,
                std::string callName,
                int startRank,
                int stride,
                int count,
                int msgType,
                char *text,
                int textLen,
                int numReferences,
                uint64_t* refPIds,
                uint64_t* refLIds,
				MessageAnalysis *analyser);

        /**
         * Destructor
         */
        ~MessageRepresentation (void);

        /**
         * Checks whether the given new message belongs into this representation.
         */
        bool belongsToRepresentation (
                int msgId,
                size_t fileId,
                std::string callName,
                int msgType,
                std::string text,
                int startRank,
                int stride,
                int count
                );

        /**
         * Add to representation.
         */
        void addToRepresentation (
                int startRank,
                int stride,
                int count);

        /**
         * Creates event(s) from this representation.
         */
        void forwardRepresentation (handleNewMessageReducedP fNewMsg);
    };


	/**
     * Implementation of I_MessageReduction.
     */
    class MessageReduction : public gti::ModuleBase<MessageReduction, I_MessageReduction>, I_FinishListener
    {
    public:
			/**
			 * Constructor.
			 * @param instanceName name of this module instance.
			 */
            MessageReduction (const char* instanceName);

			/**
			 * Destructor.
			 */
			virtual ~MessageReduction (void);

			/**
			 * @see I_MessageReduction::reduce.
			 */
			GTI_ANALYSIS_RETURN reduce (
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
			        gti::I_ChannelId *thisChannel,
			        std::list<gti::I_ChannelId*> *outFinishedChannels
			);

			/**
			 * @see I_MessageReduction::reduceStrided
			 */
			GTI_ANALYSIS_RETURN reduceStrided (
			        int msgId,
			        uint64_t pIdRef,
			        uint64_t lIdRef,
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
			        gti::I_ChannelId *thisChannel,
			        std::list<gti::I_ChannelId*> *outFinishedChannels
			);

			/**
			 * @see I_FinishListener::finish
			 */
			void finish (void);

			/**
			 * The timeout function, see gti::I_Reduction.timeout
			 */
			void timeout (void);

    protected:
			I_ParallelIdAnalysis *myPIdModule;
			I_LocationAnalysis *myLIdModule;
			I_FinishNotify *myNotify;

			handleNewMessageReducedP myIntroduceMessage;

			std::list<MessageRepresentation> myReps;

			bool myGotFinish;

			MessageAnalysis myAnalyser;

    }; /*class MessageReduction */
} /*namespace MUST*/

#endif /*MESSAGEREDUCTION_H*/
