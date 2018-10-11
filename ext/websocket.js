var Win={
wss : "ws://127.0.0.1:65530/win"
,enable:false
,ws:null
,close : function () {Win.ws.send("close")}
,full : function () {Win.ws.send("full")}
,topmost : function (a) {Win.ws.send("topmost")}
,nonetop : function (a) {Win.ws.send("nonetop")}
,max : function () {Win.ws.send("max")}
,min : function () {Win.ws.send("min")}
,restore : function () {Win.ws.send("restore")}
,thin : function () {Win.ws.send("thin")}
,normal : function () {Win.ws.send("normal")}
,less : function () {Win.ws.send("less")}
,fullscreen : function () {Win.ws.send("fullscreen")}
,drag : function () {Win.ws.send("drag" )}
,drop : function () {Win.ws.send("drop" )}
,move : function (e) {ev = window.event || e;ev.button === 0 ? Win.ws.send("move|" + ev.screenX + "|" + ev.screenY) : ""}
,onDrag : function dragOn() {Win.drag();document.addEventListener("mousemove", Win.move, true)}
,offDrag : function dragOff() {Win.drag();document.removeEventListener("mousemove", Win.move, true)}
};
(function () {
    Win.ws=new WebSocket(Win.wss);
    Win.ws.onopen=function () {
        Win.enable=true
    }
})();