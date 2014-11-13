/*
    Property Bag Description
    var CommunicationDictionarySample = {
    
        KEY_REQUEST_ID : 0, // Input - 0-GetWeather
        KEY_CONDITIONS: "", // Output -
        KEY_TEMPERATURE: 0, // Output -
    };
*/

var REQUEST_ID_GET_WEATHER       = 0;
var OPEN_WEATHER_MAP_URL         = "http://api.openweathermap.org/data/2.5/weather?lat={0}&lon={1}";
var OPEN_WEATHER_MAP_URL_SMAPLE  = "http://api.openweathermap.org/data/2.5/weather?lat=42.414747044171925&lon=-71.50293471631109";

var kelvinToCelsius = function(v) {
    
    return Math.round(v) - 273.15;
};

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

function getWeather() {
    
    navigator.geolocation.getCurrentPosition(
        getWeatherFromGpsPos,
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
            }
        }
    );
}

Init();
