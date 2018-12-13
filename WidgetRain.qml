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
    property string sock: "RAIN"


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
            id:rain24
            width:parent.width
            height: parent.childHeight()
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "Загальна кількість, мм"
            }

            Text {
                id: rain_full
                anchors.right: parent.right
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "0.00"
            }

        }

        Rectangle {
            id:start
            width:parent.width
            height: parent.childHeight()
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "Початок"
            }

            Text {
                id:timeStartRain
                height:parent.height
                anchors.right: parent.right
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "0.00"
            }

        }

        Rectangle {
            id:duration
            width:parent.width
            height: parent.childHeight()
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "Тривалість, хв"
            }

            Text {
                id:timeValue
                height:parent.height
                anchors.right: parent.right
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "0.00"
            }

        }
        Rectangle {
            id:average
            width:parent.width
            height: parent.childHeight()
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "Середня інтенсивність, мм/хв"
            }

            Text {
                id:aver_intensity
                height:parent.height
                anchors.right: parent.right
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "0.00"
            }


        }

        Rectangle {
            id:max
            width:parent.width
            height: parent.childHeight()
            Text {
                width:20
                height:parent.height
                font.family: col.fontFamily
                font.pixelSize: col.fontSize()
                text: "Макс.інтенсивність, мм/хв"
            }

            Text {
                id:max_intensity
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
                rain_full.text=data.Rain_full.toFixed(2)
                timeStartRain.text=data.TimeStartRain
                timeValue.text=data.TimeValue.toFixed(2)
                aver_intensity.text=data.Aver_intensity.toFixed(2)
                max_intensity.text=data.Max_intensity.toFixed(2)
                if (state!="NORMAL") state="NORMAL"
                watch.restart()
            }
        }
     }


}


