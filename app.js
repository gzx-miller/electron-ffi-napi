const { ipcRenderer, BrowserWindow } = require('electron');

let playerX = 0;
let playerY = 0;
function getPlayerPos() {
  var player = window.player;
  var actualTop = player.offsetTop;
  var actualLeft = player.offsetLeft;
  var current = player.offsetParent;
  while (current !== null) {
    actualTop += current.offsetTop;
    actualLeft += current.offsetLeft;
    current = current.offsetParent;
  }
  playerX = actualLeft;
  playerY = actualTop;
  console.log(`getPlayerPos: ${playerX}, ${playerY}`)
}

let winLeft = 0, winTop = 0;
function getWinPos() {
  if (window.screenLeft < -5000 ||
      window.screenTop < -5000 || (
      winLeft === window.screenLeft &&
      winTop === window.screenTop)) {
        return false;
  }
  winLeft = window.screenLeft;
  winTop = window.screenTop;
  return true;
}

let scrollTop = 0;

function test_ffi_napi() {
  try {
    let ffi = require("ffi-napi");
    var ref = require("ref-napi");
    window.ffi_napi = ffi.Library("my_win/release/my_win.dll", {
      create_win: ["bool", ["int", "int", "int", "int", "int", "int", "int", "int"]],
      set_win_pos: ["bool", ["int", "int", "int", "int", "int", "int"]],
      set_win_size: ["bool", ["int", "int"]],
      show_win: ["bool", ["int"]],
      quit_win: ["bool", []],
      set_callback: ['void', ['pointer']]
    });

    // set js callback into dll
    var c_CallBack = ffi.Callback('int', ['int'], 
    function(id) {
      console.log(`[callback] ${id}`);
      return 12345678;
    });
    process.on('exit', () => {
      c_CallBack // keep reference avoid gc
    })
    window.ffi_napi.set_callback(c_CallBack);

    console.log("[init] fii.Library:", ffi_napi);
  } catch (error) {
    console.error("[err] ffi.Library:", error);
  }
}

function observePlayerSize() {  
  var body = document.getElementsByTagName("body")[0];
  var player = document.getElementById("player");
  window.onresize = () =>{  // TestCode: change dom size
    body.style.width = window.innerWidth + "px";
    body.style.height = window.innerHeight + "px";
    // player.style.width = window.innerWidth * 0.75 + "px";
    // player.style.height = window.innerHeight * 0.75 + "px";
  }
  window.onresize();

  var container = document.getElementById("container");
  container.addEventListener("scroll", (ev) => {  // TestCode: change dom pos
    console.log("on scroll: " + container.scrollTop);
    getWinPos();
    scrollTop = container.scrollTop;
    // let x = winLeft + playerX;
    // let y = winTop + playerY - scrollTop;
    window.ffi_napi.set_win_pos(winLeft, winTop, window.innerHeight, playerX, playerY, scrollTop);
  });

  let preWidth = 0, preHeight = 0;
  var observer = new MutationObserver(function (mutations) {
    mutations.forEach(function (mutation) {
      if (player.clientWidth != preWidth || 
          player.clientHeight != preHeight) {
          preWidth = player.clientWidth;
          preHeight = player.clientHeight;
          console.log(`mutation: ${player.clientWidth}, ${player.clientHeight}`);
          window.ffi_napi.set_win_size(player.clientWidth, player.clientHeight);
      }
    });
  });
  var config = {
    attributes: true,
    subtree: false,
    childList: false,
    attributeOldValue: true,
    attributeFilter: ["style"],
  };
  observer.observe(player, config);
}

function regClick() {
  document.querySelector('#max_btn').onclick = (e) => {
    ipcRenderer.send('switch_max', '');
  };
  document.querySelector('#min_btn').onclick = () => {
      ipcRenderer.send('switch_min', '');
      console.log(`switch_min`);
      window.ffi_napi.show_win(1);
  };
  document.querySelector('#close_btn').onclick = () => {
    window.close();
  };
}

ipcRenderer.on("window_closed", () =>{
  window.ffi_napi.quit_win();
});

ipcRenderer.on("window_move", (ev, arg)=>{
  if (!getWinPos()) return false;
  // let x = winLeft + playerX;
  // let y = winTop + playerY - scrollTop;
  // console.log(`on recv window_move: ${x}, ${y}`);
  window.ffi_napi.set_win_pos(winLeft, winTop, window.innerHeight, playerX, playerY, scrollTop);
});

function initWin() {
  let hwnd = ipcRenderer.sendSync('getWindowId');
  window.player = document.getElementById("player");
  getPlayerPos();
  getWinPos();
  // let x = winLeft + playerX;
  // let y = winTop + playerY - scrollTop;
  // console.log(`initWin: ${x}, ${y}, ${player.clientWidth}, ${player.clientHeight}, ${hwnd}`);
  window.ffi_napi.create_win(
    winLeft, winTop, window.innerHeight,
    playerX, playerY, 
    player.clientWidth, player.clientHeight, 
    hwnd);
}
window.onload = () => {
  test_ffi_napi();
  observePlayerSize();
  regClick();
  initWin();  
};
