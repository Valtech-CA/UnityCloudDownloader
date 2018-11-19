import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0

Item {
    id: buildTargetDelegate
    height: 120
    width: 400

    Column {
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
            id: platformText
            text: platform
            font.italic: true
            font.pointSize: 14
            color: Material.foreground
        }
    }
}
