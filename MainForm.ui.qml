import QtQuick 2.8

Rectangle {
    width: 360
    height: 360

    WidgetWind {anchors.fill: parent}
    //WidgetTemp {anchors.fill: parent}
    //Widget1{anchors.fill: parent}
    //WidgetRain{anchors.fill: parent}
    //WidgetBar{anchors.fill: parent}
   // WidgetGraph{anchors.fill: parent}
    /*Column {
        id: columnMain
        anchors.fill: parent

        Row {
            id: rowMain
            width: parent.width
            height: parent.height*0.7
             WidgetWind {
                width: parent.width*0.3
                height: parent.height
                direction: cfg.direction
            }
            Column {
                id: column1
                width: parent.width*0.4
                height: parent.height
                //WidgetCurrentTime { anchors.horizontalCenter: parent.horizontalCenter}
                //WidgetVNGO {width: parent.width}
                //WidgetBar { width: parent.width}
                WidgetTemp {width: parent.width}
                //WidgetCurrentWeather {width: parent.width}
                //WidgetTrend {width: parent.width}
                //WidgetAddInfo {width: parent.width}
            }

            WidgetWind {
                width: parent.width*0.3
                height: parent.height
                direction: cfg.direction_R
            }

        }

        Row {
            id: rowVisibility
            height: parent.height*0.15
            WidgetVisibility {
                anchors.fill: parent
            }
        }
        Row {
            id: rowMetar
            height: parent.height*0.15
            WidgetMetar {
                anchors.fill: parent
            }
        }
    }*/
}
