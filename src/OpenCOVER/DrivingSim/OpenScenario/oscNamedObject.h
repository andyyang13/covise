/* This file is part of COVISE.

You can use it under the terms of the GNU Lesser General Public License
/* This file is part of COVISE.

You can use it under the terms of the GNU Lesser General Public License
version 2.1 or later, see lgpl-2.1.txt.

* License: LGPL 2+ */

#ifndef OSC_NAMED_OBJECT_H
#define OSC_NAMED_OBJECT_H

#include <oscExport.h>
#include <oscObjectBase.h>
#include <oscObjectVariable.h>

#include <oscVariables.h>
#include <oscUserDataList.h>


namespace OpenScenario {

/// \class This class represents a generic OpenScenario Object
class OPENSCENARIOEXPORT oscNamedObject: public oscObjectBase
{
public:
    oscNamedObject()
    {
        OSC_ADD_MEMBER(name);
        OSC_OBJECT_ADD_MEMBER(userDataList, "oscUserDataList");
    }

    oscString name;
    oscUserDataListArrayMember userDataList;
};

typedef oscObjectVariable<oscNamedObject *> oscNamedObjectMember;

}

#endif //OSC_NAMED_OBJECT_H
