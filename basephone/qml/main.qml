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

import QtQuick 2.5
import QtMultimedia 5.4
import QtQuick.Window 2.2

Window {
    id: container
    visible: true
    width: Screen.desktopAvailableWidth
    height: Screen.desktopAvailableHeight
    visibility: Window.Maximized
    flags: Qt.FramelessWindowHint|Qt.Window

    /*time data*/
    property var locale: Qt.locale()
    property date currentTime : new Date()

    readonly property double scalew: (width / 1920)    // Default width is 1920
    readonly property double scaleh: (height / 1080)    // Default height is 1080

    readonly property int mainscreen_width: 1280 * scalew
    readonly property int mainscreen_height: 800 * scaleh
    readonly property int mainscreen_x: 30 * scalew
    readonly property int mainscreen_y: 20 * scaleh

    property int subscreen_width: 0
    property int subscreen_height: 0
    property int subscreen_x: 0
    property int subscreen_y: 0

    Rectangle {
        id: root
        x:0
        y:0
        width: container.width
        height: container.height
        gradient: Gradient {
            GradientStop { position: 0.0; color: "white" }
            GradientStop { position: 1.0; color: "#656565" }
        }

        Streamplayer {
            id: stream1
            width: 1280 * scalew
            height: 800 * scaleh
            x: 30 * scalew
            y: 20 * scaleh
            color: "#ECECEC"
            source: "rtsp://" + serverIP + ":5001/camera" // "serverIP" is one-time variable.
                                                          // Do not use for other purposes.
            main_screen: true
            title: "STREAM 1"
            mouse_area.onClicked: {
                swap_screen (stream1)
            }
        }

        Streamplayer {
            id: stream2
            width: 540 * scalew
            height: 320 * scaleh
            x : 1350 * scalew                      //stream1.x + stream1.width + 40
            y : 20 * scaleh
            color: "#ECECEC"
            source: "rtsp://" + serverIP + ":5002/camera"
            main_screen: false
            title: "STREAM 2"
            mouse_area.onClicked: {
                swap_screen (stream2)
            }
        }

        Streamplayer{
            id:stream3
            width: 540 * scalew
            height: 320 * scaleh
            x : 1350 * scalew
            y : 380 * scaleh                        //stream2.y + stream2.height + 40
            color: "#ECECEC"
            source: "rtsp://" + serverIP + ":5003/camera"
            main_screen: false
            title: "STREAM 3"
            mouse_area.onClicked: {
                swap_screen (stream3)
            }
        }

        Streamplayer{
            id:stream4
            width: 540 * scalew
            height: 320 * scaleh
            x : 1350 * scalew
            y : 740 * scaleh                        //stream3.y + stream3.height + 40
            color: "#ECECEC"
            source: "rtsp://" + serverIP + ":5004/camera"
            main_screen: false
            title: "STREAM 4"
            mouse_area.onClicked: {
                swap_screen (stream4)
            }
        }

        /*Renesas logo*/
        Image {
            id: renesas_logo
            source: "images/doorphone_logo.png"
            x: 30 * scalew
            y: 833 * scaleh
            width: 800 * scalew
            height: 235 * scaleh
            visible: true
        }

        /*Setting icon
         *Setting dialog will appear when this icon is clicked
         *The swap function will not work until the setting dialog close*/
        Image {
            id: setting_icon1
            source: "images/Gear-256.png"
            x: mainscreen_x
            y: mainscreen_y + mainscreen_height - height
            z: 10
            visible: true
            width: 100 * scalew
            height: 100 * scalew

            MouseArea {
                z: parent.z
                anchors.fill: parent
                onClicked: {
                    setting_dialog.visible = true
                    stream1.mouse_area_enabled = false
                    stream2.mouse_area_enabled = false
                    stream3.mouse_area_enabled = false
                    stream4.mouse_area_enabled = false
                }
            }
        }

        /*Setting dialog*/
        Rectangle {
            id: setting_dialog
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            z: 0
            visible: false
            width: 480 * scalew
            height: 270 * scaleh

            Rectangle {
                id: setting_bar
                x: 0
                y: 0
                width: parent.width
                height: 30 * scaleh
                color: "#0f4c81"

                Image {
                    id: icon_setting_bar
                    x: 0
                    y: 0
                    source: "images/Gear-256.png"
                    width: parent.height
                    height: parent.height
                }

                Text {
                    id: setting_title
                    anchors.left: icon_setting_bar.right
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    color: "white"
                    font.pointSize: 10 * scaleh
                    font.family: "Open Sans"
                    text: qsTr("Basephone Setting")
                }
            }

            Rectangle {
                id: setting_field
                width: parent.width
                height: parent.height - setting_bar.height
                anchors.top: setting_bar.bottom
                border.color: "#0f4c81"
                border.width: 2 * scalew

                Text {
                    id: server_cam1
                    text: qsTr("Cam1 Server")
                    font.family: "Open Sans"
                    font.pixelSize: 16 * scaleh
                    width: 110 * scalew
                    x: 25 * scalew
                    y: 30 * scaleh
                }

                /*Input source of stream 1*/
                Textfield {
                    id: cam1_input
                    anchors.left: server_cam1.right
                    anchors.verticalCenter: server_cam1.verticalCenter
                    width: 320 * scalew
                    height: 30 * scaleh
                    text: stream1.source
                }

                Text {
                    id: server_cam2
                    text: qsTr("Cam2 Server")
                    font.family: "Open Sans"
                    font.pixelSize: 16 * scaleh
                    width: server_cam1.width
                    x: server_cam1.x
                    y: server_cam1.y + server_cam1.height + 20 * scaleh

                }

                /*Input source of stream 2*/
                Textfield {
                    id: cam2_input
                    anchors.left: server_cam2.right
                    anchors.verticalCenter: server_cam2.verticalCenter
                    width: cam1_input.width
                    height: cam1_input.height
                    text: stream2.source
                }

                Text {
                    id: server_cam3
                    text: qsTr("Cam3 Server")
                    font.family: "Open Sans"
                    font.pixelSize: 16 * scaleh
                    width: server_cam1.width
                    x: server_cam2.x
                    y: server_cam2.y + server_cam2.height + 20 * scaleh
                }

                /*Input source of stream 3*/
                Textfield {
                    id: cam3_input
                    anchors.left: server_cam3.right
                    anchors.verticalCenter: server_cam3.verticalCenter
                    width: cam1_input.width
                    height: cam1_input.height
                    text: stream3.source
                }

                Text {
                    id: server_cam4
                    text: qsTr("Cam4 Server")
                    font.family: "Open Sans"
                    font.pixelSize: 16 * scaleh
                    width: server_cam1.width
                    x: server_cam3.x
                    y: server_cam3.y + server_cam3.height + 20 * scaleh

                }

                /*Input source of stream 4*/
                Textfield {
                    id: cam4_input
                    anchors.left: server_cam4.right
                    anchors.verticalCenter: server_cam4.verticalCenter
                    width: cam1_input.width
                    height: cam1_input.height
                    text: stream4.source
                }

                /*OK button
                 *When this button is clicked, source of stream is set by string in
                 *cam1_input, cam2_input, cam3_input and cam4_input
                 *If theser Textfield is empty, source of stream is not be changed*/
                Rectangle {
                    id: ok_button
                    width: 70 * scalew
                    height: 35 * scaleh
                    radius: 3 * scaleh
                    color: "#0f4c81"
                    x: 100 * scalew
                    y: setting_field.height - height -10 * scaleh

                    Text {
                        id: ok_text
                        text: qsTr("OK")
                        color: "white"
                        font.pixelSize: 16 * scaleh
                        font.family: "Open Sans"
                        anchors.verticalCenter: ok_button.verticalCenter
                        anchors.horizontalCenter: ok_button.horizontalCenter
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (cam1_input.placeholderVisible !== true) {
                                stream1.stop_media()
                                stream1.source = cam1_input.text
                                stream1.play_media()
                            }
                            if (cam2_input.placeholderVisible !== true) {
                                stream2.stop_media()
                                stream2.source = cam2_input.text
                                stream2.play_media()
                            }
                            if (cam3_input.placeholderVisible !== true) {
                                stream3.stop_media()
                                stream3.source = cam3_input.text
                                stream3.play_media()
                            }
                            if (cam4_input.placeholderVisible !== true) {
                                stream4.stop_media()
                                stream4.source = cam4_input.text
                                stream4.play_media()
                            }
                            setting_dialog.visible = false
                            stream1.mouse_area_enabled = true
                            stream2.mouse_area_enabled = true
                            stream3.mouse_area_enabled = true
                            stream4.mouse_area_enabled = true
                        }
                    }

                }

                /*Cancel button
                 *Setting dialog is closed when this button is clicked*/
                Rectangle {
                    id: cancel_button
                    width: 70 * scalew
                    height: 35 * scaleh
                    radius: 3 * scaleh
                    color: "#0f4c81"
                    x: 290 * scalew
                    y: setting_field.height - height -10 * scaleh

                    Text {
                        id: cancel_text
                        text: qsTr("CANCEL")
                        color: "white"
                        font.pixelSize: 16 * scaleh
                        font.family: "Open Sans"
                        anchors.verticalCenter: cancel_button.verticalCenter
                        anchors.horizontalCenter: cancel_button.horizontalCenter
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            setting_dialog.visible = false
                            stream1.mouse_area_enabled = true
                            stream2.mouse_area_enabled = true
                            stream3.mouse_area_enabled = true
                            stream4.mouse_area_enabled = true
                        }
                    }

                }
            }
        }

        /*Timer*/
        Timer {
            id: timer
            interval: 1000
            running: true
            repeat: true
            onTriggered: {
                currentTime = new Date()
                time.text = currentTime.toLocaleString(locale, Locale.LongFormat)
            }
        }

        Text {
            id: time
            x: 1300 * scalew - time.width
            y: 810 * scaleh - time.height
            font.family: "Open Sans"
            font.pointSize: 10 * scaleh
            color: "#0f5cb5"
        }
    }

    /*This function is used to change he position and widthxheight of the main screen
     *and he subscreens (the small screens on the right of monitor)*/
    function swap_screen(streamplayer) {
        /*Store position and widthxheight of subscreen*/
        subscreen_x = streamplayer.x
        subscreen_y = streamplayer.y
        subscreen_width = streamplayer.width
        subscreen_height = streamplayer.height
        /*Set position and widthxheight of main screen to subscreen*/
        streamplayer.x = mainscreen_x
        streamplayer.y = mainscreen_y
        streamplayer.width = mainscreen_width
        streamplayer.height = mainscreen_height

        /*Set position and widthxheight of subscreen to main screen*/
        if (stream1.main_screen === true) {
            stream1.x = subscreen_x
            stream1.y = subscreen_y
            stream1.width = subscreen_width
            stream1.height = subscreen_height
            stream1.main_screen = false
        } else if (stream2.main_screen === true) {
            stream2.x = subscreen_x
            stream2.y = subscreen_y
            stream2.width = subscreen_width
            stream2.height = subscreen_height
            stream2.main_screen = false
        } else if (stream3.main_screen === true) {
            stream3.x = subscreen_x
            stream3.y = subscreen_y
            stream3.width = subscreen_width
            stream3.height = subscreen_height
            stream3.main_screen = false
        } else if (stream4.main_screen === true) {
            stream4.x = subscreen_x
            stream4.y = subscreen_y
            stream4.width = subscreen_width
            stream4.height = subscreen_height
            stream4.main_screen = false
        }
        streamplayer.main_screen = true
    }
}
