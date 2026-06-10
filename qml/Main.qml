import QtQuick
import QtQuick.Controls
import LeRobot 1.0


ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "LeRobot QML"
    property bool connected: false
    property bool started: false
    property string mode: "home"
    property bool realtimeRead: false
    RobotBackend{
        id:robot;
    }

    Column{
        visible: mode === "home"
        anchors.centerIn: parent
        spacing:16

        Text{
            text: "Lerobot Control"
            font.pixelSize:28
        }
        Button{
            text:"Manual Control"
            onClicked: mode = "manual"
        }
        Button{
            text:"Auto Learn"
            onClicked:mode = "learn"
        }
    }

    Column{
        anchors.centerIn:parent;
        spacing:12
        visible:mode === "manual"
        Text{
            text:"Lerobot Servo Control"
            font.pixelSize:50
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
            validator:IntValidator{
                bottom:1
                top:253
            }
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
                onClicked:{
                    let result = robot.connectSerival(portInput.text,1000000)
                    statusText.text = result
                    connected = (result === "UART OK")
                }
            }
            Switch{
                id:torqueSwitch
                enabled: connected
                text:checked ? "Torque ON" : "Torque OFF"
                onClicked:{
                    statusText.text = robot.setTorque(Number(idInput.text),torqueSwitch.checked)
                }
            }
            Button{
                enabled:connected
                text:"Move"
                width:220
                onClicked:{
                    statusText.text = robot.moveServo(Number(idInput.text),Math.round(positionSlider.value))
                }
            }
            Button{
                enabled:connected
                text:"Read"
                width:220
                onClicked:{
                    statusText.text = robot.ReadPos(Number(idInput.text))
                }

            }
        }
            Text{
                id:statusText
                text:"Ready"
                font.pixelSize:16;
            }
        }

        Column
        {
            visible:mode === "learn"
            anchors.centerIn: parent
            spacing:12

            Text{
                text:"Auto Learn"
                font.pixelSize:50
            }
            Button{
                text:"Start Learn"
                onClicked:{
                    let result = robot.startLearn()
                    learnStatusText.text = result
                    started = (result === "Auto learn started")
                }
            }
            Button{
                enabled:started
                text:"Stop Learn"
                onClicked:{
                    learnStatusText.text = robot.stopLearn();
                }
            }
            Text{
                id:learnStatusText
                text:"Status:"
                font.pixelSize:30
            }
            Button{
                width:100
                text:"Check Pos"
                onClicked:{
                    mode = "check"
                }
            }
        }
        //返回
        Item{
            visible:mode === "learn"
            anchors.fill: parent
            Button{
                text:"Back"
                anchors.left:parent.left
                anchors.top:parent.top
                anchors.margins:16
                onClicked: mode = "home"
            }
        }

        Item{
            visible:mode === "manual"
            anchors.fill: parent
            Button{
            text:"Back"
            anchors.left:parent.left
            anchors.top:parent.top
            anchors.leftMargin: 16
            anchors.topMargin: 16
            onClicked: mode = "home"
        }
        }
        Item{
            visible:mode === "check"
            anchors.fill: parent
            Button{
            text:"Back"
            anchors.left:parent.left
            anchors.top:parent.top
            anchors.leftMargin: 16
            anchors.topMargin: 16
            onClicked: mode = "learn"
        }
        }

        Item{
            visible:mode === "check"
            Timer{
                interval:1
                running:mode === "check" && connected
                repeat:true
                onTriggered:{
                    pos1Text.text = robot.ReadPos(1)
                    pos2Text.text = robot.ReadPos(2)
                    pos3Text.text = robot.ReadPos(3)
                    pos4Text.text = robot.ReadPos(4)
                    pos5Text.text = robot.ReadPos(5)
                    pos6Text.text = robot.ReadPos(6)
                }
            }
            
            anchors.fill: parent
            Text{
                id:checkpos
                text:"Check Pos"
                font.pixelSize:50
                anchors.top:parent.top
                anchors.horizontalCenter:parent.horizontalCenter
                anchors.topMargin:20
            }
            Column{    
            id:posColumn
            anchors.left:parent.left
            anchors.top:parent.top
            anchors.leftMargin:150
            anchors.topMargin:140
            spacing:8
            Text { id: pos1Text; text: "ID1: --";font.pixelSize:30 }
            Text { id: pos2Text; text: "ID2: --";font.pixelSize:30 }
            Text { id: pos3Text; text: "ID3: --";font.pixelSize:30 }
            Text { id: pos4Text; text: "ID4: --";font.pixelSize:30 }
            Text { id: pos5Text; text: "ID5: --";font.pixelSize:30 }
            Text { id: pos6Text; text: "ID6: --";font.pixelSize:30 }

            }
            }

    }

