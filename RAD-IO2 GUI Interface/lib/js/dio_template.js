let dio_template = {};

dio_template.generate_template = function(obj,index){
    function din_template(i) {
        return `
            <td class="device_channel${i} em1" data-device="ma_device_c${i}">
                <div class="form-group neorad_tag">
                    <label for="device_check_tag${index}_${i}">${def_lang.text_tagname}:</label>
                    <input class="form-control form-control-sm ma_device_channel_tag" id="device_check_tag${index}_${i}" maxlength="14" pattern="[a-zA-Z0-9]" data-checkbox="device${index+1}_ch${i}" tabindex="${index * 8 + i}">
                </div>
                <div class="form-group neorad_enable">
                    <div class="form-check">
                        <input type="checkbox" class="form-check-input device_check_status" id="device_check_status${index}_${i}" data-check="${i}" checked>
                        <label for="device_check_status${index}_${i}" class="form-check-label ma_device_channel_status">${def_lang.text_enable}</label>
                    </div>
                </div>
                <div class="form-group neorad_sample">
                    <label for="device_check_pullrate${index}_${i}">${def_lang.text_sample}:</label>
                    <select class="form-control form-control-sm group_select device_check_pullrate_select" id="device_check_pullrate${index}_${i}">
                        <option value="10">10 ms</option>
                        <option value="20">20 ms</option>
                        <option value="50">50 ms</option>
                        <option value="100">100 ms</option>
                        <option value="200">200 ms</option>
                        <option value="500">500 ms</option>
                        <option value="1000">1000 ms</option>
                        <option value="2000">2000 ms</option>
                        <option value="5000">5000 ms</option>
                        <option value="0">${def_lang.text_custom}</option>
                    </select>
                    <div class="input-group input-group-sm mb-3 opacity">
                        <input type="number" class="form-control form-control-sm group_select ma_device_channel_pullrate" value="1000" disabled>
                        <div class="input-group-prepend">
                            <label class="input-group-text form-control-sm">ms</label>
                        </div>
                    </div>
                </div>
                <div class="form-group">
                    <div class="form-group">
                        <label for="dio_v_trig_1_${index}_${i}">Voltage trigger:</label>
                        <div class="input-group input-group-sm mb-3 opacity">
                            <input type="number" class="form-control form-control-sm" id="dio_v_trig_1_${index}_${i}">
                            <div class="input-group-prepend">
                                <label class="input-group-text form-control-sm">V</label>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="form-group">
                    <label for="dio_mode_select_1_${index}_${i}">CH1 Mode:</label>
                    <select class="form-control form-control-sm dio_mode_select" id="dio_mode_select_1_${index}_${i}">
                        <option value="1">Digital Input</option>
                        <option value="2">PWM input</option>
                        <option value="3">Period</option>
                        <option value="4">Frequency</option>
                        <option value="5">Analog</option>
                    </select>
                    <div class="input-group input-group-sm mb-3 opacity">
                        <input type="number" class="form-control form-control-sm dio_mode_select_input">
                        <div class="input-group-prepend">
                            <label class="input-group-text form-control-sm dio_mode_select_label hidden"></label>
                        </div>
                    </div>
                </div>
                <div class="form-group">
                    <div class="form-group">
                        <label for="dio_v_trig_1_${index}_${i}">CH1 Clock prescaler:</label>
                        <div class="input-group input-group-sm mb-3 opacity">
                            <input type="number" class="form-control form-control-sm" id="dio_v_trig_1_${index}_${i}">
                            <div class="input-group-prepend">
                                <label class="input-group-text form-control-sm">Hz</label>
                            </div>
                        </div>
                    </div>
                </div>
                <hr>
                <div class="form-group">
                    <label for="dio_mode_select_2_${index}_${i}">CH2 Mode:</label>
                    <select class="form-control form-control-sm dio_mode_select" id="dio_mode_select_2_${index}_${i}">
                        <option value="1">Digital Input</option>
                        <option value="2">PWM input</option>
                        <option value="3">Period</option>
                        <option value="4">Frequency</option>
                        <option value="5">Analog</option>
                    </select>
                    <div class="input-group input-group-sm mb-3 opacity">
                        <input type="number" class="form-control form-control-sm dio_mode_select_input">
                        <div class="input-group-prepend">
                            <label class="input-group-text form-control-sm dio_mode_select_label hidden"></label>
                        </div>
                    </div>
                </div>
                <div class="form-group">
                    <div class="form-group">
                        <label for="dio_v_trig_2_${index}_${i}">CH2 Clock prescaler:</label>
                        <div class="input-group input-group-sm mb-3 opacity">
                            <input type="number" class="form-control form-control-sm" id="dio_v_trig_2_${index}_${i}">
                            <div class="input-group-prepend">
                                <label class="input-group-text form-control-sm">Hz</label>
                            </div>
                        </div>
                    </div>
                </div>
                <hr>
                <div class="form-group">
                    <label for="dio_mode_select_3_${index}_${i}">CH3 Mode:</label>
                    <select class="form-control form-control-sm dio_mode_select" id="dio_mode_select_3_${index}_${i}">
                        <option value="1">Digital Input</option>
                        <option value="2">PWM input</option>
                        <option value="3">Period</option>
                        <option value="4">Frequency</option>
                        <option value="5">Analog</option>
                    </select>
                    <div class="input-group input-group-sm mb-3 opacity">
                        <input type="number" class="form-control form-control-sm dio_mode_select_input">
                        <div class="input-group-prepend">
                            <label class="input-group-text form-control-sm dio_mode_select_label hidden"></label>
                        </div>
                    </div>
                </div>
                <div class="form-group">
                    <div class="form-group">
                        <label for="dio_v_trig_3_${index}_${i}">CH3 Clock prescaler:</label>
                        <div class="input-group input-group-sm mb-3 opacity">
                            <input type="number" class="form-control form-control-sm" id="dio_v_trig_3_${index}_${i}">
                            <div class="input-group-prepend">
                                <label class="input-group-text form-control-sm">Hz</label>
                            </div>
                        </div>
                    </div>
                </div>
            </td>
        `;
    }

    function dout_template(i) {
        return `
            <td class="device_channel${i} em1" data-device="ma_device_c${i}">
                <div class="form-group neorad_tag">
                    <label for="device_check_tag${index}_${i}">${def_lang.text_tagname}:</label>
                    <input class="form-control form-control-sm ma_device_channel_tag" id="device_check_tag${index}_${i}" maxlength="14" pattern="[a-zA-Z0-9]" data-checkbox="device${index+1}_ch${i}" tabindex="${index * 8 + i}">
                </div>
                <div class="form-group neorad_enable">
                    <div class="form-check">
                        <input type="checkbox" class="form-check-input device_check_status" id="device_check_status${index}_${i}" data-check="${i}" checked>
                        <label for="device_check_status${index}_${i}" class="form-check-label ma_device_channel_status">${def_lang.text_enable}</label>
                    </div>
                </div>
                <div class="form-group neorad_sample">
                    <label for="device_check_pullrate${index}_${i}">${def_lang.text_sample}:</label>
                    <select class="form-control form-control-sm group_select device_check_pullrate_select" id="device_check_pullrate${index}_${i}">
                        <option value="10">10 ms</option>
                        <option value="20">20 ms</option>
                        <option value="50">50 ms</option>
                        <option value="100">100 ms</option>
                        <option value="200">200 ms</option>
                        <option value="500">500 ms</option>
                        <option value="1000">1000 ms</option>
                        <option value="2000">2000 ms</option>
                        <option value="5000">5000 ms</option>
                        <option value="0">${def_lang.text_custom}</option>
                    </select>
                    <div class="input-group input-group-sm mb-3 opacity">
                        <input type="number" class="form-control form-control-sm group_select ma_device_channel_pullrate" value="1000" disabled>
                        <div class="input-group-prepend">
                            <label class="input-group-text form-control-sm">ms</label>
                        </div>
                    </div>
                </div>
                <div class="form-group">
                    <label for="dout_ch1_select${index}_${i}">CH1 State:</label>
                    <select class="form-control form-control-sm dout_ch1_select" id="dout_ch1_select${index}_${i}">
                        <option value="1">Low</option>
                        <option value="2">High</option>
                        <option value="3">HighZ</option>
                    </select>
                </div>
                <div class="form-group">
                    <label for="dout_ch2_select${index}_${i}">CH2 State:</label>
                    <select class="form-control form-control-sm dout_ch2_select" id="dout_ch2_select${index}_${i}">
                        <option value="1">Low</option>
                        <option value="2">High</option>
                        <option value="3">HighZ</option>
                    </select>
                </div>
                <div class="form-group">
                    <div class="form-group">
                        <label for="dout_pwm${index}_${i}">PWM frequency:</label>
                        <div class="input-group input-group-sm mb-3 opacity">
                            <input class="form-control form-control-sm dout_pwm" id="dout_pwm${index}_${i}">
                            <div class="input-group-prepend">
                                <label class="input-group-text form-control-sm">Hz</label>
                            </div>
                        </div>
                    </div>
                </div>
                <div class="form-group">
                    <label for="dout_one_shot${index}_${i}">One Shot:</label>
                    <select class="form-control form-control-sm dout_one_shot" id="dout_one_shot${index}_${i}">
                        <option value="1">Count</option>
                        <option value="2">Time</option>
                    </select>
                </div>
                <div class="form-group">
                    <label for="hbridge_mode_select${index}_${i}">H bridge Mode:</label>
                    <select class="form-control form-control-sm hbridge_mode_select" id="hbridge_mode_select${index}_${i}">
                        <option value="1">Forward</option>
                        <option value="2">Reverse</option>
                        <option value="3">HighZ</option>
                        <option value="4">Break</option>
                    </select>
                </div>
            </td>
        `;
    }

    function can_template(i) {
        return `
            <td class="device_channel${i} em1" data-device="ma_device_c${i}">
                <div class="form-group neorad_can_mode">
                    <label>${def_lang.can_mode}:</label>
                    <select class="form-control form-control-sm ma_device_can_mode">
                        <option value="1">${def_lang.can_auto}</option>
                        <option value="0">${def_lang.can_manual}</option>
                    </select>
                </div>
                <div class="form-group neorad_can_set">
                    <label>${def_lang.can_type}:</label>
                    <select class="form-control form-control-sm device_check_status ma_device_can_type">
                        <option value="1">${def_lang.can_type_classic}</option>
                        <option value="2">${def_lang.can_type_fd}</option>
                    </select>
                    <select class="form-control form-control-sm device_check_status ma_device_can_id_type">
                        <option value="3">${def_lang.can_type_std}</option>
                        <option value="4">${def_lang.can_type_xtd}</option>
                    </select>
                </div>
                <div class="form-group neorad_can_id">
                    <label>${def_lang.can_id}:</label>
                    <input class="form-control form-control-sm device_check_status ma_device_can_id" pattern="[a-fA-F0-9]" placeholder="${def_lang.can_id}">
                    <div class="can_id_alert"></div>
                </div>
                <div class="form-group neorad_can_msg">
                    <label>${def_lang.can_byte}:</label>
                    <input class="form-control form-control-sm device_check_status ma_device_can_byte" pattern="[a-fA-F0-9]" placeholder="${def_lang.can_byte}">
                    <div class="byte_id_alert"></div>
                </div>
            </td>
        `;
    }

    function _table_head(){
        let th_temp = "";
        for (let i = 1; i < 9; i++)
        {
            if(i < 5)
            {
                th_temp += `
                <th class="ma_device_channel_head em15" data-deviceHead="ma_device_c${i}">
                    <i class="fas fa-wave-square"></i>
                    DIN${i}
                </th>
                `
            }
            else
            {
                th_temp += `
                <th class="ma_device_channel_head em15" data-deviceHead="ma_device_c${i}">
                    <i class="fas fa-wave-square"></i>
                    DOUT${i}
                </th>
                `
            }
        }

        return `
        <thead class="thead-dark">
        <tr>
            ${th_temp}
        </tr>
        </thead>
        `;
    }

    let template_td = "";
    let can_td = "";
    let temp_template = "";
    let table_head = _table_head();

    for (let i = 1; i < 9; i++)
    {
        can_td += can_template(i);
        if(i < 5)
        {
            template_td += din_template(i);
            temp_template += `
                <td class="device_channel${i} em1" data-device="ma_device_c${i}" data-bank="${i}">
                    <label for="">Mode</label>
                    <div class="input-group input-group-sm mb-3 opacity">
                        <input class="form-control form-control-sm" type="number" step="0.001" placeholder="Channel 1">
                        <div class="input-group-prepend">
                            <label class="input-group-text form-control-sm">V</label>
                        </div>
                    </div>
                    <label for="">Mode</label>
                    <div class="input-group input-group-sm mb-3 opacity">
                        <input class="form-control form-control-sm" type="number" step="0.001" placeholder="Channel 2">
                        <div class="input-group-prepend">
                            <label class="input-group-text form-control-sm">V</label>
                        </div>
                    </div>
                    <label for="">Mode</label>
                    <div class="input-group input-group-sm mb-3 opacity">
                        <input class="form-control form-control-sm" type="number" step="0.001" placeholder="Channel 3">
                        <div class="input-group-prepend">
                            <label class="input-group-text form-control-sm">V</label>
                        </div>
                    </div>
                </td>
                `;
        }
        else
        {
            template_td += dout_template(i);
            temp_template += `
                <td class="device_channel${i} em1" data-device="ma_device_c${i}" data-bank="${i}">
                    <label for="">Mode</label>
                    <div class="input-group input-group-sm mb-3 opacity">
                        <input class="form-control form-control-sm" type="number" step="0.001" placeholder="Channel 1" disabled>
                        <div class="input-group-prepend">
                            <label class="input-group-text form-control-sm">V</label>
                        </div>
                    </div>
                    <label for="">Mode</label>
                    <div class="input-group input-group-sm mb-3 opacity">
                        <input class="form-control form-control-sm" type="number" step="0.001" placeholder="Channel 2" disabled>
                        <div class="input-group-prepend">
                            <label class="input-group-text form-control-sm">V</label>
                        </div>
                    </div>
<!--                    <label for="">Mode</label>-->
<!--                    <div class="input-group input-group-sm mb-3 opacity">-->
<!--                        <input class="form-control form-control-sm" type="number" step="0.001" placeholder="Channel 3" disabled>-->
<!--                        <div class="input-group-prepend">-->
<!--                            <label class="input-group-text form-control-sm">V</label>-->
<!--                        </div>-->
<!--                    </div>-->
                </td>
                `;
        }

    }

    return  `
             <div class="flex">
                <div>
                    <div class="ma_device_table_head ma_device_table_tab1 em15 d-inline-block pointer">
                        <span class="device_type" data-deviceType="${obj.deviceType}">neoRAD-IO2-DIO</span>
                        <span class="ma_device_serial_span">- ${obj.serialNumber}</span>
                    </div><!--
                 --><div class="ma_device_table_head_inv ma_device_table_tab2 em15 d-inline-block pointer">${def_lang.tab_dev_set}</div><!--
                 --><div class="ma_device_table_head_inv ma_device_table_tab3 em15 d-inline-block pointer">${def_lang.tab_can_set}</div><!--
                 --><div class="ma_device_table_head_inv ma_device_table_tab4 em15 d-inline-block pointer">${def_lang.text_moreinfo}</div>
                </div>
                <div>
                    <button type="button" class="btn btn-success device_save hidden" tabindex="-1">${def_lang.device_save}</button>
                    <button type="button" class="btn btn-success device_default hidden" tabindex="-1">${def_lang.device_default}</button>
                </div>
            </div>
            
            <table class="table table1">
                ${table_head}
                <tbody>
                </tbody>
            </table>
            
            <table class="table table2 hidden">
                ${table_head}
                <tbody>
                <tr class="row1">
                    ${template_td}
                </tr>
                </tbody>
                <tfoot>
                    <td class="em1">${def_lang.device_group}</td>
                    <td><button type="button" class="btn btn-info btn-sm device_group disabled" data-type="${obj.deviceType}">${def_lang.setting_off}</button></td>
                    <td></td><td></td><td></td><td></td><td></td><td></td>
                </tfoot>
            </table>
            
            <table class="table table3 hidden">
                ${table_head}
                <tbody>
                <tr class="row2">
                    ${can_td}
                </tr>
                </tbody>
                <tfoot>
                    <td class="em1">${def_lang.can_fill}</td>
                    <td>
                        <select class="form-control form-control-sm can_fill_select">
                            <option value="1">1 Bank</option>
                            <option value="2">2 Bank</option>
                            <option value="3">${def_lang.can_wdevice}</option>
                        </select>
                    </td>
                    <td class="em1">${def_lang.can_startid}</td>
                    <td>
                        <input class="form-control form-control-sm can_fill_id" pattern="[a-fA-F0-9]">
                    </td>
                    <td>
                        <button type="button" class="btn btn-info btn-sm can_fill_apply">${def_lang.can_apply}</button>
                    </td>
                    <td></td><td></td><td></td>
                </tfoot>
            </table>
            
            <table class="table table4 hidden">
                <div class="table4 hidden">
                    <div class="card card-body em1">
                        <div>${def_lang.text_manufacture} ${obj.manufacture_month}/${obj.manufacture_day}/${obj.manufacture_year}</div>
                        <div>${def_lang.text_firmware}: ${obj.firmwareVersion_major}.${obj.firmwareVersion_minor}</div>
                        <div>${def_lang.text_onlinetime}: <span class="ma_device_channel_time"></span></div>
                    </div>
                </div>
            </table>
            
            <table class="table table5">
                <tbody>
                <tr>
                    ${temp_template}
                </tr>
                </tbody>
            </table>
    `;
};

dio_template.events = function(){
    $(".ma_device").on('change', '.dio_mode_select',function () {
        switch ($(this).val()) {
            case "1":
                $(this).next().find('.dio_mode_select_label').addClass('hidden');
                $(this).next().find('.dio_mode_select_input').val("");
                break;
            case "2":
                $(this).next().find('.dio_mode_select_label').removeClass('hidden').text("%");
                $(this).next().find('.dio_mode_select_input').val("");
                break;
            case "3":
                $(this).next().find('.dio_mode_select_label').removeClass('hidden').text("Hz");
                $(this).next().find('.dio_mode_select_input').val("");
                break;
            case "4":
                $(this).next().find('.dio_mode_select_label').addClass('hidden');
                $(this).next().find('.dio_mode_select_input').val("");
                break;
            case "5":
                $(this).next().find('.dio_mode_select_label').addClass('hidden');
                $(this).next().find('.dio_mode_select_input').val("");
                break;
            case "6":
                $(this).next().find('.dio_mode_select_label').addClass('hidden');
                $(this).next().find('.dio_mode_select_input').val("");
                break;
        }
    });

    $(".ma_device").on("focus",".dio_mode_select_input", function () {
        $(this).select();
    });

    $(".ma_device").on("blur",".dio_mode_select_input", function () {
        let mode = $(this).parents('td').find('.dio_mode_select').val();
        switch (mode) {
            case "1":
                break;
            case "2":
                if($(this).val() < 0 || $(this).val() > 100)
                {
                    $(this).val("");
                }
                break;
            case "3":
                if($(this).val() < 0 || $(this).val() > 65535)
                {
                    $(this).val("");
                }
                break;
            case "4":
                break;
            case "5":
                break;
            case "6":
                break;
        }
    });

    $(".ma_device").on('change', '.dout_ch1_select',function () {
        if($(this).val() == "2")
        {

        }
    });

    $(".ma_device").on('change', '.dout_ch2_select',function () {
        $(this).val();
    });
};

module.exports = dio_template;