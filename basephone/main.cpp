/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia. For licensing terms and
** conditions see http://qt.digia.com/licensing. For further information
** use the contact form at http://qt.digia.com/contact-us.
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
** In addition, as a special exception, Digia gives you certain additional
** rights. These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/QStandardPaths>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtGui/QGuiApplication>
#include <QtQuick/QQuickItem>
#include <QtGui/QScreen>
#include <QtQml/QQmlApplicationEngine>

#include <QtCore/QTimer>
#include <signal.h>

void exit_properly (int);
static QGuiApplication *p_app;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    p_app = &app;	/* For calling quit in signal handler */
    QString serverIpParam("192.168.5.182");

    if (argc > 1) {
        serverIpParam = argv[1];
    }

    //Show information of screen (all monitors)
    // If 2 screen availabe, chose the larger as it is usually default
    QScreen *screen;
    if (QGuiApplication::screens().size() <= 1) {
        screen = QGuiApplication::primaryScreen();
    } else {
        QScreen *tmp_screen;
        if(QGuiApplication::screens().at(0) == nullptr)
            screen = QGuiApplication::screens().at(1);
        else if(QGuiApplication::screens().at(1) == nullptr )
            screen = QGuiApplication::screens().at(0);
        else {
            screen = QGuiApplication::screens().at(0);
            tmp_screen = QGuiApplication::screens().at(1);
            if((screen->availableSize().width() * screen->availableSize().height()) < (tmp_screen->availableSize().width() * tmp_screen->availableSize().height()))
                screen = QGuiApplication::screens().at(1);
        }
    }
    qDebug() << "Information for screen:" << screen->name();
    qDebug() << "  Available geometry:" << screen->availableGeometry().x() << screen->availableGeometry().y() << screen->availableGeometry().width() << "x" << screen->availableGeometry().height();
    qDebug() << "  Available size:" << screen->availableSize().width() << "x" << screen->availableSize().height();
    qDebug() << "Server IP adress:" << serverIpParam;

    engine.rootContext()->setContextProperty("screenHeight", screen->availableSize().height() );
    engine.rootContext()->setContextProperty("screenWidth", screen->availableSize().width());
    engine.rootContext()->setContextProperty("serverIP", serverIpParam);

    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));

    signal (SIGINT, exit_properly);
    signal (SIGTERM, exit_properly);
    return app.exec();
}

void exit_properly (int sig)
{
        (void)(sig);
	signal (SIGINT, SIG_DFL);
	p_app->exit();
}
