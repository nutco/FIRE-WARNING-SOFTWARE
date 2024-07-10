const express = require('express');
var WebSocket = require('ws');

const { connectDatabase, addDatabase } = require('./database');
const { set } = require('mongoose');



const app = express();
var server = app.listen(4000, () => {
    console.log('server listening on port 4000');
});

var arduinoDeviceState = {
    isBtnTurnOnBuzzer: false,
    btnSystemOn: true,
    isGasSensor1: false,
    isGasSensor2: false,
    isBuzzerCry: false
};

var ws = new WebSocket.Server({ server });
var clients = [];

connectDatabase();

function getDateTimeNow() {
    const date = new Date(Date.now());

    // Tháng bắt đầu từ 0
    return `${date.getDate()}/${date.getMonth() + 1}/${date.getFullYear()} ${date.getHours()}:${date.getMinutes()}:${date.getSeconds()}`;
}

ws.on('connection', (socket, req) => {

    clients.push({
        socket,
        req
    });
    // var abc = JSON.stringify({
    //     "isBtnTurnOnBuzzer": true,
    //     "btnSystemOn": true,
    //     "isGasSensor1": false,
    //     "isGasSensor2": false,
    //     "isBuzzerCry": true,
    // })
    // clients[clients.length - 1].socket.send("123");

    receiveRequest(socket, // nhận tín hiệu client 
        (client, message) => { // nhận tín hiệu esp
            // chỉ có 1 esp 
        },
        (client, message) => { // nhận tín hiệu browser
            // console.log(`browser: ` + message.buttonState);
            // console.log(`browser: ` + message.buzzerState);
            // console.log(`browser: ` + message.motionSensorState);

            // setTimeout(function () { client.socket.send(abc); }, 5000);

            // client.socket.send(message);
            // client.socket.send(message.btnSystemOn);
            // client.socket.send(message.isGasSensor1);
            // client.socket.send(message.isGasSensor2);
            // client.socket.send(message.isBuzzerCry);
            // console.log(message);
            // broadcastSendRequest(client.socket, (client, message) => {
            //     var data = {
            //         command: message,
            //     };
            //     // console.log(message.toString());
            //     var json = JSON.stringify(data); // Chuyển đổi thành chuỗi

            //     client.socket.send(json);
            // // },
            // (client, message) => {
            //     client.socket.send(message);
            // }, message.toString());
            // client.socket.send(message);

        }, arduinoDeviceState);

    console.log(`connection`);

    socket.on('message', (message) => {
        receiveRequest(socket, receiveRequestESP, receiveRequestSharp, message);
    });

    socket.on('close', () => {

        clients = clients.filter((client) => {
            return client.socket != socket;
        });

        console.log(`${getDateTimeNow()} : disconnect`);
    });

    socket.on('error', () => {
        console.log(`${getDateTimeNow()} : error`);
    });
});


function broadcastSendRequest(socket, sendRequestESP, sendRequestSharp, message, exception = () => { }) {//gửi dữ liệu đến tất cả client còn lại
    clients.forEach((client) => {
        if (client.socket != socket) {
            const user_agent = client.req.headers['user-agent'];
            if (user_agent.includes('websocket-sharp')) {
                sendRequestSharp(client, message);
            }
            else if (user_agent.includes('arduino')) {
                sendRequestESP(client, message);
            }
            else {
                exception();
            }
        }
    });
}

function receiveRequestESP(client, message) {//nhận tín hiệu từ eps rồi gửi đến các client còn lại

    arduinoDeviceState = JSON.parse(message);

    console.log(`${getDateTimeNow()}: ${arduinoDeviceState.isBtnTurnOnBuzzer} ${arduinoDeviceState.btnSystemOn} ${arduinoDeviceState.isGasSensor1} ${arduinoDeviceState.isGasSensor2} ${arduinoDeviceState.isBuzzerCry}`);
    addDatabase(arduinoDeviceState);

    broadcastSendRequest(client.socket, (client, message) => { // tham so 2 gui toi esp
        // chỉ có một esp
    },
        (client, message) => { // app desktop
            if (message) {
                var text = JSON.stringify(message);
                client.socket.send(text);
            }
        },
        arduinoDeviceState);
}

function receiveRequestSharp(client, message) {//nhận tín hiệu từ trình duyệt rồi gửi đến các client còn lại

    // console.log(message.toString());

    broadcastSendRequest(client.socket, (client, message) => {
        var data = {
            command: message,
        };
        console.log(message.toString());

        var json = JSON.stringify(data); // Chuyển đổi thành chuỗi

        client.socket.send(json);
    },
        (client, message) => {
            // console.log(message.toString());
            // client.socket.send(message);
            var data = {
                command: message,
            };
            console.log(message.toString());

            var json = JSON.stringify(data); // Chuyển đổi thành chuỗi

            client.socket.send(json);
        }, message.toString());
}


function receiveRequest(socket, receiveRequestESP, receiveRequestSharp, message, exception = () => { }) {
    clients.forEach((client) => {
        if (client.socket == socket) {
            const user_agent = client.req.headers['user-agent'];

            if (user_agent.includes('websocket-sharp')) {
                receiveRequestSharp(client, message);
            }
            else if (user_agent.includes('arduino')) {
                receiveRequestESP(client, message);
            }
            else {
                exception();
            }

        }
    });
}