function DBC() {
    this._ready = false;
    this._conn = new WebSocket('ws://127.0.0.1:65530/db');
    this._conn.onopen = () => {
        this._ready = true
    };
    this._pool = new Map();
    this._database = new Map();
    if (typeof DBC._initialized === "undefined") {
        DBC.prototype.send = function (message) {
            id = (new Date()).getTime();
            this._conn.send(id + `|` + message);
            return new Promise((rs, rj) => {
                this._pool.set(id, {rs: rs, rj: rj})
            });
        };
        DBC.prototype.receive = function (message) {
            id = message.substring(0, message.indexOf("|"));
            msg = message.substring(message.indexOf("|"));
            if (msg.startsWith('ERROR')) {
                this._pool.get(id).rj(msg)
            } else {
                this._pool.get(id).rs(msg)
            }
            this._pool.delete(id)
        };
        DBC.prototype.database = function (name) {
            this._database.forEach((v, k) => {
                if (k === name) {
                    return v
                }
            });
            this._database.set(name, {
                name: name,
                open: (password) => {
                    return this.send('OPEN|' + name + '|' + password);
                },
                query: (query) => {
                    return this.send('QUERY|' + name + '|' + query)
                },
                exec: (query) => {
                    return this.send('EXEC|' + name + '|' + query)
                },
                close: () => {
                    this._database.delete(name);
                    return this.send('CLOSE|' + name)
                },
                delete: () => {
                    this._database.delete(name);
                    return this.send('DELETE|' + name)
                }
            });
            return this._database.get(name)
        };
        this._conn.onmessage = ev => {
            this.receive(ev.data)
        };
        this._conn.onerror = ev => {
            this._pool.forEach((v) => {
                v.rj("ERROR|Connection closed :" + ev)
            });
            this._pool.clear()
        };
        DBC._initialized = true;
    }
}

function TestDBC() {
    let dbc=new DBC();
    let db=dbc.database('local');
    db.open('123').then((e)=>console.log(e)).catch((e)=>console.log(e));
}