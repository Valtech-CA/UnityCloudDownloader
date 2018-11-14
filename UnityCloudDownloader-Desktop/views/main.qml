import QtQuick 2.9
import QtQuick.Controls 2.2

StackView {
    id: mainStack
    width: 640
    height: 480
    anchors.fill: parent
    visible: true

    initialItem: ProfileList {}
}
