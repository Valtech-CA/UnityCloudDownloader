import QtQuick 2.9
import QtQuick.Controls 2.4

Rectangle {
    id: profileDelegate
    width: parent.width
    height: 120

    Column {
        id: column
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.verticalCenter: parent.verticalCenter
        spacing: 2

        Text {
            id: nameText
            text: name
            font.pointSize: 24
        }

        Text {
            id: pathText
            text: roothPath
            font.italic: true
            font.pointSize: 14
        }
    }


}

/*##^## Designer {
    D{i:4;anchors_height:400;anchors_width:200;anchors_x:8;anchors_y:8}
}
 ##^##*/
