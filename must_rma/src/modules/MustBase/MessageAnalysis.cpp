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
 * @file MessageAnalysis.cpp
 *       @see MUST::MessageAnalysis.
 *
 *  @date 16.12.2017
 *  @author Felix Dommes
 */

#include "MessageAnalysis.h"

#include <iostream>

using namespace must;
using namespace std;



MessageAnalysis::MessageAnalysis() :
	analysers(), callbacks(), stringCallbacks()
{
	//nothing to do
}

MessageAnalysis::~MessageAnalysis()
{
	//nothing to do
}


void MessageAnalysis::addAnalyser(int msgType, regex pattern, vector<ComparisonType> classComparisons, ComparisonType default_diff_len)
{
	//if another handler exists, remove it first
	auto it2 = this->callbacks.find(msgType);
	if (it2 != this->callbacks.end()) this->callbacks.erase(it2);
	auto it3 = this->stringCallbacks.find(msgType);
	if (it3 != this->stringCallbacks.end()) this->stringCallbacks.erase(it3);
	//add new handler (or replace previous)
	AnalysisInformation info;
	info.pattern = pattern;
	info.cmps = classComparisons;
	info.default_diff_len = default_diff_len;
	this->analysers[msgType].push_back(info);
}

void MessageAnalysis::setCallback(int msgType, MessageAnalysisCallback callback)
{
	//if another handler exists, remove it first
	auto it1 = this->analysers.find(msgType);
	if (it1 != this->analysers.end()) this->analysers.erase(it1);
	auto it3 = this->stringCallbacks.find(msgType);
	if (it3 != this->stringCallbacks.end()) this->stringCallbacks.erase(it3);
	//add new handler (or replace previous)
	this->callbacks[msgType] = callback;
}

void MessageAnalysis::addStringCallback(int msgType, regex pattern, MessageAnalysisStringCallback callback)
{
	//if another handler exists, remove it first
	auto it1 = this->analysers.find(msgType);
	if (it1 != this->analysers.end()) this->analysers.erase(it1);
	auto it2 = this->callbacks.find(msgType);
	if (it2 != this->callbacks.end()) this->callbacks.erase(it2);
	//add new handler (or replace previous)
	StringCallbackInformation info;
	info.pattern = pattern;
	info.callback = callback;
	this->stringCallbacks[msgType].push_back(info);
}

bool MessageAnalysis::isMsgTypeRegistered(int msgType)
{
	//check for occurrence in each of the maps. if not found, return false
	if (this->analysers.find(msgType) != this->analysers.end()) return true;
	if (this->callbacks.find(msgType) != this->callbacks.end()) return true;
	if (this->stringCallbacks.find(msgType) != this->stringCallbacks.end()) return true;
	return false;
}

vector<string> MessageAnalysis::getMatchFromMessage(int msgType, string msg, size_t *index_out)
{
	*index_out = -1;
	vector<string> matches;

	if (this->callbacks.find(msgType) != this->callbacks.end())
	{
		return matches; //callback uses no regex, early return with empty matches
	}
	else if (this->analysers.find(msgType) != this->analysers.end())
	{ //analyser registered -> iterate through regexes
		auto it = this->analysers[msgType].begin();
		for (; it != this->analysers[msgType].end(); it++)
		{
			(*index_out)++;
			smatch match;
			if (regex_match(msg, match, it->pattern))
			{ //regex fits, transform results into vector and return them
				for (int i = 1; i < match.size(); i++)
				{
					matches.push_back(match.str(i));
				}
				matches.shrink_to_fit();
				return matches; //match found, early return
			}
		}
		//no regex fits
		*index_out = -1;
	}
	else if (this->stringCallbacks.find(msgType) != this->stringCallbacks.end())
	{ //stringCallback registered -> iterate through regexes
		auto it = this->stringCallbacks[msgType].begin();
		for (; it != this->stringCallbacks[msgType].end(); it++)
		{
			(*index_out)++;
			smatch match;
			if (regex_match(msg, match, it->pattern))
			{ //regex fits, transform results into vector and return them
				for (int i = 1; i < match.size(); i++)
				{
					matches.push_back(match.str(i));
				}
				matches.shrink_to_fit();
				return matches; //match found, early return
			}
		}
		//no regex fits
		*index_out = -1;
	}
	else
	{ //msgType was not registered, empty matches are returned
		return matches; //early return
	}

	//failure (no regex fits) => give an report
	cerr << "No registered regex fits to this message with the msgId/msgType " << msgType << "!" << endl;
	//output message in debug mode?
	return matches; //return empty matches
}


bool MessageAnalysis::checkEquals(
		int msgType1, string msg1,
		int msgType2, string msg2)
{
	if (msgType1 != msgType2) return false; //different message types can not be equal
	
	//check if there is a callback registered. only this method will handle it, others have matches only!
	if (this->callbacks.find(msgType1) != this->callbacks.end())
	{
		return this->callbacks[msgType1](msg1, msg2);
	}
	
	//call more specified methods else -> allow higher efficiency (user can save matches)
	size_t index1;
	vector<string> vecMsg1 = this->getMatchFromMessage(msgType1, msg1, &index1);
	return this->checkEquals(msgType1, index1, vecMsg1, msgType2, msg2);
}

bool MessageAnalysis::checkEquals(
		int msgType1, size_t index1, const vector<string> &msg1,
		int msgType2, string msg2)
{
	if (msgType1 != msgType2) return false; //different message types can not be equal

	//use more specified method (does not need to check if msg is as callback registered)
	size_t index2;
	vector<string> vecMsg2 = this->getMatchFromMessage(msgType2, msg2, &index2);
	return this->checkEquals(msgType1, index1, msg1, msgType2, index2, vecMsg2);
}

bool MessageAnalysis::checkEquals(
		int msgType1, size_t index1, const vector<string> &msg1,
		int msgType2, size_t index2, const vector<string> &msg2)
{
	if (msgType1 != msgType2 || index1 != index2) return false; //different message types can not be equal
	if (index1 == -1) return false; //no regex matched, default to no reduction
	
	//there is only one handler (taken care of in set-functions)
	if (this->callbacks.find(msgType1) != this->callbacks.end())
	{ //if a callback is registered, return false. when this is executed, there are only empty matches.
		return false;
	}
	else if (this->analysers.find(msgType1) != this->analysers.end())
	{ //check message-matches based on the index's CMP-vector
		//find CMP-vector
		auto it = this->analysers[msgType1].begin();
		auto end = this->analysers[msgType1].end();
		for (int i = 0; i < index1 && it != end; i++) it++;
		if (it == end) return false; //index does not fit, default to no reduction
		//check the sizes of the messages' field vectors
		if (it->default_diff_len == CMP_EQUAL &&  msg1.size() != msg2.size()) return false; //defaut to false when field vectors are not of equal size
		//check the matches based on the CMP-vector
		for (int i = 0; i < msg1.size(); i++)
		{
			if (i >= msg2.size()) break; //ignore remaining fields
			ComparisonType cmp_val = CMP_EQUAL; //default to CMP_EQUAL, when cmp_vector is too small
			if (i < it->cmps.size()) cmp_val = it->cmps[i];
			switch (cmp_val)
			{
				case CMP_EQUAL:
					if (msg1[i].compare(msg2[i]) != 0) return false;
					break;
				case CMP_IGNORE:
					break;
			}
		}
		//checks finished, messages are equal (if match/field vectors are not of the same size, default to ignore of remaining)
		return true;
	}
	else if (this->stringCallbacks.find(msgType1) != this->stringCallbacks.end())
	{ //check message-matches based on the index's stringCallback
		//find callback function
		auto it = this->stringCallbacks[msgType1].begin();
		auto end = this->stringCallbacks[msgType1].end();
		for (int i = 0; i < index1 && it != end; i++) it++;
		if (it == end) return false; //index does not fit, default to no reduction
		//call callback function
		return it->callback(msg1, msg2);
	}

	//message type was not registered -> return false, so it will lead to duplicate instead of missing messages
	return false;
}

