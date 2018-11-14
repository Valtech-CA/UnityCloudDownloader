import QtQuick 2.9
import QtQuick.Controls 2.4

Page {
    id: profilePage
    anchors.fill: parent

    footer: ToolBar {
        Label {
            text: "Add"
        }
        Label {
            text: "Remove"
        }
    }

    ListView {
        id: profileListView
        anchors.fill: parent
        delegate: ProfileDelegate {}
    }
}
