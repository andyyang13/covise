/* This file is part of COVISE.

You can use it under the terms of the GNU Lesser General Public License
version 2.1 or later, see lgpl-2.1.txt.

* License: LGPL 2+ */

#ifndef OSC_MANEUVER_LIST_H
#define OSC_MANEUVER_LIST_H

#include <oscExport.h>
#include <oscObjectBase.h>
#include <oscObjectVariable.h>

#include <oscCancelConditionTypeAGroups.h>
#include <oscEndConditionTypeAGroups.h>
#include <oscStartConditionTypeAGroups.h>
#include <oscManeuverTypeBGroup.h>


namespace OpenScenario {

/// \class This class represents a generic OpenScenario Object
class OPENSCENARIOEXPORT oscManeuverList: public oscObjectBase
{
public:
    oscManeuverList()
    {
        OSC_OBJECT_ADD_MEMBER(startConditionGroups, "oscStartConditionTypeAGroups");
        OSC_OBJECT_ADD_MEMBER(endConditionGroups, "oscEndConditionTypeAGroups");
        OSC_OBJECT_ADD_MEMBER(cancelConditionGroups, "oscCancelConditionTypeAGroups");
        OSC_OBJECT_ADD_MEMBER(maneuverGroup, "oscManeuverTypeBGroup");
    };

    oscStartConditionTypeAGroupsArrayMember startConditionGroups;
    oscEndConditionTypeAGroupsArrayMember endConditionGroups;
    oscCancelConditionTypeAGroupsArrayMember cancelConditionGroups;
    oscManeuverTypeBGroupArrayMember maneuverGroup;
};

typedef oscObjectVariable<oscManeuverList *>oscManeuverListMember;

}

#endif //OSC_MANEUVER_LIST_H
