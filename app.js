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

  ffi_napi.create_win(100, 100, 300, 200);
  ffi_napi.set_win_pos(200, 200);
  ffi_napi.set_win_size(800, 600);
  //ffi_napi.quit_win();
}

window.onload = () => {
  test_ffi_napi();

  var body = document.getElementsByTagName("body")[0];
  var player1 = document.getElementById("player1");
  var player2 = document.getElementById("player2");
  window.onresize = () =>{
    body.style.width = window.innerWidth + "px";
    body.style.height = window.innerHeight + "px";
    player1.style.width = window.innerWidth / 2 + "px";
    player1.style.height = window.innerHeight / 2 + "px";
    player2.style.width = window.innerWidth / 2 + "px";
    player2.style.height = window.innerHeight / 2 + "px";
  }
  window.onresize();

  var container = document.getElementById("container");
  container.addEventListener("scroll", (ev) => {
    console.log("on scroll");
  });

  var observer = new MutationObserver(function (mutations) {
    mutations.forEach(function (mutation) {
      console.log(`mutation: ${player1.clientWidth}, ${player1.clientHeight}`);
      ffi_napi.set_win_size(player1.clientWidth, player1.clientHeight);
    });
  });
  var config = {
    attributes: true,
    subtree: false,
    childList: false,
    attributeOldValue: true,
    attributeFilter: ["style"],
  };
  observer.observe(player1, config);
};
