/* This file is part of MUST (Marmot Umpire Scalable Tool)
 *
 * Copyright (C)
 *  2011-2015 ZIH, Technische Universitaet Dresden, Federal Republic of Germany
 *  2011-2015 Lawrence Livermore National Laboratories, United States of America
 *  2013-2015 RWTH Aachen University, Federal Republic of Germany
 *
 * See the file LICENSE.txt in the package base directory for details
 */

/**
 * @file MessageAnalysis.h
 *       @see MUST::MessageAnalysis.
 *
 *  @date 16.12.2017
 *  @author Felix Dommes
 */

#ifndef MESSAGEANALYSIS_H
#define MESSAGEANALYSIS_H

#include <string>
#include <map>
#include <list>
#include <vector>
#include <regex>


namespace must
{
	enum ComparisonType
	{
		CMP_IGNORE,
		CMP_EQUAL
	};

	typedef bool (*MessageAnalysisCallback)(std::string msg1, std::string msg2);
	typedef bool (*MessageAnalysisStringCallback)(const std::vector<std::string> &msg1, const std::vector<std::string> &msg2);

	class MessageAnalysis
	{
		private:
			struct AnalysisInformation
			{
				std::regex pattern;
				std::vector<ComparisonType> cmps;
				ComparisonType default_diff_len; //flag for default behaviour for different string vector sizes
			};
			struct StringCallbackInformation
			{
				std::regex pattern;
				MessageAnalysisStringCallback callback;
			};

		private:
			std::map<int, std::list<AnalysisInformation>> analysers;
			std::map<int, MessageAnalysisCallback> callbacks;
			std::map<int, std::list<StringCallbackInformation>> stringCallbacks;

		public:
			MessageAnalysis();
			~MessageAnalysis();

			void addAnalyser(int msgType, std::regex pattern, std::vector<ComparisonType> classComparisons, ComparisonType default_diff_len = CMP_EQUAL); //if another handler exists, remove it; empty/unsufficient vector causes remaining fields to be compared (no ignore)
			void setCallback(int msgType, MessageAnalysisCallback callback); //if another handler exists, remove it
			void addStringCallback(int msgType, std::regex pattern, MessageAnalysisStringCallback callback); //if another handler exists, remove it
			bool isMsgTypeRegistered(int msgType);
			std::vector<std::string> getMatchFromMessage(int msgType, std::string msg, size_t *index_out);
			bool checkEquals(int msgType1, std::string msg1,
					int msgType2, std::string msg2);
			bool checkEquals(int msgType1, size_t index1, const std::vector<std::string> &msg1,
					int msgType2, std::string msg2);
			bool checkEquals(int msgType1, size_t index1, const std::vector<std::string> &msg1,
					int msgType2, size_t index2, const std::vector<std::string> &msg2);
	};
}

#endif //MESSAGEANALYSIS_H

