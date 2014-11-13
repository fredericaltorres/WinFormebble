//////////////////////////////////////////////////////////////////////////////////////////

/*

var CommunicationDictionarySample = {
    KEY_REQUEST_ID : 0, // Input - 0-GetWeather, 1-Get:Location
    KEY_CONDITIONS: "", // Output -
    KEY_LOCATION : "",  // Output -
    KEY_TEMPERATURE: 0, // Output -
    KEY_VIBRATE: 0,     // Output - 1-true, 0 false
    KEY_STREET: "",     // Output -
};

*/
var REQUEST_ID_GET_WEATHER  = 0;
var REQUEST_ID_GET_LOCATION = 1;

var FredMapQuestKey               = "Fmjtd%7Cluurn96a2u%2Ca2%3Do5-9w85ga";
var MAPQUEST_REVERSE_LOCATION_URL = "http://www.mapquestapi.com/geocoding/v1/reverse?key={0}&location={1},{2}";
var OPEN_WEATHER_MAP_URL          = "http://api.openweathermap.org/data/2.5/weather?lat={0}&lon={1}";

/*    
    Flickr Api to get address
    https://api.flickr.com/services/rest/?method=flickr.places.findByLatLon&api_key=b732076fc242bf05e5f0637eaa439d62&lat=42.41917177356681&lon=-71.4998582911204&accuracy=16&format=json&nojsoncallback=1
    
    Yahoo Forcast
    http://query.yahooapis.com/v1/public/yql?q=select%20%20item.condition,%20item.forecast,%20astronomy,%20wind,%20atmosphere%20from%20weather.forecast%20where%20woeid%20=%2055986987%20and%20u%20=%20%22f%22%20%7Ctruncate(count=4)&format=json
*/

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

var PREVIOUS_ADDRESS = null;

function getAddressFromGpsPos(pos) {
    
    var url = MAPQUEST_REVERSE_LOCATION_URL.format(FredMapQuestKey, pos.coords.latitude, pos.coords.longitude);        
    httpGet(url, 
        function(responseText) {
            var o = JSON.parse(responseText);
            var l = o.results[0].locations[0];
            var address = "{0}, {1}, {2}".format(l.adminArea5, l.adminArea3, l.adminArea1);
            var street = l.street;
            var vibrate = 0; // false;
            if(address !== PREVIOUS_ADDRESS) {
                vibrate          = 1;
                PREVIOUS_ADDRESS = address;
            }
            var dictionary  = { 
                KEY_REQUEST_ID : REQUEST_ID_GET_LOCATION,
                KEY_LOCATION   : address,
                KEY_VIBRATE    : vibrate,
                KEY_STREET     : street,
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

///Init();
