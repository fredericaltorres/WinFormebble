/*
 * A Pebble Watchface that control a PowerPoint presentation
 * using the winRemoteControl api
 * Demo for Code Camp 22, MA, USA
 * (C) Torres Frederic 2014
 * Licence: MIT
 */
var KEY_REQUEST_ID_NEXT     = 0;
var KEY_REQUEST_ID_PREVIOUS = 1;
var KEY_REQUEST_ID_QUIT     = 2; 
var KEY_REQUEST_ID_OK       = 3; 
var KEY_REQUEST_ID_GET_IP   = 4;

var URL            = 'http://[IP]:1964/WinRemoteControl?json=';
var URL_JSON_PARAM = '{"Action":2,"Keys":"{[KEY]}"}';

function getComputerIp() {
    
    return localStorage.getItem("ComputerIp");
}

function sendCommand(key) {
    var param,
        keyValue,
        computerIp = getComputerIp();
    
    if(!computerIp) {
        traceError("No computer ip set");
        return;
    }
    
    switch(key) {
        case KEY_REQUEST_ID_NEXT     : keyValue = "RIGHT";  break;
        case KEY_REQUEST_ID_PREVIOUS : keyValue = "LEFT";   break;
        case KEY_REQUEST_ID_QUIT     : keyValue = "ESCAPE"; break;
        default: return;
    }
    param     = URL_JSON_PARAM.replace("[KEY]", keyValue);
    var url   = URL.replace('[IP]', computerIp);
    trace(url + param);
    url      += encodeURIComponent(param);
    
    httpGet(url,
        function(responseText) {            
            var dictionary  = { KEY_REQUEST_ID : KEY_REQUEST_ID_OK };
            sendMessageToWatch(dictionary, "sendCommand");
        }      
    );
}

function Init() {
    
    Pebble.addEventListener('ready',  // Listen for when the watchface is opened
        function(e) {
            trace("App started, connected:{0}, Browser Agent:{1}".format(e.ready, navigator.userAgent));
        }
    );    
    Pebble.addEventListener('appmessage', // Listen for when an AppMessage is received
        function(e) {
            trace("Requested From App:"+JSON.stringify(e.payload));
            sendCommand(e.payload.KEY_REQUEST_ID);
        }
    );
    
    var AppConfigurationUrl = "https://dl.dropboxusercontent.com/u/6376212/Pebble.Configuration.winRemoteControl.html";
    Pebble.addEventListener("showConfiguration",
        function(e) {
            trace("Opening Config app:{0}".format(AppConfigurationUrl))
            Pebble.openURL(AppConfigurationUrl);
        }
    );
    Pebble.addEventListener("webviewclosed",
        function(e) {
            trace("Closing Config App:");
            trace("  Config :{0}".format(e.response));
            if(e.response) {
                var configuration = JSON.parse(decodeURIComponent(e.response));
                trace("  Config :{0}".format(JSON.stringify(configuration)));
                localStorage.setItem("ComputerIp", configuration.ComputerIp);
                sendMessageToWatch(
                    {
                        KEY_REQUEST_ID  : KEY_REQUEST_ID_GET_IP,
                        KEY_COMPUTER_IP : configuration.ComputerIp
                    }, 
                    "webviewclosed event"
                );
            }
            else {
                trace("No data was send from the config app");
            }
        }
    );    
}

Init();
