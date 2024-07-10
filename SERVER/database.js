const mongoose = require('mongoose');

const moment = require('moment');

async function connectDatabase() {

    try {
        await mongoose.connect('mongodb://127.0.0.1:27017/an_ninh_gia_dinh');
        console.log('connect database');
    }
    catch (ex) {
        console.log('error connecting database');
    }

}

function ArduinoDeviceState() {
    this.create = function () {
        var arduino_device_state_schema = new mongoose.Schema({
            isBtnTurnOnBuzzer: Boolean,
            isGasSensor1: Boolean,
            isGasSensor2: Boolean,
            isBuzzerCry: Boolean,
            btnSystemOn: Boolean,
            time: { type: String, default: moment().format("YYYY-MM-DD HH:mm:ss") }
        }, {
            timestamps: false,
        });

        return mongoose.model('arduino_device_state', arduino_device_state_schema);
    }
}

ArduinoDeviceStateModel = new ArduinoDeviceState().create();

async function addDatabase({ isBtnTurnOnBuzzer, isGasSensor1, isGasSensor2, isBuzzerCry, btnSystemOn }) {
    const arduinoDeviceState = new ArduinoDeviceStateModel({
        isBtnTurnOnBuzzer,
        isGasSensor1,
        isGasSensor2,
        isBuzzerCry,
        btnSystemOn
    });

    arduinoDeviceState.save().then(() => {
        console.log('Thêm Dữ Liệu Thành Công');
    })
        .catch(() => {
            console.log('Thêm Dữ Liệu Thất Bại');
        });
}

module.exports = {
    connectDatabase,
    addDatabase,
};