/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtWidgets>
#include <QtNetwork>
#include "lightmaps.h"
#include "mapzoom.h"

MapZoom::MapZoom()
    : QMainWindow(0)
{
    map = new LightMaps(this);
    setCentralWidget(map);
    map->setFocus();

    QAction *DelhiAction = new QAction(tr("&Delhi"), this);
    QAction *BangaloreAction = new QAction(tr("&Bangalore"), this);
    QAction *MumbaiAction = new QAction(tr("&Mumbai"), this);
    QAction *nightModeAction = new QAction(tr("Night Mode"), this);
    nightModeAction->setCheckable(true);
    nightModeAction->setChecked(false);
    connect(DelhiAction, SIGNAL(triggered()), SLOT(chooseDelhi()));
    connect(BangaloreAction, SIGNAL(triggered()), SLOT(chooseBangalore()));
    connect(MumbaiAction, SIGNAL(triggered()), SLOT(chooseMumbai()));
    connect(nightModeAction, SIGNAL(triggered()), map, SLOT(toggleNightMode()));

    QMenu *menu = menuBar()->addMenu(tr("&Options"));
    menu->addAction(DelhiAction);
    menu->addAction(BangaloreAction);
    menu->addAction(MumbaiAction);
    menu->addSeparator();
    menu->addAction(nightModeAction);

    QNetworkConfigurationManager manager;
    if (manager.capabilities() & QNetworkConfigurationManager::NetworkSessionRequired) {
        // Get saved network configuration
        QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
        settings.beginGroup(QLatin1String("QtNetwork"));
        const QString id =
            settings.value(QLatin1String("DefaultNetworkConfiguration")).toString();
        settings.endGroup();

        // If the saved network configuration is not currently discovered use the system
        // default
        QNetworkConfiguration config = manager.configurationFromIdentifier(id);
        if ((config.state() & QNetworkConfiguration::Discovered) !=
            QNetworkConfiguration::Discovered) {
            config = manager.defaultConfiguration();
        }

        networkSession = new QNetworkSession(config, this);
        connect(networkSession, SIGNAL(opened()), this, SLOT(sessionOpened()));

        networkSession->open();
    } else {
        networkSession = 0;
    }

    setWindowTitle(tr("Light Maps"));
}

void MapZoom::sessionOpened()
{
    // Save the used configuration
    QNetworkConfiguration config = networkSession->configuration();
    QString id;
    if (config.type() == QNetworkConfiguration::UserChoice) {
        id = networkSession->sessionProperty(
                QLatin1String("UserChoiceConfiguration")).toString();
    } else {
        id = config.identifier();
    }

    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    settings.beginGroup(QLatin1String("QtNetwork"));
    settings.setValue(QLatin1String("DefaultNetworkConfiguration"), id);
    settings.endGroup();
}

void MapZoom::chooseDelhi()
{
    map->setCenter(28.61394, 77.20902);
}

void MapZoom::chooseBangalore()
{
    map->setCenter(23.02251, 72.57136);
}

void MapZoom::chooseMumbai()
{
    map->setCenter(18.96335, 72.93149);
}

