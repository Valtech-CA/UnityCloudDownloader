import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3

ToolBar {
    RowLayout {
        anchors.fill: parent

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
