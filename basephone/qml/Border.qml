import QtQuick 2.5

/*This is the border which has gradient color set by border_color property
 *The width of border is set by border_width property
 *This border cover a rectangle which has rec_x and rec_y as position and
 *rec_width and rec_height as widthxheight*/

Item {

    /*Properties of Border*/
    property int border_width: 0
    property int rec_height: 0
    property int rec_width: 0
    property int rec_x: 0
    property int rec_y: 0
    property color border_color: "black"

    /*Left border*/
    Rectangle {
        id: border_left
        visible: true
        width: border_width * 2
        height: rec_height + border_width * 2
        x: rec_x - border_width
        y: rec_y - border_width
        radius: border_width

        gradient: Gradient {
            GradientStop { position: 0.0; color: "white" }
            GradientStop { position: 0.5; color: border_color }
            GradientStop { position: 1.0; color: "white" }
        }
    }

    /*Right border*/
    Rectangle {
        id: border_right
        visible: true
        width: border_width * 2
        height: rec_height + border_width * 2
        radius: border_width
        x: border_left.x + rec_width
        y: border_left.y
        gradient: Gradient {
            GradientStop { position: 0.0; color: "white" }
            GradientStop { position: 0.5; color: border_color }
            GradientStop { position: 1.0; color: "white" }
        }
    }

    /*Top and bottom border*/
    Rectangle {
        id: border_center
        visible: true
        width: rec_height + border_width * 2
        height: rec_width
        x: rec_x + (rec_width - width)/2
        y: rec_y + (rec_height - height)/2
        rotation: 90

        gradient: Gradient {
            GradientStop { position: 0.0; color: "white" }
            GradientStop { position: 0.5; color: border_color }
            GradientStop { position: 1.0; color: "white" }
        }
    }
}
