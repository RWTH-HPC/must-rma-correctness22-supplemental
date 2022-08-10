/* Part of the MUST Project, under BSD-3-Clause License
 * See https://hpc.rwth-aachen.de/must/LICENSE for license information.
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * @file ELPSubModule.cpp
 *       @see MUST::ELPSubModule.
 *
 *  @date 22.07.2014
 *  @author Felix Muenchhalfen
 */

#include "ModuleBase.h"
#include "I_ELPSubModule.h"

#ifndef ELPSUBMODULE_H
#define	ELPSUBMODULE_H

using namespace gti;

namespace must
{

class ELPSubModule : public gti::ModuleBase<ELPSubModule, I_ELPSubModule> {
public:
    ELPSubModule(const char* instanceName);
    virtual ~ELPSubModule();
private:

};

}

#endif	/* ELPSUBMODULE_H */

