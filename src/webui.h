#ifndef webui_h
#define webui_h

#include <Arduino.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
// #include <ESP8266NetBIOS.h>
#include <ESP8266WebServer.h>

String getDeviceInfo();
ESP8266WebServer webServer(80);


void initWebUI() {
    // wsServer.onEvent(webSocketEvent);
    // wsServer.begin();

    // webServer.on("/set-text", HTTP_POST, []() {
    //     String line1 = webServer.arg("line1");
    //     String line2 = webServer.arg("line2");
    //     updateDisplayText(line1, line2);
    // });

    webServer.on("/info", HTTP_GET, []() {
        String json = getDeviceInfo();
        webServer.send(200, "application/json", json);
    });

    // webServer.on("/", HTTP_GET, []() {
    //     webServer.sendHeader("Location", "http://" + webServer.hostHeader() + "/index.html", true);
    //     webServer.send(302, "text/plain", "Redirecting to index page");
    // });

    webServer.serveStatic("/", LittleFS, "/webui/static/", "max-age=60"); //"max-age=3600" or "no-cache"
    webServer.begin();
}

void loopWebUI() {
    webServer.handleClient();
}

#endif