const {app, BrowserWindow, Menu, ipcMain} = require('electron')
const path = require('path')
const url = require('url')

let mainWin;

function createWindow () {
  mainWin = new BrowserWindow({
    width: 800,
    height: 600,
    frame: false,
    transparent: true,
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
  })
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

ipcMain.on('resetWindowSize', (event, size) => {
  mainWin.setSize(size.width, size.height);
});

ipcMain.on('switch_min', (event, arg) => {
  let isMini = mainWin.isMinimized();
  if(!isMini) {
    mainWin.minimize();
    event.sender.send("show_window", { show: 0 });
  }
});

