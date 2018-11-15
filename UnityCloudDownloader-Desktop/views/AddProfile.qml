import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

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
                Layout.rightMargin: 10
                onClicked: {
                    mainStack.pop()
                }
            }
        }
    }

    Text {
        x: 309
        color: "#cccccc"
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
            color: "#cccccc"
            text: qsTr("Name")
            font.pointSize: 16
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: nameField
            placeholderText: qsTr("Profile Name")
            Layout.fillWidth: true
        }

        Text {
            color: "#cccccc"
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
                apiField.color = "#cccccc"
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
                    if (apiField.text == apiKey) {
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
            color: "#cccccc"
            text: qsTr("Rooth Path")
            font.pointSize: 16
            Layout.alignment: Qt.AlignRight
        }

        TextField {
            id: pathField
            placeholderText: qsTr("Root Directory")
            Layout.fillWidth: true
        }

        Item {
            id: status
            Layout.columnSpan: 2
            Layout.fillWidth: true
        }

        Item {
            Layout.columnSpan: 2
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

    }

}

/*##^## Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
 ##^##*/