import QtQuick 2.8
import QtQuick.Window 2.2
import Qt.labs.settings 1.0
import org.easylogging.qml 1.3

Window {
    id:window
    visible: true
    property var cfg: 0
    x:0
    y:0
    width:Screen.width
    height: Screen.height
    Settings {
        category: "mainWindow"
        property alias x: window.x
        property alias y: window.y
        property alias width: window.width
        property alias height: window.height
    }
    MainForm {
        id:form
        anchors.fill: parent
        Connections {
            target:zmq
            onRecv: { console.log(socket,JSON.stringify(data))
            }
            onOkConfigure:console.log("zmq configured")
            onErrConfigure:console.log("zmq not configured")
        }
    }
    Component.onCompleted: {
        cfg = JSON.parse(config.config)
        if (cfg.zmq) { zmq.configure(cfg.zmq)
        }
    }
}
