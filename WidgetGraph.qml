import QtQuick 2.8
import QtCharts 2.2


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
    property variant pressures: []


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

   ChartView {
        id: chartView
        //title: "Graph"
        height: parent.height / 4 * 3
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        antialiasing: true



        ValueAxis{
            id: valueAxisY
            min: 800
            max: 1020
            titleText: "Rainfall [mm]"
        }

        ValueAxis {
            id: valueAxisX
            //visible: false
            min: 0
            max: 3*60*60*1000
        }



        LineSeries {
            id: line
            axisX: valueAxisX
            axisY: valueAxisY
            name: "line"
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
        property variant now: 0
        function filtr(el){
            return now-el[1]<3*60*60*1000
            //return now-el[1]<30000
        }

        function add(el){
            line.append(3*60*60*1000-(now-el[1]),el[0])
        }

        onRecv: {
            if (socket==sock) {
                if (("timeout" in data) && data.timeout) {
                    if (state!="NODRV") state = "NODRV"
                    watch.restart()
                    return
                } // {'pressure':100, 'QFE':20, 'QNH':42}
                //pressure.text=data.pressure.toFixed(0)
                console.log(data.pressure)
                console.log(new Date(data.time))
                now=Date.now()
                pressures.push([data.pressure,new Date(data.time)])
                pressures=pressures.filter(filtr)
                line.clear()
                pressures.forEach(add)
                console.log(pressures.length)
                if (state!="NORMAL") state="NORMAL"
                watch.restart()
            }
        }
     }
}

