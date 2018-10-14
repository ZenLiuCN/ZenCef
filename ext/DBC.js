function DBC() {
    this._ready = false;
    this._conn = new WebSocket('ws://127.0.0.1:65530/db');
    this._conn.onopen = () => {
        this._ready = true
    };
    this._pool = new Map();
    this.database = new Map();
    if (typeof DBC._initialized === "undefined") {
        DBC.prototype.send = function (message) {
            id = (new Date()).getTime();
            this._conn.send(id + `|` + message);
            return new Promise((rs, rj) => {
                this._pool.set(id, {rs, rj})
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
        DBC.prototype.open = function (name, password) {
            this.database.forEach((v, k) => {
                if (k === name) {
                    return v
                }
            });
            this.database.set(name, function () {
                this.send('OPEN|' + name + '|' + password);
                return {
                    name: name,
                    query: (query) => {
                        return this.send('QUERY|' + name + '|' + query)
                    },
                    exec: (query) => {
                        return this.send('EXEC|' + name + '|' + query)
                    },
                    close: () => {
                        this.database.delete(name);
                        return this.send('CLOSE|' + name)
                    },
                    delete: () => {
                        this.database.delete(name);
                        return this.send('DELETE|' + name)
                    }
                }
            });
            return this.database.get(name)
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