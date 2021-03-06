import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Controls.Material 2.3

ItemDelegate {
    id: profileDelegate
    height: 120
    width: 400

    onClicked: {
        ListView.view.currentIndex = index
    }

    Rectangle {
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.right: parent.right
        anchors.rightMargin: 30
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 4
        height: 2
        color: Material.color(Material.Grey)
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
