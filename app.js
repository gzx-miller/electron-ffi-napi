const { ipcRenderer, BrowserWindow } = require('electron');

let playerLeft = 0;
let playerTop = 0;
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
  playerLeft = actualLeft;
  playerTop = actualTop;
  console.log(`getPlayerPos: ${playerLeft}, ${playerTop}`)
}

let winLeft = 0, winTop = 0;
let scrollTop = 0;
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

function test_ffi_napi() {
  try {
    let ffi = require("ffi-napi");
    var ref = require("ref-napi");
    window.ffi_napi = ffi.Library("my_win/release/my_win.dll", {
      create_win: ["bool", ["int", "int", "int", "int", "int"]],
      set_win_pos: ["bool", ["int", "int"]],
      set_win_size: ["bool", ["int", "int"]],
      quit_win: ["bool", []],
      show_win: ["bool", ["int"]]
    });
    console.log("[init] fii.Library:", ffi_napi);
  } catch (error) {
    console.error("[err] ffi.Library:", error);
  }
}

function observePlayerSize() {  
  var body = document.getElementsByTagName("body")[0];
  var player = document.getElementById("player");
  window.onresize = () =>{
    body.style.width = window.innerWidth + "px";
    body.style.height = window.innerHeight + "px";
    player.style.width = window.innerWidth * 0.8 + "px";
    player.style.height = window.innerHeight * 0.8 + "px";
  }
  window.onresize();

  var container = document.getElementById("container");
  container.addEventListener("scroll", (ev) => {
    console.log("on scroll: " + container.scrollTop);
    scrollTop = container.scrollTop;
    let x = winLeft + playerLeft;
    let y = winTop + playerTop - scrollTop;
    window.ffi_napi.set_win_pos(x, y);
  });

  var observer = new MutationObserver(function (mutations) {
    mutations.forEach(function (mutation) {
      console.log(`mutation: ${player.clientWidth}, ${player.clientHeight}`);
      window.ffi_napi.set_win_size(player.clientWidth, player.clientHeight);
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
  };
  document.querySelector('#close_btn').onclick = () => {
    window.ffi_napi.quit_win();
    window.close();
  };
}

function initWin() {
  let hwnd = ipcRenderer.sendSync('getWindowId');
  window.player = document.getElementById("player");
  getPlayerPos();
  getWinPos();
  let x = playerLeft;
  let y = playerTop;
  console.log(`initWin: ${x}, ${y}, ${player.clientWidth}, ${player.clientHeight}, ${hwnd}`);
  ffi_napi.create_win(x, y, player.clientWidth, player.clientHeight, hwnd);
}
window.onload = () => {
  test_ffi_napi();
  observePlayerSize();
  regClick();
  initWin();  
};
