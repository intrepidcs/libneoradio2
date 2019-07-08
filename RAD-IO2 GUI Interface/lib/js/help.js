const english_help = `
<h3>Menus</h3>  
<br>
<h5>View</h5>
<p>Open Data Folder - This will open Finder/File Explorer and show where all the data files are saved.</p>
<br>
<h3>Tabs</h3>
<br>
<h5>Device Page</h5>
<p>Device table consists 4 tabs. Each tab shows different information accordingly. </p>
<p>1. Settings for each bank is in "Device Settings" tab.</p>
<p>2. You can set CAN settings in "CAN Settings".</p>
<p>3. "Device Info" shows hardware firmware version.</p>
<p>You can only save settings after devices found, or when you stop polling data</p>
<br>
<h5>Graph Page</h5>
<p> You can start Plotting after your devices go online.</p>
<p>All Plot data is saved as a JSON file to local storage. You can access this folder using the "View" Menu</p>
<p>You can load previous plots by selecting it and press load.</p>
<br>
<h5>Calibration</h5>
<p>Calibration can be turned on by setting it to "On" via the cog on the top right corner.</p>
<p>You need to read before you do any changes.</p>
<p>You can use interactive mode for faster calibration. When using interactive mode, read data will be updated for the current focused bank.</p>
<br>
<h5>Click the cog on the top right hand side for settings.</h5>
<p>You need to restart the program for settings to take effect. Except for Calibration.</p>
<p>Temperature unit only applies to readings on GUI.</p>
`;

const japanese_help = english_help;

const korean_help = english_help;

const chinese_help = english_help;

const help_locale = {
    "en": english_help,
    "en-AU": english_help,
    "en-CA": english_help,
    "en-GB": english_help,
    "en-NZ": english_help,
    "en-US": english_help,
    "en-ZA": english_help,
    "ja": japanese_help,
    "jw": japanese_help,
    "ko": korean_help,
    "zh": chinese_help,
    "zh-CN": chinese_help,
    "zh-TW": chinese_help
};

module.exports = help_locale;