import QtQuick 2.8



Rectangle {
    id:root
    width: 360
    height: 260
    radius: 3
    border.width: 1
    // default state with no data at all
    border.color: "#0a0ac8"//'#acacac'
    //color:"#d2d2df"
    color:"blue"
    opacity: 0.4
    property string sock: "BAR"


    states:[
        State {
            name:"NORMAL"
            PropertyChanges { target: root; border.color: '#acacac'}
            PropertyChanges { target: root; color: 'white'}
            PropertyChanges { target: root; opacity: 1 }
        },
        State {
            name:"NODRV"
            PropertyChanges { target: root; border.color: '#c80a0a'}
            PropertyChanges { target: root; color: '#dfd2d2'}
            PropertyChanges { target: root; opacity: 0.4 }
        }
    ]


    Row{
        id:row
        spacing: 5
        anchors.margins: spacing
        anchors.fill: parent
        property string fontFamily: "Liberation Mono"
        function childWidth() {
            return width / children.length - spacing
        }
        function fontSize() {
            var hSize = height/10
            var wSize = width
            return hSize>wSize ? wSize: hSize
        }

        Rectangle{
            id:rec1
            width:parent.childWidth()
            height:parent.height
            Text{
                id:t1
                width:parent.width
                height:20
                font.family: row.fontFamily
                font.pixelSize: row.fontSize()
                text: "QFE"
            }

            Text{
                id:qfe2
                width:parent.width
                anchors.bottom: parent.bottom
                font.family: row.fontFamily
                font.pixelSize: row.fontSize()*2
                text:"BBB"
            }

            Text{
                id:qfe
                width:parent.width
                anchors.verticalCenter: parent.verticalCenter
                font.family: row.fontFamily
                font.pixelSize: row.fontSize()*2
                text:"CCC"
            }
        }

        Rectangle{
            id:rec2
            width:parent.childWidth()
            height:parent.height
            Text{
                id:t2
                width:parent.width
                height:20
                font.family: row.fontFamily
                font.pixelSize: row.fontSize()
                text: "QNH"
            }

            Text{
                id:qnh2
                width:parent.width
                anchors.bottom: parent.bottom
                font.family: row.fontFamily
                font.pixelSize: row.fontSize()*2
                text:"BBB"
            }

            Text{
                id:qnh
                width:parent.width
                anchors.verticalCenter: parent.verticalCenter
                font.family: row.fontFamily
                font.pixelSize: row.fontSize()*2
                text:"CCC"
            }
        }

        Rectangle{
            id:rec3
            width:parent.childWidth()
            height:parent.height
            Text{
                id:t3
                width:parent.width
                height:20
                font.family: row.fontFamily
                font.pixelSize: row.fontSize()
                text: "   "
            }

            Text{
                id:pressure2
                width:parent.width
                anchors.bottom: parent.bottom
                font.family: row.fontFamily
                font.pixelSize: row.fontSize()*2
                text:"BBB"
            }

            Text{
                id:pressure
                width:parent.width
                anchors.verticalCenter: parent.verticalCenter
                font.family: row.fontFamily
                font.pixelSize: row.fontSize()*2
                text:"CCC"
            }
        }

    }





    Timer {
        id:watch
        interval:20000
        repeat: true
        running: false
        onTriggered: if (root.state!="") root.state=""
    }

    Connections {
        target:zmq
        onRecv: {
            if (socket==sock) {
                if (("timeout" in data) && data.timeout) {
                    if (state!="NODRV") state = "NODRV"
                    watch.restart()
                    return
                } // {'pressure':100, 'QFE':20, 'QNH':42}
                pressure.text=data.pressure.toFixed(0)
                qfe.text=data.QFE.toFixed(0)
                qnh.text=data.QNH.toFixed(0)
                pressure2.text=2*data.pressure
                if (state!="NORMAL") state="NORMAL"
                watch.restart()
            }
        }
     }
}

