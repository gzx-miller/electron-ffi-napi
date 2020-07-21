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
      show_win: ["bool", ["int"]]
    });
    console.log("[init] fii.Library:", ffi_napi);
  } catch (error) {
    console.error("[err] ffi.Library:", error);
  }

  ffi_napi.create_win(100, 100, 300, 200);
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

ipcRenderer.on("show_window", (ev, arg)=>{
  console.log("on recv show_window:" + JSON.stringify(arg));
  window.ffi_napi.show_win(arg.show);
});

ipcRenderer.on("window_resize", (ev, arg)=>{
  console.log("on recv window_resize:" + JSON.stringify(arg));
  window.ffi_napi.set_win_size(arg.w, arg.h);
});

let preLeft, preTop;
ipcRenderer.on("window_move", (ev, arg)=>{
  if (preLeft !== window.screenLeft || preTop !== window.screenTop) {
    preLeft = window.screenLeft;
    preTop = window.screenTop;
    let x = window.screenLeft// + window.outerWidth - window.innerWidth - 1;
    let y = window.screenTop// + window.outerHeight - window.innerHeight - 1
    console.log(`on recv window_move: ${x}, ${y}`);
    window.ffi_napi.set_win_pos(x, y);
  }
});

window.onload = () => {
  test_ffi_napi();
  observePlayerSize();
  regClick();
};
