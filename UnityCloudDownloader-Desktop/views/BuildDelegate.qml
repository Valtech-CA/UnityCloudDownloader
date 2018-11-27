import QtQuick 2.9
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.0
import QtQuick.Controls 2.3
import ucd 1.0

ItemDelegate {
    id: buildDelegateItem
    height: 120
    width: 600

    onClicked: ListView.view.currentIndex = index

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

    ProgressBar {
        id: downloadProgressBar
        value: downloadProgress
        indeterminate: isQueued
        width: parent.width - 60
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 8
        anchors.horizontalCenter: parent.horizontalCenter
        visible: isQueued || isDownloading
    }

    Label {
        id: downloadSpeedText
        font.pointSize: 11
        anchors.right: downloadProgressBar.right
        anchors.bottom: downloadProgressBar.top
        anchors.bottomMargin: 2
        text: formattedDataSize(downloadSpeed) + "/s"
        visible: isDownloading && downloadProgress < 1
    }

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

    BusyIndicator {
        anchors.fill: icon
        running: icon.status === Image.Loading
    }

    Column {
        anchors.left: icon.right
        anchors.leftMargin: 20
        anchors.verticalCenter: parent.verticalCenter

        Row {
            spacing: 10

            Text {
                text: "#" + buildNumber
                width: 120
                color: Material.foreground
                font.pointSize: 28
            }

            Text {
                text: name
                color: Material.foreground
                font.pointSize: 28
            }
        }

        Row {
            spacing: 10

            Text {
                text: Qt.formatDateTime(createTime, "d MMMM hh:mm")
                color: Material.color(Material.Grey)
                font.pointSize: 12
            }

            Text {
                text: formattedDataSize(artifactSize)
                color: Material.color(Material.Grey)
                font.pointSize: 12
                visible: artifactSize != 0
            }
        }
    }

    Image {
        id: statusIcon
        asynchronous: true
        anchors.right: parent.right
        anchors.rightMargin: 30
        anchors.verticalCenter: parent.verticalCenter
        width: 60
        height: 60
        source: imageForStatus(buildStatus)

        property string tooltipText: ""

        MouseArea {
            id: statusArea
            anchors.fill: statusIcon
            hoverEnabled: true

            ToolTip.visible: containsMouse
            ToolTip.text: statusIcon.tooltipText
            ToolTip.delay: 1000
        }

        function imageForStatus(status) {
            if (status === Build.Queued) {
                tooltipText = qsTr("queued");
                return "queued.png"
            } else if (status === Build.SentToBuilder) {
                tooltipText = qsTr("sent to builder")
                return "queued.png"
            } else if (status === Build.Started) {
                tooltipText = qsTr("build in progress")
                return "started.png"
            } else if (status === Build.Restarted) {
                tooltipText = qsTr("restarted")
                return "restarted.png"
            } else if (status === Build.Success) {
                tooltipText = qsTr("build successful")
                return "success.png"
            } else if (status === Build.Failure) {
                tooltipText = qsTr("failed")
                return "failure.png"
            } else if (status === Build.Canceled) {
                tooltipText = qsTr("canceled")
                return "canceled.png"
            } else {
                tooltipText = qsTr("unknown")
                return "unknown.png"
            }
        }
    }

    ToolButton {
        id: downloadButton
        width: 60
        height: 60
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: statusIcon.left
        anchors.rightMargin: 4
        visible: !!artifactPath && !isQueued && !isDownloading && !isDownloaded

        onClicked: downloadManually(buildRef)

        Image {
            fillMode: Image.PreserveAspectFit
            anchors.fill: parent
            anchors.margins: 8
            source: "download.png"
            scale: parent.pressed ? 0.95 : parent.hovered ? 1 : 0.9
        }
    }

    ToolButton {
        id: openFolder
        width: 60
        height: 60
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: statusIcon.left
        anchors.rightMargin: 4
        visible: isDownloaded

        onClicked: openBuildFolder(buildRef)

        Image {
            fillMode: Image.PreserveAspectFit
            anchors.fill: parent
            anchors.margins: 8
            source: "folder.png"
            scale: parent.pressed ? 0.95 : parent.hovered ? 1 : 0.9
        }
    }
}
