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
    property string sock: "PRECCalc"


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

    Column {
        id:col
        spacing: 5
        anchors.margins: spacing
        anchors.fill: parent
        property string fontFamily: "Liberation Mono"
        function childHeight() {
            return height / children.length - spacing
        }
        function fontSize() {
            var hSize = childHeight()
            var wSize = width / 20
            return hSize>wSize ? wSize: hSize
        }

        Rectangle {
            id:prec24
            width:parent.width
            height: parent.childHeight()
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "Поточні 24 години,мм"
            }

            Text {
                id:prec24h
                anchors.right: parent.right
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "0.00"
            }

        }

        Rectangle {
            id: prec12
            width:parent.width
            height: parent.childHeight()
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "Поточні 12 годин,мм"
            }

            Text {
                id: prec12h
                height:parent.height
                anchors.right: parent.right
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "0.00"
            }

        }

        Rectangle {
            id:prec1
            width:parent.width
            height: parent.childHeight()
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "Поточна година,мм"
            }

            Text {
                id:prec1h
                height:parent.height
                anchors.right: parent.right
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "0.00"
            }

        }
        Rectangle {
            id:prec10
            width:parent.width
            height: parent.childHeight()
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "10 хв, мм"
            }

            Text {
                id:prec10_full
                height:parent.height
                anchors.right: parent.right
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "0.00"
            }


        }

        Rectangle {
            id:prec_1
            width:parent.width
            height: parent.childHeight()
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "1 хв, мм"
            }

            Text {
                id:prec_1min
                height:parent.height
                anchors.right: parent.right
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "0.00"
            }
        }



    }

    Timer {
        id:watch
        interval:5000
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
                }
                prec24h.text=data.Prec24h.toFixed(2)
                prec12h.text=data.Prec12h.toFixed(2)
                prec1h.text=data.Prec1h.toFixed(2)
                prec10_full.text=data.Prec10_full.toFixed(2)
                prec_1min.text=data.Prec_1min.toFixed(2)
                if (state!="NORMAL") state="NORMAL"
                watch.restart()
            }
        }
     }


}


