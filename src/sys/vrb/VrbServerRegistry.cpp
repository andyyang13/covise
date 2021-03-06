/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */
#include "VrbServerRegistry.h"
#include <util/unixcompat.h>
#include <net/tokenbuffer.h>
#include <VRBClientList.h>
#include <iostream>

#include <assert.h>


using namespace covise;
namespace vrb
{
VrbServerRegistry::VrbServerRegistry(int session)
    :sessionID(session)
{
}

VrbServerRegistry::~VrbServerRegistry()
{
}

serverRegClass *VrbServerRegistry::getClass(const std::string & name)
{

    auto cl = classes.find(name);
    if (cl == classes.end())
    {
        return nullptr;
    }
    return cl->second.get();
}
/// set a Value or create new Entry
void VrbServerRegistry::setVar(int ID, const std::string &className, const std::string &name, covise::TokenBuffer &value, bool s)
{

    serverRegClass *rc = getClass(className);
    if (!rc)
    {
        rc = new serverRegClass(className, ID);
        classes[className].reset(rc);

    }
    rc->setID(ID);
    serverRegVar *rv = rc->getVar(name);
    if (rv)
    {
        rv->setValue(value);
    }
    else
    {
        rv = new serverRegVar(rc, name, value);
        rc->append(rv);
    }

    //call observers
    std::set<int> collectiveObservers = *rc->getOList();
    collectiveObservers.insert(rv->getOList()->begin(), rv->getOList()->end());
    sendVariableChange(rv, collectiveObservers);
    updateUI(rv);


}

void VrbServerRegistry::updateUI(serverRegVar *rv)
{

}
/// create new Entry
void VrbServerRegistry::create(int ID, const std::string &className, const std::string &name, covise::TokenBuffer &value, bool s)
{
    serverRegClass *rc = getClass(className);
    if (rc)
    {
        serverRegVar *rv = rc->getVar(name);
        if (rv)
        {
            return;
        }
    }
    setVar(ID, className, name, value, s);
}
/// get a boolean Variable
int VrbServerRegistry::isTrue(int ID, const std::string &className, const std::string &name, int def)
{
    serverRegClass *rc = getClass(className);
    if (rc)
    {
        serverRegVar *rv = rc->getVar(name);
        if (rv)
        {
            bool b;
            rv->getValue() >> b;
            rv->getValue().rewind();
            return b;
        }
        return def;
    }
    return def;
}

void VrbServerRegistry::setOwner(int id)
{
    if (id < 0)
    {
        owner = id;
    }
}

int VrbServerRegistry::getOwner()
{
    return owner;
}

void VrbServerRegistry::deleteEntry(const std::string &className, const std::string &name)
{
    classes[className]->deleteVar(name);
}

void VrbServerRegistry::deleteEntry()
{
    for (const auto cl : classes)
    {
        cl.second->deleteAllNonStaticVars();
    }
}


void VrbServerRegistry::sendVariableChange(serverRegVar * rv, std::set<int> observers)
{
    covise::TokenBuffer sb;
    sb << rv->getClass()->getID(); //sender ID
    sb << rv->getClass()->getName();
    sb << rv->getName();
    sb << rv->getValue();

    for (const auto client : observers)
    {
        VRBSClient *cl = clients.get(client);
        if (cl)
        {
            clients.sendMessageToID(sb, client, COVISE_MESSAGE_VRB_REGISTRY_ENTRY_CHANGED);
        }
    }
}

void VrbServerRegistry::observeVar(int ID, const std::string &className, const std::string &variableName, covise::TokenBuffer &value)
{
    auto classIt = classes.find(className);
    if (classIt == classes.end()) //if class does not exists create it
    {
        auto rc = std::make_shared<serverRegClass>(className, ID);
        classIt = classes.emplace(className, rc).first;
    }
    classIt->second->observeVar(ID, variableName, value);
}

void VrbServerRegistry::observeClass(int ID, const std::string &className)
{
    auto classIt = classes.find(className);
    if (classIt == classes.end()) //if class does not exists create it
    {
        auto rc = std::make_shared<serverRegClass>(className, ID);
        classIt = classes.emplace(className, rc).first;
    }
    classIt->second->observe(ID);
}
///unobserve a single variable
void VrbServerRegistry::unObserveVar(int ID, const std::string &className, const std::string &variableName)
{
    auto classIt = classes.find(className);
    if (classIt != classes.end())
    {
        classIt->second->unObserveVar(ID, variableName);
    }
    else
    {
        if (!variableName.empty())
            std::cerr << "Variable " << variableName << " not found in class " << className << " ID: " << ID << std::endl;
        else
            std::cerr << "Class " << className << " ID: " << ID << " not found" << std::endl;
    }
}
///unobserve a class and all its variables
void VrbServerRegistry::unObserveClass(int ID, const std::string &className)
{
    auto classIt = classes.find(className);
    if (classIt != classes.end())
    {
        classIt->second->unObserve(ID);
    }
}
///observer "recvID" gets removed from all classes and variables
void VrbServerRegistry::unObserve(int recvID)
{
    for (const auto cl : classes)
    {
        cl.second->unObserve(recvID);
    }
}
/////////////SERVERREGVAR/////////////////////////////////////////////////
serverRegVar::~serverRegVar()
{
    informDeleteObservers();
}

void serverRegVar::update(int recvID)
{
    covise::TokenBuffer sb;
    sb << myClass->getID();
    sb << myClass->getName();
    sb << getName();

    sb << getValue();
    clients.sendMessageToID(sb, recvID, COVISE_MESSAGE_VRB_REGISTRY_ENTRY_CHANGED);
}

void serverRegVar::informDeleteObservers()
{

    covise::TokenBuffer sb;
    sb << getClass()->getID();
    sb << getClass()->getName();
    sb << getName();
    sb << getValue();
    std::set<int> combinedObservers = observers;;
    combinedObservers.insert(getClass()->getOList()->begin(), getClass()->getOList()->end());
    for (const int obs : combinedObservers)
    {
        clients.sendMessageToID(sb, obs, COVISE_MESSAGE_VRB_REGISTRY_ENTRY_DELETED);
    }
}

void serverRegVar::setLastEditor(int id)
{
    lastEditor = id;
}

int serverRegVar::getLastEditor()
{
    return lastEditor;
}

/////////////SERVERREGCLASS/////////////////////////////////////////////////
void serverRegClass::observe(int recvID)
{
    observers.insert(recvID);
}

void serverRegClass::observeVar(int recvID, const std::string &variableName, covise::TokenBuffer &value)
{
    serverRegVar *rv = getVar(variableName);
    if (!rv)
    {
        rv = new serverRegVar(this, variableName, value);
        append(rv);
    }
    rv->observe(recvID);

}

void serverRegClass::unObserveVar(int recvID, const std::string &variableName)
{
    serverRegVar *rv = getVar(variableName);
    if (rv)
    {
        rv->unObserve(recvID);
    }
}
void serverRegClass::unObserve(int recvID)
{
    observers.erase(recvID);
    for (const auto &var : myVariables)
    {
        var.second->unObserve(recvID);
    }
}
}
