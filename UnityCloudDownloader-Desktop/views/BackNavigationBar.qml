import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

ToolBar {
    RowLayout {
        anchors.fill: parent

        Button {
            id: refreshButton
            Layout.leftMargin: 10
            Image {
                fillMode: Image.PreserveAspectFit
                anchors.fill: parent
                anchors.margins: 4
                source: "white-sync.png"
                scale: parent.pressed ? 0.95 : parent.hovered ? 1 : 0.9
            }

            onClicked: refreshSync()
        }

        Item {
            Layout.fillWidth: true
        }

        Button {
            id: backButton
            Layout.rightMargin: 10
            Layout.minimumWidth: 120
            text: qsTr("Back")

            onClicked: mainStack.pop()
        }
    }
}
