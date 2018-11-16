import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Material 2.3

Page {
    id: addProfilePage

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Back")
                Layout.minimumWidth: 120
                Layout.rightMargin: 10
                onClicked: {
                    mainStack.pop()
                }
            }
        }
    }

    Text {
        color: Material.foreground
        text: qsTr("Add New Profile")
        anchors.top: parent.top
        anchors.topMargin: 20
        font.pointSize: 30
        anchors.horizontalCenter: parent.horizontalCenter
    }

    GridLayout {
        id: grid
        columns: 2
        anchors.right: parent.right
        anchors.rightMargin: 80
        anchors.left: parent.left
        anchors.leftMargin: 80
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        columnSpacing: 10

        Text {
            color: Material.foreground
            text: qsTr("Name")
            font.pointSize: 16
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: nameField
            placeholderText: qsTr("Profile Name")
            Layout.fillWidth: true
            property bool isValid: !!text
        }

        Text {
            color: Material.foreground
            text: qsTr("Api Key")
            font.pointSize: 16
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: apiField
            placeholderText: qsTr("Unity API Key")
            Layout.fillWidth: true

            property bool isValid: false

            onEditingFinished: {
                unityClient.testKey(apiField.text)
            }

            onTextEdited: {
                apiField.color = Material.foreground
                testApiTimer.restart()
            }

            Timer {
                id: testApiTimer
                interval: 1200
                running: false
                repeat: false
                onTriggered: {
                    unityClient.testKey(apiField.text)
                }
            }

            Connections {
                target: unityClient
                onKeyTested: {
                    if (apiField.text === apiKey) {
                        apiField.isValid = isValid
                        if (isValid) {
                            apiField.color = "#33cc33"
                        } else {
                            apiField.color = "#cc3300"
                        }
                    }
                }
            }
        }

        Text {
            color: Material.foreground
            text: qsTr("Rooth Path")
            font.pointSize: 16
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: pathField
            placeholderText: qsTr("Root Directory")
            readOnly: true
            Layout.fillWidth: true

            property bool isValid: !!text

            onPressed: {
                folderDialog.visible = true
            }
        }

        Button {
            id: nextButton
            text: qsTr("Add")
            Layout.columnSpan: 2
            Layout.alignment: Qt.AlignHCenter
            Layout.minimumWidth: 120
            enabled: nameField.isValid && apiField.isValid && pathField.isValid

            onClicked: {
                profileModel.createProfile(nameField.text, apiField.text, pathField.text)
                mainStack.pop()
            }
        }

        Item {
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

    }

    FileDialog {
        id: folderDialog
        title: qsTr("Choose a root folder")
        selectFolder: true
        folder: shortcuts.documents

        onAccepted: {
            pathField.text = urlToPath(folderDialog.fileUrl)
        }
    }

}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/
