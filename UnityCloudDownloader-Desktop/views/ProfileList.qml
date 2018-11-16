import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import "qrc:/views"

Page {
    id: profilePage

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent
            Button {
                id: addButton
                text: qsTr("Add")
                Layout.fillWidth: true
                Layout.leftMargin: 10
                onClicked: {
                    mainStack.push("qrc:/views/AddProfile.qml")
                }
            }

            Button {
                id: removeButton
                text: qsTr("Remove")
                Layout.fillWidth: true
                Layout.rightMargin: 10
                enabled: profileListView.currentIndex !== -1
                onClicked: {
                    profileModel.remove(profileListView.currentIndex)
                }
            }
        }
    }

    ListView {
        id: profileListView
        anchors.fill: parent
        model: profileModel
        delegate: ProfileDelegate {}

        Component.onCompleted: {
            profileListView.currentIndex = -1
            console.log(profileListView.currentIndex)
        }
    }
}
