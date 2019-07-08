const Addon = require("bindings")("neoRAD_IO2");
if(Addon)
{
    window.Addon = Addon;
}
const emitStream = require('emit-stream');
const through = require('through');
const EventEmitter = require('events');
const neoRADIO2_RunStates = {
    'neoRADIO2state_Disconnected' : 0,
    'neoRADIO2state_ConnectedWaitForAppStart' : 1,
    'neoRADIO2state_ConnectedWaitIdentResponse' : 2,
    'neoRADIO2state_ConnectedWaitReadSettings' : 3,
    'neoRADIO2state_ConnectedWaitWriteSettings' : 4,
    'neoRADIO2state_Connected' : 5,
};
Object.freeze(neoRADIO2_RunStates);
let startFailure = 0;

function NativeConstructor(){
    let myPath;
    let todayDate = new Date();
    let Month = todayDate.getUTCMonth() + 1;
    let Day = todayDate.getUTCDate();
    let Hours = todayDate.getHours();
    let Minutes = todayDate.getMinutes();
    let DateString = `${Month}_${Day}_${Hours}_${Minutes}`;

    myPath = path.join(mypath, `IntrepidCS\/neoRAD-IO2\/${DateString}.csv`);

    let writeStream = 0;

    let emitter = new EventEmitter();
    if(typeof Addon == 'undefined')
    {
        change_status_text("Missing SDK, Native module can't be loaded");
        return;
    }
    let worker = new Addon.DataWorker(
        (event, value) => {
            emitter.emit(event, value);
        },
        () => {
            emitter.emit("close");
        },
        error => {
            emitter.emit("error", error);
        },
        {name: "Data Stream"}
    );

    let returnObj = {};
    let EmitEvents = {};
    let device_found_once = 0;
    let send_settings_once = 0;
    let start_get_data = 0;
    let deviceSave = 0;
    const temperatureString = "d";
    let startTime = 0;
    let autoSave = 0;
    returnObj.neoRADIO_status = 0;

    if(settings.get("tempUnit"))
    {
        isFahrenheit = settings.get("tempUnit");
    }

    if(settings.get("save") && settings.get("save") == "1")
    {
        autoSave = 1;
    }

    for(let i = 1; i < 9; i++)
    {
        for(let j = 1; j < 9; j++)
        {
            PlotSeries.data[(j - 1) + (i - 1) * 8] = [];
            PlotHistory.data[(j - 1) + (i - 1) * 8] = [];
            PlotData.data[(j - 1) + (i - 1) * 8] = [];
        }
    }

    EmitEvents.from = emitter;
    EmitEvents.from.stream = () => {
        return emitStream(EmitEvents.from).pipe(
            through(function (data) {
                if (data[0] == "close")
                {
                    this.end();
                }
                else
                {
                    this.queue(data);
                }
            }));
    };

    EmitEvents.to = {
        emit: (name, data) => {
            worker.sendToAddon(name, data);
        },
        stream: (name, end) => {
            return through(function write(data) {
                    if (Array.isArray(data))
                    {
                        if (data[0] == "close")
                        {
                            this.end();
                        }
                        else
                        {
                            EmitEvents.to.emit(data[0], data[1]);
                        }
                    }
                    else
                    {
                        EmitEvents.to.emit(name, data);
                    }
                },
                end);
        }
    };

    EmitEvents.from.on('error_msg', msg => {
        switch (msg) {
            case "100":
                worker.closeInput();
                terminateAll();
                change_status_text("Device failed to connect, please check physical connection");
                break;
            case "101":
                worker.closeInput();
                terminateAll();
                change_status_text("No devices found, please try again");
                break;
            case "102":
                change_status_text("connection_state = Connected Wait Ident Response. Please try reconnecting");
                break;
            case "103":
                change_status_text('Connection timed out');
                break;
            case "200":
                change_status_text('Write settings failed');
                break;
            case "300":
                change_status_text('Store calibration data failed');
                $("#storeButton").prop('disabled',false);
                break;
        }
    });

    EmitEvents.from.on('killall', () => {
        console.log("connection terminated");
        terminateAll();
    });

    EmitEvents.from.on('paused', () => {
        console.log("paused");
    });

    EmitEvents.from.on('device_found', device_found => {
        returnObj.device_found = JSON.parse(device_found);
        // console.log(returnObj.device_found);
        if(device_found != "null" && device_found_once === 0)
        {
            if(returnObj.device_found.State != neoRADIO2_RunStates.neoRADIO2state_Connected)
            {
                returnObj.close();
                $('#device_connect').prop('disabled',false);
                startFailure++;
                if(startFailure > 2)
                {
                    change_status_text("Try reconnecting the cable");
                    startFailure = 0;
                }

                returnObj.checkOnlineDevices();
            }
            showConnected();
            readSettings(returnObj.device_found);
            device_found_once = 1;
            returnObj.neoRADIO_status = 1;
            $('#device_connect').prop('disabled',false);
        }
    });

    EmitEvents.from.on('data_stream', data_stream => {
        startDataStream();
        if(writeStream === 0)
        {
            if(autoSave)
            {
                writeStream = fs.createWriteStream( myPath, {"flags":"a"});
            }
            else
            {
                writeStream = fs.createWriteStream( path.join(mypath, `IntrepidCS\/neoRAD-IO2\/auto.csv`), {"flags":"w"});
            }
        }
        $('#device_startlog').prop('disabled',false);
        if(start_get_data)
        {
            processRawData(JSON.parse(data_stream));
        }
    });

    EmitEvents.from.on('settings_reply_count', settings_reply_count => {
        change_status_text(`Saving in progress ${((parseInt(settings_reply_count) + 1) * 12.5).toFixed(0)}%`);
    });

    EmitEvents.from.on('settings_reply', settings_reply => {
        returnObj.device_found = JSON.parse(settings_reply);
        // console.log(returnObj.device_found);
        reloadSettings(returnObj.device_found);
        $("html,body").removeClass('wait');
        $('.device_save').prop('disabled',false);
        $('.device_save_canhub').prop('disabled',false);
    });

    EmitEvents.from.on('cal_read', data_stream => {
        data_stream = JSON.parse(data_stream);
        $("#readButton").prop('disabled',false);
        if(data_stream)
        {
            $('.cal_table_tc_tables').html('');

            let data_length = Object.keys(data_stream['data']).length;
            for(let i = 0; i < data_length; i++)
            {
                if(data_stream['data'][i]['calpoints'] == "invalid")
                {
                    change_status_text('Calibration data invalid, please Clear data first');
                    return;
                }
            }

            switch (data_stream["type"]) {
                case 0:
                    $('.cal_table_tc_tables').append(append_cal_table(data_stream));
                    $('.cal_table_').removeClass("hidden");
                    $('.interactive_wrapper').removeClass("hidden");
                    break;
                case 3:
                    $('.cal_table_tc_tables').append(append_cal_table(data_stream));
                    let AIN_select = $('.cal_ain_range').find(`.neorad_ain_sample_input:checked`).val();
                    let AIN_range = "";
                    if(AIN_select == "low")
                    {
                        AIN_range = $('.cal_ain_range').find('.ain_low_select option:selected').val();
                    }
                    else if(AIN_select == "high")
                    {
                        AIN_range = $('.cal_ain_range').find('.ain_high_select option:selected').val();
                    }
                    $(`#cal_table_${AIN_range}`).removeClass("hidden");
                    $('.interactive_wrapper').removeClass("hidden");
                    break;
                case 4:
                    $('.cal_table_tc_tables').append(append_cal_table_aout(data_stream));
                    $('.cal_table_').removeClass("hidden");
                    $('.interactive_wrapper').addClass("hidden");
                    break;
                default:
                    break;
            }

            difference();
        }
        else
        {
            change_status_text('Read calibration data failed');
        }
    });

    EmitEvents.from.on('cal_settings', data_stream => {
        if(data_stream && data_stream == "OK")
        {
            $("html,body").removeClass('wait');
            change_status_text('Calibration stored');
            $("#storeButton").prop('disabled',false);
        }
        else if(data_stream && data_stream != "OK")
        {
            change_status_text(`Storing in progress ${((parseFloat(data_stream) + 1) * 16.6).toFixed(0)}%`);
        }
    });

    EmitEvents.from.on('cal_inter', data_stream => {
        data_stream = JSON.parse(data_stream);
        $(".calibration_bank").text(data_stream['bank'] + 1);
        if(data_stream['tempData'] !== -1000 && data_stream['tempData'] != null)
        {
            if($(document.activeElement).attr('data-bank') != (data_stream['bank'] + 1))
            {
                return;
            }

            switch (data_stream['type']) {
                case 0:
                    $(document.activeElement).val(parseFloat(data_stream['tempData'].toFixed(2)));
                    break;
                case 1:
                    break;
                case 2:
                    break;
                case 3:
                    $(document.activeElement).val(parseFloat(data_stream['tempData'].toFixed(4)));
                    break;
                case 4:
                    break;
                case 5:
                    break;
                default:
                    $(document.activeElement).val(parseFloat(data_stream['tempData'].toFixed(2)));
                    break;
            }
        }
    });

    EmitEvents.from.on('cal_clear', data_stream => {
        if(data_stream && data_stream == "done")
        {
            change_status_text('Calibration data cleared');
        }
        else
        {
            change_status_text('Clear calibration data failed');
        }
        $("#resetCalButton").prop('disabled',false).removeClass("disabled");
    });

    returnObj.close = () => {
        worker.closeInput();
        terminateAll();
    };

    returnObj.pauseData = () => {
        returnObj.neoRADIO_status = 1;
        EmitEvents.to.emit(0, "0");
    };

    returnObj.checkOnlineDevices = () => {
        if(device_found_once === 0)
        {
            EmitEvents.to.emit(1, 1);
        }
    };

    returnObj.startDataPush = () => {
        EmitEvents.to.emit(3, 3);
        returnObj.neoRADIO_status = 2;
        if(startTime === 0)
        {
            let todayDate = new Date();
            let Hours = todayDate.getHours();
            let Minutes = todayDate.getMinutes();
            if(Minutes < 10)
            {
                Minutes = `0${Minutes}`;
            }
            startTime = `${Hours}.${Minutes}`;
        }
    };

    returnObj.SetPwrRly = function(that){
        if(returnObj.neoRADIO_status == 2)
        {
            EmitEvents.to.emit(0, "0");
        }

        let type = parseInt($(that).attr('data-type'));
        let deviceID = $(that).attr('data-number');
        let ExtraArray = [];
        let bString = "";

        $.each($(that).find('.input_selector_value'),function (i, el) {
            bString += $(el).val();
        });

        ExtraArray[0] = [];
        ExtraArray[1] = bString.split("");
        ExtraArray[1] = ExtraArray[1].reverse();
        bString = ExtraArray[1].join("");
        ExtraArray[1] = bString;

        /*
        ExAr[0] = Original State
        ExAr[1] = Current State
        */
        ExtraArray[1] = parseInt(ExtraArray[1],2);
        ExtraArray[0] = parseInt(returnObj.device_found['PWRRLY_STATUS'][parseInt(deviceID) - 1]);

        let settingsHere = {
            deviceType: type,
            deviceLink: parseInt(deviceID) - 1,
            extraSettings: ExtraArray
        };

        let JSONstr = JSON.stringify(settingsHere);
        EmitEvents.to.emit(9, JSONstr);
    };

    returnObj.SetAoutValue = function(that, bank){
        let deviceID = $(that).attr('data-number');
        let ExtraArray = [0, 0, 0];
        let channel1 = parseFloat($(that).find(`.device_channel${bank} .neorad_aout_input_1`).val());
        let channel2 = parseFloat($(that).find(`.device_channel${bank} .neorad_aout_input_2`).val());
        let channel3 = parseFloat($(that).find(`.device_channel${bank} .neorad_aout_input_3`).val());

        if(channel1 > 0 && channel1 <= 5)
        {
            ExtraArray[0] = parseInt(channel1 * 13107);
        }
        if(channel2 > 0 && channel2 <= 5)
        {
            ExtraArray[1] = parseInt(channel2 * 13107);
        }
        if(channel3 > 0 && channel3 <= 5)
        {
            ExtraArray[2] = parseInt(channel3 * 13107);
        }

        let settingsHere = {
            bank: parseInt(bank),
            deviceLink: parseInt(deviceID) - 1,
            extraSettings: ExtraArray
        };

        let JSONstr = JSON.stringify(settingsHere);
        EmitEvents.to.emit(10, JSONstr);
    };

    returnObj.gatherData = function(that){
        let type = parseInt($(that).attr('data-type'));
        let deviceID = $(that).attr('data-number');
        let iloop = $(that).find('.table1 .ma_device_channel_head').length;

        for (let i = 1; i <= iloop; i++)
        {
            let check_stat = $(that).find(`.device_channel${i} .ma_device_channel_status`).prev().prop('checked');
            let sample_stat = $(that).find(`.device_channel${i} .ma_device_channel_pullrate`).val();
            sample_stat = parseInt(sample_stat);

            let CanMode = $(that).find(`.device_channel${i} .ma_device_can_mode`).val();
            let CanMsgType = $(that).find(`.device_channel${i} .ma_device_can_type`).val();
            let CanMsgTypeID = $(that).find(`.device_channel${i} .ma_device_can_id_type`).val();
            let CanMsgToSend;
            if(!CanMsgType)
            {
                CanMsgType = 1;
            }

            if(CanMsgType == 1 && CanMsgTypeID == 3)
            {
                CanMsgToSend = 0;
            }
            if(CanMsgType == 1 && CanMsgTypeID == 4)
            {
                CanMsgToSend = 1;
            }
            if(CanMsgType == 2 && CanMsgTypeID == 3)
            {
                CanMsgToSend = 2;
            }
            if(CanMsgType == 2 && CanMsgTypeID == 4)
            {
                CanMsgToSend = 3;
            }
            let CanId = $(that).find(`.device_channel${i} .ma_device_can_id`).val();
            let CanLocation = $(that).find(`.device_channel${i} .ma_device_can_byte`).val();

            if(CanMode == "1")
            {
                CanMsgToSend = 0xFF;
            }

            if(type == "2" && i !== 1)
            {
                CanId = 0;
                CanMsgToSend = 0xFF;
                CanLocation = 0;
            }

            let tag_name = $(that).find(`.device_channel${i} .ma_device_channel_tag`).val();
            let tag_json = [];
            let tag_name_length = tag_name.length;
            for(let j = 0; j < tag_name_length; j++)
            {
                tag_json[j] = tag_name.charCodeAt(j);
            }

            if(sample_stat < 10 || !sample_stat){
                sample_stat = 10;
            }

            if(sample_stat > 600000){
                sample_stat = 600000;
            }

            // ExtraArrau
            let ExtraArray = [];

            switch (type) {
                case 0:
                    if(check_stat)
                    {
                        check_stat = 1;
                    }
                    else
                    {
                        check_stat = 0;
                    }
                    break;
                case 1:
                    break;
                case 2:
                    check_stat = 1;
                    break;
                case 3:
                    if(check_stat)
                    {
                        let AIN_select = $(that).find(`.device_channel${i} .neorad_ain_sample_input:checked`).val();
                        if(AIN_select == "low")
                        {
                            check_stat = $(that).find(`.device_channel${i} .ain_low_select`).val();
                        }
                        else if(AIN_select == "high")
                        {
                            check_stat = $(that).find(`.device_channel${i} .ain_high_select`).val();
                        }
                    }
                    else
                    {
                        check_stat = 0;
                    }
                    break;
                case 4:
                    check_stat = 1;
                    let check_stat1 = $(that).find(`.device_channel${i} .device_check_status1`).prop('checked');
                    let check_stat2 = $(that).find(`.device_channel${i} .device_check_status2`).prop('checked');
                    let check_stat3 = $(that).find(`.device_channel${i} .device_check_status3`).prop('checked');
                    ExtraArray = [check_stat1 ? 1:0, check_stat2 ? 1:0, check_stat3 ? 1:0];
                    break;
                case 5:
                    break;
                default:
                    break;
            }

            sendSettings(i, check_stat, sample_stat, type, deviceID, tag_json, CanId, CanMsgToSend, CanLocation, ExtraArray);
        }

        if(type == "5")
        {
            let settingsHere = {
                bank: 0x01,
                enables: 0x01,
                reportRate: 100,
                deviceType: 5,
                deviceLink: parseInt(deviceID),
                tagName:[],
                CanId: 0,
                CanMsgType: 0,
                CanLocation: 0,
                extraSettings: [[0,1],[2,3],[4,5]]
            };

            let JSONstr = JSON.stringify(settingsHere);
            EmitEvents.to.emit(4, JSONstr);

            send_settings_once = 1;
            deviceSave = deviceID - 1;
        }
    };

    returnObj.setDefault = function (that)
    {
        $(that).find('.ma_device_channel_tag').val("");
        $(that).find('.ma_device_channel_status').prev().prop('checked',true);
        $(that).find('.ma_device_channel_pullrate').val("100");
        $(that).find('.device_check_pullrate_select option[value="100"]').prop('selected',true);
        $(that).find('.ma_device_can_mode option[value="1"]').prop('selected',true);
        $(that).find('.ma_device_can_type').prop("disabled",true);
        $(that).find('.ma_device_can_id_type').prop("disabled",true);
        $(that).find('.ma_device_can_id').prop("disabled",true);
        $(that).find('.ma_device_can_byte').prop("disabled",true);
    };

    function showConnected(){
        let deviceConnected = returnObj.device_found.maxID_Device;
        PlotHistory.maxD = deviceConnected;
        PlotData.maxD = deviceConnected;
        let devices = [];
        if(deviceConnected > 0)
        {
            for (let i = 0; i < deviceConnected; i++)
            {
                devices[i] = {
                    deviceType : returnObj.device_found['chainlist'][`device${i}`]['channel0']['deviceType'],
                    serialNumber : returnObj.device_found['serialNumber'][i],
                };

                if(settings.get("cal") && settings.get("cal") == 1)
                {
                    $('#device_serial').html('');
                    $.each(devices, function (i, item) {
                        if(item.deviceType != "2" && item.deviceType != "5")
                        {
                            $('#device_serial').append($('<option>', {
                                value: item.deviceType,
                                text : item.serialNumber,
                                "data-index" : i
                            }));
                        }
                    });
                }

                let deviceObject = {
                    deviceType : returnObj.device_found['chainlist'][`device${i}`]['channel0']['deviceType'],
                    serialNumber : returnObj.device_found['serialNumber'][i],
                    manufacture_year : returnObj.device_found['manufacture_year'][i],
                    manufacture_month : returnObj.device_found['manufacture_month'][i],
                    manufacture_day : returnObj.device_found['manufacture_day'][i],
                    firmwareVersion_major : returnObj.device_found['firmwareVersion_major'][i],
                    firmwareVersion_minor : returnObj.device_found['firmwareVersion_minor'][i]
                };

                let AppendTemplate;
                switch (deviceObject.deviceType)
                {
                    case 1:
                        let dio_template = require("../js/dio_template");
                        AppendTemplate = dio_template.generate_template(deviceObject,i);
                        dio_template.events();
                        break;
                    default:
                        AppendTemplate = template1(deviceObject,i);
                        break;
                }

                $(".ma_device .table-responsive-sm").append(`<div class="ma_device_wrap device${i}" data-type="${deviceObject.deviceType}" data-number="${i+1}">${AppendTemplate}</div>`);
            }
        }

        $('#device_connect').prop('disabled',true).addClass('hidden');
        $('#device_startlog').removeClass('disabled hidden').prop('disabled',false);
        $('#device_reload').removeClass('hidden');
        $("#readButton").prop('disabled',false);
        $("#importButton").prop('disabled',false);
    }

    function startDataStream(){
        $('#device_startlog').prop('disabled',true).text(def_lang.text_logging).removeClass('btn-info').addClass('btn-success disabled');
        $('.device_save').prop('disabled',true).addClass('btn-warning disabled').removeClass('btn-success');
        $('.device_default').prop('disabled',true).addClass('btn-warning disabled').removeClass('btn-success');
        $('.device_check_status').prop('disabled',true);
        $('.ma_device_channel_pullrate').prop('disabled',true);
        $('.device_check_pullrate_select').prop('disabled',true);
        $('.ma_device_channel_tag').prop('disabled',true);
        $('.neorad_ain_sample_input').prop('disabled',true);
        $('.ain_select').prop('disabled',true);
        $('#device_stop').removeClass('disabled hidden').prop('disabled',false);
        $(".ma_calibration button").prop('disabled',true);

        setTimeout(function () {
            start_get_data = 1;
        },100);
    }

    function readSettings(raw) {
        if(raw.State == neoRADIO2_RunStates.neoRADIO2state_Connected)
        {
            let max_device = raw.maxID_Device;
            let init_warning = "Warning!";
            let init_warning_status = false;
            for (let i = 0; i < max_device; i++)
            {
                for(let j = 0; j < 8; j++)
                {
                    let ChipOnDevice = j + 1;
                    let EnableStatus = raw['chainlist'][`device${i}`][`channel${j}`]['settingsEnables'];
                    let ReportRate = raw['chainlist'][`device${i}`][`channel${j}`]['settingsReportRate'];
                    let deviceType = raw['chainlist'][`device${i}`][`channel${j}`]['deviceType'];
                    let canArbid = raw['chainlist'][`device${i}`][`channel${j}`]['settingsCanArbid'];
                    let canLocation = raw['chainlist'][`device${i}`][`channel${j}`]['settingsCanLocation'];
                    let canMsgType = raw['chainlist'][`device${i}`][`channel${j}`]['settingsCanMsgType'];
                    let canType, canIdType;

                    switch (canMsgType) {
                        case 0:
                            canType = 1;
                            canIdType = 3;
                            $(`.device${i} .device_channel${ChipOnDevice} .ma_device_can_mode`).val("0");
                            break;
                        case 1:
                            canType = 1;
                            canIdType = 4;
                            $(`.device${i} .device_channel${ChipOnDevice} .ma_device_can_mode`).val("0");
                            break;
                        case 2:
                            canType = 2;
                            canIdType = 3;
                            $(`.device${i} .device_channel${ChipOnDevice} .ma_device_can_mode`).val("0");
                            break;
                        case 3:
                            canType = 2;
                            canIdType = 4;
                            $(`.device${i} .device_channel${ChipOnDevice} .ma_device_can_mode`).val("0");
                            break;
                        case 255:
                            $(`.device${i} .device_channel${ChipOnDevice} .ma_device_can_mode`).val("1");
                            $(`.device${i} .device_channel${ChipOnDevice} .ma_device_can_type`).prop("disabled",true);
                            $(`.device${i} .device_channel${ChipOnDevice} .ma_device_can_id_type`).prop("disabled",true);
                            $(`.device${i} .device_channel${ChipOnDevice} .ma_device_can_id`).prop("disabled",true);
                            $(`.device${i} .device_channel${ChipOnDevice} .ma_device_can_byte`).prop("disabled",true);
                            break;
                    }

                    let tagName = raw['chainlist'][`device${i}`][`channel${j}`]['settingsNameArray'];
                    let tagNameString = "";
                    let tag_name_length = tagName.length;
                    for(let k = 0; k < tag_name_length; k++)
                    {
                        tagNameString += String.fromCharCode(tagName[k]);
                    }

                    PlotSeries.enables.push(EnableStatus);

                    if(EnableStatus > 0)
                    {
                        $(`.device${i} .device_channel${ChipOnDevice} .ma_device_channel_status`).text(def_lang.text_enable).prev().prop('checked',true);
                    }
                    else
                    {
                        $(`.device${i} .device_channel${ChipOnDevice} .ma_device_channel_status`).text(def_lang.text_disable).prev().prop('checked',false);
                    }

                    $(`.device${i} .device_channel${ChipOnDevice} .ma_device_channel_pullrate`).val(ReportRate);
                    if( ReportRate == 10 ||
                        ReportRate == 20 ||
                        ReportRate == 50 ||
                        ReportRate == 100 ||
                        ReportRate == 200 ||
                        ReportRate == 500 ||
                        ReportRate == 1000 ||
                        ReportRate == 2000 ||
                        ReportRate == 5000
                    )
                    {
                        $(`.device${i} .device_channel${ChipOnDevice} .device_check_pullrate_select option[value="${ReportRate}"]`).prop('selected',true);
                    }
                    else
                    {
                        $(`.device${i} .device_channel${ChipOnDevice} .device_check_pullrate_select option[value="0"]`).prop('selected',true);
                        $(`.device${i} .device_channel${ChipOnDevice} .ma_device_channel_pullrate`).prop('disabled',false);
                        $(`.device${i} .device_channel${ChipOnDevice} .ma_device_channel_pullrate`).parent().removeClass('opacity');
                    }

                    if(tagName && tagName != "")
                    {
                        $(`.device${i} .device_channel${ChipOnDevice} .ma_device_channel_tag`).val(tagNameString);
                    }

                    $(`.device${i} .device_channel${ChipOnDevice} .ma_device_can_type`).val(canType);
                    $(`.device${i} .device_channel${ChipOnDevice} .ma_device_can_id_type`).val(canIdType);
                    $(`.device${i} .device_channel${ChipOnDevice} .ma_device_can_id`).val(`0x${canArbid.toString(16).toUpperCase()}`);
                    $(`.device${i} .device_channel${ChipOnDevice} .ma_device_can_byte`).val(canLocation);

                    switch (deviceType) {
                        case 0:
                            break;
                        case 1:
                            break;
                        case 2:
                            $('.ma_device_wrap[data-type="2"] .device_default').addClass('hidden').prop('disabled',true);
                            if(raw['PWRRLY_STATUS'][i])
                            {
                                let movePixel = $('.input_selector').width();
                                let bStatus = parseInt(raw['PWRRLY_STATUS'][i]).toString(2);
                                let arrayholder = bStatus.split("");
                                arrayholder = arrayholder.reverse();
                                bStatus = arrayholder.join("");
                                let bstatus_length = bStatus.length;
                                for(let k = 0; k < bstatus_length; k++)
                                {
                                    if(bStatus[k] == 1)
                                    {
                                        $(`.device${i} .device_channel${k+1} #rly_on_${i}_${k+1}`).prop('checked',true);
                                        let currentSelector = $(`.device${i} .device_channel${k+1} #input_selector_bg_${i}_${k+1}`);
                                        let currentValue = $(currentSelector).find('.input_selector_value').val();
                                        if (currentValue == "0")
                                        {
                                            $(currentSelector).find('.input_selector').animate({left:(movePixel * 0.87)}, {
                                                duration: 0,
                                                complete: () => {
                                                    $(currentSelector).prev().removeClass("offline");
                                                    $(currentSelector).next().addClass("online");
                                                    $(currentSelector).find('.input_selector_value').val("1");
                                                }
                                            });
                                        }
                                        else if(currentValue == "1")
                                        {
                                            $(currentSelector).find('.input_selector').animate({left:-1},{
                                                duration: 0,
                                                complete: () => {
                                                    $(currentSelector).prev().addClass("offline");
                                                    $(currentSelector).next().removeClass("online");
                                                    $(currentSelector).find('.input_selector_value').val("0");
                                                }
                                            });
                                        }
                                    }
                                }
                            }
                            break;
                        case 3:
                            if(EnableStatus > 0 && EnableStatus < 4)
                            {
                                $(`.device${i} .device_channel${ChipOnDevice} .ain_low_select option[value=${EnableStatus}]`).prop('selected',true);
                            }
                            else if(EnableStatus > 3 && EnableStatus < 7)
                            {
                                $(`.device${i} .device_channel${ChipOnDevice} .neorad_ain_sample_input[value="high"]`).prop("checked",true);
                                $(`.device${i} .device_channel${ChipOnDevice} .ain_low_select`).addClass('hidden');
                                $(`.device${i} .device_channel${ChipOnDevice} .ain_high_select`).removeClass('hidden');
                                $(`.device${i} .device_channel${ChipOnDevice} .ain_high_select option[value=${EnableStatus}]`).prop('selected',true);
                            }
                            break;
                        case 4:
                            let settings1_enable = raw['chainlist'][`device${i}`][`channel${j}`]['settings1']['enabled'];
                            let settings2_enable = raw['chainlist'][`device${i}`][`channel${j}`]['settings2']['enabled'];
                            let settings3_enable = raw['chainlist'][`device${i}`][`channel${j}`]['settings3']['enabled'];

                            if(settings1_enable > 0)
                            {
                                $(`.device${i} .device_channel${ChipOnDevice} .device_check_status1`).prop('checked',true);
                                $(`.device${i} .device_channel${ChipOnDevice} .ma_device_channel_status1`).text(def_lang.text_enable);
                                $(`.device${i} .device_channel${ChipOnDevice} .neorad_aout_input_1`).prop('disabled',false);
                            }
                            else
                            {
                                $(`.device${i} .device_channel${ChipOnDevice} .device_check_status1`).prop('checked',false);
                                $(`.device${i} .device_channel${ChipOnDevice} .ma_device_channel_status1`).text(def_lang.text_disable);
                                $(`.device${i} .device_channel${ChipOnDevice} .neorad_aout_input_1`).prop('disabled',true);
                            }

                            if(settings2_enable > 0)
                            {
                                $(`.device${i} .device_channel${ChipOnDevice} .device_check_status2`).prop('checked',true);
                                $(`.device${i} .device_channel${ChipOnDevice} .ma_device_channel_status2`).text(def_lang.text_enable);
                                $(`.device${i} .device_channel${ChipOnDevice} .neorad_aout_input_2`).prop('disabled',false);
                            }
                            else
                            {
                                $(`.device${i} .device_channel${ChipOnDevice} .device_check_status2`).prop('checked',false);
                                $(`.device${i} .device_channel${ChipOnDevice} .ma_device_channel_status2`).text(def_lang.text_disable);
                                $(`.device${i} .device_channel${ChipOnDevice} .neorad_aout_input_2`).prop('disabled',true);
                            }

                            if(settings3_enable > 0)
                            {
                                $(`.device${i} .device_channel${ChipOnDevice} .device_check_status3`).prop('checked',true);
                                $(`.device${i} .device_channel${ChipOnDevice} .ma_device_channel_status3`).text(def_lang.text_enable);
                                $(`.device${i} .device_channel${ChipOnDevice} .neorad_aout_input_3`).prop('disabled',false);
                            }
                            else
                            {
                                $(`.device${i} .device_channel${ChipOnDevice} .device_check_status3`).prop('checked',false);
                                $(`.device${i} .device_channel${ChipOnDevice} .ma_device_channel_status3`).text(def_lang.text_disable);
                                $(`.device${i} .device_channel${ChipOnDevice} .neorad_aout_input_3`).prop('disabled',true);
                            }

                            let initOutputValue1 = raw['chainlist'][`device${i}`][`channel${j}`]['settings1']['initOutputValue'];
                            let initOutputValue2 = raw['chainlist'][`device${i}`][`channel${j}`]['settings2']['initOutputValue'];
                            let initOutputValue3 = raw['chainlist'][`device${i}`][`channel${j}`]['settings3']['initOutputValue'];

                            $(`.device${i} .device_channel${ChipOnDevice} .neorad_aout_initinput_1`).val(initOutputValue1 / 13107);
                            $(`.device${i} .device_channel${ChipOnDevice} .neorad_aout_initinput_2`).val(initOutputValue2 / 13107);
                            $(`.device${i} .device_channel${ChipOnDevice} .neorad_aout_initinput_3`).val(initOutputValue3 / 13107);
                            break;
                        case 5:
                            break;
                    }
                }

                if(raw['chainlist'][`device${i}`]['channel0']['notInit'] == 1)
                {
                    init_warning += ` Device ${i+1} not initialized. `;
                    init_warning_status = true;
                }
            }

            if(parseInt($('#device_serial option:selected').val()) == 3)
            {
                $('.ma_calibration .neorad_ain_sample').removeClass('hidden');
            }
            else
            {
                $('.ma_calibration .neorad_ain_sample').addClass('hidden');
            }

            if(init_warning_status)
            {
                change_status_text(init_warning);
            }
        }
    }

    function processRawData(raw) {
        if(raw.State == neoRADIO2_RunStates.neoRADIO2state_Connected)
        {
            let rawObj = {};
            let unit;
            let time_us = raw['Timeus'];
            let max_device = raw.maxID_Device;

            for (let i = 0; i < max_device; i++)
            {
                for (let j = 1; j < 9; j++)
                {
                    let measured_value;
                    let displayString;
                    let deviceType = raw[i]['deviceType'];
                    let channelValue = raw[i][j - 1];
                    if (raw[i])
                    {
                        switch (deviceType)
                        {
                            case 0:
                                unit = "&#176C";
                                if (isFahrenheit == 1)
                                {
                                    unit = "&#176F";
                                }

                                measured_value = temperatureConversion(channelValue);
                                if (measured_value !== undefined && measured_value !== temperatureString)
                                {
                                    displayString = `${(measured_value.toFixed(2))}${unit}`;
                                }

                                if (measured_value === temperatureString)
                                {
                                    $(`.device${i} .device_channel${j} .ma_device_channel_temp`).html(def_lang.text_disc).removeClass("em15 online").addClass("em1 offline");
                                    $(`#cbtemp_d${i}_c${j}`).html(def_lang.text_disc).removeClass("online").addClass("offline");
                                }
                                else if(measured_value !== undefined)
                                {
                                    $(`.device${i} .device_channel${j} .ma_device_channel_temp`).html(displayString).addClass('em15 online').removeClass("em1 offline");
                                    $(`#cbtemp_d${i}_c${j}`).html(displayString).addClass('online').removeClass("offline");
                                }
                                break;
                            case 3:
                                if($(`input[name="ain_${i}_b${j}"]:checked`).val() == "low")
                                {
                                    measured_value = voltageConversion(channelValue);
                                    unit = "mV";
                                    if(measured_value != undefined && measured_value != "d" && measured_value != "error")
                                    {
                                        displayString = `${(measured_value * 1000).toFixed(3)}${unit}`;
                                        $(`.device${i} .device_channel${j} .ma_device_channel_temp`).html(displayString).addClass('em15 online').removeClass("em1 offline");
                                        $(`#cbtemp_d${i}_c${j}`).html(displayString).addClass('online').removeClass("offline");
                                    }
                                    else if(measured_value == "error")
                                    {
                                        displayString = "Over Range";
                                        $(`.device${i} .device_channel${j} .ma_device_channel_temp`).html(displayString).addClass('em15 online').removeClass("em1 offline");
                                        $(`#cbtemp_d${i}_c${j}`).html(displayString).addClass('online').removeClass("offline");
                                    }
                                }
                                else if($(`input[name="ain_${i}_b${j}"]:checked`).val() == "high")
                                {
                                    measured_value = voltageConversion(channelValue);
                                    unit = "V";
                                    if(measured_value != undefined && measured_value != "d" && measured_value != "error")
                                    {
                                        displayString = `${measured_value.toFixed(4)}${unit}`;
                                        $(`.device${i} .device_channel${j} .ma_device_channel_temp`).html(displayString).addClass('em15 online').removeClass("em1 offline");
                                        $(`#cbtemp_d${i}_c${j}`).html(displayString).addClass('online').removeClass("offline");
                                    }
                                    else if(measured_value == "error")
                                    {
                                        displayString = "Over Range";
                                        $(`.device${i} .device_channel${j} .ma_device_channel_temp`).html(displayString).addClass('em15 online').removeClass("em1 offline");
                                        $(`#cbtemp_d${i}_c${j}`).html(displayString).addClass('online').removeClass("offline");
                                    }
                                }
                                break;
                        }

                        if (measured_value != temperatureString && measured_value != undefined)
                        {
                            tempPushToArray(measured_value, i, (j-1), exportTime(time_us));
                        }
                    }

                    if(measured_value === temperatureString)
                    {
                        rawObj['time'] = exportTime(time_us);
                        rawObj[`device${i+1}_bank${j}`] = temperatureString;
                    }
                    else if(measured_value !== undefined)
                    {
                        rawObj['time'] = exportTime(time_us);
                        switch (deviceType)
                        {
                            case 0:
                                rawObj[`device${i+1}_bank${j}`] = parseFloat(measured_value.toFixed(2));
                                break;
                            case 3:
                                rawObj[`device${i+1}_bank${j}`] = measured_value;
                                break;
                        }
                    }

                    if(deviceType == "2")
                    {
                        break;
                    }
                }
            }

            $(`.ma_device_channel_time`).html(`${readableTime(time_us)}`);
            if(rawObj['time'])
            {
                dumpDataFile(rawObj);
            }
        }
    }

    function sendSettings(bank, enables, reportRate, deviceType, deviceLink, tagName, CanId, CanMsgType, CanLocation, extra){
        let settingsHere = {
            bank: parseInt(bank) - 1,
            enables: parseInt(enables),
            reportRate: parseInt(reportRate),
            deviceType: parseInt(deviceType),
            deviceLink: parseInt(deviceLink) - 1,
            tagName: tagName,
            CanId: parseInt(CanId),
            CanMsgType: parseInt(CanMsgType),
            CanLocation: parseInt(CanLocation),
            extraSettings: extra
        };

        let JSONstr = JSON.stringify(settingsHere);
        EmitEvents.to.emit(4, JSONstr);
        send_settings_once = 1;
        deviceSave = deviceLink - 1;
    }

    function reloadSettings(raw) {
        let objvalue = Object.values(raw["chainlist"])[deviceSave];
        PlotSeries.enables = [];
        let obj_length = Object.keys(objvalue).length;
        for(let i = 0; i < obj_length; i++)
        {
            let enables = Object.values(objvalue)[i]['settingsEnables'];
            let samplerate = Object.values(objvalue)[i]['settingsReportRate'];

            if(enables == 0)
            {
                $(`.device${deviceSave} .device_channel${i+1} .ma_device_channel_status`).prev().prop('checked',false).text(def_lang.text_disable);
                $(`.device${deviceSave} .device_channel${i+1} .ma_device_channel_temp`).text("");
            }
            else if(enables == 1)
            {
                $(`.device${deviceSave} .device_channel${i+1} .ma_device_channel_status`).prev().prop('checked',true).text(def_lang.text_enable);
                $(`.device${deviceSave} .device_channel${i+1} .ma_device_channel_temp`).text("");
            }

            $(`.device_${deviceSave} .device_channel${i+1} .ma_device_channel_pullrate`).val(samplerate);

            let canArbid = Object.values(objvalue)[i]['settingsCanArbid'];
            let canLocation = Object.values(objvalue)[i]['settingsCanLocation'];
            let canMsgType = Object.values(objvalue)[i]['settingsCanMsgType'];
            let canType, canIdType;
            switch (canMsgType) {
                case 0:
                    canType = 1;
                    canIdType = 3;
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_can_mode`).val("0");
                    break;
                case 1:
                    canType = 1;
                    canIdType = 4;
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_can_mode`).val("0");
                    break;
                case 2:
                    canType = 2;
                    canIdType = 3;
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_can_mode`).val("0");
                    break;
                case 3:
                    canType = 2;
                    canIdType = 4;
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_can_mode`).val("0");
                    break;
                case 255:
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_can_mode`).val("1");
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_can_type`).prop("disabled",true);
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_can_id_type`).prop("disabled",true);
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_can_id`).prop("disabled",true);
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_can_byte`).prop("disabled",true);
                    break;
            }

            let tagName = Object.values(objvalue)[i]['settingsNameArray'];
            let tagNameString = "";
            for(let k = 0; k < 14; k++)
            {
                tagNameString += String.fromCharCode(tagName[k]);
            }

            if(tagName && tagName != "")
            {
                $(`.device_${deviceSave} .device_channel${i+1} .ma_device_channel_tag`).val(tagNameString);
            }

            $(`.device_${deviceSave} .device_channel${i+1} .ma_device_can_type`).val(canType);
            $(`.device_${deviceSave} .device_channel${i+1} .ma_device_can_id_type`).val(canIdType);
            $(`.device_${deviceSave} .device_channel${i+1} .ma_device_can_id`).val(canArbid);
            $(`.device_${deviceSave} .device_channel${i+1} .ma_device_can_byte`).val(canLocation);

            if(Object.values(objvalue)[i]['deviceType'] == 4)
            {
                let settings1_enable = Object.values(objvalue)[i]['settings1']['enabled'];
                let settings2_enable = Object.values(objvalue)[i]['settings2']['enabled'];
                let settings3_enable = Object.values(objvalue)[i]['settings3']['enabled'];

                if(settings1_enable > 0)
                {
                    $(`.device${deviceSave} .device_channel${i+1} .device_check_status1`).prop('checked',true);
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_channel_status1`).text(def_lang.text_enable);
                    $(`.device${deviceSave} .device_channel${i+1} .neorad_aout_input_1`).prop('disabled',false);
                }
                else
                {
                    $(`.device${deviceSave} .device_channel${i+1} .device_check_status1`).prop('checked',false);
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_channel_status1`).text(def_lang.text_disable);
                    $(`.device${deviceSave} .device_channel${i+1} .neorad_aout_input_1`).prop('disabled',true);
                }

                if(settings2_enable > 0)
                {
                    $(`.device${deviceSave} .device_channel${i+1} .device_check_status2`).prop('checked',true);
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_channel_status2`).text(def_lang.text_enable);
                    $(`.device${deviceSave} .device_channel${i+1} .neorad_aout_input_2`).prop('disabled',false);
                }
                else
                {
                    $(`.device${deviceSave} .device_channel${i+1} .device_check_status2`).prop('checked',false);
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_channel_status2`).text(def_lang.text_disable);
                    $(`.device${deviceSave} .device_channel${i+1} .neorad_aout_input_2`).prop('disabled',true);
                }

                if(settings3_enable > 0)
                {
                    $(`.device${deviceSave} .device_channel${i+1} .device_check_status3`).prop('checked',true);
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_channel_status3`).text(def_lang.text_enable);
                    $(`.device${deviceSave} .device_channel${i+1} .neorad_aout_input_3`).prop('disabled',false);
                }
                else
                {
                    $(`.device${deviceSave} .device_channel${i+1} .device_check_status3`).prop('checked',false);
                    $(`.device${deviceSave} .device_channel${i+1} .ma_device_channel_status3`).text(def_lang.text_disable);
                    $(`.device${deviceSave} .device_channel${i+1} .neorad_aout_input_3`).prop('disabled',true);
                }

                let initOutputValue1 = Object.values(objvalue)[i]['settings1']['initOutputValue'];
                let initOutputValue2 = Object.values(objvalue)[i]['settings2']['initOutputValue'];
                let initOutputValue3 = Object.values(objvalue)[i]['settings3']['initOutputValue'];

                $(`.device${i} .device_channel${ChipOnDevice} .neorad_aout_initinput_1`).val(initOutputValue1 / 13107);
                $(`.device${i} .device_channel${ChipOnDevice} .neorad_aout_initinput_2`).val(initOutputValue2 / 13107);
                $(`.device${i} .device_channel${ChipOnDevice} .neorad_aout_initinput_3`).val(initOutputValue3 / 13107);
            }
        }

        let max_device = returnObj.device_found.maxID_Device;
        for (let i = 0; i < max_device; i++)
        {
            for(let j = 0; j < 8; j++)
            {
                let EnableStatus = raw['chainlist'][`device${i}`][`channel${j}`]['settingsEnables'];
                PlotSeries.enables.push(EnableStatus);
            }
        }

        change_status_text('Settings saved');
    }

    function readableTime(time) {
        let date = new Date(time);
        return `${date.getUTCHours()} hours ${date.getUTCMinutes()} minutes ${date.getUTCSeconds()} seconds`;
    }

    function exportTime(time) {
        let date = new Date(time);
        let hour = date.getUTCHours();
        let min = date.getUTCMinutes();
        let sec = date.getUTCSeconds();
        let millisec = date.getUTCMilliseconds();
        let returntime;
        if(min < 10 && min > 0)
        {
            min = `0${min}`;
        }

        if(sec < 10 && sec > 0)
        {
            sec = `0${sec}`;
        }

        if(millisec < 100 && millisec > 0)
        {
            if(millisec < 10)
            {
                millisec = `00${millisec}`;
            }
            else
            {
                millisec = `0${millisec}`;
            }
        }

        if(hour === 0)
        {
            returntime = `${min}:${sec}:${millisec}`;
        }
        else
        {
            returntime = `${hour}:${min}:${sec}:${millisec}`;
        }

        return returntime;
    }

    function terminateAll() {
        $(".ma_device .online").removeClass("online").addClass("offline").text(def_lang.text_offline);
        $(".ma_graph .online").removeClass("online").addClass("offline").text(def_lang.text_offline);

        $('.cal_table_tc').addClass('hidden');
        if($('.ma_calibration .input_selector_bg').find('.input_selector_value').val() == "1")
        {
            $('.ma_calibration .input_selector_bg').find('.input_selector').animate({left:-1}, {
                duration: 0,
                complete: () => {
                    $('.ma_calibration .input_selector_bg').prev().addClass("offline");
                    $('.ma_calibration .input_selector_bg').next().removeClass("online");
                    $('.ma_calibration .input_selector_bg').find('.input_selector_value').val("0");
                    $('.cal_table_tc').off('focus','.cal_me');
                }
            });
        }

        $('#device_stop').addClass('disabled hidden').prop('disabled',true);
        $(".ma_device_channel_temp").text("");
        $(".ma_device_channel_time").text("");
        $(".ma_device_wrap").remove();
        clickStop();

        setTimeout(() => {
            $('#device_connect').prop('disabled',false).removeClass('hidden');
            $('#device_startlog').prop('disabled',true).text(def_lang.device_startlog).removeClass('btn-success disabled').addClass('btn-info hidden');
            $('#device_stop').addClass('hidden');
            $('#device_reload').addClass('hidden');
            $('.device_save').addClass('hidden');
            $('.device_default').addClass('hidden');
            $('.neorad_ain_sample_input').prop('disabled',false);
            $('.ain_select').prop('disabled',false);
        },100);

        $('.neorad_ain_sample').addClass('hidden');
        $(".ma_calibration button").prop('disabled',true);
        $("#device_serial option[data-index='0']").prop('selected',true);
        $(".cal_table_tc_tables").html("");

        returnObj.neoRADIO_status = 0;
        globalObj = {};
        plotlyInit = true;
        PlotSeries.enables = [];

        if($('.ma_device').hasClass('hidden'))
        {
            $('.nav-link').blur();
            $('.nav_device').click();
        }

        if($("html,body").hasClass('wait'))
        {
            $("html,body").removeClass('wait');
        }
    }

    returnObj.pauseAll = function () {
        $('.device_default').prop('disabled',false).removeClass('btn-warning disabled').addClass('btn-success');
        $('#device_startlog').prop('disabled',false).text(def_lang.device_startlog).removeClass('btn-success disabled').addClass('btn-info');
        $('.device_check_pullrate_select').prop('disabled',false);
        $('.ma_device_channel_tag').prop('disabled',false);
        $('.device_check_status').prop('disabled',false);
        $('.device_check_pullrate_select').each(function (index, element) {
            if($(element).val() == 0)
            {
                $(element).next().prop('disabled',false);
            }
        });

        $(".ma_device_can_mode").each(function (index, element) {
            if($(element).val() == "0")
            {
                $($('.ma_device_can_type')[index]).prop('disabled',false);
                $($('.ma_device_can_id_type')[index]).prop('disabled',false);
                $($('.ma_device_can_id')[index]).prop('disabled',false);
                $($('.ma_device_can_byte')[index]).prop('disabled',false);
            }
        });

        $('.neorad_ain_sample_input').prop('disabled',false);
        $('.ain_select').prop('disabled',false);

        $("#readButton").prop('disabled',false);
        $("#importButton").prop('disabled',false);
        if($('.cal_table_tc_tables table').length > 0)
        {
            $(".ma_calibration button").prop('disabled',false);
        }

        setTimeout(function () {
            $('.device_save').prop('disabled',false).removeClass('btn-warning disabled').addClass('btn-success');
        },200);
    };

    returnObj.checkTagName = function() {
        let deviceConnected = returnObj.device_found.maxID_Device;
        for(let i = 0; i < deviceConnected; i++)
        {
            for(let j = 1; j < 9; j++)
            {
                let currentValue = $(`#device_check_tag${i}_${j}`);
                if(currentValue.val())
                {
                    let data_id = $(currentValue).attr('data-checkbox');
                    let chart_id = $('.ma_graph_sidebar_nav_channel').find(`input[data-checkbox="${data_id}"]`).next('label');
                    chart_id.text(currentValue.val());
                }
            }
        }
    };

    let dumpData = [];
    let FirstHeader = true;
    function dumpDataFile(raw) {
        dumpData.push(raw);
        let DataJSON;

        if(dumpData.length > 50)
        {
            while(Object.keys(dumpData[0]).length !== Object.keys(dumpData[dumpData.length - 1]).length)
            {
                dumpData.shift();
            }

            DataJSON = Papa.unparse(dumpData,{
                header:FirstHeader
            });

            if(FirstHeader === true)
            {
                FirstHeader = false;
            }

            if(writeStream !== 0)
            {
                writeStream.write(`${DataJSON} \r\n`);
            }
            dumpData = [];
        }
    }

    function temperatureConversion(value) {
        if(value === -1000 || value === null)
        {
            return temperatureString;
        }

        let converted = value;
        if(isFahrenheit == 1)
        {
            converted = value * 9 / 5 + 32;
        }

        return converted;
    }

    function voltageConversion(value) {
        if(value === -1000)
        {
            return temperatureString;
        }
        else if(value == "inf")
        {
            return "error";
        }

        return value;
    }

    function tempPushToArray(raw,device,bank,time_us) {
        if(typeof raw != "number")
        {
            raw = 0;
        }

        if(PlotSeries.time == "")
        {
            PlotSeries.time = [time_us];
        }

        if(PlotSeries.data[bank+(device*8)].length == 0)
        {
            PlotSeries.data[bank+(device*8)].push(parseFloat(raw.toFixed(1)));
        }

        let len = PlotHistory.time.length;
        PlotHistory.data[bank+(device*8)][len] = parseFloat(raw.toFixed(1));
        if(PlotHistory.time[PlotHistory.time.length - 1] !== time_us)
        {
            PlotHistory.time.push(time_us);
        }

        if(len > 10000)
        {
            console.time('reduce');
            let date = new Date();
            let month = date.getMonth() + 1;
            let day = date.getDate();
            let Minutes = date.getMinutes();
            if(Minutes < 10)
            {
                Minutes = `0${Minutes}`;
            }
            let end = `${date.getHours()}.${Minutes}`;

            let len1 = len;
            while (len1--)
            {
                if(len1 % 4 === 0)
                {
                    PlotHistory.time.splice(len1, 3);
                }
            }
            let plot_history_max = PlotHistory.maxD;
            for(let i = 0; i < plot_history_max; i++)
            {
                for(let j = 0; j < 8; j++)
                {
                    let len2 = len;
                    while (len2--)
                    {
                        if(len2 % 4 === 0)
                        {
                            PlotHistory.data[j + (i * 8)].splice(len2, 3);
                        }
                    }

                    while(PlotHistory.data[j + (i * 8)].length > 2500)
                    {
                        PlotHistory.data[j + (i * 8)].pop();
                    }
                }
            }

            if(autoSave)
            {
                PlotData.time.push(...PlotHistory.time);
                for(let i = 0; i < 8; i++)
                {
                    for(let j = 0; j < 8; j++)
                    {
                        PlotData.data[j + (i * 8)].push(...PlotHistory.data[j + (i * 8)]);
                    }
                }
            }

            PlotHistory.time = [];
            for(let i = 0; i < 8; i++)
            {
                for(let j = 0; j < 8; j++)
                {
                    PlotHistory.data[j + (i * 8)] = [];
                }
            }

            if (PlotData.time.length > 15000 && autoSave)
            {
                let Path = path.join(mypath, `IntrepidCS\/neoRAD-IO2\/PlotHistory\/${month}-${day} ${startTime}-${end}.json`);
                addSelectOption(`${month}-${day} ${startTime}-${end}`);
                fs.writeFile(Path, JSON.stringify(PlotData), err => {
                    if (err) throw err;
                });
                startTime = end;

                PlotData.time = [];
                for (let i = 0; i < 9; i++)
                {
                    for (let j = 0; j < 8; j++)
                    {
                        PlotData.data[j + (i * 8)] = [];
                    }
                }
            }

            console.timeEnd('reduce');
        }
    }

    returnObj.readCalSettings = () => {
        let tableOBJ = {
            device: parseInt($('#device_serial option:selected').attr('data-index')),
            type: parseInt($('#device_serial option:selected').val()),
            deviceChannel: 0,
            deviceRange: 0
        };

        $(".ma_calibration button").prop('disabled',false);
        EmitEvents.to.emit(6, JSON.stringify(tableOBJ));
    };

    returnObj.saveCal = () => {
        let tablelength = $(".cal_table_").length;
        for(let i = 0; i < tablelength; i++)
        {
            let tableOBJ = {
                device: parseInt($('#device_serial option:selected').attr('data-index')),
                type: parseInt($('#device_serial option:selected').val()),
                pt_array:[],
                bank1:[],
                bank2:[],
                bank3:[],
                bank4:[],
                bank5:[],
                bank6:[],
                bank7:[],
                bank8:[],
                deviceChannel: 0,
                deviceRange: i
            };

            if(tableOBJ.type == 4)
            {
                tableOBJ.deviceChannel = parseInt($($('.cal_th_header')[i]).attr('data-channel'));
            }

            let th_length = $(`#cal_table_${i} .cal_input_th`).length;
            for (let th_length_i = 0; th_length_i < th_length; th_length_i++)
            {
                let tRowI = $(`#cal_table_${i} .cal_input_th`)[th_length_i];
                let tRow = $(tRowI).closest('tr').find('td');
                let PointValue = parseFloat($(tRowI).val());
                if(!PointValue)
                {
                    PointValue = 0;
                }

                //AOUT value is a 24bit int stored in 32bit value
                if(tableOBJ.type == 4)
                {
                    if(parseFloat($(tRowI).val()) > 5 || parseFloat($(tRowI).val()) < 0)
                    {
                        change_status_text('Please enter a correct value for Cal point');
                        $("html,body").removeClass('wait');
                        $("#storeButton").prop('disabled',false);
                        return;
                    }
                    else
                    {
                        PointValue = parseInt(parseFloat($(tRowI).val()) * 3355443);
                    }
                }
                tableOBJ.pt_array.push(PointValue);
                for(let j = 1; j < 9; j++)
                {
                    let cSelector = $(tRow)[j+1];
                    let cSelectorFind = $(cSelector).find('.cal_me');
                    let Value = $(cSelectorFind).val();
                    //diff for AOUT
                    if(Value && !isNaN(Value))
                    {
                        if(tableOBJ.type == 4)
                        {
                            if(parseFloat(Value) > 5 || parseFloat(Value) < 0)
                            {
                                $(cSelectorFind).focus();
                                change_status_text('Please enter a correct value');
                                $("html,body").removeClass('wait');
                                $("#storeButton").prop('disabled',false);
                                return;
                            }
                            tableOBJ[`bank${j}`].push(parseInt(parseFloat(Value) * 3355443));
                        }
                        else
                        {
                            tableOBJ[`bank${j}`].push(parseFloat(Value));
                        }
                    }
                    else
                    {
                        $(cSelectorFind).focus();
                        change_status_text('Please enter a correct value');
                        $("html,body").removeClass('wait');
                        $("#storeButton").prop('disabled',false);
                        return;
                    }
                }
            }

            if(tableOBJ.pt_array.length === 0)
            {
                change_status_text('Points empty');
                $("html,body").removeClass('wait');
                $("#storeButton").prop('disabled',false);
                return;
            }

            EmitEvents.to.emit(7, JSON.stringify(tableOBJ));
        }
    };

    returnObj.interactiveCal = bank => {
        let tableOBJ = {
            device: parseInt($('#device_serial option:selected').attr('data-index')),
            bank: bank,
            type: parseInt($('#device_serial option:selected').val())
        };

        EmitEvents.to.emit(5, JSON.stringify(tableOBJ));
    };

    returnObj.manualCal = () => {
        EmitEvents.to.emit(0, 1);
    };

    returnObj.clearCal = () => {
        let tableOBJ = {
            device: parseInt($('#device_serial option:selected').attr('data-index'))
        };

        EmitEvents.to.emit(8, JSON.stringify(tableOBJ));
    };

    return returnObj;
}

// initialize native object
let globalObj;

$('#device_connect').on('click',function(){
    $(this).prop('disabled',true);
    globalObj = new NativeConstructor();
    globalObj.checkOnlineDevices();
});

$('#device_startlog').on('click',function(){
    setTimeout(function () {
        globalObj.startDataPush();
        $(this).prop('disabled',true);
        $('#device_stop').prop('disabled',false);
    },600);
});

$('#device_stop').on('click',function () {
    $(this).prop('disabled',true);
    if(globalObj.neoRADIO_status === 2)
    {
        globalObj.neoRADIO_status = 1;
        globalObj.pauseData();
        setTimeout(function () {
            globalObj.pauseAll();
        },100);
    }
});

$('#device_reload').on('click',() => {
    globalObj.close();
});

$('.ma_device').on('click','.device_group',function(){
    $(this).toggleClass('disabled');
    let deviceType = $(this).attr('data-type');
    if($(this).hasClass('disabled'))
    {
        $(this).text("Off");
        let selector = $(this).parents('.ma_device_wrap').find('.group_select');
        let selector2 = $(this).parents('.ma_device_wrap').find('.group_select_ain_radio');
        let selector3 = $(this).parents('.ma_device_wrap').find('.group_select_ain_select');
        let selector4 = $(this).parents('.ma_device_wrap').find('.device_check_status');
        $(selector).off();
        $(selector2).off();
        $(selector3).off();
        $(selector4).off();
    }
    else
    {
        $(this).text("On");
        let selector = $(this).parents('.ma_device_wrap').find('.group_select');
        let selector2 = $(this).parents('.ma_device_wrap').find('.group_select_ain_radio');
        let selector3 = $(this).parents('.ma_device_wrap').find('.group_select_ain_select');

        $(selector).on('change', function() {
            $(selector).val($(this).val());
        });
        $(selector2).on('change', function() {
            $(selector2).each((index , element) => {
                if($(element).val() == $(this).val())
                {
                    $(element).prop('checked',true);
                }
            });

            if($(this).val() == "low")
            {
                $(this).parents('.ma_device_wrap').find('.ain_low_select').removeClass("hidden");
                $(this).parents('.ma_device_wrap').find('.ain_high_select').addClass("hidden");
            }
            else if($(this).val() == "high")
            {
                $(this).parents('.ma_device_wrap').find('.ain_low_select').addClass("hidden");
                $(this).parents('.ma_device_wrap').find('.ain_high_select').removeClass("hidden");
            }
        });
        $(selector3).on('change', function() {
            $(selector3).val($(this).val());
        });

        if(deviceType == 4)
        {
            let selector4a = $(this).parents('.ma_device_wrap').find('.device_check_status1');
            let selector4b = $(this).parents('.ma_device_wrap').find('.device_check_status2');
            let selector4c = $(this).parents('.ma_device_wrap').find('.device_check_status3');
            $(selector4a).on('change', function() {
                $(selector4a).prop('checked', $(this).prop('checked'));
            });
            $(selector4b).on('change', function() {
                $(selector4b).prop('checked', $(this).prop('checked'));
            });
            $(selector4c).on('change', function() {
                $(selector4c).prop('checked', $(this).prop('checked'));
            });
        }
        else
        {
            let selector4 = $(this).parents('.ma_device_wrap').find('.device_check_status');
            $(selector4).on('change', function() {
                $(selector4).prop('checked', $(this).prop('checked'));
            });
        }
    }
});

$('.ma_device').on('click','.can_fill_apply',function () {
    let Value = $(this).parents('.ma_device_wrap').find('.can_fill_select').val();
    let Startid = parseInt($(this).parents('.ma_device_wrap').find('.can_fill_id').val());
    let cantype = $(this).parents('.ma_device_wrap').find('.ma_device_can_type').val();
    let cantypeid = $(this).parents('.ma_device_wrap').find('.ma_device_can_id_type').val();

    if(isNaN(Startid))
    {
        change_status_text('Please enter a correct Value');
        return;
    }

    $(this).parents('.ma_device_wrap').find('.ma_device_can_mode').val("0");
    $(this).parents('.ma_device_wrap').find('.ma_device_can_type').prop('disabled',false);
    $(this).parents('.ma_device_wrap').find('.ma_device_can_id_type').prop('disabled',false);
    $(this).parents('.ma_device_wrap').find('.ma_device_can_id').prop('disabled',false);
    $(this).parents('.ma_device_wrap').find('.ma_device_can_byte').prop('disabled',false);

    switch (Value) {
        case "1":
            $(this).parents('.ma_device_wrap').find('.ma_device_can_id').each(function (index, element) {
                $(element).val(`0x${(index + Startid).toString(16).toUpperCase()}`);
            });
            $(this).parents('.ma_device_wrap').find('.ma_device_can_byte').each(function (index, element) {
                $(element).val(0);
            });
            break;
        case "2":
            $(this).parents('.ma_device_wrap').find('.ma_device_can_id').each(function (index, element) {
                $(element).val(`0x${(parseInt(index / 2) % 4 + Startid).toString(16).toUpperCase()}`);
            });
            $(this).parents('.ma_device_wrap').find('.ma_device_can_byte').each(function (index, element) {
                if(index % 2 == 0)
                {
                    $(element).val(0);
                }
                else
                {
                    $(element).val(4);
                }
            });
            break;
        case "3":
            $(this).parents('.ma_device_wrap').find('.ma_device_can_id').each(function (index, element) {
                $(element).val(`0x${(parseInt(Startid)).toString(16).toUpperCase()}`);
            });
            $(this).parents('.ma_device_wrap').find('.ma_device_can_byte').each(function (index, element) {
                $(element).val(index * 4);
            });
            break;
    }

    if(cantype == null || cantypeid == null)
    {
        $(this).parents('.ma_device_wrap').find('.ma_device_can_type').val("1");
        $(this).parents('.ma_device_wrap').find('.ma_device_can_id_type').val("3");
    }
    else
    {
        $(this).parents('.ma_device_wrap').find('.ma_device_can_type').val(cantype);
        $(this).parents('.ma_device_wrap').find('.ma_device_can_id_type').val(cantypeid);
    }
});

$('.ma_device').on('click','.device_save',function(){
    $('.device_save').prop('disabled',true);
    $('.device_save_canhub').prop('disabled',true);
    $('#device_startlog').prop('disabled',true);
    $('html,body').addClass('wait');
    let that = $(this).parents('.ma_device_wrap');
    globalObj.gatherData(that);
    setTimeout(function () {
        $('#device_startlog').prop('disabled',false);
    },2000);
});

$('.ma_device').on('click','.device_save_canhub',function(){
    $('.device_save').prop('disabled',true);
    $('.device_save_canhub').prop('disabled',true);
    $('#device_startlog').prop('disabled',true);
    $('html,body').addClass('wait');
    let that = $(this).parents('.ma_device_wrap');
    globalObj.gatherData(that);
    setTimeout(function () {
        $('#device_startlog').prop('disabled',false);
    },2000);
});

$('.ma_device').on('click','.device_default',function(){
    let that = $(this).parents('.ma_device_wrap').find('.table:not(.hidden)');
    globalObj.setDefault(that);
});

$('.ma_device').on('click','.ma_device_table_tab1',function(){
    $(this).parents('.ma_device_wrap').find('.table1').removeClass('hidden');
    $(this).parents('.ma_device_wrap').find('.table2').addClass('hidden');
    $(this).parents('.ma_device_wrap').find('.table3').addClass('hidden');
    $(this).parents('.ma_device_wrap').find('.table4').addClass('hidden');
    $(this).parents('.ma_device_wrap').find('.ma_device_table_tab2').addClass('ma_device_table_head_inv').removeClass('ma_device_table_head');
    $(this).parents('.ma_device_wrap').find('.ma_device_table_tab3').addClass('ma_device_table_head_inv').removeClass('ma_device_table_head');
    $(this).parents('.ma_device_wrap').find('.ma_device_table_tab4').addClass('ma_device_table_head_inv').removeClass('ma_device_table_head');
    $(this).removeClass('ma_device_table_head_inv').addClass('ma_device_table_head');
    $(this).parents('.ma_device_wrap').find('.device_save').addClass('hidden');
    $(this).parents('.ma_device_wrap').find('.device_default').addClass('hidden');
});

$('.ma_device').on('click','.ma_device_table_tab2',function(){
    $(this).parents('.ma_device_wrap').find('.table2').removeClass('hidden');
    $(this).parents('.ma_device_wrap').find('.table1').addClass('hidden');
    $(this).parents('.ma_device_wrap').find('.table3').addClass('hidden');
    $(this).parents('.ma_device_wrap').find('.table4').addClass('hidden');
    $(this).parents('.ma_device_wrap').find('.ma_device_table_tab1').addClass('ma_device_table_head_inv').removeClass('ma_device_table_head');
    $(this).parents('.ma_device_wrap').find('.ma_device_table_tab3').addClass('ma_device_table_head_inv').removeClass('ma_device_table_head');
    $(this).parents('.ma_device_wrap').find('.ma_device_table_tab4').addClass('ma_device_table_head_inv').removeClass('ma_device_table_head');
    $(this).removeClass('ma_device_table_head_inv').addClass('ma_device_table_head');
    $(this).parents('.ma_device_wrap').find('.device_save').removeClass('hidden');
    $(this).parents('.ma_device_wrap').find('.device_default').removeClass('hidden');
});

$('.ma_device').on('click','.ma_device_table_tab3',function(){
    $(this).parents('.ma_device_wrap').find('.table3').removeClass('hidden');
    $(this).parents('.ma_device_wrap').find('.table1').addClass('hidden');
    $(this).parents('.ma_device_wrap').find('.table2').addClass('hidden');
    $(this).parents('.ma_device_wrap').find('.table4').addClass('hidden');
    $(this).parents('.ma_device_wrap').find('.ma_device_table_tab1').addClass('ma_device_table_head_inv').removeClass('ma_device_table_head');
    $(this).parents('.ma_device_wrap').find('.ma_device_table_tab2').addClass('ma_device_table_head_inv').removeClass('ma_device_table_head');
    $(this).parents('.ma_device_wrap').find('.ma_device_table_tab4').addClass('ma_device_table_head_inv').removeClass('ma_device_table_head');
    $(this).removeClass('ma_device_table_head_inv').addClass('ma_device_table_head');
    $(this).parents('.ma_device_wrap').find('.device_save').removeClass('hidden');
    $(this).parents('.ma_device_wrap').find('.device_default').removeClass('hidden');
});

$('.ma_device').on('click','.ma_device_table_tab4',function(){
    $(this).parents('.ma_device_wrap').find('.table4').removeClass('hidden');
    $(this).parents('.ma_device_wrap').find('.table1').addClass('hidden');
    $(this).parents('.ma_device_wrap').find('.table2').addClass('hidden');
    $(this).parents('.ma_device_wrap').find('.table3').addClass('hidden');
    $(this).parents('.ma_device_wrap').find('.ma_device_table_tab1').addClass('ma_device_table_head_inv').removeClass('ma_device_table_head');
    $(this).parents('.ma_device_wrap').find('.ma_device_table_tab2').addClass('ma_device_table_head_inv').removeClass('ma_device_table_head');
    $(this).parents('.ma_device_wrap').find('.ma_device_table_tab3').addClass('ma_device_table_head_inv').removeClass('ma_device_table_head');
    $(this).removeClass('ma_device_table_head_inv').addClass('ma_device_table_head');
    $(this).parents('.ma_device_wrap').find('.device_save').addClass('hidden');
    $(this).parents('.ma_device_wrap').find('.device_default').addClass('hidden');
});

function saveCal() {
    $("#storeButton").prop('disabled',true);
    $('html,body').addClass('wait');
    if(globalObj == undefined || globalObj == "")
    {
        change_status_text("Connect to device first");
        $("#storeButton").prop('disabled',false);
        return;
    }

    let currentValue = $('.ma_calibration .input_selector_bg').find('.input_selector_value').val();
    if(currentValue == "1")
    {
        $('.ma_calibration .input_selector_bg').find('.input_selector').animate({left:-1},{
            duration: 50
        });

        $('.ma_calibration .input_selector_bg').prev().addClass("offline");
        $('.ma_calibration .input_selector_bg').next().removeClass("online");
        $('.ma_calibration .input_selector_bg').find('.input_selector_value').val("0");

        $('.cal_table_tc').off('focus','.cal_me');
        $("#readButton").prop('disabled',false);
        $("#calibration_bank").text("");
        globalObj.manualCal();
    }

    globalObj.saveCal();
}

function readnative(){
    if($('#device_serial option:selected').attr('data-index') == undefined)
    {
        change_status_text("No device available for calibration");
        $("#readButton").prop('disabled',false);
        return;
    }

    if(globalObj == undefined || globalObj == "")
    {
        change_status_text("Connect to device first");
        $("#readButton").prop('disabled',false);
        return;
    }

    $("#readButton").prop('disabled',true);
    $('.cal_table_tc').removeClass('hidden');
    if($("#removeButton").attr('data-toggle') == 'cancel')
    {
        editRow();
    }
    globalObj.readCalSettings();
}

function resetCal(){
    $("#resetCalButton").prop('disabled',true).addClass("disabled");
    dialog.showMessageBox({
        type:"warning",
        buttons:["OK","Cancel"],
        message:"Your Calibration Data will be cleared",
        cancelId: 0,
    },function (op) {
        if(!op)
        {
            globalObj.clearCal();
        }
        else
        {
            $("#resetCalButton").prop('disabled',false).removeClass("disabled");
        }
    });
}

let PwrRelayTimer;
$('.ma_device').on('click','.input_selector_bg',function () {
    clearTimeout(PwrRelayTimer);
    let movePixel = $('.input_selector').width();
    let currentValue = $(this).find('.input_selector_value').val();
    if (currentValue == "0")
    {
        $(this).find('.input_selector').animate({left: (movePixel * 0.87)}, {
            duration: 50,
            complete: () => {
                $(this).prev().removeClass("offline");
                $(this).next().addClass("online");
                $(this).find('.input_selector_value').val("1");

                let that = $(this).parents('.ma_device_wrap');
                if(globalObj.neoRADIO_status == 2)
                {
                    globalObj.SetPwrRly(that);
                    PwrRelayTimer = setTimeout(function () {
                        globalObj.startDataPush();
                    },100);
                }
                else
                {
                    globalObj.SetPwrRly(that);
                }
            }
        });
    }
    else if(currentValue == "1")
    {
        $(this).find('.input_selector').animate({left:-1},{
            duration: 50,
            complete: () => {
                $(this).prev().addClass("offline");
                $(this).next().removeClass("online");
                $(this).find('.input_selector_value').val("0");

                let that = $(this).parents('.ma_device_wrap');
                if(globalObj.neoRADIO_status == 2)
                {
                    globalObj.SetPwrRly(that);
                    PwrRelayTimer = setTimeout(function () {
                        globalObj.startDataPush();
                    },100);
                }
                else
                {
                    globalObj.SetPwrRly(that);
                }
            }
        });
    }
});

$('.ma_calibration').on('click','.input_selector_bg',function () {
    let movePixel = $('.input_selector').width();
    let currentValue = $(this).find('.input_selector_value').val();
    if (currentValue == "0")
    {
        $(this).find('.input_selector').animate({left:(movePixel * 0.87)}, {
            duration: 50,
            complete: () => {
                $(this).prev().removeClass("offline");
                $(this).next().addClass("online");
                $(this).find('.input_selector_value').val("1");

                $('.cal_table_tc').on('focus','.cal_me',function () {
                    let calibration_bank = parseInt($(document.activeElement).attr('data-bank'));
                    $("#readButton").prop('disabled',true);
                    $("#exportButton").prop('disabled',true);
                    $("#importButton").prop('disabled',true);
                    $("#resetCalButton").prop('disabled',true);
                    $("#removeButton").prop('disabled',true);
                    globalObj.interactiveCal(calibration_bank);
                });
            }
        });
    }
    else if(currentValue == "1")
    {
        $(this).find('.input_selector').animate({left:-1},{
            duration: 50,
            complete: () => {
                $(this).prev().addClass("offline");
                $(this).next().removeClass("online");
                $(this).find('.input_selector_value').val("0");

                $('.cal_table_tc').off('focus','.cal_me');
                $("#readButton").prop('disabled',false);
                $("#exportButton").prop('disabled',false);
                $("#importButton").prop('disabled',false);
                $("#resetCalButton").prop('disabled',false);
                $("#removeButton").prop('disabled',false);
                $("#calibration_bank").text("");
                globalObj.manualCal();
            }
        });
    }
});

let AoutTimer;
$('.ma_device').on('blur','.neorad_aout_input',function () {
    clearTimeout(AoutTimer);
    let that = $(this).parents('.ma_device_wrap');
    let bank = $(this).parents('.em1').attr('data-bank');

    if($(this).val() < 0 || $(this).val() > 5)
    {
        $(this).val(0);
    }

    if(globalObj.neoRADIO_status == 2)
    {
        globalObj.SetAoutValue(that, bank);
        AoutTimer = setTimeout(function () {
            globalObj.startDataPush();
        },100);
    }
    else
    {
        globalObj.SetAoutValue(that, bank);
    }
});

$(".ma_device").on("focus",".neorad_aout_input", function () {
    $(this).select();
});

$('.ma_device').on('keyup','.neorad_aout_input',function (e) {
    if(e.which === 13)
    {
        clearTimeout(AoutTimer);
        let that = $(this).parents('.ma_device_wrap');
        let bank = $(this).parents('.em1').attr('data-bank');

        if($(this).val() < 0 || $(this).val() > 5)
        {
            $(this).val(0);
        }

        if(globalObj.neoRADIO_status == 2)
        {
            globalObj.SetAoutValue(that, bank);
            AoutTimer = setTimeout(function () {
                globalObj.startDataPush();
            },100);
        }
        else
        {
            globalObj.SetAoutValue(that, bank);
        }

        $(this).blur();
        if($(this).parents('.input-group').next().length === 0)
        {
            if($(this).parents('td').next().length === 0)
            {
                return;
            }
            $(this).parents('td').next().find('.neorad_aout_input')[0].focus();
        }
        else
        {
            $(this).parents('.input-group').next().children('.neorad_aout_input').focus();
        }
    }
});