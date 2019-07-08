/*
NEORADIO2_DEVTYPE_TC		= 0
NEORADIO2_DEVTYPE_DIO		= 1
NEORADIO2_DEVTYPE_PWRRLY	= 2
NEORADIO2_DEVTYPE_AIN		= 3
NEORADIO2_DEVTYPE_AOUT		= 4
NEORADIO2_DEVTYPE_CANHUB	= 5
NEORADIO2_DEVTYPE_BADGE	    = 6
NEORADIO2_DEVTYPE_HOST  	= 0xFF
*/
const english = {
    tab_device: "Devices",
    tab_graph: "Graph",
    tab_setting: "Settings",
    tab_help: "Help",
    tab_calibration: "Calibration",
    tab_dev_set: "Device Settings",
    tab_can_set: "CAN Settings",
    setting_lang: "Language",
    setting_temp: "Temperature unit",
    setting_rate: "Chart refresh rate (milliseconds)",
    setting_save: "Save changes",
    setting_close: "Close",
    setting_tip: "Needs restart to take effect",
    setting_plot: "Load All Plots",
    setting_on: "On",
    setting_off: "Off",
    setting_folder: "Open Data Folder",
    setting_crash_report: "Upload Crash Report",
    setting_auto_save: "Auto Save",
    setting_theme: "Theme",
    setting_light: "Light",
    setting_dark: "Dark",
    text_enable: "Enabled",
    text_disable: "Disabled",
    text_logging: "Logging",
    text_offline: "Offline",
    text_disc: "Sensor not connected",
    text_moreinfo: "Device Info",
    text_sample: "Polling rate(ms)",
    text_tagname: "Tag name",
    text_manufacture: "Manufactured in",
    text_firmware: "Firmware Version",
    text_onlinetime: "Online for",
    text_custom: "Custom",
    text_low: "Low",
    text_high: "High",
    text_state: "Initial State",
    text_ivoltage: "Initial Voltage",
    can_mode: "CAN Mode",
    can_auto: "Auto Mode",
    can_manual: "Manual Mode",
    can_type: "CAN type",
    can_type_classic: "Classic",
    can_type_fd: "CAN FD",
    can_type_std: "Standard",
    can_type_xtd: "Extended",
    can_id: "CAN ID",
    can_byte: "Byte Start",
    can_wdevice: "Whole Device",
    can_apply: "Apply Auto Fill",
    can_fill: "Auto Fill",
    can_startid: "Start ID",
    device_connect: "Find Device",
    device_reload: "Disconnect",
    device_startlog: "Go Online",
    device_stop: "Stop",
    device_save: "Save",
    device_group: "Group Select",
    device_default: "Default",
    graph_temp: "Temperature",
    graph_update: "Updating Graph",
    graph_start: "Start",
    graph_pause: "Pause",
    graph_plot: "Plots",
    graph_current: "Current",
    graph_load: "Load",
    graph_delete: "Delete",
    cal_serial: "Serial",
    cal_hint: "Use Arrow keys to navigate",
    cal_add: "Add",
    cal_order: "Order",
    cal_remove: "Edit Points",
    cal_cancel: "Cancel",
    cal_read: "Read",
    cal_store: "Store",
    cal_export: "Export",
    cal_import: "Import",
    cal_clear: "Clear",
    cal_interactive: "Interactive",
    cal_manual: "Manual",
    cal_stored: "Stored",
    cal_measured: "Measured",
    cal_error: "Error",
    cal_points: "Calibration points"
};

const japanese = {
    tab_device: "デバイス",
    tab_graph: "グラフ",
    tab_setting: "設定",
    tab_help: "ヘルプ",
    tab_calibration: "較正",
    tab_dev_set: "デバイス設定",
    tab_can_set: "CAN 設定",
    setting_lang: "言語",
    setting_temp: "温度単位",
    setting_rate: "グラフのリフレッシュレート (ミリ秒)",
    setting_save: "保存",
    setting_close: "閉じる",
    setting_tip: "再始動を有効にする必要がある",
    setting_plot: "全グラフを読み込む",
    setting_on: "オン",
    setting_off: "オフ",
    setting_folder: "データフォルダを開く",
    setting_crash_report: "クラッシュレポートをアップロード",
    setting_auto_save: "自動セーブ",
    setting_theme: "テーマ",
    setting_light: "ライト",
    setting_dark: "ダーク",
    text_enable: "启用",
    text_disable: "关闭",
    text_logging: "ロギング",
    text_offline: "オフライン",
    text_disc: "接続切断",
    text_moreinfo: "詳細情報",
    text_sample: "サンプルレート(ms)",
    text_tagname: "タグ名",
    text_manufacture: "製造年月日",
    text_firmware: "ファームウェアのバージョン",
    text_onlinetime: "オンライン時間",
    text_custom: "カスタム",
    text_low: "ロー",
    text_high: "ハイ",
    text_state: "状態",
    text_ivoltage: "初期電圧",
    can_mode: "CAN モード",
    can_auto: "自動モード",
    can_manual: "手動モード",
    can_type: "CAN タイプ",
    can_type_classic: "クラシック",
    can_type_fd: "CAN FD",
    can_type_std: "スタンダード",
    can_type_xtd: "拡張",
    can_id: "CAN ID",
    can_byte: "開始バイト",
    can_wdevice: "全部デバイス",
    can_apply: "アプライ",
    can_fill: "オートフィル",
    can_startid: "スタート ID",
    device_connect: "デバイスの検索",
    device_reload: "切断",
    device_startlog: "ログを開始",
    device_stop: "ストップ",
    device_save: "セーブ",
    device_group: "グループ選択",
    device_default: "既定値に戻す",
    graph_temp: "温度",
    graph_update: "更新中",
    graph_start: "スタート",
    graph_pause: "一時停止",
    graph_plot: "チャート",
    graph_current: "現在",
    graph_load: "ロード",
    graph_delete: "削除する",
    cal_serial: "シリアル",
    cal_hint: "Use Arrow keys to navigate",
    cal_add: "追加",
    cal_order: "並べ替え",
    cal_remove: "編集ポイント",
    cal_cancel: "キャンセル",
    cal_read: "読む",
    cal_store: "セーブ",
    cal_export: "エクスポート",
    cal_import: "インポート",
    cal_clear: "クリアー",
    cal_interactive: "インタラクティブ",
    cal_manual: "マニュアル",
    cal_stored: "保存しました",
    cal_measured: "測定済み",
    cal_error: "エラー",
    cal_points: "較正ポイント"
};

const korean = {
    tab_device: "Devices",
    tab_graph: "Graph",
    tab_setting: "Settings",
    tab_help: "Help",
    tab_calibration: "Calibration",
    tab_dev_set: "Device Settings",
    tab_can_set: "CAN Settings",
    setting_lang: "Language",
    setting_temp: "Temperature unit",
    setting_rate: "Chart refresh rate (milliseconds)",
    setting_save: "Save changes",
    setting_close: "Close",
    setting_tip: "Needs restart to take effect",
    setting_plot: "Load All Plots",
    setting_on: "On",
    setting_off: "Off",
    setting_folder: "Open Data Folder",
    setting_crash_report: "Upload Crash Report",
    setting_auto_save: "Auto Save",
    setting_theme: "Theme",
    setting_light: "Light",
    setting_dark: "Dark",
    text_enable: "Enabled",
    text_disable: "Disabled",
    text_logging: "Logging",
    text_offline: "Offline",
    text_disc: "Sensor not connected",
    text_moreinfo: "click for more info",
    text_sample: "Polling rate(ms)",
    text_tagname: "Tag name",
    text_manufacture: "Manufactured in",
    text_firmware: "Firmware Version",
    text_onlinetime: "Online for",
    text_custom: "Custom",
    text_low: "Low",
    text_high: "High",
    text_state: "State",
    text_ivoltage: "Initial Voltage",
    can_mode: "CAN Mode",
    can_auto: "Auto Mode",
    can_manual: "Manual Mode",
    can_type: "CAN type",
    can_type_classic: "Classic",
    can_type_fd: "CAN FD",
    can_type_std: "Standard",
    can_type_xtd: "Extended",
    can_id: "CAN ID",
    can_byte: "Byte Start",
    can_wdevice: "Whole Device",
    can_apply: "Apply",
    can_fill: "Auto Fill",
    can_startid: "Start ID",
    device_connect: "Find Device",
    device_reload: "Disconnect",
    device_startlog: "Go Online",
    device_stop: "Stop",
    device_save: "Save",
    device_group: "Group Select",
    device_default: "Default",
    graph_temp: "Temperature",
    graph_update: "Updating Graph",
    graph_start: "Start",
    graph_pause: "Pause",
    graph_plot: "Plots",
    graph_current: "Current",
    graph_load: "Load",
    graph_delete: "Delete",
    cal_serial: "Serial",
    cal_hint: "Use Arrow keys to navigate",
    cal_add: "Add",
    cal_order: "Order",
    cal_remove: "Edit Points",
    cal_cancel: "Cancel",
    cal_read: "Read",
    cal_store: "Store",
    cal_export: "Export",
    cal_import: "Import",
    cal_clear: "Clear",
    cal_interactive: "Interactive",
    cal_manual: "Manual",
    cal_stored: "Stored",
    cal_measured: "Measured",
    cal_error: "Error",
    cal_points: "Calibration points"
};

const chinese = {
    tab_device: "设备列表",
    tab_graph: "数据图表",
    tab_setting: "设置",
    tab_help: "帮助",
    tab_calibration: "标定",
    tab_dev_set: "设备设置",
    tab_can_set: "CAN设置",
    setting_lang: "语言",
    setting_temp: "温度单位",
    setting_rate: "图表刷新率 (毫秒)",
    setting_save: "保存设置",
    setting_close: "关闭",
    setting_tip: "生效需要重启软件",
    setting_plot: "加载所有图表",
    setting_on: "开启",
    setting_off: "关闭",
    setting_folder: "打开数据文件目录",
    setting_crash_report: "上传崩溃报告",
    setting_auto_save: "自动保存",
    setting_theme: "主题",
    setting_light: "默认",
    setting_dark: "黑暗",
    text_enable: "启用",
    text_disable: "未启用",
    text_logging: "记录中",
    text_offline: "离线",
    text_disc: "传感器未连接",
    text_moreinfo: "点击显示更多信息",
    text_sample: "采样率(毫秒)",
    text_tagname: "标签名",
    text_manufacture: "生产于",
    text_firmware: "固件版本",
    text_onlinetime: "在线时间",
    text_custom: "自定义",
    text_low: "低",
    text_high: "高",
    text_state: "初始状态",
    text_ivoltage: "初始电压",
    can_mode: "CAN 模式",
    can_auto: "自动模式",
    can_manual: "手动模式",
    can_type: "CAN 类型",
    can_type_classic: "经典",
    can_type_fd: "CAN FD",
    can_type_std: "标准",
    can_type_xtd: "扩展",
    can_id: "CAN ID",
    can_byte: "开始字节",
    can_wdevice: "全设备",
    can_apply: "应用",
    can_fill: "自动填充",
    can_startid: "起始 ID",
    device_connect: "寻找设备",
    device_reload: "断开连接",
    device_startlog: "开始",
    device_stop: "暂停",
    device_save: "保存设置",
    device_group: "全选",
    device_default: "默认设置",
    graph_temp: "温度",
    graph_update: "更新中",
    graph_start: "开始",
    graph_pause: "暂停",
    graph_plot: "图表",
    graph_current: "当前",
    graph_load: "加载",
    graph_delete: "删除",
    cal_serial: "序列号",
    cal_hint: "可以使用上下左右按键来快速切换单元格",
    cal_add: "添加",
    cal_order: "排序",
    cal_remove: "编辑",
    cal_cancel: "取消",
    cal_read: "读取",
    cal_store: "保存",
    cal_export: "导出",
    cal_import: "导入",
    cal_clear: "清除",
    cal_interactive: "互动模式",
    cal_manual: "手动模式",
    cal_stored: "预设",
    cal_measured: "测量",
    cal_error: "差值",
    cal_points: "标定点"
};

const Tchinese = {
    tab_device: "設備列表",
    tab_graph: "數據圖表",
    tab_setting: "設置",
    tab_help: "幫助",
    tab_calibration: "校準",
    tab_dev_set: "設備設置",
    tab_can_set: "CAN設置",
    setting_lang: "語言",
    setting_temp: "溫度單位",
    setting_rate: "圖表刷新率 (毫秒)",
    setting_save: "保存設置",
    setting_close: "關閉",
    setting_tip: "生效需要重啟程式",
    setting_plot: "加載所有圖表",
    setting_on: "開啟",
    setting_off: "關閉",
    setting_folder: "打開數據文件目錄",
    setting_crash_report: "上傳崩潰報告",
    setting_auto_save: "自動保存",
    setting_theme: "主題",
    setting_light: "默認",
    setting_dark: "黑暗",
    text_enable: "啟用",
    text_disable: "未啟用",
    text_logging: "記錄中",
    text_offline: "離線",
    text_disc: "傳感器未連接",
    text_moreinfo: "點擊顯示更多訊息",
    text_sample: "採樣率(毫秒)",
    text_tagname: "標籤名",
    text_manufacture: "生產于",
    text_firmware: "固件版本",
    text_onlinetime: "在線時間",
    text_custom: "自定義",
    text_low: "低",
    text_high: "高",
    text_state: "初始狀態",
    text_ivoltage: "初始電壓",
    can_mode: "CAN 模式",
    can_auto: "自動模式",
    can_manual: "手動模式",
    can_type: "CAN 類型",
    can_type_classic: "經典",
    can_type_fd: "CAN FD",
    can_type_std: "標準",
    can_type_xtd: "擴展",
    can_id: "CAN ID",
    can_byte: "開始字節",
    can_wdevice: "全設備",
    can_apply: "應用",
    can_fill: "自動填充",
    can_startid: "起始 ID",
    device_connect: "尋找設備",
    device_reload: "斷開鏈接",
    device_startlog: "開始",
    device_stop: "暫停",
    device_save: "保存設置",
    device_group: "全選",
    device_default: "默認設置",
    graph_temp: "溫度",
    graph_update: "更新中",
    graph_start: "開始",
    graph_pause: "暫停",
    graph_plot: "圖表",
    graph_current: "當前",
    graph_load: "加載",
    graph_delete: "刪除",
    cal_serial: "序列號",
    cal_hint: "可以使用上下左右按鍵來快速切換單元格",
    cal_add: "添加",
    cal_order: "排序",
    cal_remove: "編輯",
    cal_cancel: "取消",
    cal_read: "讀取",
    cal_store: "保存",
    cal_export: "導出",
    cal_import: "導入",
    cal_clear: "清除",
    cal_interactive: "互動模式",
    cal_manual: "手動模式",
    cal_stored: "預設",
    cal_measured: "測量",
    cal_error: "差值",
    cal_points: "標定點"
};

const locale = {
    "en": english,
    "ja": japanese,
    "jw": japanese,
    "ko": korean,
    "zh": chinese,
    "zh-CN": chinese,
    "zh-TW": Tchinese
};

let def_lang = english;
let storageLocale = settings.get('locale');
if(storageLocale && locale[storageLocale])
{
    def_lang = locale[storageLocale];
    $(`#language_select option[value="${storageLocale}"]`).prop('selected',true);
}
else if(locale[systemLan] !== undefined)
{
    def_lang = locale[systemLan];
    settings.set('locale',systemLan);
    $(`#language_select option[value="${systemLan}"]`).prop('selected',true);
}

$('#nav-link-device').text(def_lang.tab_device);
$('#nav-link-graph').text(def_lang.tab_graph);
$('#nav-link-help').text(def_lang.tab_help);
$('#nav-link-calibration').text(def_lang.tab_calibration);

$('#modal-title').text(def_lang.tab_setting);
$('.input-group-text[for="language_select"]').text(def_lang.setting_lang);
$('.input-group-text[for="temp_select"]').text(def_lang.setting_temp);
$('.input-group-text[for="push_rate"]').text(def_lang.setting_rate);
$('.input-group-text[for="plot_load"]').text(def_lang.setting_plot);
$('#plot_load option[value="1"]').text(def_lang.setting_on);
$('#plot_load option[value="0"]').text(def_lang.setting_off);
$('.input-group-text[for="cal_select"]').text(def_lang.tab_calibration);
$('#cal_select option[value="1"]').text(def_lang.setting_on);
$('#cal_select option[value="0"]').text(def_lang.setting_off);
$('.ma_calibration label[for="ain_low_cal"]').text(def_lang.text_low);
$('.ma_calibration label[for="ain_high_cal"]').text(def_lang.text_high);
$('.input-group-text[for="crash_select"]').text(def_lang.setting_crash_report);
$('#crash_select option[value="true"]').text(def_lang.setting_on);
$('#crash_select option[value="false"]').text(def_lang.setting_off);
$('.input-group-text[for="auto_save"]').text(def_lang.setting_auto_save);
$('#auto_save option[value="1"]').text(def_lang.setting_on);
$('#auto_save option[value="0"]').text(def_lang.setting_off);
$('.input-group-text[for="theme_select"]').text(def_lang.setting_theme);
$('#theme_select option[value="0"]').text(def_lang.setting_light);
$('#theme_select option[value="1"]').text(def_lang.setting_dark);
$('.modal-footer .btn-primary').text(def_lang.setting_save);
$('.modal-footer .btn-secondary').text(def_lang.setting_close);
$('.modal-body .input-group .btn-info').text(def_lang.setting_folder);
$('#modal_tip').text(def_lang.setting_tip);

$('#device_connect').text(def_lang.device_connect);
$('#device_reload').text(def_lang.device_reload);
$('#device_startlog').text(def_lang.device_startlog);
$('#device_stop').text(def_lang.device_stop);

$('#graph_update').text(def_lang.graph_update);
$('#tooltip_start').text(def_lang.graph_start);
$('#tooltip_pause').text(def_lang.graph_pause);
$('.ma_graph label[for="Plots"]').text(def_lang.graph_plot);
$('#Plots option[value="c"]').text(def_lang.graph_current);
$('.ma_graph .plot_padding .btn-success').text(def_lang.graph_load);
$('.ma_graph .plot_padding .btn-danger').text(def_lang.graph_delete);

$('#cal_hint').text(def_lang.cal_hint);
$('label[for="device_serial"]').text(def_lang.cal_serial);
$('#addButton').text(def_lang.cal_add);
$('#orderButton').text(def_lang.cal_order);
$('#removeButton').text(def_lang.cal_remove);
$('#readButton').text(def_lang.cal_read);
$('#storeButton').text(def_lang.cal_store);
$('#saveButton').text(def_lang.cal_store);
$('#exportButton').text(def_lang.cal_export);
$('#importButton').text(def_lang.cal_import);
$('#resetCalButton').text(def_lang.cal_clear);
$('#interactiveButton').text(def_lang.cal_interactive);
$('#manualButton').text(def_lang.cal_manual);

function template1(obj,index) {
    let deviceType, channelName1, channelName2;
    let bankIcon;
    let template_td = "";
    let can_td = "";
    let temp_template = "";

    function default_template(i, extrastuff) {
        if(!extrastuff)
        {
            extrastuff = "";
        }

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
                ${extrastuff}
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

    function can_out_template(i) {
        return `
            <td class="device_channel${i} em1" data-device="ma_device_c${i}">
                <div class="form-group neorad_can_mode">
                    <label>${def_lang.can_mode}:</label>
                    <select class="form-control form-control-sm ma_device_can_mode">
                        <option value="1">${def_lang.can_auto}</option>
                        <option value="0">${def_lang.can_manual}</option>
                    </select>
                </div>
                <div class="form-group neorad_can_id">
                    <label>${def_lang.can_id}:</label>
                    <select class="form-control form-control-sm device_check_status ma_device_can_id_type">
                        <option value="3">${def_lang.can_type_std}</option>
                        <option value="4">${def_lang.can_type_xtd}</option>
                    </select>
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

    function ain_template(i) {
        let _ain_template = `
            <div class="form-group neorad_ain_sample">
                <div class="form-check form-check-inline">
                    <input class="form-check-input neorad_ain_sample_input group_select_ain_radio" type="radio" name="ain_${index}_b${i}" id="ain_low_${index}_b${i}" value="low" checked>
                    <label class="form-check-label" for="ain_low_${index}_b${i}">${def_lang.text_low}</label>
                </div>
                <div class="form-check form-check-inline">
                    <input class="form-check-input neorad_ain_sample_input group_select_ain_radio" type="radio" name="ain_${index}_b${i}" id="ain_high_${index}_b${i}" value="high">
                    <label class="form-check-label" for="ain_high_${index}_b${i}">${def_lang.text_high}</label>
                </div>
                <select class="form-control form-control-sm ain_select ain_low_select group_select_ain_select" id="ain_low_select_${index}_b${i}">
                    <option value="1">&#177 250 mV</option>
                    <option value="2">&#177 1000 mV</option>
                    <option value="3">&#177 5000 mV</option>
                </select>
                <select class="form-control form-control-sm hidden ain_select ain_high_select group_select_ain_select" id="ain_high_select_${index}_b${i}">
                    <option value="4">&#177 10 V</option>
                    <option value="5">&#177 20 V</option>
                    <option value="6">&#177 50 V</option>
                </select>
            </div>
        `;

        return default_template(i, _ain_template);
    }

    function rly_template(i) {
        return `
            <td class="device_channel${i}" data-device="ma_device_c${i}">
                <div class="form-group neorad_tag">
                    <label for="device_check_tag${index}_${i}">${def_lang.text_tagname}:</label>
                    <input class="form-control form-control-sm ma_device_channel_tag" id="device_check_tag${index}_${i}" maxlength="14" pattern="[a-zA-Z0-9]" data-checkbox="device${index+1}_ch${i}" tabindex="${index * 8 + i}">
                </div>
                <div class="form-group neorad_rly_state">
                    <label>${def_lang.text_state}:</label>
                    <div class="form-check form-check-inline">
                        <input class="form-check-input neorad_rly_input" type="radio" name="rly_${index}_${i}" id="rly_off_${index}_${i}" value="0" checked disabled>
                        <label class="form-check-label" for="rly_off_${index}_${i}">${def_lang.setting_off}</label>
                    </div>
                    <div class="form-check form-check-inline">
                        <input class="form-check-input neorad_rly_input" type="radio" name="rly_${index}_${i}" id="rly_on_${index}_${i}" value="1" disabled>
                        <label class="form-check-label" for="rly_on_${index}_${i}">${def_lang.setting_on}</label>
                    </div>
                </div>
            </td>
        `;
    }

    function aout_template(i) {
        return `
            <td class="device_channel${i}" data-device="ma_device_c${i}">
                <div class="form-group neorad_tag">
                    <label for="device_check_tag${index}_${i}">${def_lang.text_tagname}:</label>
                    <input class="form-control form-control-sm ma_device_channel_tag" id="device_check_tag${index}_${i}" maxlength="14" pattern="[a-zA-Z0-9]" data-checkbox="device${index+1}_ch${i}" tabindex="${index * 8 + i}">
                </div>
                <label>${def_lang.text_ivoltage}</label>
                <div class="form-check">
                    <input type="checkbox" class="form-check-input device_check_status device_check_status1" id="aout_checkbox1_${index}_${i}" data-check="${i}" checked>
                    <label class="form-check-label ma_device_channel_status1" for="aout_checkbox1_${index}_${i}">${def_lang.text_enable}</label>
                </div>
                <div class="input-group input-group-sm mb-3 opacity">
                    <input class="form-control form-control-sm neorad_aout_initinput_1" type="number" step="0.001" placeholder="Channel 1" disabled>
                    <div class="input-group-prepend">
                        <label class="input-group-text form-control-sm">V</label>
                    </div>
                </div>
                <hr>
                <div class="form-check">
                    <input type="checkbox" class="form-check-input device_check_status device_check_status2" id="aout_checkbox2_${index}_${i}" data-check="${i}" checked>
                    <label class="form-check-label ma_device_channel_status2" for="aout_checkbox2_${index}_${i}">${def_lang.text_enable}</label>
                </div>
                <div class="input-group input-group-sm mb-3 opacity">
                    <input class="form-control form-control-sm neorad_aout_initinput_2"  type="number" step="0.001" placeholder="Channel 2" disabled>
                    <div class="input-group-prepend">
                        <label class="input-group-text form-control-sm">V</label>
                    </div>
                </div>
                <hr>
                <div class="form-check">
                    <input type="checkbox" class="form-check-input device_check_status device_check_status3" id="aout_checkbox3_${index}_${i}" data-check="${i}" checked>
                    <label class="form-check-label ma_device_channel_status3" for="aout_checkbox3_${index}_${i}">${def_lang.text_enable}</label>
                </div>
                <div class="input-group input-group-sm mb-3 opacity">
                    <input class="form-control form-control-sm neorad_aout_initinput_3" type="number" step="0.001" placeholder="Channel 3" disabled>
                    <div class="input-group-prepend">
                        <label class="input-group-text form-control-sm">V</label>
                    </div>
                </div>
            </td>
        `;
    }

    function _table_head(bankIcon,channelName1,channelName2){
        let th_temp = "";

        for (let i = 1; i < 9; i++)
        {
            if(i < 5)
            {
                th_temp += `
                <th class="ma_device_channel_head em15" data-deviceHead="ma_device_c${i}">
                    ${bankIcon}
                    ${channelName1}${i}
                </th>
                `
            }
            else
            {
                th_temp += `
                <th class="ma_device_channel_head em15" data-deviceHead="ma_device_c${i}">
                    ${bankIcon}
                    ${channelName2}${i}
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

    switch (obj.deviceType)
    {
        case 0:
            deviceType = "neoRAD-IO2-TC";
            channelName1 = channelName2 = "Bank";
            bankIcon = `<i class="fas fa-thermometer-quarter"></i>`;

            for (let i = 1; i < 9; i++)
            {
                template_td += default_template(i);
                can_td += can_template(i);
                temp_template += `
                <td class="device_channel${i}" data-device="ma_device_c${i}">
                    <div class="neorad_temp">
                        <div class="ma_device_channel_set ma_device_channel_temp em15"></div>
                    </div>
                </td>
                `;
            }
            break;
        case 2:
            deviceType = "neoRAD-IO2-PWRRLY";
            channelName1 = channelName2 = "Relay";
            bankIcon = '<i class="fas fa-bolt"></i>';

            can_td = `${can_out_template(1)}`;
            for (let i = 1; i < 9; i++)
            {
                temp_template += `
                <td class="device_channel${i}" data-device="ma_device_c${i}">
                    <div>
                        <div class="input_selector_text offline">${def_lang.setting_off}</div>
                        <div class="input_selector_bg" id="input_selector_bg_${index}_${i}">
                            <img class="input_selector" src="../img/slide_1.png">
                            <input class="input_selector_value hidden" id="input_selector_value_${index}_${i}" value="0" disabled>
                        </div>
                        <div class="input_selector_text">${def_lang.setting_on}</div>
                    </div>
                </td>
                `;
                template_td += rly_template(i);
                if(i !== 8)
                {
                    can_td += "<td></td>";
                }
            }
            break;
        case 3:
            deviceType = "neoRAD-IO2-AIN";
            channelName1 = channelName2 = "Bank";
            bankIcon = '<i class="fas fa-bolt"></i>';

            for (let i = 1; i < 9; i++)
            {
                template_td += ain_template(i);
                can_td += can_template(i);
                temp_template += `
                <td class="device_channel${i}" data-device="ma_device_c${i}">
                    <div class="neorad_temp">
                        <div class="ma_device_channel_set ma_device_channel_temp em15"></div>
                    </div>
                </td>
                `;
            }
            break;
        case 4:
            deviceType = "neoRAD-IO2-AOUT";
            channelName1 = channelName2 = "Bank";
            bankIcon = '<i class="fas fa-bolt"></i>';

            for (let i = 1; i < 9; i++)
            {
                template_td += aout_template(i);
                can_td += can_out_template(i);

                temp_template += `
                <td class="device_channel${i} em1" data-device="ma_device_c${i}" data-bank="${i}">
                    <div class="input-group input-group-sm mb-3 opacity">
                        <input class="form-control form-control-sm neorad_aout_input neorad_aout_input_1" type="number" step="0.001" placeholder="Channel 1">
                        <div class="input-group-prepend">
                            <label class="input-group-text form-control-sm">V</label>
                        </div>
                    </div>
                    <div class="input-group input-group-sm mb-3 opacity">
                        <input class="form-control form-control-sm neorad_aout_input neorad_aout_input_2" type="number" step="0.001" placeholder="Channel 2">
                        <div class="input-group-prepend">
                            <label class="input-group-text form-control-sm">V</label>
                        </div>
                    </div>
                    <div class="input-group input-group-sm mb-3 opacity">
                        <input class="form-control form-control-sm neorad_aout_input neorad_aout_input_3" type="number" step="0.001" placeholder="Channel 3">
                        <div class="input-group-prepend">
                            <label class="input-group-text form-control-sm">V</label>
                        </div>
                    </div>
                </td>
                `;
            }
            break;
        case 5:
            deviceType = "neoRAD-IO2-CANHUB";

            for (let i = 1; i < 9; i++)
            {
                template_td += default_template(i);
                can_td += can_template(i);
                temp_template += `
                <td class="device_channel${i}" data-device="ma_device_c${i}">
                    <div class="neorad_temp">
                        <div class="ma_device_channel_set ma_device_channel_temp em15"></div>
                    </div>
                </td>
                `;
            }
            break;
        default:
            break;
    }

    let table_head = _table_head(bankIcon,channelName1,channelName2);
    let returnTemplate = "";
    let templateDefaultButton = `
        <button type="button" class="btn btn-success device_default hidden" tabindex="-1">${def_lang.device_default}</button>
    `;
    if(obj.deviceType == 2)
    {
        templateDefaultButton = "";
    }

    if(obj.deviceType != 5)
    {
        returnTemplate =
            `
            <div class="flex">
                <div>
                    <div class="ma_device_table_head ma_device_table_tab1 em15 d-inline-block pointer">
                        <span class="device_type" data-deviceType="${obj.deviceType}">${deviceType}</span>
                        <span class="ma_device_serial_span">- ${obj.serialNumber}</span>
                    </div><!--
                 --><div class="ma_device_table_head_inv ma_device_table_tab2 em15 d-inline-block pointer">${def_lang.tab_dev_set}</div><!--
                 --><div class="ma_device_table_head_inv ma_device_table_tab3 em15 d-inline-block pointer">${def_lang.tab_can_set}</div><!--
                 --><div class="ma_device_table_head_inv ma_device_table_tab4 em15 d-inline-block pointer">${def_lang.text_moreinfo}</div>
                </div>
                <div>
                    <button type="button" class="btn btn-success device_save hidden" tabindex="-1">${def_lang.device_save}</button>
                    ${templateDefaultButton}
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
            `
    }
    else
    {
        returnTemplate =
            `
            <div class="flex">
                <div>
                    <div class="ma_device_table_head ma_device_table_tab1 em15 d-inline-block pointer">
                        <span class="device_type" data-deviceType="${obj.deviceType}">${deviceType}</span>
                        <span class="ma_device_serial_span">- ${obj.serialNumber}</span>
                    </div>
                </div>
                <div>
                    <button type="button" class="btn btn-success device_save_canhub">${def_lang.device_save}</button>
                </div>
            </div>
            
            <div>
                <div class="card card-body">
                    <div>${def_lang.text_manufacture} ${obj.manufacture_month}/${obj.manufacture_day}/${obj.manufacture_year}</div>
                    <div>${def_lang.text_firmware}: ${obj.firmwareVersion_major}.${obj.firmwareVersion_minor}</div>
                    <div>${def_lang.text_onlinetime}: <span class="ma_device_channel_time"></span></div>
                </div>
                <table class="table">
                    <tbody>
                        <tr>
                            <td>         
                                <div class="form-group">
                                    <label class="form-check-label">CAN</label>
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <div class="form-check">
                                        <input type="checkbox" class="form-check-input">
                                        <label class="form-check-label">${def_lang.text_enable}</label>
                                    </div>
                                    <div class="form-check">
                                        <input type="checkbox" class="form-check-input">
                                        <label class="form-check-label">Specify by Baud</label>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <label>Baud Rate</label>
                                    <select class="form-control form-control-sm">
                                        <option>20</option>
                                        <option>33</option>
                                        <option>50</option>
                                        <option>62</option>
                                        <option>83</option>
                                        <option>100</option>
                                        <option>125</option>
                                        <option>250</option>
                                        <option>500</option>
                                        <option>800</option>
                                        <option>1000</option>
                                        <option>666</option>
                                        <option>2000</option>
                                        <option>4000</option>
                                        <option>5000</option>
                                        <option>6667</option>
                                        <option>8000</option>
                                        <option>10000</option>
                                    </select>
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <label>TQ SEG1</label>
                                    <input class="form-control form-control-sm">
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <label>TQ SEG2</label>
                                    <input class="form-control form-control-sm">
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <label>TQ Prop</label>
                                    <input class="form-control form-control-sm">
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <label>Sync</label>
                                    <input class="form-control form-control-sm">
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <label>BRP-1</label>
                                    <input class="form-control form-control-sm">
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <label>Mode</label>
                                    <select class="form-control form-control-sm">
                                        <option value="0">NORMAL</option>
                                        <option value="1">icsDISABLE</option>
                                        <option value="2">LOOPBACK</option>
                                        <option value="3">LISTEN_ONLY</option>
                                        <option value="7">LISTEN_ALL</option>
                                        <option value="0x80">DEFAULT_NORMAL</option>
                                    </select>
                                </div>
                            </td>
                        </tr>
                        <tr>
                            <td>
                                 <div class="form-group">
                                     <label class="form-check-label">CAN FD</label>
                                 </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <div class="form-check">
                                        <input type="checkbox" class="form-check-input">
                                        <label class="form-check-label">${def_lang.text_enable}</label>
                                    </div>
                                    <div class="form-check">
                                        <input type="checkbox" class="form-check-input">
                                        <label class="form-check-label">ISO</label>
                                    </div>
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <label>Baud Rate</label>
                                    <select class="form-control form-control-sm">
                                        <option>20</option>
                                        <option>33</option>
                                        <option>50</option>
                                        <option>62</option>
                                        <option>83</option>
                                        <option>100</option>
                                        <option>125</option>
                                        <option>250</option>
                                        <option>500</option>
                                        <option>800</option>
                                        <option>1000</option>
                                        <option>666</option>
                                        <option>2000</option>
                                        <option>4000</option>
                                        <option>5000</option>
                                        <option>6667</option>
                                        <option>8000</option>
                                        <option>10000</option>
                                    </select>
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <label>TQ SEG1</label>
                                    <input class="form-control form-control-sm">
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <label>TQ SEG2</label>
                                    <input class="form-control form-control-sm">
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <label>TQ Prop</label>
                                    <input class="form-control form-control-sm">
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <label>Sync</label>
                                    <input class="form-control form-control-sm">
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <label>BRP-1</label>
                                    <input class="form-control form-control-sm">
                                </div>
                            </td>
                            <td>
                                <div class="form-group">
                                    <label>TDC</label>
                                    <input class="form-control form-control-sm">
                                </div>
                            </td>
                        </tr>
                    </tbody>
                </table>
            </div>
            `
    }

    return returnTemplate;
}

function template2(obj,index) {
    let deviceType, channelName1, channelName2;
    switch (obj.deviceType)
    {
        case 0: deviceType = "neoRAD-IO2-TC";
            channelName1 = channelName2 = "Bank";
            break;
        case 2: deviceType = "neoRAD-IO2-PWRRLY";
            channelName1 = channelName2 = "Relay";
            break;
        case 3: deviceType = "neoRAD-IO2-AIN";
            channelName1 = channelName2 = "Bank";
            break;
        case 4: deviceType = "neoRAD-IO2-AOUT";
            channelName1 = channelName2 = "Bank";
            break;
        case 5: deviceType = "neoRAD-IO2-CANHUB";
            break;
    }

    let template_td = "";
    for (let i = 1; i < 9; i++)
    {
        if(obj['enables'][`channel${i-1}`]['settingsEnables'] > "0")
        {
            template_td += `        
                <div class="form-check flex_no_align">
                    <input type="checkbox" class="form-check-input cb_device_checkbox" id="cb_device${index}_${channelName1}${i}" data-checkbox="device${index+1}_bank${i}" checked disabled>
                    <label for="cb_device${index}_${channelName1}${i}" class="form-check-label">${channelName1}${i}</label>
                    <span class="online" id="cbtemp_d${index}_c${i}"></span>
                </div>
            `
        }
        else
        {
            template_td += `        
                <div class="form-check hidden">
                    <input type="checkbox" class="form-check-input cb_device_checkbox" id="cb_device${index}_${channelName1}${i}" data-checkbox="device${index+1}_ch${i}" checked disabled>
                    <label for="cb_device${index}_${channelName1}${i}" class="form-check-label">${channelName1}${i}</label>
                </div>
            `
        }
    }

    return `            
        <div class="ma_graph_sidebar_nav ma_graph_sidebar_bg">
            <div class="ma_graph_sidebar_nav_title">
                <span class="ma_graph_sidebar_nav_title_device">${deviceType}</span>
                <span class="ma_graph_sidebar_nav_title_serial">-${obj.serialNumber}</span>
            </div>
            <div class="ma_graph_sidebar_nav_channel">
                <div class="form-group">
                    ${template_td}
                </div>
            </div>
        </div>
    `
}

function cal_row_temp(type, data, index, fCall) {
    let can_td = '';
    let tdh_extra = '';
    let td_extra = '';
    let floatPoints = 2;
    let cal_index = "";

    if(data !== null && index !== null)
    {
        cal_index = data['calpoints'][index];
    }
    else
    {
        cal_index = "";
    }

    if(cal_index === 0)
    {
        cal_index = "0";
    }

    if(fCall == "onclick")
    {
        td_extra = `<i class="fas fa-minus-square ${cal_index}" onclick="deleteRow(this)"></i>`
    }
    else
    {
        td_extra = `<i class="fas fa-minus-square ${cal_index ? "hidden" : ""}" onclick="deleteRow(this)"></i>`
    }

    switch (type)
    {
        case "0":
            if(cal_index && cal_index != "0")
            {
                if(typeof cal_index == "string")
                {
                    cal_index = parseFloat(cal_index);
                }
                cal_index = parseFloat(cal_index.toFixed(floatPoints));
            }
            tdh_extra = `<input type="number" class="cal_input_th" value="${cal_index}" min="-200" max="1372" ${cal_index ? "disabled" : "enabled"}> &#176C`;
            break;
        case "3":
            floatPoints = 5;
            if(cal_index && cal_index != "0")
            {
                if(typeof cal_index == "string")
                {
                    cal_index = parseFloat(cal_index);
                }
                cal_index = parseFloat(cal_index.toFixed(floatPoints));
            }
            tdh_extra = `<input type="number" class="cal_input_th" value="${cal_index}" ${cal_index ? "disabled" : "enabled"}> V`;
            break;
        case "4":
            floatPoints = 5;
            if(cal_index && cal_index != "0")
            {
                if(typeof cal_index == "string")
                {
                    cal_index = parseInt(cal_index);
                }
                cal_index = parseFloat((cal_index / 3355443).toFixed(floatPoints));
            }
            tdh_extra = `<input type="number" class="cal_input_th" value="${cal_index}" ${cal_index ? "disabled" : "enabled"}> V`;
            break;
    }

    if(type == "4")
    {
        for(let i = 1; i < 9; i++)
        {
            let value = "";
            if(data !== null && index !== null)
            {
                if(data['datapoints'] == undefined || data == undefined || data['datapoints'][i-1] == undefined)
                {
                    return
                }

                value = parseFloat((data['datapoints'][i-1][index] / 3355443).toFixed(floatPoints));
            }
            can_td += `
            <td>
                <div class="flex">
                    <input class="cal_input cal_stored" disabled placeholder="${def_lang.cal_stored}" value="${value}">
                    <input class="cal_input cal_me" placeholder="Enter Here" data-bank="${i}" value="${value}">
                    <input class="cal_input cal_diff" disabled>
                </div>
            </td>
        `;
        }

        return `
        <tr>
            <td>
                ${tdh_extra}
            </td>
            <td>
                <input class="cal_input cal_stored" disabled value="${def_lang.cal_stored}">
                <input class="cal_input" disabled value="${def_lang.cal_measured}">
                <input class="cal_input offline" disabled value="${def_lang.cal_error}">
                ${td_extra}
            </td>
            ${can_td}
        </tr>
    `;
    }
    else
    {
        for(let i = 1; i < 9; i++)
        {
            let value = "";
            if(data !== null && index !== null)
            {
                if(data['datapoints'] == undefined || data == undefined || data['datapoints'][i-1] == undefined)
                {
                    return
                }
                value = parseFloat(data['datapoints'][i-1][index].toFixed(floatPoints));
            }
            can_td += `
            <td>
                <div class="flex">
                    <input class="cal_input cal_stored" disabled placeholder="${def_lang.cal_stored}" value="${value}">
                    <input class="cal_input cal_me" placeholder="Enter Here" data-bank="${i}" value="${value}">
                    <input class="cal_input cal_diff" disabled>
                </div>
            </td>
        `;
        }

        return `
        <tr>
            <td>
                ${tdh_extra}
            </td>
            <td>
                <input class="cal_input cal_stored" disabled value="${def_lang.cal_stored}">
                <input class="cal_input" disabled value="${def_lang.cal_measured}">
                <input class="cal_input offline" disabled value="${def_lang.cal_error}">
                ${td_extra}
            </td>
            ${can_td}
        </tr>
    `;
    }
}

function alert_template(str) {
    return `
    <div class="alert alert-danger alert-dismissible fade show" role="alert">
        ${str}
    </div>
    `;
}

function append_cal_table(data) {
    let table = "";

    function tableTemp(cal_rows, index){
        return `
        <table class="table table-striped cal_table_ hidden" id="cal_table_${index}" data-type="${data['type']}">
            <thead class="thead-dark">
            <tr>
                <th class="cal_th_header">${def_lang.cal_points}</th>
                <th>
                    <span class="calibration_bank"></span>
                </th>
                <th>Bank1</th>
                <th>Bank2</th>
                <th>Bank3</th>
                <th>Bank4</th>
                <th>Bank5</th>
                <th>Bank6</th>
                <th>Bank7</th>
                <th>Bank8</th>
            </tr>
            <tbody>
                ${cal_rows}
            </tbody>
            <tfoot>
            <tr>
                <td>
                    <i class="fas fa-plus-square hidden" onclick="addRow(${index})"></i>
                </td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
            </tr>
            </tfoot>
        </table>
        `
    }

    let data_length = Object.keys(data['data']).length;
    for(let datapoints = 0; datapoints < data_length; datapoints++){
        let cal_row = "";
        let datapoints_length = data['data'][datapoints]['calpoints'].length;
        for(let i = 0; i < datapoints_length; i++)
        {
            cal_row += cal_row_temp($('#device_serial option:selected').val(), data['data'][datapoints], i);
        }

        table += tableTemp(cal_row,datapoints);
    }

    return table;
}

function append_cal_table_aout(data) {
    let table = "";

    function tableTemp(cal_rows, index){
        return `
        <table class="table table-striped cal_table_ hidden" id="cal_table_${index}" data-type="${data['type']}">
            <thead class="thead-dark">
            <tr>
                <th class="cal_th_header" data-channel="${index}">Channel ${index+1}</th>
                <th>
                    <span id="calibration_bank"></span>
                </th>
                <th>Bank1</th>
                <th>Bank2</th>
                <th>Bank3</th>
                <th>Bank4</th>
                <th>Bank5</th>
                <th>Bank6</th>
                <th>Bank7</th>
                <th>Bank8</th>
            </tr>
            <tbody>
                ${cal_rows}
            </tbody>
            <tfoot>
            <tr>
                <td>
                    <i class="fas fa-plus-square hidden" onclick="addRow(${index})"></i>
                </td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
                <td></td>
            </tr>
            </tfoot>
        </table>
        `
    }

    let data_length = Object.keys(data['data']).length;
    for(let datapoints = 0; datapoints < data_length; datapoints++){
        let cal_row = "";
        let datapoints_length = data['data'][datapoints]['calpoints'].length;
        for(let i = 0; i < datapoints_length; i++)
        {
            cal_row += cal_row_temp($('#device_serial option:selected').val(), data['data'][datapoints], i);
        }

        table += tableTemp(cal_row,datapoints);
    }

    return table;
}