//this file register inner ext  for control window status
var WinObj;WinObj||(WinObj={});(function(){
    WinObj.wss="ws://127.0.0.1:65530/win";
    WinObj.close=function(){WinObj.WinWS.send("win:close")};
    WinObj.full=function(){WinObj.WinWS.send("win:full")};
    WinObj.topMost=function(a){WinObj.WinWS.send("win:topMost:"+a)};
    WinObj.max=function(){WinObj.WinWS.send("win:max")};
    WinObj.min=function(){WinObj.WinWS.send("win:min")};
    WinObj.restore=function(){WinObj.WinWS.send("win:restore")};
    WinObj.frame=function(a){WinObj.WinWS.send("win:frame:"+a)};
    WinObj.drag=function(a,e){a?WinObj.WinWS.send("win:drag:start|"+e.x+"|"+e.y):WinObj.WinWS.send("win:drag:stop")};
    WinObj.onMouseMove=function(a){eve=window.event||ev;eve.button===0?WinObj.WinWS.send("win:drag:move|"+eve.screenX+"|"+eve.screenY):''}})();