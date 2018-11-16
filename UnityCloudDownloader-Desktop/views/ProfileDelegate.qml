import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3

Item {
    id: profileDelegate
    height: 120
    width: 400

    Rectangle {
        id: rect
        anchors.fill: parent
        anchors.margins: 4
        color: "transparent"
        border.color: Material.color(Material.Grey)
        border.width: 2
        radius: 10
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            profileDelegate.ListView.view.currentIndex = index
        }
    }

    Column {
        id: column
        anchors.left: parent.left
        anchors.leftMargin: 40
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

    ArrowButton {
        id: arrow
        width: 80
        height: 80
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 30

        onClicked: {
            mainStack.push("./ProjectList.qml", {"profileId": id})
        }
    }


}
