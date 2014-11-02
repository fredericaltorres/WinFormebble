var GEOLOCATION_TIMEOUT     = 15000;
var GEOLOCATION_MAXIMUM_AGE = 60000;

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

//////////////////////////////////////////////////////////////////////////////////////////

var CommunicationDictionarySample = {
    KEY_REQUEST_ID : 0, // 0-GetWeather, 1-Get:Location
    KEY_CONDITIONS: "",
    KEY_LOCATION : "",
    KEY_TEMPERATURE: 0
};
    
var REQUEST_ID_GET_WEATHER  = 0;
var REQUEST_ID_GET_LOCATION = 1;

var FredMapQuestKey               = "Fmjtd%7Cluurn96a2u%2Ca2%3Do5-9w85ga";
var MAPQUEST_REVERSE_LOCATION_URL = "http://www.mapquestapi.com/geocoding/v1/reverse?key={0}&location={1},{2}";
var OPEN_WEATHER_MAP_URL          = "http://api.openweathermap.org/data/2.5/weather?lat={0}&lon={1}";

function getCurrentAddress(lat, lon, callback) {
    
    var url = MAPQUEST_REVERSE_LOCATION_URL.format(FredMapQuestKey, lat, lon);        
    httpGet(url, 
        function(responseText) {
            var o = JSON.parse(responseText);
            var l = o.results[0].locations[0];
            var address = "{0}, {1}, {2}".format(l.adminArea5, l.adminArea3, l.adminArea1);
            Trace("Address:" + address);
            callback(address);
        }
    );    
}

function getWeatherFromGpsPos(pos) {
    
    var url = OPEN_WEATHER_MAP_URL.format(pos.coords.latitude, pos.coords.longitude);
    httpGet(url,
        function(responseText) {            
            var json        = JSON.parse(responseText);           
            var temperature = kelvinToCelsius(json.main.temp); // Temperature
            var conditions  = json.weather[0].main; // Conditions
            var dictionary  = { 
                KEY_REQUEST_ID : REQUEST_ID_GET_WEATHER,
                KEY_TEMPERATURE: temperature,
                KEY_CONDITIONS : conditions,
            };
            sendMessageToWatch(dictionary, "getWeatherFromGpsPos");
        }      
    );
}

function getAddressFromGpsPos(pos) {
    
    var url = MAPQUEST_REVERSE_LOCATION_URL.format(FredMapQuestKey, pos.coords.latitude, pos.coords.longitude);        
    httpGet(url, 
        function(responseText) {
            var o = JSON.parse(responseText);
            var l = o.results[0].locations[0];
             var dictionary  = { 
                KEY_REQUEST_ID : REQUEST_ID_GET_LOCATION,                
                KEY_LOCATION   : "{0}, {1}, {2}".format(l.adminArea5, l.adminArea3, l.adminArea1)
            };
            sendMessageToWatch(dictionary, "getAddressFromGpsPos");  
        }
    ); 
}

function getWeather() {
    
    navigator.geolocation.getCurrentPosition(
        getWeatherFromGpsPos,
        function(err) { Trace("Error requesting location!"); },
        { timeout: GEOLOCATION_TIMEOUT, maximumAge: GEOLOCATION_MAXIMUM_AGE }
    );
}

function getLocation() {
    
    navigator.geolocation.getCurrentPosition(
        getAddressFromGpsPos,
        function(err) { Trace("Error requesting location!"); },
        { timeout: GEOLOCATION_TIMEOUT, maximumAge: GEOLOCATION_MAXIMUM_AGE }
    );
}


function Init() {
    
    Pebble.addEventListener('ready',  // Listen for when the watchface is opened
        function(e) {
            Trace("App started, connected:{0}, Browser Agent:{1}".format(e.ready, navigator.userAgent));
        }
    );    
    Pebble.addEventListener('appmessage', // Listen for when an AppMessage is received
        function(e) {
            Trace("Requested From App:"+JSON.stringify(e.payload));
            switch(e.payload.KEY_REQUEST_ID) {
                case REQUEST_ID_GET_WEATHER  : getWeather();  break;
                case REQUEST_ID_GET_LOCATION : getLocation(); break;
            }
        }
    );
}

Init();
