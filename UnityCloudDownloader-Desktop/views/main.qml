import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import "."

Window {
    id: mainWindow
    title: qsTr("Unity Cloud Downloader")
    width: 600
    height: 800
    visible: true

    StackView {
        id: mainStack
        anchors.fill: parent

        initialItem: ProfileList {
            width: parent.width
            height: parent.height
        }
    }

    Connections {
        // use this connection declaration because of QTBUG-47917
        target: mainWindow
        onClosing: {
            while (mainStack.depth > 1)
                mainStack.pop()
        }
    }
}
