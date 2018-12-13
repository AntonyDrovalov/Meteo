import QtQuick 2.8

//Rectangle {
  //  width: 100
    //height: 62
//}


Rectangle {
    id:root
    width: 360
    height: 260
    radius: 3
    border.width: 1
    // default state with no data at all
    border.color: "#0a0ac8"//'#acacac'
    //color:"#d2d2df"
    color:"green"
    opacity: 0.4
    property string sock: "temp"


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
            id:temp
            width:parent.width
            height: parent.childHeight()
            //Item{width:1;height:parent.height}
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "Температура повітря"
            }

            Text {
                id:temperature
                anchors.right: parent.right
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "0.00"
            }

            //Item{width:1;height:parent.height}
        }

        Rectangle {
            id:hum
            width:parent.width
            height: parent.childHeight()
            //Item{width:1;height:parent.height}
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "Вологість"
            }

            Text {
                id:humidity
                height:parent.height
                anchors.right: parent.right
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "77 %"
            }

            //Item{width:1;height:parent.height}
        }

        Rectangle {
            id:point
            width:parent.width
            height: parent.childHeight()
            //Item{width:1;height:parent.height}
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "Точка роси"
            }

            Text {
                id:dewPoint
                height:parent.height
                anchors.right: parent.right
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "23 "
            }

            //Item{width:1;height:parent.height}
        }
        Rectangle {
            id:max
            width:parent.width
            height: parent.childHeight()
            //Item{width:1;height:parent.height}
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "Макс"
            }

            Text {
                id:maxT
                height:parent.height
                anchors.right: parent.right
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "500°C"
            }

            Text {
                id:timeMaxT
                //width:20
                anchors.horizontalCenter:parent.horizontalCenter
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "00:00:00"
            }

            //Item{width:1;height:parent.height}
        }

        Rectangle {
            id:min
            width:parent.width
            height: parent.childHeight()
            //Item{width:1;height:parent.height}
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "Мін"
            }

            Text {
                id:minT
                height:parent.height
                anchors.right: parent.right
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "100°C"
            }

            Text {
                id:timeMinT
                //width:20
                anchors.horizontalCenter:parent.horizontalCenter
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "00:00:00"
            }

            //Item{width:1;height:parent.height}
        }

        Rectangle {
            id:sred
            width:parent.width
            height: parent.childHeight()
            //Item{width:1;height:parent.height}
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "Середньодобова"
            }

            Text {
                id:t24
                height:parent.height
                anchors.right: parent.right
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "300°C"
            }

            //Item{width:1;height:parent.height}
        }

    }

    /*Row {
        spacing: 5
        anchors.margins: spacing
        anchors.fill: parent
        Column {
            id:col
           spacing: 5
           width:(parent.width-parent.spacing*4)/3
           height: parent.height
           Item {width:parent.width;height:1}
           Text {
               width: parent.width
               height: parent.height-qfe.height-parent.spacing*3
               font.pixelSize: 10
               font.family: "Liberation Sans"
               text: "QFE"
           }
           Text {
               id:qfe
               width: parent.width
               height: 20
               font.pixelSize: 20
               font.family: "Liberation Sans"
               text:"0.0"
           }
           Item {width:parent.width;height:1}
       }
       Column {
           width:col.width
           height: parent.height
           spacing: col.spacing
           Item {width:parent.width;height:1}
           Text {
               width: parent.width
               height: parent.height-qnh.height-parent.spacing*3
               font.pixelSize: 10
               font.family: "Liberation Sans"
               text: "QNH"
           }
           Text {
               id:qnh
               width: parent.width
               height: 20
               font.pixelSize: 20
               font.family: "Liberation Sans"
               text:"0.0"
           }
           Item {width:parent.width;height:1}
       }
       Item {
           width:col.width
           height:parent.height
           Text {
               id:bar
               y:parent.height*3/4
               font.pixelSize: 10
               font.family: "Liberation Sans"
               text:"0.0"
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
                bar.text=data.pressure.toFixed(2)
                qfe.text=data.QFE.toFixed(2)
                qnh.text=data.QNH.toFixed(2)
                if (state!="NORMAL") state="NORMAL"
                watch.restart()
            }
        }
     }*/
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
                temperature.text=data.Temperature.toFixed(2)
                humidity.text=data.Humidity.toFixed(2)
                dewPoint.text=data.Dewpoint.toFixed(2)
                maxT.text=data.MaxT.toFixed(2)
                timeMaxT.text=data.TimeMaxT
                minT.text=data.MinT.toFixed(2)
                timeMinT.text=data.TimeMinT
                t24.text=data.T24.toFixed(2)
                if (state!="NORMAL") state="NORMAL"
                watch.restart()
            }
        }
     }

}


