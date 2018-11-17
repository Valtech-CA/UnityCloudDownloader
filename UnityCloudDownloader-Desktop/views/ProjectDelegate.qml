import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0

Item {
    id: projectDelegate
    height: 120
    width: 400

    Image {
        id: icon
        width: 60
        height: 60
        asynchronous: true
        source: iconPath
        anchors.left: parent.left
        anchors.leftMargin: 30
        anchors.verticalCenter: parent.verticalCenter
        sourceSize.width: 60
        sourceSize.height: 60
    }

    Text {
        text: name
        anchors.left: icon.right
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        color: Material.foreground
        font.pointSize: 32
    }

    ArrowButton {
        width: 80
        height: 80
        anchors.right: parent.right
        anchors.rightMargin: 30
        anchors.verticalCenter: parent.verticalCenter
    }


}
