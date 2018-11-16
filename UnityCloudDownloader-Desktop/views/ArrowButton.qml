import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.0

Item {
    id: arrowButton

    signal clicked()

    Image {
        id: arrow
        width: parent.width
        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        fillMode: Image.PreserveAspectFit
        source: "angle_right.png"
        sourceSize.width: 400
        sourceSize.height: 400
        scale: area.pressed ? 0.95 : area.containsMouse ? 1 : 0.9
    }

    MouseArea {
        id: area
        anchors.fill: parent
        hoverEnabled: true

        onClicked: arrowButton.clicked()
    }

}

/*##^## Designer {
    D{i:0;autoSize:true;height:100;width:100}
}
 ##^##*/
