"use strict"
const colorScheme = ["#d8d8d8", "#ffff00", "#00ff00", "#ff0000", "#4444ff", "#0000ff"]
const colorZz = 0, colorTm = 1, colorSm = 2, colorMm = 3, colorHd = 4, colorHn = 5
const colorsOld = Array(60).fill(-1), colorsNew = Array(60).fill(colorZz)
function updateLedStrip() {
    /** @type {HTMLCanvasElement} */ const cnv = document.querySelector("canvas.analog-display")
    /** @type {CanvasRenderingContext2D} */ const ctx = cnv.getContext("2d")
    ctx.setTransform(1, 0, 0, 1, 0, 0)
    ctx.translate(cnv.width / 2, cnv.height / 2)
    const rL = Math.round(cnv.height / 80), rD = rL * 1.1, y = -cnv.height * 0.43
    for (let i = 0; i < 60; ++i) {
        if (colorsOld[i] != colorsNew[i]) {
            ctx.beginPath()
            ctx.fillStyle = ctx.strokeStyle = colorScheme[0]
            ctx.ellipse(0, y, rD, rD, 0, 0, 2 * Math.PI)
            ctx.fill()
            ctx.closePath()

            ctx.beginPath()
            ctx.fillStyle = colorScheme[colorsOld[i] = colorsNew[i]]
            ctx.ellipse(0, y, rL, rL, 0, 0, 2 * Math.PI)
            ctx.fill()
            ctx.closePath()
        }
        ctx.rotate(Math.PI / 30)
    }
}

function updateRadialClockDisplay(date) {
    const ledsCount = 60, ledsPerHour = ledsCount / 12, ledsPerMinute = ledsCount / 60
    const ts = date / 1000 - 60 * new Date().getTimezoneOffset() 
    const ss = Math.floor(ts % 60)
    const mm = Math.floor(ts / 60) % 60
    const hh = Math.floor(ts / 3600) % 24
    for(let i = 0; i < ledsCount; ++i) {
        if (ss == i / ledsPerMinute) {
            colorsNew[i] = colorSm
        }
        else if (mm == i / ledsPerMinute && (ss % 2 == 0)) {
            colorsNew[i] = colorMm
        }
        else if (((i % ledsPerHour > 0) || (ss % 2 > 0))
                && (12 - 1 + hh) % 12 == Math.floor(Math.floor(
                    ledsCount + i - 1 - Math.floor(mm * ledsPerHour / 60)) % ledsCount / ledsPerHour)) {
            colorsNew[i] = (hh < 6 || hh > 17) ? colorHn : colorHd
        }
        else if (i % ledsPerHour) {
            colorsNew[i] = colorZz
        }
        else colorsNew[i] = colorTm
    }
    updateLedStrip()
}

const formatterT = new Intl.DateTimeFormat("en-US", { hour: "numeric", minute: "numeric", second: "numeric", hour12: false })
const formatterD = new Intl.DateTimeFormat("en-US", { weekday: "short", year: "numeric", month: "short", day: "numeric" })
function updateDigitalClockDisplay(date) {
    const panelT = document.querySelector("div.digital-display-time")
    panelT.innerText = formatterT.format(date)
    const panelD = document.querySelector("div.digital-display-date")
    panelD.innerText = formatterD.format(date)
}

let remoteDateTime = 0, acceptDateTime = 0
function refreshTime() {
    if (acceptDateTime > 0) {
        const date = remoteDateTime + (Date.now() - acceptDateTime)
        updateDigitalClockDisplay(date)
        updateRadialClockDisplay(date)
    }
    setTimeout(refreshTime, 1000 - Date.now() % 1000)
}

function updateForecast(forecast) {
    if(forecast.hasOwnProperty("description")) {
        const panelD = document.querySelector("div.weather-descr")
        panelD.innerHTML = forecast.description
        const panelA = document.querySelector("div.weather-attrs")
        panelA.innerHTML = `${forecast.temperature}°C, wind ${forecast.windside}, ${forecast.windspeed}m/s`
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
            reject(`XMLHttpRequest error: ${method} /${url}${params ? " " + JSON.stringify(params) : ""}`)
        }
        if (params instanceof Object) {
            rq.setRequestHeader("Content-Type", "application/x-www-form-urlencoded")
            rq.send(Object.keys(params).reduce(
                (a, k) => `${a}${a.length ? "&" : ""}${k}=${params[k]}`, ""))
        }
        else if (params?.constructor === String) {
            rq.send(params)
        }
        else rq.send()
    })
}

async function requestInfoAsync() {
    if (window.location.hostname.length > 0) {
        const rsp = await makeRequestAsync("GET", "info")
        if (rsp.status === 200) {
            const info = JSON.parse(rsp.response)
            if(info.hasOwnProperty("time")) {
                remoteDateTime = info.time * 1000
                acceptDateTime = Date.now()
                refreshTime()
            }
            if(info.hasOwnProperty("forecast")) {
                updateForecast(info.forecast)
            }
        }
        else {
            console.log("Info request failed:", rsp)
            setTimeout(requestInfoAsync, 3000)
        }
    }
    else {
        const info = JSON.parse('{"time":1718420332,"forecast":{"time":1718408700,"weathercode":1,"description":"Mainly clear","temperature":20.0,"windspeed":1.5,"winddir":270.0,"windside":"West","windsideabbr":"W"}}')        
        remoteDateTime = info.time * 1000
        acceptDateTime = Date.now()
        refreshTime()
        updateForecast(info.forecast)
    }
} requestInfoAsync()