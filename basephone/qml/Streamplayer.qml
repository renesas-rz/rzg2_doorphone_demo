import QtQuick 2.5
import QtMultimedia 5.4

/*Streamplayer qml type use MediaPlayer to receive rtsp server stream
 *Source of stream is set by source property
 *There is a label on the top left of the rectangle which can be set text
 *by title property*/

Rectangle {
    id: root
    visible: true

    property alias color: stream_field.color
    property alias source: media_player.source
    property alias mouse_area_enabled: mouse_area.enabled
    property alias mouse_area: mouse_area
    property alias title: title.text

    property bool main_screen: false

    function play_media() {
        media_player.play()
    }

    function stop_media() {
        media_player.stop()
    }

    Border {
        id: border_stream
        border_width: 5 * scalew
        rec_height: stream_field.height
        rec_width: stream_field.width
        rec_x: stream_field.x
        rec_y: stream_field.y
        border_color: "#0f4c81"
    }

    Rectangle {
        id: stream_field
        anchors.fill: parent

        MediaPlayer {
            id: media_player
            source: "rtsp://192.168.5.182:5001/camera"

            Component.onCompleted: {
                media_player.play()
            }

            onPlaybackStateChanged: {
                if (media_player.playbackState !== MediaPlayer.PlayingState) {
                    media_player.play()
                }
            }
        }

        VideoOutput {
            id: video_output
            anchors.fill: parent
            fillMode: VideoOutput.Stretch
            visible: true
            source: media_player
        }

        Rectangle {
            id: title_field
            x: 0
            y: 0
            width: 100 * scalew
            height: 42 * scaleh
            color: "black"
            opacity: 0.4
        }


        Text {
            id: title
            anchors.verticalCenter: title_field.verticalCenter
            anchors.horizontalCenter: title_field.horizontalCenter
            font.family: "Open Sans"
            color: "white"
            font.pixelSize: 18 * scaleh
            text: qsTr("STREAM 1")
        }

        /*When the subscreen is clicked, it will change width, height and position
         *to become main screen and the main screen will change width, height and
         *position to become subscreen*/
        MouseArea {
            id: mouse_area
            anchors.fill: parent
            onClicked: {
                if (stream_field.mainscreen === false) {
                    swap_screen(stream_field)
                }
            }
        }
    }
}
