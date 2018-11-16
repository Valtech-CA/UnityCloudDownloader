import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3
import "qrc:/views"

Item {
    id: profileDelegate
    width: parent.width
    height: 120

    MouseArea {
        anchors.fill: parent
        onClicked: {
            profileDelegate.ListView.view.currentIndex = index
        }
    }

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
            color: Material.foreground
        }

        Text {
            id: pathText
            text: rootPath
            font.italic: true
            font.pointSize: 14
            color: Material.foreground
        }
    }

}

/*##^## Designer {
    D{i:4;anchors_height:400;anchors_width:200;anchors_x:8;anchors_y:8}
}
 ##^##*/
