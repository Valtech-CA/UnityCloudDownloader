import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3
import ucd 1.0

Page {
    id: projectList

    property string profileId: ""

    footer: ToolBar {
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

    ListView {
        id: projectListView
        anchors.fill: parent
        model: ProjectsModel {
            database: ucdDb
            profileId: projectList.profileId
        }

        delegate: ProjectDelegate {
            width: parent.width
        }
        currentIndex: -1
    }

}
