/****************************************************************************
**
** Copyright (C) 2015 Jolla Ltd.
** Contact: http://jolla.com/
**
** This file is part of Qt Creator.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** Other Usage
**
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Digia.
**
****************************************************************************/

#ifndef MEREMULATORDEVICETESTER_H
#define MEREMULATORDEVICETESTER_H

#include "meremulatordevice.h"

#include <remotelinux/linuxdevicetester.h>

namespace Mer {
namespace Internal {

class MerEmulatorDeviceTester : public RemoteLinux::GenericLinuxDeviceTester
{
    Q_OBJECT

public:
    explicit MerEmulatorDeviceTester(QObject *parent = 0);
    ~MerEmulatorDeviceTester();

    void testDevice(const ProjectExplorer::IDevice::ConstPtr &deviceConfiguration);
    void stopTest();

private slots:
    void onConnectionStateChanged();

private:
    MerEmulatorDevice::ConstPtr m_device;
    bool m_pastVmStart;
};

}
}

#endif // MEREMULATORDEVICETESTER_H