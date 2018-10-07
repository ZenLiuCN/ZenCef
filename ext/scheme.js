var Win = {
    scheme: 'window://'
    ,moving:false
    , min: function () {let x = new XMLHttpRequest();x.open('GET', Win.scheme + 'min',true);x.send();}
    , max: function () {let x = new XMLHttpRequest();x.open('GET', Win.scheme + 'max',true);x.send();}
    , full: function () {let x = new XMLHttpRequest();x.open('GET', Win.scheme + 'full',true);x.send();}
    , close: function () {let x = new XMLHttpRequest();x.open('GET', Win.scheme + 'close',true);x.send();}
    , restore: function () {let x = new XMLHttpRequest();x.open('GET', Win.scheme + 'restore',true);x.send();}
    , topmost: function () {let x = new XMLHttpRequest();x.open('GET', Win.scheme + 'topmost',true);x.send();}
    , nonetop: function () {let x = new XMLHttpRequest();x.open('GET', Win.scheme + 'nonetop',true);x.send();}
    , thin: function () {let x = new XMLHttpRequest();x.open('GET', Win.scheme + 'thin',true);x.send();}
    , normal: function () {let x = new XMLHttpRequest();x.open('GET', Win.scheme + 'normal',true);x.send();}
    , less: function () {let x = new XMLHttpRequest();x.open('GET', Win.scheme + 'less',true);x.send();}
    , fullscreen: function () {let x = new XMLHttpRequest();x.open('GET', Win.scheme + 'less',true);x.send();}
    , drag: function () {let x = new XMLHttpRequest();x.open('GET', Win.scheme + 'drag',true);x.send();Win.moving=true;}
    , drop: function () {let x = new XMLHttpRequest();x.open('GET', Win.scheme + 'drop',true);x.send();Win.moving=false;}
    , move: function (ev) {eve = window.event || ev;  if (Win.moving&&eve.button === 0 ){let x = new XMLHttpRequest();x.open('GET', Win.scheme + 'move|'+eve.screenX+"|"+eve.screenY,true);x.send(); }}
    ,onDrag : function () {Win.drag();document.addEventListener("mousemove", Win.move, true)}
    ,offDrag : function () {Win.drop();document.removeEventListener("mousemove", Win.move, true)}
};
(function () {
    alert("move window action will cause crash")
})();