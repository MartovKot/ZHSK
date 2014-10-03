import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Window 2.2

ApplicationWindow {
    id: mainwind
    visible: true
    width: 640
    height: 480
    title: qsTr("KvitMain")
    flags: Qt.FramelessWindowHint


    property int qwX: 0;  property int qwY: 0
    property int owX: 0;    property int owY: 0

    Image {
        anchors.fill: parent
        source: "qrc:/theme/fon.png"
    }

    // временный блок
//    menuBar: MenuBar {
//        Menu {
//            title: qsTr("File")
//            MenuItem {
//                text: qsTr("&Open")
//                onTriggered: console.log("Open action triggered");
//            }
//            MenuItem {
//                text: qsTr("Exit")
//                onTriggered: Qt.quit();
//            }
//        }
//    }
    //---


    MouseArea {
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.fill: parent

        onPressed:
        {
            mainwind.owX =  mouseX
            mainwind.owY = mouseY
        }

        onPositionChanged:
        {
            if (pressedButtons == Qt.LeftButton){
                qwX = mouseX - owX
                qwY = mouseY - owY
                mainwind.setX(mainwind.x + qwX)
                mainwind.setY(mainwind.y + qwY)
            }
        }
    }

    Text {
        id: address
        x: 10
        y: 10
        width: 357
        height: 40
        color: "#ffffff"
        text: qsTr("Адрес")
        font.family: "Monotype Corsiva"
        styleColor: "#ffffff"
        wrapMode: Text.NoWrap
        font.pixelSize: 24

    }

    Text {
        id: organization
        x: 10
        y: 50
        width: 357
        height: 40
        color: "#ffffff"
        text: qsTr("Организация")
        font.family: "Monotype Corsiva"
        styleColor: "#ffffff"
        wrapMode: Text.NoWrap
        font.pixelSize: 24
    }

    Row {
        id: row1
        x: 0
        y: 0
        width: 360
        height: 80

        Text {
            id: text1
            x: 8
            y: 105
            width: 63
            height: 37
            color: "#ffffff"
            text: qsTr("Text")
            font.pixelSize: 12
        }

        Button {
            id: button1
            x: 102
            y: 112
            text: qsTr("Button")
        }
    }

}
