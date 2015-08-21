/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://www.qt.io/licensing.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "echoipcserver.h"

#include "cmbcodecompletedmessage.h"
#include "cmbcompletecodemessage.h"
#include "cmbechomessage.h"
#include "cmbendmessage.h"
#include "cmbregisterprojectsforcodecompletionmessage.h"
#include "cmbregistertranslationunitsforcodecompletionmessage.h"
#include "cmbunregisterprojectsforcodecompletionmessage.h"
#include "cmbunregistertranslationunitsforcodecompletionmessage.h"
#include "connectionserver.h"

#include <QCoreApplication>
#include <QDebug>


namespace ClangBackEnd {

void EchoIpcServer::dispatch(const QVariant &message)
{
    IpcServerInterface::dispatch(message);
}

void EchoIpcServer::end()
{
    ConnectionServer::removeServer();
    QCoreApplication::quit();
}

void EchoIpcServer::registerTranslationUnitsForCodeCompletion(const RegisterTranslationUnitForCodeCompletionMessage &message)
{
    echoMessage(QVariant::fromValue(message));
}

void EchoIpcServer::unregisterTranslationUnitsForCodeCompletion(const UnregisterTranslationUnitsForCodeCompletionMessage &message)
{
    echoMessage(QVariant::fromValue(message));
}

void EchoIpcServer::registerProjectPartsForCodeCompletion(const RegisterProjectPartsForCodeCompletionMessage &message)
{
    echoMessage(QVariant::fromValue(message));
}

void EchoIpcServer::unregisterProjectPartsForCodeCompletion(const UnregisterProjectPartsForCodeCompletionMessage &message)
{
    echoMessage(QVariant::fromValue(message));
}

void EchoIpcServer::completeCode(const CompleteCodeMessage &message)
{
    echoMessage(QVariant::fromValue(message));
}

void EchoIpcServer::echoMessage(const QVariant &message)
{
    client()->echo(EchoMessage(message));
}

} // namespace ClangBackEnd
