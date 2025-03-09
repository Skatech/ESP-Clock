class TimeEmitter {
    #time = 0
    #from = 0
    #task = 0
    #func = null

    /** @callback TimeCallback @param {number} time */

    /** @param {TimeCallback} func */
    constructor(func) {
        this.#func = func
    }

    /** @param {number} time */
    start(time) {
        stop()
        this.#time = time
        this.#from = Date.now()
        this.#task = setInterval(() => this.#func(this.time), 1000)
    }

    stop() {
        if (this.#task)
            clearInterval(this.#task)
    }

    /** @return {number} */
    get time() {
        return this.#time + (Date.now() - this.#from)
    }
}

const timeFormatter = new Intl.DateTimeFormat('en-US', { hour: 'numeric', minute: 'numeric', second: 'numeric', hour12: false })
const dateFormatter = new Intl.DateTimeFormat('en-US', { weekday: 'short', year: 'numeric', month: 'short', day: 'numeric' })
function updateTime(time) {
    const date = new Date(time)
    const timePanel = document.querySelector('div.time-panel')
    timePanel.innerHTML = timeFormatter.format(date)
    
    const datePanel = document.querySelector('div.date-panel')
    const dateString = dateFormatter.format(date)
    if (datePanel.innerHTML != dateString) {
        datePanel.innerHTML = dateString
    }
}

function updateForecast(forecast) {
    if(forecast.hasOwnProperty('description')) {
        const descrPanel = document.querySelector('div.weather-descr-panel')
        descrPanel.innerHTML = forecast.description
        const attrsPanel = document.querySelector('div.weather-attrs-panel')
        attrsPanel.innerHTML = `${forecast.temperature}°C, wind ${forecast.windside}, ${forecast.windspeed}m/s`
    }
}

/** @param {'GET'|'POST'} method @param {string} url @param {object|string} params @returns {Promise<{status: number, response: string}>} */
function makeRequestAsync(method, url, params = undefined) {
    return new Promise(function(resolve, reject) {
        const rq = new XMLHttpRequest()
        rq.open(method, url, true)
        rq.onload = function() {
            resolve({status: rq.status, response: rq.response})
        }
        rq.onerror = function() {
            reject(`XMLHttpRequest error: ${method} /${url}${params ? ' ' + JSON.stringify(params) : ''}`)
        }
        if (params instanceof Object) {
            rq.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded')
            rq.send(Object.keys(params).reduce(
                (a, k) => `${a}${a.length ? '&' : ''}${k}=${params[k]}`, ''))
        }
        else if (params?.constructor === String) {
            rq.send(params)
        }
        else rq.send()
    })
}

async function requestInfoAsync() {
    if (window.location.hostname === '') {
        updateInfo(JSON.parse(
            '{"time":1718420332,"forecast":{"time":1718408700,"weathercode":1,"description":"Mainly clear","temperature":20.0,"windspeed":1.5,"winddir":270.0,"windside":"West","windsideabbr":"W"}}'))
    }
    else {
        const rsp = await makeRequestAsync('GET', 'info');
        if (rsp.status === 200) {
            const info = JSON.parse(rsp.response)
            if(info.hasOwnProperty('time')) {
                const time = info.time * 1000
                updateTime(time)
                timeEmitter.start(time)
            }
            if(info.hasOwnProperty('forecast')) {
                updateForecast(info.forecast)
            }
        }
        else console.log('Info request failed:', rsp);
    }
}

const timeEmitter = new TimeEmitter(updateTime)
requestInfoAsync();