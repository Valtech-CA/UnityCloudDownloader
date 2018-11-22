import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3
import ucd 1.0

Page {
    id: buildList

    property string buildTargetId: ""

    footer: BackNavigationBar {}

    ListView {
        id: buildListView
        anchors.fill: parent
        model: BuildsModel {
            buildTargetId: buildList.buildTargetId
        }

        delegate: BuildDelegate {
            width: parent.width
        }
        currentIndex: -1

        highlight: ItemHighlight {}
        highlightMoveDuration: 200
    }

}
