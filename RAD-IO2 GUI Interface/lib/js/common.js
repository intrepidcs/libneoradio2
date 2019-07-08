$(document).ready(() => {
    let rate = settings.get('rate');
    refreshRate = rate;
    if(rate == 100 || rate == 200 || rate == 1000 || rate == 2000 || rate == 5000)
    {
        $(`#push_rate option[value="${rate}"]`).prop('selected',true)
    }

    if(settings.get("tempUnit") && settings.get("tempUnit") == 1)
    {
        $('#temp_select option[value="1"]').prop('selected',true);
    }

    if(settings.get("plot") && settings.get("plot") == 0)
    {
        $('#plot_load option[value="0"]').prop('selected',true);
    }

    if(settings.get("cal") && settings.get("cal") == 1)
    {
        $('#cal_select option[value="1"]').prop('selected',true);
        $('.nav_calibration').removeClass('hidden');
    }

    if(settings.get("crash") && settings.get("crash") == "true")
    {
        $('#crash_select option[value="true"]').prop('selected',true);
    }

    if(settings.get("save") && settings.get("save") == "1")
    {
        $('#auto_save option[value="1"]').prop('selected',true);
    }

    if(settings.get("theme") && settings.get("theme") == "1")
    {
        $('#theme_select option[value="1"]').prop('selected',true);
        $('.main_area').addClass('theme-black-bg');
        $('#modal_set').addClass('theme-grey-bg');
        $('#modal_about').addClass('theme-grey-bg');
        plot_bg_color = "#000";
        plot_font_color = "#fff";
    }

    const markdowndata = require('../js/help');
    let locale = settings.get('locale');
    if(!locale)
    {
        locale = "en";
    }
    $("#ma_info").html(markdowndata[locale]);

    $(document).on('visibilitychange', function()
    {
        if(document.visibilityState == 'hidden')
        {
            console.log('hidden');
        }
        else
        {
            console.log('showing');
        }
    });
});

// environment control-----------------------------------------
$(document).on('click','a[href^="http"]', function (e){
    e.preventDefault();
    Shell.openExternal(this.href);
});

//listen to user drag and drop files and parse the dropped file
document.addEventListener('drop', e => {
    e.preventDefault();
    e.stopPropagation();
});

//preventing user from dragging text over
document.addEventListener('dragover', e => {
    e.preventDefault();
    e.stopPropagation();
});
// environment control end-------------------------------------

// tab view change
$('.nav_device').on('click', () => {
    $('.ma').addClass('hidden');
    $('.nav-item').removeClass('active');
    $('.nav_device').addClass('active');
    $('.ma_device').removeClass('hidden');
});

$('.nav_graph').on('click',() => {
    $('.ma').addClass('hidden');
    $('.nav-item').removeClass('active');
    $('.nav_graph').addClass('active');
    $('.ma_graph').removeClass('hidden');

    if(!plotlyInit)
    {
        resizePlot();
    }
});

$('.nav_info').on('click',() => {
    $('.ma').addClass('hidden');
    $('.nav-item').removeClass('active');
    $('.nav_info').addClass('active');
    $('.ma_info').removeClass('hidden');
});

$('.nav_calibration').on('click', () => {
    $('.ma').addClass('hidden');
    $('.nav-item').removeClass('active');
    $('.nav_calibration').addClass('active');
    $('.ma_calibration').removeClass('hidden');
});

$('.nav_setting').on('click',() => {
    $('#modal_set').modal({
        keyboard: false
    });
});

function clickEffect(){
    this.classList.add('cbutton-click');
    this.addEventListener('animationend', function () {
        this.classList.remove('cbutton-click');
    });
}

const cbutton = document.querySelectorAll(".cbutton");
[...cbutton].forEach(arg => arg.addEventListener('click',clickEffect));

function closeWindow() {
    app.getCurrentWindow().close();
}

ipcRenderer.on('about', function() {
    $('#modal_about').modal({
        keyboard: false
    });
});

ipcRenderer.on('device', function() {
    $('.nav_device').click();
});

ipcRenderer.on('graph', function() {
    $('.nav_graph').click();
});

ipcRenderer.on('help', function() {
    $('.nav_info').click();
});

ipcRenderer.on('setting', function() {
    $('.nav_setting').click();
});

$('.ma_device, .ma_calibration').on('change','.neorad_ain_sample_input',function () {
    if($(this).val() == "low")
    {
        $(this).parents('.neorad_ain_sample').find(`.ain_low_select`).removeClass('hidden');
        $(this).parents('.neorad_ain_sample').find(`.ain_high_select`).addClass('hidden');
    }
    else if($(this).val() == "high")
    {
        $(this).parents('.neorad_ain_sample').find(`.ain_low_select`).addClass('hidden');
        $(this).parents('.neorad_ain_sample').find(`.ain_high_select`).removeClass('hidden');
    }
});

$('.ma_device').on('change','.device_check_pullrate_select',function () {
    if($(this).val() == 0)
    {
        $(this).next().find('input').prop('disabled', false);
        $(this).next().removeClass('opacity');
    }
    else
    {
        $(this).next().find('input').prop('disabled', true);
        $(this).next().find('input').val($(this).val());
        $(this).next().addClass('opacity');
    }
});

$('.ma_device').on('blur','.ma_device_can_id',function () {
    let hex = parseInt($(this).val()).toFixed(0);
    let type = $(this).parents('td').find('.ma_device_can_id_type').val();
    let alerthtml = $(this).parent().find('.can_id_alert');

    if(hex == "NaN")
    {
        console.log('not a number');
    }

    if(type == "3")
    {
        if(hex > 0x7FF)
        {
            $(alerthtml).html(`${alert_template('Invalid ID')}`);
        }
    }

    if(type == "4")
    {
        if(hex > 0x7FFFFFF)
        {
            $(alerthtml).html(`${alert_template('Invalid ID')}`);
        }
    }

    setTimeout(function () {
        $(alerthtml).html("");
    },3000);
    can_validation();
});

$('.ma_device').on('change','.ma_device_can_type',function () {
    $(this).parents('td').find('.can_id_alert').html('');
    $(this).parents('tr').find('.ma_device_can_type').val($(this).val());
});

$('.ma_device').on('change','.ma_device_can_mode',function () {
    if($(this).val() == "1")
    {
        $(this).parents('td').find('.ma_device_can_type').prop("disabled",true);
        $(this).parents('td').find('.ma_device_can_id_type').prop("disabled",true);
        $(this).parents('td').find('.ma_device_can_id').prop("disabled",true);
        $(this).parents('td').find('.ma_device_can_byte').prop("disabled",true);
    }
    else if($(this).val() == "0")
    {
        $(this).parents('td').find('.ma_device_can_type').prop("disabled",false);
        $(this).parents('td').find('.ma_device_can_id_type').prop("disabled",false);
        $(this).parents('td').find('.ma_device_can_id').prop("disabled",false);
        $(this).parents('td').find('.ma_device_can_byte').prop("disabled",false);
    }
});

$('.ma_device').on('blur','.ma_device_can_byte',function () {
    let value = parseInt($(this).val()).toFixed(0);
    let type = $(this).parents('td').find('.ma_device_can_type').val();
    let alerthtml = $(this).parent().find('.byte_id_alert');

    if(type == "1")
    {
        if(value > 8)
        {
            $(alerthtml).html(`${alert_template('Invalid value')}`);
        }
    }

    if(type == "2")
    {
        if(value > 64)
        {
            $(alerthtml).html(`${alert_template('Invalid value')}`);
        }
    }

    setTimeout(function () {
        $(alerthtml).html("");
    },3000);
});

$('#device_serial').on('change',function () {
    switch ($(this).val()) {
        case "0":
            $(this).parent('.sidepanel_input_group').next('.cal_ain_range').addClass('hidden');
            break;
        case "2":
            $(this).parent('.sidepanel_input_group').next('.cal_ain_range').addClass('hidden');
            break;
        case "3":
            $(this).parent('.sidepanel_input_group').next('.cal_ain_range').removeClass('hidden');
            break;
    }
});

$('.cal_table_tc_tables').on('blur','.cal_me',function () {
    let first = parseFloat($(this).closest('tr').find('.cal_input_th').val());
    let second = parseFloat($(this).val());

    let type = parseInt($('#device_serial option:selected').val());
    let final;
    switch (type) {
        case 0:
            final = parseFloat((first - second).toFixed(2));
            break;
        case 3:
            final = parseFloat((first - second).toFixed(5));
            break;
    }

    if(final == "NaN")
    {
        final = 0;
    }
    $(this).next().val(final);
});

$('.cal_table_tc_tables').on('keydown','input',function (ev) {
    if(ev.which > 36 && ev.which < 41)
    {
        ev.preventDefault();
    }
});

$('.cal_table_tc_tables').on('keyup',function (e) {
    let td = $(e.target).closest('td');
    let tr = td.closest('tr');
    let pos = td[0].cellIndex;
    if(e.which === 38) //up
    {
        let moveToRow = tr.prev('tr');
        if (moveToRow.length)
        {
            $(moveToRow[0].cells[pos]).find('input').focus();
        }
    }

    if(e.which === 40) //down
    {
        let moveToRow = tr.next('tr');
        if (moveToRow.length)
        {
            $(moveToRow[0].cells[pos]).find('input').focus();
        }
    }

    if(e.which === 37) //left
    {
        $(e.target).closest('td').prev().find('input').focus();
    }

    if(e.which === 39) //right
    {
        $(e.target).closest('td').next().find('input').focus();
    }
});

$('.cal_table_tc_tables').on('keyup','.cal_input_th',function (e) {
    if(e.which === 13)
    {
        organize();
        $(`.cal_input_th[value="${$(this).val()}"]`).focus();
    }
});

$(".cal_table_tc_tables").on("focus",".cal_input, .cal_input_th", function () {
    $(this).select();
});

$("#ain_low_cal, #ain_high_cal, #ain_low_select_cal, #ain_high_select_cal").on("change",function () {
    let AIN_select = $('.cal_ain_range').find(`.neorad_ain_sample_input:checked`).val();
    let AIN_range = "";
    if(AIN_select == "low")
    {
        AIN_range = parseInt($('.cal_ain_range').find('.ain_low_select').val());
    }
    else if(AIN_select == "high")
    {
        AIN_range = parseInt($('.cal_ain_range').find('.ain_high_select').val());
    }

    $('.cal_table_').addClass("hidden");
    $(`#cal_table_${AIN_range}`).removeClass("hidden");

    organize();
});

function closeModal() {
    let language = $('#language_select').val();
    let temperature = $('#temp_select').val();
    let rate = $('#push_rate').val();
    let plot = $('#plot_load').val();
    let cal = $('#cal_select').val();
    let crash = $('#crash_select').val();
    let save = $('#auto_save').val();
    let theme = $('#theme_select').val();
    settings.set('locale', language);
    settings.set('tempUnit',temperature);
    settings.set('rate',rate);
    settings.set('plot',plot);
    settings.set('cal',cal);
    settings.set('crash',crash);
    settings.set('save',save);
    settings.set('theme',theme);
    isFahrenheit = temperature;
    refreshRate = rate;
    $('#modal_set').modal('hide');

    if(cal == 1)
    {
        $('.nav_calibration').removeClass('hidden');
    }
    else if(cal == 0)
    {
        $('.nav_calibration').addClass('hidden');
    }

    if(theme == 1)
    {
        $('.main_area').addClass('theme-black-bg');
        $('#modal_set').addClass('theme-grey-bg');
        $('#modal_about').addClass('theme-grey-bg');
        plot_bg_color = "#000";
        plot_font_color = "#fff";
    }
    else if(theme == 0)
    {
        $('.main_area').removeClass('theme-black-bg');
        $('#modal_set').removeClass('theme-grey-bg');
        $('#modal_about').removeClass('theme-grey-bg');
        plot_bg_color = "#fff";
        plot_font_color = "#000";
    }
}

function cancelModal() {
    let language = "en", temperature = 0, rate = 100, plot = 1, cal = 0, crash = false, save = 0, theme = 0;

    if(settings.get('locale'))
    {
        language = settings.get('locale');
    }
    if(settings.get('tempUnit'))
    {
        temperature = settings.get('tempUnit');
    }
    if(settings.get('rate'))
    {
        rate = settings.get('rate');
    }
    if(settings.get('plot'))
    {
        plot = settings.get('plot');
    }
    if(settings.get('cal'))
    {
        cal = settings.get('cal');
    }
    if(settings.get('crash'))
    {
        crash = settings.get('crash');
    }
    if(settings.get('save'))
    {
        save = settings.get('save');
    }
    if(settings.get('theme'))
    {
        theme = settings.get('theme');
    }

    $(`#language_select option[value="${language}"]`).prop('selected',true);
    $(`#temp_select option[value="${temperature}"]`).prop('selected',true);
    $(`#push_rate option[value="${rate}"]`).prop('selected',true);
    $(`#plot_load option[value="${plot}"]`).prop('selected',true);
    $(`#cal_select option[value="${cal}"]`).prop('selected',true);
    $(`#crash_select option[value="${crash}"]`).prop('selected',true);
    $(`#auto_save option[value="${save}"]`).prop('selected',true);
    $(`#theme_select option[value="${theme}"]`).prop('selected',true);
}

function openFolder() {
    let Path = path.join(mypath, `IntrepidCS\/neoRAD-IO2`);
    Shell.showItemInFolder(Path);
}

function Dialog(type) {
    dialog.showMessageBox({
        type:"warning",
        buttons:[template[Locale]['reload_ok']],
        message: template[Locale][`${type}`]
    });
}

function can_validation() {
    let max_device = globalObj.device_found.maxID_Device;
    for(let i = 0; i < max_device; i++)
    {
        let id_node = Array.from($(`.device${i} .ma_device_can_id`));
        let byte_node = Array.from($(`.device${i} .ma_device_can_byte`));
        let id_node_length = id_node.length;
        for(let j = 0; j < id_node_length; j++)
        {
            for(let k = 0; k < id_node_length; k++){
                if(id_node[j].value != 0)
                {
                    if(id_node[j].value == id_node[k].value)
                    {
                        if(byte_node[j] - byte_node[k] < 4 && byte_node[k] - byte_node[j] < 4){
                            console.log("overlap error");
                        }
                    }
                }
            }
        }
    }
}

function change_status_text(str) {
    $('.navbar_items').hide();
    $('.statustext').html(`<span>${str}</span>`);
    $('.statustext span').delay(3000).fadeOut(1000,function () {
        $('.navbar_items').show();
    });
}

function difference() {
    $('.cal_diff').each(function (index, elem) {
        let first = parseFloat($(elem).closest('tr').find('.cal_input_th').val());
        let second = parseFloat($(elem).prev().val());

        let type = parseInt($('#device_serial option:selected').val());
        let final;
        switch (type) {
            case 0:
                final = parseFloat((first - second).toFixed(2));
                break;
            case 3:
                final = parseFloat((first - second).toFixed(5));
                break;
        }

        if(final == "NaN")
        {
            final = 0;
        }
        $(elem).val(final);
    })
}

function addRow(Parent) {
    let parent = $(`.cal_table_tc_tables #cal_table_${Parent}`);
    let row = $(`#cal_table_${Parent} .cal_input_th`).length;

    if(row > 10)
    {
        change_status_text("Maximum points reached");
        return;
    }

    switch ($('#device_serial').val())
    {
        case "0" :
            $($(parent).find('tbody')).append(`${cal_row_temp($('#device_serial option:selected').val(),null,null, addRow.caller.name)}`);
            break;
        case "3" :
            $($(parent).find('tbody')).append(`${cal_row_temp($('#device_serial option:selected').val(),null,null, addRow.caller.name)}`);
            break;
        default:
            break;
    }

    $(`#cal_table_${Parent} .cal_input_th`).focus();
    $('.ma_calibration').scrollTop($(document).height());
}

function editRow() {
    let parent = $('.cal_table_tc_tables');
    if(parent.length > 0)
    {
        $(parent).find('i').toggleClass('hidden');
        $('#addButton').prop('disabled', !$('#addButton').prop('disabled'));
        if($("#removeButton").attr('data-toggle') == "edit")
        {
            $('#removeButton').text(def_lang.cal_cancel);
            $("#removeButton").attr('data-toggle','cancel');
            $('.ma_calibration .cal_input_th').prop('disabled',false);
        }
        else
        {
            $('#removeButton').text(def_lang.cal_remove);
            $("#removeButton").attr('data-toggle','edit');
            $('.ma_calibration .cal_input_th').prop('disabled',true);
        }
    }
}

function organize() {
    $('#addButton').prop('disabled', true);
    $('#removeButton').prop('disabled', true);
    let parent = $('.cal_table_tc_tables .cal_table_:not(.hidden)');
    if(parent.length > 0)
    {
        $(".cal_table_:not(.hidden) .cal_input_th").each(function (index,element) {
            $(element).attr('value',$(element).val());
        });

        $(".cal_table_:not(.hidden) .cal_input").each(function (index,element) {
            $(element).attr('value',$(element).val());
        });

        let tempA = Array.from($(".cal_table_:not(.hidden) .cal_input_th")).sort((a, b) => {
            let A = $(a).val();
            let B = $(b).val();
            if ((A - B) === 0)
            {
                $(b).remove();
            }
            return A - B;
        });

        let nodeList = Array.from(tempA, x => $(x).closest('tr')[0]);
        $('.cal_table_:not(.hidden) tbody').html('');
        nodeList.map(function (value) {
            if(value){
                $('.cal_table_:not(.hidden) tbody').append(`<tr>${$(value).html()}</tr>`);
            }
        });
    }
    $('#addButton').prop('disabled', false);
    $('#removeButton').prop('disabled', false);
}

function deleteRow(that) {
    $(that).parents('tr').remove();
}

function saveJSON() {
    let currentSerial = $('#device_serial option:selected').text();
    let tableOBJ = {};

    if($(".cal_table_tc_tables table").attr('data-type') != $('#device_serial option:selected').val())
    {
        change_status_text('Calibration table incorrect');
        return;
    }

    let tablelength = $(".cal_table_").length;
    for(let i = 0; i < tablelength; i++)
    {
        if(tablelength == 1)
        {
            let rowlength = $(`#cal_table_${i} .cal_input_th`).length;
            for(let j = 0; j < rowlength; j++)
            {
                let a = $($(`#cal_table_${i} .cal_input_th`)[j]).val();
                let b = $($(`#cal_table_${i} .cal_input_th`)[j]).closest('tr').find('td');
                tableOBJ[`${a}`] = {};

                for(let k = 1; k < 9; k++)
                {
                    tableOBJ[`${a}`][`bank${k}`] = parseFloat($(b[k+1]).find('.cal_me').val())
                }
            }
        }
        else
        {
            let rangeNumber = $(`.cal_wrapper .cal_ain_range .ain_select option[value="${i}"]`).attr('data-range');
            if(rangeNumber > 100)
            {
                rangeNumber = rangeNumber / 1000;
            }

            tableOBJ[`${rangeNumber}V`] = {};
            let rowlength = $(`#cal_table_${i} .cal_input_th`).length;
            for(let j = 0; j < rowlength; j++)
            {
                let a = $($(`#cal_table_${i} .cal_input_th`)[j]).val();
                let b = $($(`#cal_table_${i} .cal_input_th`)[j]).closest('tr').find('td');
                tableOBJ[`${rangeNumber}V`][`${a}`] = {};

                for(let k = 1; k < 9; k++)
                {
                    tableOBJ[`${rangeNumber}V`][`${a}`][`bank${k}`] = parseFloat($(b[k+1]).find('.cal_me').val())
                }
            }
        }
    }

    let saveJSON = {
        serial: currentSerial,
        type: $('#device_serial option:selected').val(),
        data: tableOBJ
    };

    let fileName = saveJSON.serial;
    let myPath = path.join(mypath, `IntrepidCS\/neoRAD-IO2\/${fileName}.json`);

    dialog.showSaveDialog({
        title:'Save JSON',
        defaultPath: myPath
    },(fileName) => {
        if (fileName === undefined){
            dialog.showMessageBox({
                type:"warning",
                message:"You didn't save the file"
            });
            return;
        }
        fs.writeFile(fileName, JSON.stringify(saveJSON, null, 2), (err) => {
            if (err) throw err;
            dialog.showMessageBox({
                type:"info",
                message:"File saved successfully"
            });
        });
    });
}

function readJSON() {
    let serial = $('#device_serial option:selected').text();
    let myPath = path.join(mypath, `IntrepidCS\/neoRAD-IO2\/${serial}.json`);

    dialog.showOpenDialog({
        title:'Open JSON',
        defaultPath: myPath,
        filters:[{name:'JSON file', extensions: ['json','JSON']}],
        properties:['openFile']
    }, (fileNames)=>{
        if (fileNames === undefined) return;
        let fileName = fileNames[0];
        let dataObj = "";
        fs.readFile(fileName, 'utf8', function (err, data) {
            if (err) throw err;
            dataObj = JSON.parse(data);
            if(dataObj["serial"] != serial)
            {
                change_status_text('Serial different from file');
                return
            }

            let data_stream = {
                type:parseInt(dataObj["type"]),
            };

            switch (data_stream.type) {
                case 0:
                    data_stream["data"] = {0:{}};
                    data_stream["data"][0]['calpoints'] = [];
                    data_stream["data"][0]['datapoints'] = {
                        0:{datapoints:[]},
                        1:{datapoints:[]},
                        2:{datapoints:[]},
                        3:{datapoints:[]},
                        4:{datapoints:[]},
                        5:{datapoints:[]},
                        6:{datapoints:[]},
                        7:{datapoints:[]}
                    };
                    let obj_length = Object.keys(dataObj["data"]).length;
                    for(let  i = 0; i < obj_length; i++)
                    {
                        data_stream["data"][0]['calpoints'].push(parseFloat(Object.keys(dataObj["data"])[i]));
                        for(let k = 0; k < 8; k++)
                        {
                            data_stream["data"][0]['datapoints'][k]['datapoints'].push(Object.values(dataObj["data"])[i][`bank${k+1}`]);
                        }

                    }

                    break;
                case 3:
                    data_stream["data"] = {
                        0:{
                            calpoints:[],
                            datapoints:{
                                0:{datapoints:[]},
                                1:{datapoints:[]},
                                2:{datapoints:[]},
                                3:{datapoints:[]},
                                4:{datapoints:[]},
                                5:{datapoints:[]},
                                6:{datapoints:[]},
                                7:{datapoints:[]}
                            }
                        },
                        1:{
                            calpoints:[],
                            datapoints:{
                                0:{datapoints:[]},
                                1:{datapoints:[]},
                                2:{datapoints:[]},
                                3:{datapoints:[]},
                                4:{datapoints:[]},
                                5:{datapoints:[]},
                                6:{datapoints:[]},
                                7:{datapoints:[]}
                            }},
                        2:{
                            calpoints:[],
                            datapoints:{
                                0:{datapoints:[]},
                                1:{datapoints:[]},
                                2:{datapoints:[]},
                                3:{datapoints:[]},
                                4:{datapoints:[]},
                                5:{datapoints:[]},
                                6:{datapoints:[]},
                                7:{datapoints:[]}
                            }
                        },
                        3:{
                            calpoints:[],
                            datapoints:{
                                0:{datapoints:[]},
                                1:{datapoints:[]},
                                2:{datapoints:[]},
                                3:{datapoints:[]},
                                4:{datapoints:[]},
                                5:{datapoints:[]},
                                6:{datapoints:[]},
                                7:{datapoints:[]}
                            }
                        },
                        4:{
                            calpoints:[],
                            datapoints:{
                                0:{datapoints:[]},
                                1:{datapoints:[]},
                                2:{datapoints:[]},
                                3:{datapoints:[]},
                                4:{datapoints:[]},
                                5:{datapoints:[]},
                                6:{datapoints:[]},
                                7:{datapoints:[]}
                            }
                        },
                        5:{
                            calpoints:[],
                            datapoints:{
                                0:{datapoints:[]},
                                1:{datapoints:[]},
                                2:{datapoints:[]},
                                3:{datapoints:[]},
                                4:{datapoints:[]},
                                5:{datapoints:[]},
                                6:{datapoints:[]},
                                7:{datapoints:[]}
                            }
                        }
                    };

                    for(let  i = 0; i < 6; i++)
                    {
                        data_stream["data"][i]["calpoints"] = Object.keys(Object.values(dataObj["data"])[i]);
                        for(let l = 0; l < 8; l++)
                        {
                            let obj_length = Object.keys(Object.values(dataObj["data"])[i]).length;
                            for(let k = 0; k < obj_length; k++)
                            {
                                let calpoints = Object.values(Object.values(dataObj["data"])[i])[k];
                                data_stream["data"][i]["datapoints"][l]["datapoints"][k] = Object.values(calpoints)[l];
                            }
                        }
                    }

                    console.log(data_stream);
                    break;
                default:
                    break;
            }

            $('.cal_table_tc_tables').html('');
            $('.cal_table_tc_tables').append(append_cal_table(data_stream));

            switch (data_stream["type"]) {
                case 0:
                    $('.cal_table_').removeClass("hidden");
                    break;
                case 3:
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
                    break;
                default:
                    break;
            }

            difference();
            organize();

            $(".ma_calibration button").prop('disabled',false);
        });
    });
}

// crash report
const electron = require('electron');
const {crashReporter} = electron;
let upload = false;
if(settings.get('crash') && settings.get('crash') == "true")
{
    upload = true;
}
crashReporter.start({
    productName: "Electron",
    companyName: "gdinod",
    submitURL: "https://submit.backtrace.io/gdinod/193e5505e1c918a555bfd6ce330ad1478b8fedde652392499efe2057e0d40e8d/minidump",
    uploadToServer: upload
});
// crash report end