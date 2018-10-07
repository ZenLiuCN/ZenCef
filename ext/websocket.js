var Win={
wss : "ws://127.0.0.1:65530/win"
,enable:false
,ws:null
,close : function () {Win.ws.send("win:close")}
,full : function () {Win.ws.send("win:full")}
,topmost : function (a) {Win.ws.send("win:topmost")}
,nonetop : function (a) {Win.ws.send("win:nonetop")}
,max : function () {Win.ws.send("win:max")}
,min : function () {Win.ws.send("win:min")}
,restore : function () {Win.ws.send("win:restore")}
,thin : function () {Win.ws.send("win:thin")}
,normal : function () {Win.ws.send("win:normal")}
,less : function () {Win.ws.send("win:less")}
,fullscreen : function () {Win.ws.send("win:fullscreen")}
,drag : function () {Win.ws.send("win:drag" )}
,drop : function () {Win.ws.send("win:drop" )}
,move : function (e) {ev = window.event || e;ev.button === 0 ? Win.ws.send("win:move|" + ev.screenX + "|" + ev.screenY) : ""}
,onDrag : function dragOn() {Win.drag();document.addEventListener("mousemove", Win.move, true)}
,offDrag : function dragOff() {Win.drag();document.removeEventListener("mousemove", Win.move, true)}
};
(function () {
    Win.ws=new WebSocket(Win.wss);
    Win.ws.onopen=function () {
        Win.enable=true
    }
})();