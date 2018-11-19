import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3
import ucd 1.0

Page {
    id: buildTargetList

    property string projectId: ""

    footer: BackNavigationBar {}

    ListView {
        id: buildTargetListView
        anchors.fill: parent
        model: BuildTargetsModel {
            database: ucdDb
            projectId: buildTargetList.projectId
        }

        delegate: BuildTargetDelegate {
            width: parent.width
        }
        currentIndex: -1

        highlight: ItemHighlight {}
        highlightMoveDuration: 200
    }

}
