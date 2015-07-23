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

#include "meremulatormodedialog.h"

#include "ui_meremulatormodedialog.h"
#include "meremulatordevice.h"
#include "merconnection.h"

#include <coreplugin/icore.h>
#include <projectexplorer/kitinformation.h>
#include <projectexplorer/project.h>
#include <projectexplorer/session.h>
#include <projectexplorer/target.h>
#include <utils/qtcassert.h>

#include <QDesktopWidget>

using namespace Mer;
using namespace Mer::Internal;

MerEmulatorModeDialog::MerEmulatorModeDialog(QObject *parent)
    : QObject(parent),
      m_action(new QAction(this)),
      m_dialog(0),
      m_ui(0),
      m_kit(0),
      m_emulator(0)
{
    auto sessionManager = static_cast<ProjectExplorer::SessionManager *>(
            ProjectExplorer::SessionManager::instance());
    auto kitManager = static_cast<ProjectExplorer::KitManager *>(
            ProjectExplorer::KitManager::instance());

    m_action->setEnabled(false);
    m_action->setText(tr("&Emulator mode..."));

    connect(m_action, &QAction::triggered,
            this, &MerEmulatorModeDialog::execDialog);

    onStartupProjectChanged(ProjectExplorer::SessionManager::startupProject());
    connect(sessionManager, &ProjectExplorer::SessionManager::startupProjectChanged,
            this, &MerEmulatorModeDialog::onStartupProjectChanged);

    connect(kitManager, &ProjectExplorer::KitManager::kitUpdated,
            this, &MerEmulatorModeDialog::onKitUpdated);
}

MerEmulatorModeDialog::~MerEmulatorModeDialog()
{
}

QAction *MerEmulatorModeDialog::action() const
{
    return m_action;
}

void MerEmulatorModeDialog::setEmulator(MerEmulatorDevice *emulator)
{
    m_emulator = emulator;
    m_action->setEnabled(m_emulator != 0);
}

void MerEmulatorModeDialog::onStartupProjectChanged(ProjectExplorer::Project *project)
{
    if (m_project != 0) {
        m_project->disconnect(this);
        onActiveTargetChanged(0);
    }

    m_project = project;

    if (m_project != 0) {
        onActiveTargetChanged(m_project->activeTarget());
        connect(m_project, &ProjectExplorer::Project::activeTargetChanged,
                this, &MerEmulatorModeDialog::onActiveTargetChanged);
    }
}

void MerEmulatorModeDialog::onActiveTargetChanged(ProjectExplorer::Target *target)
{
    if (m_target != 0) {
        m_target->disconnect(this);
        setEmulator(0);
    }

    m_target = target;

    if (m_target != 0) {
        onTargetKitChanged();
        connect(m_target, &ProjectExplorer::Target::kitChanged,
                this, &MerEmulatorModeDialog::onTargetKitChanged);
    }
}

void MerEmulatorModeDialog::onTargetKitChanged()
{
    Q_ASSERT(m_target != 0);

    if (m_kit != 0) {
        setEmulator(0);
    }

    m_kit = m_target->kit();

    if (m_kit != 0) {
        onKitUpdated(m_kit);
    }
}

void MerEmulatorModeDialog::onKitUpdated(ProjectExplorer::Kit *kit)
{
    Q_ASSERT(kit != 0);

    if (kit != m_kit) {
        return;
    }

    auto device = ProjectExplorer::DeviceKitInformation::device(m_kit);
    setEmulator(const_cast<MerEmulatorDevice *>(
                    dynamic_cast<const MerEmulatorDevice *>(device.data())));
}

void MerEmulatorModeDialog::execDialog()
{
    Q_ASSERT(m_emulator != 0);
    QTC_ASSERT(m_ui == 0, return); // possible, but little issue..

    m_dialog = new QDialog(Core::ICore::dialogParent());
    m_ui = new Ui::MerEmulatorModeDialog;
    m_ui->setupUi(m_dialog);

    m_ui->deviceNameLabel->setText(m_emulator->displayName());

    const QMap<QString, QSize> models = m_emulator->availableDeviceModels();
    int currentModelIndex = -1;
    for (auto it = models.begin(); it != models.end(); ++it) {
        const QString label = QStringLiteral("%1 (%2x%3)")
            .arg(it.key())
            .arg(it.value().width())
            .arg(it.value().height());
        m_ui->deviceModelComboBox->addItem(label, it.key());
        if (it.key() == m_emulator->deviceModel()) {
            currentModelIndex = m_ui->deviceModelComboBox->count() - 1;
        }
    }
    m_ui->deviceModelComboBox->setCurrentIndex(currentModelIndex);

    QRadioButton *orientationRadioButton = m_emulator->orientation() == Qt::Vertical
        ? m_ui->portraitRadioButton
        : m_ui->landscapeRadioButton;
    orientationRadioButton->setChecked(true);

    QRadioButton *viewModeRadioButton = m_emulator->isViewScaled()
        ? m_ui->scaledViewModeRadioButton
        : m_ui->originalViewModeRadioButton;
    viewModeRadioButton->setChecked(true);

    connect(m_ui->deviceModelComboBox, &QComboBox::currentTextChanged,
            this, &MerEmulatorModeDialog::guessOptimalViewMode);
    connect(m_ui->portraitRadioButton, &QRadioButton::toggled,
            this, &MerEmulatorModeDialog::guessOptimalViewMode);

    if (m_emulator->connection()->isVirtualMachineOff()) {
        m_ui->restartEmulatorCheckBox->setChecked(false);
        m_ui->restartEmulatorCheckBox->setEnabled(false);
    }

    if (m_dialog->exec() != QDialog::Accepted) {
        goto end;
    }

    m_emulator->setDeviceModel(m_ui->deviceModelComboBox->currentData().toString());
    m_emulator->setOrientation(m_ui->portraitRadioButton->isChecked()
                               ? Qt::Vertical
                               : Qt::Horizontal);
    m_emulator->setViewScaled(m_ui->scaledViewModeRadioButton->isChecked());

    if (!m_emulator->connection()->isVirtualMachineOff()
            && m_ui->restartEmulatorCheckBox->isChecked()) {
        m_emulator->connection()->lockDown(true);
        m_emulator->connection()->lockDown(false);
        m_emulator->connection()->connectTo();
    }

end:
    delete m_ui, m_ui = 0;
    delete m_dialog, m_dialog = 0;
}

void MerEmulatorModeDialog::guessOptimalViewMode()
{
    Q_ASSERT(m_emulator != 0);

    const QSize desktopSize = qApp->desktop()->availableGeometry().size();

    QSize selectedSize = m_emulator->availableDeviceModels()
        .value(m_ui->deviceModelComboBox->currentData().toString());
    if (m_ui->landscapeRadioButton->isChecked()) {
        selectedSize.transpose();
    }

    const bool tooBig = selectedSize.boundedTo(desktopSize) != selectedSize;

    QRadioButton *viewModeRadioButton = tooBig
        ? m_ui->scaledViewModeRadioButton
        : m_ui->originalViewModeRadioButton;
    viewModeRadioButton->setChecked(true);
}
