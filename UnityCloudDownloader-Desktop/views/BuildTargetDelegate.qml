import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.3

ItemDelegate {
    id: buildTargetDelegate
    height: 120
    width: 600
    clip: true

    onClicked: ListView.view.currentIndex = index

    Behavior on height {
        NumberAnimation {
            easing.type: Easing.OutSine
        }
    }

    Item {
        id: mainBlock
        height: 120
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.top: parent.top
        anchors.topMargin: 0

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

        Switch {
            id: syncSwitch
            text: qsTr("sync")
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: arrow.left
            anchors.rightMargin: 10
            Component.onCompleted: {
                checked = sync
                checkedChanged.connect(function() { sync = checked })
            }
        }

        ArrowButton {
            id: arrow
            width: 80
            height: 80
            anchors.right: parent.right
            anchors.rightMargin: 30
            anchors.verticalCenter: parent.verticalCenter

            onClicked: {
                mainStack.push("./BuildList.qml", { "buildTargetId": buildTargetId })
            }
        }
    }

    Item {
        id: configBox
        anchors.top: parent.top
        anchors.topMargin: 120
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        opacity: 0

        Behavior on opacity {
            OpacityAnimator {
                easing.type: Easing.OutSine
            }
        }

        GroupBox {
            id: optionBox
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.rightMargin: 10
            title: qsTr("builds to keep")

            GridLayout {
                anchors.top: parent.top
                anchors.left: parent.left
                columns: 4

                Text {
                    text: qsTr("max")
                    color: Material.foreground
                    Layout.alignment: Qt.AlignRight
                }

                SpinBox {
                    id: maxSpin
                    from: minSpin.value
                    editable: true
                    Component.onCompleted: {
                        value = maxBuilds
                        valueChanged.connect(function() { maxBuilds = value })
                    }
                }

                Text {
                    text: qsTr("min")
                    color: Material.foreground
                    Layout.alignment: Qt.AlignRight
                }

                SpinBox {
                    id: minSpin
                    from: 1
                    editable: true
                    Component.onCompleted: {
                        value = minBuilds
                        valueChanged.connect(function() { minBuilds = value });
                    }
                }

                Text {
                    text: qsTr("days")
                    color: Material.foreground
                    Layout.alignment: Qt.AlignRight
                }

                SpinBox {
                    id: daysSpin
                    from: 1
                    to: 365
                    editable: true
                    Component.onCompleted: {
                        value = maxDaysOld
                        valueChanged.connect(function() { maxDaysOld = value; })
                    }
                }

                Item {
                    Layout.columnSpan: 2
                }
            }
        }
    }

    states: [
        State {
            name: "expanded"
            when: syncSwitch.checked

            PropertyChanges {
                target: configBox
                opacity: 1
            }

            PropertyChanges {
                target: buildTargetDelegate
                height: 120 + optionBox.implicitHeight + 10
            }
        }
    ]
}
