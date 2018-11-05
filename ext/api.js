function WinApi() {
    return this.init()
}

WinApi.prototype = {
    constructor: WinApi
    , wss: "ws://127.0.0.1:65530/win"
    , enable: false
    , ws: null
    , init() {
        this.ws = new WebSocket(Win.wss);
        this.ws.onopen = function () {
            this.enable = true
        }
        this.onclose = function () {
            this.enable = false
        }
    }
    , close: function () {
        this.ws.send("win:close")
    }
    , full: function () {
        this.ws.send("win:full")
    }
    , topmost: function (a) {
        this.ws.send("win:topmost")
    }
    , nonetop: function (a) {
        this.ws.send("win:nonetop")
    }
    , max: function () {
        this.ws.send("win:max")
    }
    , min: function () {
        this.ws.send("win:min")
    }
    , restore: function () {
        this.ws.send("win:restore")
    }
    , thin: function () {
        this.ws.send("win:thin")
    }
    , normal: function () {
        this.ws.send("win:normal")
    }
    , less: function () {
        this.ws.send("win:less")
    }
    , fullscreen: function () {
        this.ws.send("win:fullscreen")
    }
    , drag: function () {
        this.ws.send("win:drag")
    }
    , drop: function () {
        this.ws.send("win:drop")
    }
    , move: function (e) {
        ev = window.event || e;
        ev.button === 0 ? this.ws.send("win:move|" + ev.screenX + "|" + ev.screenY) : ""
    }
    , onDrag: function () {
        this.drag();
        document.addEventListener("mousemove", this.move, true)
    }
    , offDrag: function () {
        this.drag();
        document.removeEventListener("mousemove", this.move, true)
    }
};

function Jsdbc() {
}

Jsdbc.prototype = {
    constructor: Jsdbc
    , wss: "ws://127.0.0.1:65530/db"
    , enable: false
    , opened: false
    , name: null
    , ws: null
    , Pool: {}
    , connect: function () {
        return new Promise((r, j) => {
            this.ws = new WebSocket(this.wss);
            this.ws.onopen = () => {
                console.log("db connected")
                this.enable = true
                r()
            }
            this.ws.onerror = (e) => {
                if (typeof j !== "undefined") {
                    j(e)
                }
                console.log(e)
            }
            this.ws.onmessage = (e) => {
                o = JSON.parse(e.data)
                if (typeof o['id'] !== 'undefined' && typeof this.Pool[o['id']] !== 'undefined') {
                    if (o.hasOwnProperty(`status`) && !o[`status`]) {
                        this.Pool[o['id']].reject(o)
                    } else {
                        this.Pool[o['id']].resolve(o)
                    }
                } else {
                    console.log("received ", e)
                }
            }
            this.ws.onclose = () => {
                this.opened = false
                this.enable = false
            }
        })
    }
    , _send: function (obj) {
        return new Promise((resolve, reject) => {
            id = this._getid()
            this.Pool[id] = {resolve, reject}
            obj[`id`] = id
            this.ws.send(JSON.stringify(obj))
        })
    }
    , _sendQuerys: function (obj) {
        return new Promise((resolve, reject) => {
            if (this.opened && this.name !== null) {
                obj['name'] = this.name
                this._send(obj).then(e => resolve(e)).catch(e => reject(e))
            } else {
                reject('not open database')
            }
        })
    }
    , _getid: function () {
        return (new Date()).getTime().toString()
    }
    , open: function (name, pwd) {
        return new Promise((r, j) => {
            this._send({
                cmd: 'open',
                name: name,
                pwd: pwd
            }).then((e) => {
                this.name = name
                this.opened = true
                r(e)
            }).catch((e) => j(e))
        })
    }
    , exec: function (query) {
        return this._send({
            cmd: 'exec',
            name: name,
            query: query
        })
    }
    , query: function (query) {
        return this._sendQuerys({
            cmd: 'query',
            query: query
        })
    }
    , batchQuery: function (querys) {
        return this._sendQuerys({
            cmd: 'query',
            querys: querys
        })
    }
    , batchExec: function (querys) {
        return this._sendQuerys({
            cmd: 'exec',
            querys: querys
        })
    }
    , close: function () {
        return new Promise((r, j) => {
            this._sendQuerys({
                cmd: 'close'
            }).then(e => {
                this.opened = false
                this.name = null
                r(e)
            }).catch(e => j(e))
        })
    }
    , delete: function () {
        return new Promise((r, j) => {
            this._sendQuerys({
                cmd: 'delete'
            }).then(e => {
                this.opened = false
                this.name = null
                r(e)
            }).catch(e => j(e))
        })
    }
    , disconnect: function () {
        return new Promise((r, j) => {
            this.ws.onclose = function () {
                r()
            }
            this.ws.onerror = function (e) {
                j(e)
            }
            this.ws.close()
        })
    }
}

function SchemeApi() {

}

SchemeApi.prototype = {
    constructor: SchemeApi
    , scheme: 'window://'
    , xhr: new XMLHttpRequest()
    , moving: false
    , _send: function (cmd) {
        return new Promise((r, j) => {
            this.xhr.abort()
            this.xhr.open('GET', this.scheme + cmd, true);
            this.xhr.onload = function (e) {
                if (e.status === 200 || e.status === 304) {
                    r(e)
                } else {
                    j(e)
                }
            }
            this.xhr.send();
        })
    }
    , isCef: function () {
        return new Promise((r, j) => {
            this.xhr.timeout = 500
            this.xhr.open('GET', this.scheme + 'min?12345', true)
            this.xhr.onload = function (e) {
                if (e.status === 200 || e.status === 304) {
                    r(e)
                } else {
                    j(e)
                }
            }
            this.xhr.timeout = function (e) {
                j(e)
            }
            this.xhr.onerror = function (e) {
                j(e)
            }
            this.xhr.send()
        })

    }
    , min: function () {
        return this._send('min')
    }
    , max: function () {
        return this._send('max')

    }
    , full: function () {
        return this._send('full')

    }
    , close: function () {
        return this._send('close')
    }
    , restore: function () {
        return this._send('restore')

    }
    , topmost: function () {
        return this._send('topmost')

    }
    , nonetop: function () {
        return this._send('nonetop')

    }
    , thin: function () {
        return this._send('thin')
    }
    , normal: function () {
        return this._send('normal')
    }
    , less: function () {
        return this._send('less')
    }
    , fullscreen: function () {
        return this._send('fullscreen')
    }
    , drag: function () {
        let x = new XMLHttpRequest();
        x.open('GET', this.scheme + 'drag', true);
        x.send();
        this.moving = true;
    }
    , drop: function () {
        let x = new XMLHttpRequest();
        x.open('GET', this.scheme + 'drop', true);
        x.send();
        this.moving = false;
    }
    , move: function (ev) {
        eve = window.event || ev;
        if (this.moving && eve.button === 0) {
            let x = new XMLHttpRequest();
            x.open('GET', this.scheme + 'move|' + eve.screenX + "|" + eve.screenY, true);
            x.send();
        }
    }
    /*,onDrag : function () {Win.drag();document.addEventListener("mousemove", Win.move, true)}
    ,offDrag : function () {Win.drop();document.removeEventListener("mousemove", Win.move, true)}*/
};

function ScannerApi(handler) {
    return this.init(handler)
}

ScannerApi.prototype = {
    constructor: ScannerApi,
    wss: `ws://127.0.0.1:65530/scan`,
    ws: null,
    hander: null,
    init: function (handler) {
        return new Promise((r, j) => {
            this.hander = handler
            this.ws = new WebSocket(this.wss);
            this.ws.onopen = () => {
                console.log("scanner connected")
                this.enable = true
                r()
            }
            this.ws.onerror = (e) => {
                if (typeof j !== "undefined") {
                    j(e)
                }
                console.log(e)
            }
            this.ws.onmessage = (e) => {
                if (this.hander !== null) {
                    switch (this.hander(e.data)) {
                        case 1:
                            this.success()
                            break
                        case 2:
                            this.timeout()
                            break
                        default:
                            this.fail()
                    }
                } else {
                    console.log(`received with out handler`, e)
                }
            }
            this.ws.onclose = () => {
                this.hander = null
                this.opened = false
                this.enable = false
            }
        })
    }
    , start: function () {
        this.ws.send(`START`)
    }, stop: function () {
        this.ws.send(`STOP`)
    }, success: function () {
        this.ws.send(`SUCCESS`)
    }, fail: function () {
        this.ws.send(`FAIL`)
    }, timeout: function () {
        this.ws.send(`TIMEOUT`)
    }
}

function TestJsdbc() {
    let dbc = new Jsdbc()
    dbc.connect()
        .then(() => dbc.open("name", "pwd"))
        .then(() => dbc.exec(`create table foo(id int,name text)`))
        .then(() => dbc.exec(`insert into foo values(1,'123')`))
        .then(() => dbc.query(`select * from foo`))
        .then((e) => {
            console.log(e);
            return dbc.close()
        })
        .then(() => dbc.disconnect())
        .catch(e => console.log(e))
}

function TestSchemeApi() {
    let dbc = new SchemeApi()
    dbc.max()
        .then(() => dbc.min())
        .then(() => dbc.restore())
        .then(() => dbc.fullscreen())
        .then(() => dbc.less())
        .then(() => dbc.full())
        .then(() => dbc.normal())
        .catch(e => console.log(e))
}

function TestWinApi() {
    let win = new WinApi()

}