/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

/*! Basic class to synchronize the state of any variable with other clients connectet to vrb
for variables of types other than: bool, int, float, double, string, TokenBuffer
the serialize and deserialize methods have to be implemented
make sure the variable name is unique for each SharedState e.g. by naming the variable Plugin.Class.Variablename.ID

*/
#ifndef VRB_SHAREDSTATE_H
#define VRB_SHAREDSTATE_H
	
#include <vector>
#include <string>
#include <functional>
#include <cassert>

#include <net/tokenbuffer.h>
#include <util/coExport.h>
#include <vrbclient/regClass.h>




namespace vrb
{
enum SharedStateType
{
    USE_COUPLING_MODE, //0
    NEVER_SHARE, //1
    ALWAYS_SHARE //2
};
class clientRegVar;
///convert the value to a TokenBuffer
template<class T>
void serialize(covise::TokenBuffer &tb, const T &value)
{
    int typeID = 0;
    tb << typeID;
    tb << value;
}

template <>
VRBEXPORT void serialize<std::vector<std::string>>(covise::TokenBuffer &tb, const std::vector<std::string> &value);

///converts the TokenBuffer back to the value
template<class T>
void deserialize(covise::TokenBuffer &tb, T &value)
{
    int typeID;
    tb >> typeID;
    tb >> value;
}

template <>
VRBEXPORT void deserialize<std::vector<std::string>>(covise::TokenBuffer &tb, std::vector<std::string> &value);





class  VRBEXPORT SharedStateBase : public regVarObserver

{
public:
    SharedStateBase(std::string name, SharedStateType mode);

    virtual ~SharedStateBase();

    //! let the SharedState call the given function when the registry entry got changed from the server
    void setUpdateFunction(std::function<void(void)> function);

    //! returns true if the last value change was made by an other client
    bool valueChangedByOther() const;

    std::string getName() const;

    //! is called from the registryAcces when the registry entry got changed from the server
    void update(clientRegVar *theChangedRegEntry) override;
    void setID(int id);
    void resubscribe(int id);
protected:

    virtual void deserializeValue(covise::TokenBuffer &data) = 0;
    void subscribe(covise::TokenBuffer &&val);
    void setVar(covise::TokenBuffer &&val);
    const std::string className = "SharedState";
    std::string variableName;

    bool doSend = false;
    bool doReceive = false;
    bool valueChanged = false;
    std::function<void(void)> updateCallback;

    VrbClientRegistry *m_registry = nullptr;

private:
    int sessionID = 0; ///the session to send updates to 
};

template <class T>
class  SharedState : public SharedStateBase
{
public:
    SharedState<T>(std::string name, T value = T(), SharedStateType mode = USE_COUPLING_MODE)
        : SharedStateBase(name, mode)
        , m_value(value)
    {
        assert(m_registry);
        covise::TokenBuffer data;
        serialize(data, m_value);
        subscribe(std::move(data));
    }

    SharedState<T> &operator=(T value)
    {
        if (m_value != value)
        {
            m_value = value;
            push();
        }
        return *this;
    }

    operator T() const
    {
        return m_value;
    }

    void deserializeValue(covise::TokenBuffer &data) override
    {
        deserialize(data, m_value);
    }

    //! sends the value change to the vrb
    void push()
    {
        valueChanged = false;
        covise::TokenBuffer data;
        serialize(data, m_value);
        setVar(std::move(data));
    }

    const T &value() const
    {
        return m_value;
    }

private:
    T m_value; ///the value of the SharedState

};
}
#endif


