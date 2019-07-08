const electron = require('electron');
const {BrowserWindow, app, ipcMain, Menu, dialog, shell} = electron;
const settings = require('electron-settings');

const path = require('path');
const url = require('url');
const isDev = require('electron-is-dev');

const windowStateKeeper = require('electron-window-state');

let mainWindow;
let menu;
let DevENV = false;

if(process.platform === 'linux')
{
    const sudo = require('sudo-prompt');
    let options = {
      name: 'Electron'
    };
    sudo.exec(`cd /etc/udev/rules.d && touch 99-hidraw-permissions.rules && echo 'KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0666" ' >> 99-hidraw-permissions.rules`, options,
      function(error, stdout, stderr) {
        if (error) throw error;
        console.log('stdout: ' + stdout);
      }
    );
}

app.on('ready', function() {
    let {width, height} = electron.screen.getPrimaryDisplay().workAreaSize;

    if(width < 1336)
    {
        width = 1024;
    }

    if(height < 768)
    {
        height = 574;
    }

    let mainWindowState = windowStateKeeper({
        defaultWidth: width,
        defaultHeight: height
    });

    if (isDev)
    {
        DevENV = true;
    }

    mainWindow = new BrowserWindow({
        x: mainWindowState.x,
        y: mainWindowState.y,
        width: mainWindowState.width,
        height: mainWindowState.height,
        minWidth: 1024,
        minHeight: 574,
        title: "Data logger",
        backgroundColor: "#a09ea1",
        webPreferences: {
            nodeIntegration: true,
            devTools:DevENV
        }
    });

    if (isDev)
    {
        // Open the DevTools.
        mainWindow.webContents.openDevTools();
    }

    mainWindowState.manage(mainWindow);
    loadMain("canvas");

    mainWindow.on('closed', () => {
        mainWindow = null;
    });

    mainWindow.on('unresponsive',() => {
        console.log('page hanged');
    });

    setMenu();
    Menu.setApplicationMenu(menu);
});

app.on('window-all-closed', () => {
    app.quit();
});

ipcMain.on('get-locale', (event, arg) => {
    event.returnValue = app.getLocale();
});

ipcMain.on('get-path', (event, arg) => {
    event.returnValue = app.getPath('home');
});

function loadMain(page){
    mainWindow.loadURL(url.format({
        pathname: path.join(__dirname, `/lib/html/${page}.html`),
        protocol: 'file:',
        slashes: true
    }))
}

function setMenu() {
    const template = require('./lib/js/temp_main');
    let locale = "en";
    let local_locale = settings.get('locale');
    if(local_locale)
    {
        locale = local_locale;
    }
    else
    {
        locale = app.getLocale();
        settings.set("locale", locale);
        if(locale !== "en" && locale !== "ja" && locale !== "jw" && locale !== "ko" && locale !== "zh" && locale !== "zh-CN" && locale !== "zh-TW")
        {
            locale = "en";
            settings.set("locale", "en");
        }
    }

    menu = Menu.buildFromTemplate([
        {
            label: template[locale]['menu_main'],
            submenu: [
                {
                    label: template[locale]['menu_about'],
                    click(){
                        let focusedWindow = BrowserWindow.getFocusedWindow();
                        focusedWindow.webContents.send('about');
                    }
                },
                {type:'separator'},
                {
                    label: template[locale]['menu_exit'],
                    click(){
                        dialog.showMessageBox({
                            type:"warning",
                            buttons:[template[locale]['reload_ok'],template[locale]['reload_cancel']],
                            message: template[locale]['quit']
                        },function (op) {
                            if(!op)
                            {
                                app.quit()
                            }
                        });
                    }
                }
            ]
        },
        {
            label: template[locale]['menu_view'],
            submenu: [
                {
                    label: template[locale]['menu_view_device'],
                    click() {
                        let focusedWindow = BrowserWindow.getFocusedWindow();
                        focusedWindow.webContents.send('device');
                    }
                },
                {
                    label: template[locale]['menu_view_graph'],
                    click() {
                        let focusedWindow = BrowserWindow.getFocusedWindow();
                        focusedWindow.webContents.send('graph');
                    }
                },
                {type:'separator'},
                {
                    label: template[locale]['menu_view_help'],
                    click() {
                        let focusedWindow = BrowserWindow.getFocusedWindow();
                        focusedWindow.webContents.send('help');
                    }
                },
                {type:'separator'},
                {
                    label: template[locale]['menu_folder'],
                    click() {
                        let Path = path.join(app.getPath('home'), `IntrepidCS\/neoRAD-IO2`);
                        shell.showItemInFolder(Path);
                    }
                }
            ]
        },
        {
            label: template[locale]['menu_advanced'],
            submenu:[
                {
                    label: template[locale]['menu_reload'],
                    click() {
                        dialog.showMessageBox({
                            type:"warning",
                            buttons:[template[locale]['reload_ok'],template[locale]['reload_cancel']],
                            message: template[locale]['reload_message']
                        },function (op) {
                            if(!op)
                            {
                                mainWindow.reload();
                            }
                        });
                    }
                }
            ]
        }
    ]);
}