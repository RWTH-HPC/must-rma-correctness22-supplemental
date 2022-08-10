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
 * @file MessageAnalysisAdder.h
 *       @see MUST::MessageAnalysisAdder.
 *
 *  @date 16.12.2017
 *  @author Felix Dommes
 */

#ifndef MESSAGEANALYSISADDER_H
#define MESSAGEANALYSISADDER_H

#include "MessageAnalysis.h"

//extern void dataraceAnalyserInit(must::MessageAnalysis *analyser);


void addAnalysers(must::MessageAnalysis *analyser)
{
	if (analyser == NULL) return;

	//dataraceAnalyserInit(analyser);
}

#endif //MESSAGEANALYSISADDER_H

