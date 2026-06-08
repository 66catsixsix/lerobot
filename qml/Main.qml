import QtQuick
import QtQuick.Controls

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "LeRobot QML"

    Column{
        anchors.centerIn:parent;
        spacing:12

        Text{
            text:"Lerobot Servo Control"
            font.pixelSize:24
        }
        TextField{
            id:portInput
            width:220
            text:"/dev/ttyACM0"
            placeholderText:"Serial Port"
        }
        TextField{
            id:idInput
            width:220
            text:"1"
            placeholderText:"Servo ID"
        }
        Slider{
            id:positionSlider
            width:260
            from:0
            to:4095 
            value:1500
        }
        Text{
            text:"Position:"+ Math.round(positionSlider.value)
            font.pixelSize:18
        }
        Row{
            spacing:10

            Button{
                text:"Connect"
                onClicked:statusText.text = robot.connectSerial(portInput.text, 1000000)
            }
            Button{
                text:"Torque On"
                onClicked:statusText.text = robot.setTorque(Number(idInput.text), true)
            }
            Button{
                text:"Torque Off"
                onClicked:statusText.text = robot.setTorque(Number(idInput.text), false)
            }
            Button{
                text:"Move"
                width:220
                onClicked:{
                    statusText.text = robot.moveServo(Number(idInput.text), Math.round(positionSlider.value))
                }
            }
            Button{
                text:"Read Position"
                width:220
                onClicked:statusText.text = robot.readPosition(Number(idInput.text))
            }
            Text{
                id:statusText
                text:"Ready"
                font.pixelSize:16;
            }
        }

    }
}
