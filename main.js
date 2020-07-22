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
    alwaysOnTop: false,
    webPreferences: {
      nodeIntegration: true
    }
  })

  mainWin.loadURL(url.format({
    pathname: path.join(__dirname, 'index.html'),
    protocol: 'file:',
    slashes: true
  }))

  mainWin.webContents.openDevTools();

  mainWin.on('closed', () => {
    mainWin = null
  });
  mainWin.on("restore", () => {
    mainWin.webContents.send("show_window", { show: 1 });
  });
  mainWin.on('minimize', ()=> {
    mainWin.webContents.send("show_window", { show: 0 });
  });
  mainWin.on('move', () => {
    mainWin.webContents.send("window_move");
  });
  mainWin.on('resize', () =>{
    console.log("on resize");
  });
}

app.on('ready', createWindow)

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
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
