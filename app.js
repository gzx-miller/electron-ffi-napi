const { ipcRenderer } = require('electron');

function test_ffi_napi() {
  try {
    let ffi = require("ffi-napi");
    var ref = require("ref-napi");
    window.ffi_napi = ffi.Library("my_win/release/little_win.dll", {
      create_win: ["bool", ["int", "int", "int", "int"]],
      set_win_pos: ["bool", ["int", "int"]],
      set_win_size: ["bool", ["int", "int"]],
      quit_win: ["bool", []],
    });
    console.log("[init] fii.Library:", ffi_napi);
  } catch (error) {
    console.error("[err] ffi.Library:", error);
  }

  // ffi_napi.create_win(100, 100, 300, 200);
  // ffi_napi.set_win_pos(200, 200);
  // ffi_napi.set_win_size(800, 600);
  //ffi_napi.quit_win();
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
    console.log("on scroll");
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
      window.close();
  };
}

window.onload = () => {
  test_ffi_napi();
  observePlayerSize();
  regClick();
};
