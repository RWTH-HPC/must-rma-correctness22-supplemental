/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file TSanMessages-Filter.cpp
 *       @see MUST::TSanMessages-Filter.
 *
 *  @date 01.03.2018
 *  @author Felix Dommes
 */

#include "MustEnums.h"

#include <iostream>

using namespace std;
using namespace must;

#define TSAN_AGGR_VERBOSE //commented: prefer too less data race reports; uncommented: prefer too many data race reports


//check case insensitive string equality
bool iEquals(const string& a, const string& b)
{
	size_t len = a.length();
	if (b.length() != len) return false;

	for (size_t i = 0; i < len; i++)
	{
		if (tolower(a[i]) != tolower(b[i])) return false;
	}

	return true;
}

//checks access type and location in code (first backtrace line) for equality
bool dataraceEqualCallback(const vector<string> &msg1, const vector<string> &msg2)
{
#ifdef TSAN_AGGR_VERBOSE
	if (msg1.size() != 6 || msg2.size() != 6) return false; //default to false when the sizes don't match the expected
#else
	if (msg1.size() != 4 || msg2.size() != 4) return false; //default to false when the sizes don't match the expected
#endif
	int size = msg1.size();
	int half = size / 2; //size is even, because of two equal parts

	//parallel comparison (string == operator is implemented)
	bool equals = true;
	for (int i = 0; i < size; i++)
	{
		if (msg1[i] != msg2[i])
		{
			equals = false;
			break;
		}
	}
	if (equals) return true;

	//crossed comparison (possibly has to be case insensitive)
	equals = true;

	for (int i = 0; i < half; i++)
	{
#ifdef TSAN_AGGR_VERBOSE //we need case insensitivity only for data access types
		if (!iEquals(msg1[i], msg2[i + half]) || !iEquals(msg1[i + half], msg2[i]))
#else
		if (msg1[i] != msg2[i + half] || msg1[i + half] != msg2[i])
#endif
		{
			equals = false;
			break;
		}
	}
	if (equals) return true;
	
	//default to false to prefer too many reports
	return false;
}

//fallback/test callback which does not aggregate anything
bool myFailCallback(string msg1, string msg2)
{
	return false;
}

