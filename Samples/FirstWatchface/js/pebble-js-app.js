var GEOLOCATION_TIMEOUT     = 15000;
var GEOLOCATION_MAXIMUM_AGE = 60000;

var xhrRequest = function (url, type, callback) {

    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
        callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
};

function locationSuccess(pos) {

    var url = "http://api.openweathermap.org/data/2.5/weather?lat=" + pos.coords.latitude + "&lon=" + pos.coords.longitude;

    xhrRequest(url, 'GET', // Send request to OpenWeatherMap

        function(responseText) {

            var json        = JSON.parse(responseText);           
            var temperature = Math.round(json.main.temp - 273.15); // Temperature Kelvin to Celcius
            var conditions  = json.weather[0].main; // Conditions            
            var dictionary  = { 
                KEY_TEMPERATURE: temperature,
                KEY_CONDITIONS : conditions
            };            
            console.log("Info:"+JSON.stringify(dictionary));            
            Pebble.sendAppMessage( // Send to Pebble
                dictionary,
                function(e) { console.log("@Weather info sent to Pebble successfully!"); },
                function(e) { console.log("@Error sending weather info to Pebble!"); }
            );
        }      
    );
}

function getWeather() {
    
    navigator.geolocation.getCurrentPosition(
        locationSuccess,
        function(err) {console.log("Error requesting location!"); },
        { timeout: GEOLOCATION_TIMEOUT, maximumAge: GEOLOCATION_MAXIMUM_AGE }
    );
}

function Init() {

    // Listen for when the watchface is opened
    Pebble.addEventListener('ready', 
        function(e) {
            console.log("PebbleKit JS ready! e="+sys.getType(e));
            getWeather(); // Get the initial weather
        }
    );
    
    // Listen for when an AppMessage is received
    Pebble.addEventListener('appmessage',
        function(e) {
            console.log("AppMessage received! e="+sys.getType(e));
            getWeather();
        }                     
    );   
}

console.log("Starting the JavaScript world! - "+ (new Date()));
console.log("User-agent header sent: " + navigator.userAgent);
Init();
