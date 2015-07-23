/****************************************************************************
**
** Copyright (C) 2015 Jolla Ltd.
** Contact: http://jolla.com/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#ifndef MEREMULATORMODEDIALOG_H
#define MEREMULATORMODEDIALOG_H

#include <QObject>
#include <QPointer>

QT_BEGIN_NAMESPACE
class QAction;
class QDialog;
QT_END_NAMESPACE

namespace ProjectExplorer {
    class Kit;
    class Project;
    class Target;
}

namespace Mer {
namespace Internal {

class MerEmulatorDevice;

namespace Ui {
    class MerEmulatorModeDialog;
}

class MerEmulatorModeDialog : public QObject
{
    Q_OBJECT

public:
    MerEmulatorModeDialog(QObject *parent = 0);
    ~MerEmulatorModeDialog();

public:
    QAction *action() const;

private:
    void setEmulator(MerEmulatorDevice *emulator);

private slots:
    void onStartupProjectChanged(ProjectExplorer::Project *project);
    void onActiveTargetChanged(ProjectExplorer::Target *target);
    void onTargetKitChanged();
    void onKitUpdated(ProjectExplorer::Kit *kit);
    void execDialog();
    void setOptimalViewMode();

private:
    QAction *m_action;
    QDialog *m_dialog;
    Ui::MerEmulatorModeDialog *m_ui;
    QPointer<ProjectExplorer::Project> m_project;
    QPointer<ProjectExplorer::Target> m_target;
    ProjectExplorer::Kit *m_kit;
    MerEmulatorDevice *m_emulator;
};

} // namespace Internal
} // namespace Mer

#endif // MEREMULATORMODEDIALOG_H
