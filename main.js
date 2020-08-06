const {app, BrowserWindow, Menu, ipcMain} = require('electron')
const path = require('path')
const url = require('url')

let mainWin;

function createWindow () {
  mainWin = new BrowserWindow({
    width: 800,
    height: 600,
    minWidth: 400,
    minHeight: 300,
    frame: false,
    transparent: true,
    backgroundColor: '#00000000',
    alwaysOnTop: false,
    title:"my_electron_win",
    webPreferences: {
      nodeIntegration: true,
      //allowRunningInsecureContent: true,
      //webSecurity: false,
      //webgl: false
    }
  })

  mainWin.loadURL(url.format({
    pathname: path.join(__dirname, 'index.html'),
    protocol: 'file:',
    slashes: true
  }))

  //mainWin.webContents.openDevTools();

  mainWin.on('closed', () => {
    mainWin = null;
  });
}

// app.disableDomainBlockingFor3DAPIs();
app.disableHardwareAcceleration();
app.on('ready', createWindow)

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    mainWin.webContents.send("window_closed");
    app.quit()
  }
})

app.on('activate', () => {
  if (mainWin === null) {
    createWindow()
  }
})

ipcMain.on('switch_min', (event, arg) => {
  let isMini = mainWin.isMinimized();
  if(!isMini) mainWin.minimize();
});

let isMaxed  = false;
ipcMain.on('switch_max', (event, arg) => {
  (isMaxed = !isMaxed)?
  mainWin.maximize():
  mainWin.unmaximize();
});

ipcMain.on('getWindowId', (event, arg) => {
  let hwnd = mainWin.getNativeWindowHandle();
  event.returnValue = hwnd.readUInt32LE(0);
});
