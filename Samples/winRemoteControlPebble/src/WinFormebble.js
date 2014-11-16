/*
 * WinFormebble Library
 * A C# and Winform friendly library for the developing for the Pebble watch
 *
 * (C) Torres Frederic 2014
 *
 * Licence: MIT
 *
 * This file contains JavaScript re-usable function working with jsCom.cs
 *
 */

var GEOLOCATION_TIMEOUT     = 15000;
var GEOLOCATION_MAXIMUM_AGE = 60000;

var trace = function(s) {
    var timeStamp = (new Date()).toLocaleTimeString();
    console.log(timeStamp+' [INFO] '+s);
};
var Trace = trace; // For compatibility reason TODO: Should be removed

var traceError = function(s) {
    var timeStamp = (new Date()).toLocaleTimeString();
    console.log(timeStamp+' [ERROR] '+s);
};

var Trace = function(s) {
    var timeStamp = (new Date()).toLocaleTimeString();
    console.log(timeStamp+' '+s);
};

var xhrRequest = function (url, type, callback) {

    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        callback(this.responseText);
    };
    Trace("http call {0} {1}".format(type, url));
    xhr.open(type, url);
    xhr.send();
};

var httpGet = function (url, callback) {
    
    xhrRequest(url, 'GET', callback);
};

var kelvinToCelsius = function(v) {
    
    return Math.round(v) - 273.15;
};

var sendMessageToWatch = function(dictionary, method) {

    Trace("{0}sendMessageToWatch({1})".format(method ? method+'().' : "", JSON.stringify(dictionary))); 
    Pebble.sendAppMessage( // Send to Pebble
        dictionary,
        function(e) { },
        function(e) { console.log("Error sending dictionary info to Pebble! method:"+method); }
    );
};
