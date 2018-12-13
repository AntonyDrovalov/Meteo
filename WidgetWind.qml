import QtQuick 2.8

Rectangle {
    id:root
    width: 270
    height: 540
    radius: 1
    border.width: 1
    property int direction: 0
    property string sock: "wind"
    // default state with no data at all
    border.color: "#0a0ac8"//'#acacac'
    color:"#d2d2df"
    opacity: 0.4
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
        spacing: 5
        anchors.fill: parent
        anchors.margins: 0

        Rectangle {
            //width:parent.width*231/271
            //height: parent.height*211/541
            width: parent.width
            height: parent.height
            Canvas {
                x:0;y:0;z:2
                anchors.fill:parent
                id:graph
                property var data: JSON.parse("{}")
                onPaint: {
                    //console.log("paint")
                    var ctx = getContext("2d")
                    //ctx.fillStyle="#ffffff"
                    //ctx.fillRect(0,0,width,height);
                    ctx.clearRect(0,0,width,height);
                    var r = (width>height)?height-100:width-100
                    r /= 2
                    // Pie
                    if (("gist" in data) && ("min2" in data)) {
                        if (data.gist.min<=data.min2.angle) {
                            ctx.strokeStyle="#c8c8c8"
                            ctx.lineWidth=1
                            ctx.fillStyle="#e6e6e6"
                            ctx.beginPath()
                            ctx.moveTo(width/2,height/2)
                            //ctx.lineTo(width/2+r*Math.sin(data.gist.min*Math.PI/180),
                            //                              height/2-r*Math.cos(data.gist.min*Math.PI/180))
                            ctx.lineTo(width/2+r*Math.sin(data.gist.min),
                                                          height/2-r*Math.cos(data.gist.min))
                            ctx.moveTo(width/2,height/2)
                            //ctx.lineTo(width/2+r*Math.sin(data.gist.max*Math.PI/180),
                            //                              height/2-r*Math.cos(data.gist.max*Math.PI/180))
                            ctx.lineTo(width/2+r*Math.sin(data.gist.max),
                                                          height/2-r*Math.cos(data.gist.max))
                            //ctx.arc(width/2,height/2,r,(90-data.gist.max)*Math.PI/180,(data.gist.max-data.gist.min)*Math.PI/180,true)
                            ctx.arc(width/2,height/2,r,(90-data.gist.max),(data.gist.max-data.gist.min),true)
                            ctx.stroke()
                        }
                    }

                    //1
                    ctx.strokeStyle="#e6e6e6"
                    ctx.lineWidth=2
                    ctx.beginPath()
                    ctx.arc(width/2,height/2,r,0,2*Math.PI)
                    ctx.stroke()
                    //2
                    if ("min2" in data) {
                        //ctx.strokeStyle="#329632"
                        ctx.strokeStyle="#109010"
                        ctx.lineWidth=5
                        ctx.beginPath()
                        ctx.moveTo(width/2,height/2)
                        //ctx.lineTo(width/2+r*Math.sin(data.min2.angle*Math.PI/180),height/2-r*Math.cos(data.min2.angle*Math.PI/180))
                        ctx.lineTo(width/2+r*Math.sin(data.min2.angle),height/2-r*Math.cos(data.min2.angle))
                        ctx.stroke()
                    }

                    //0
                    ctx.strokeStyle="#c8c8c8"
                    ctx.lineWidth=1
                    ctx.beginPath()
                    ctx.moveTo(width/2,height/2-r)
                    ctx.lineTo(width/2,height/2+r)
                    ctx.moveTo(width/2-r,height/2)
                    ctx.lineTo(width/2+r,height/2)
                    ctx.stroke()

                    //MAX2
                    if ("min2" in data) {
                        ctx.strokeStyle="#c80000"
                        ctx.lineWidth=1
                        ctx.fillStyle="#c80000"
                        ctx.beginPath()
                        var dr=2
                        for(var f=0;f<4;f++) {
                            ctx.arc(width/2,height/2,r+dr*f,(90-data.min2.max.angle-5)*Math.PI/180,(90-data.min2.max.angle+5)*Math.PI/180)
                            //ctx.arc(width/2,height/2,r+dr*f,(90-data.min2.max.angle-5),(90-data.min2.max.angle+5))
                        }
                        ctx.stroke()
                    }

                    //3
                    if ("min10" in data) {
                        ctx.strokeStyle="#5a5a82"
                        ctx.lineWidth=1
                        ctx.fillStyle="#5a5a82"
                        ctx.beginPath()
                        //ctx.ellipse(width/2-3 + r*Math.sin(data.min10.angle*Math.PI/180),height-3 -r*Math.cos(data.min10.angle*Math.PI/180),7,7);
                        ctx.ellipse(width/2-3 + r*Math.sin(data.min10.angle),height-3 -r*Math.cos(data.min10.angle),7,7);
                        ctx.stroke()
                    }
                    //RW
                    ctx.strokeStyle=ctx.createPattern("#f0f0f0",Qt.Dense5Pattern)
                    ctx.lineWidth=5
                    ctx.beginPath()
                    //ctx.moveTo(width/2 + r*Math.sin(root.direction*Math.PI/180),height/2 - r*Math.cos(root.direction*Math.PI/180));
                    //ctx.lineTo(width/2 + r*Math.sin((root.direction+180)*Math.PI/180),height/2 -r*Math.cos((root.direction+180)*Math.PI/180));
                    ctx.moveTo(width/2 + r*Math.sin(root.direction),height/2 - r*Math.cos(root.direction));
                    ctx.lineTo(width/2 + r*Math.sin((root.direction+180)),height/2 -r*Math.cos((root.direction+180)));
                    ctx.stroke()


                }
            }
        }

    }
    Timer {
        id:watch
        interval:3500
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



                graph.data = data
                graph.requestPaint()



                if (state!="NORMAL") state="NORMAL"
                watch.restart()
            }
        }
     }
}

