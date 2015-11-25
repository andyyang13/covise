/* This file is part of COVISE.

   You can use it under the terms of the GNU Lesser General Public License
   version 2.1 or later, see lgpl-2.1.txt.

 * License: LGPL 2+ */

/**************************************************************************
** ODD: OpenDRIVE Designer
**   Frank Naegele (c) 2010
**   <mail@f-naegele.de>
**   11/2/2010
**
**************************************************************************/

#ifndef TUNNELSETTINGS_HPP
#define TUNNELSETTINGS_HPP

#include "src/settings/settingselement.hpp"

namespace Ui
{
class TunnelSettings;
}

class Tunnel;
class SignalManager;

class TunnelSettings : public SettingsElement
{
    Q_OBJECT

    //################//
    // FUNCTIONS      //
    //################//

public:
    explicit TunnelSettings(ProjectSettings *projectSettings, SettingsElement *parentSettingsElement, Tunnel *tunnel);
    virtual ~TunnelSettings();

    // Observer Pattern //
    //
    virtual void updateObserver();

private:
    void updateProperties();

    //################//
    // SLOTS          //
    //################//

private slots:
    void onEditingFinished();
    void onEditingFinished(int);
    void on_sSpinBox_editingFinished();
    void onValueChanged();

    //################//
    // PROPERTIES     //
    //################//

private:
    Ui::TunnelSettings *ui;

    Tunnel *tunnel_;

    bool init_;

    bool valueChanged_;
};

#endif // TUNNELETTINGS_HPP
