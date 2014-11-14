** conditions see http://www.qt.io/licensing.  For further information
** use the contact form at http://www.qt.io/contact-us.
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
    m_mercurialContainer = Core::ActionManager::createMenu("Mercurial.MercurialMenu");
    QMenu *menu = m_mercurialContainer->menu();
    m_mercurialContainer->addSeparator(context);
    m_mercurialContainer->addSeparator(context);
    m_mercurialContainer->addSeparator(context);
    toolsMenu->addMenu(m_mercurialContainer);
    m_menuAction = m_mercurialContainer->menu()->menuAction();
    m_mercurialContainer->addAction(command);
    command->setDefaultKeySequence(QKeySequence(Core::UseMacShortcuts ? tr("Meta+H,Meta+D") : tr("Alt+G,Alt+D")));
    m_mercurialContainer->addAction(command);
    command->setDefaultKeySequence(QKeySequence(Core::UseMacShortcuts ? tr("Meta+H,Meta+L") : tr("Alt+G,Alt+L")));
    m_mercurialContainer->addAction(command);
    command->setDefaultKeySequence(QKeySequence(Core::UseMacShortcuts ? tr("Meta+H,Meta+S") : tr("Alt+G,Alt+S")));
    m_mercurialContainer->addAction(command);
    m_mercurialContainer->addSeparator(context);
    m_mercurialContainer->addAction(command);
    m_mercurialContainer->addAction(command);
    m_mercurialContainer->addAction(command);
    m_mercurialContainer->addAction(command);
    m_mercurialContainer->addAction(command);
    m_mercurialContainer->addAction(command);
    m_mercurialContainer->addAction(command);
    m_mercurialContainer->addAction(command);
    m_mercurialContainer->addAction(command);
    m_mercurialContainer->addAction(command);
    m_mercurialContainer->addAction(command);
    m_mercurialContainer->addAction(command);
    m_mercurialContainer->addAction(command);
    command->setDefaultKeySequence(QKeySequence(Core::UseMacShortcuts ? tr("Meta+H,Meta+C") : tr("Alt+G,Alt+C")));
    m_mercurialContainer->addAction(command);
    m_mercurialContainer->addAction(command);
    //    m_mercurialContainer->addAction(command);
        m_mercurialContainer->setEnabled(false);
    m_mercurialContainer->setEnabled(repoEnabled);