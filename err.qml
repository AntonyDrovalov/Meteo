import QtQuick 2.8
import QtQuick.Window 2.2

Window {
Rectangle {
	width: 360
	height: 360
	MouseArea {
		anchors.fill: parent
		onClicked: { Qt.quit();}
	}
	Text { 
		anchors.centerIn: parent
		text: "Error"
	}
}
}
