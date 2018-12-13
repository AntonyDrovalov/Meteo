import QtQuick 2.8

Rectangle {
    width: 230
    height: 40
    radius: 3
    border.color: '#acacac'
    border.width: 1
    Text {
        id:disp
        anchors.centerIn: parent
        font.pointSize: 14
        font.family: "Liberation Sans"
        color: '#acacac'
    }
    Timer {
        interval:500;running:true;repeat:true
        onTriggered: disp.text=new Date().toTimeString()
    }
}

