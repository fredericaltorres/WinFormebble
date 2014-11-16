/*
 * FredTime - a Pebble Watchface
 * 
 * (C) Torres Frederic 2014
 *
 * Display the following:
 * - Day in the week, 
 * - Month, 
 * - Day of the month
 * - Location based on MapQuest web service
 * - Temperature and weather condition based on web service openweathermap
 *
 * Use the library WinFormebble (https://github.com/fredericaltorres/WinFormebble)
 *
 * Licence: MIT
 *
 */
#include <pebble.h>  
#include <pebble_fonts.h>
#include "WinFormebble.h"

// Api data returned from JavaScript World to C world
// Dictionary properties used in communication with Pebble iOS App
// Metadata defined in project settings    
#define KEY_REQUEST_ID 0
        #define KEY_REQUEST_ID_GET_WEATHER 0
        #define KEY_REQUEST_ID_GET_LOCATION 1   
#define KEY_CONDITIONS 1
#define KEY_LOCATION 2
#define KEY_TEMPERATURE 3
#define KEY_VIBRATE 4 // 1 true 0 false
#define KEY_STREET 5
    
// Data Refresh Rate
#define WEATHER_REFRESH_RATE  50 // minutes    
#define LOCATION_REFRESH_RATE 12 // minutes    

// Strings and messages
#define WATCH_DIGIT_BUFFER "00:00"
#define NOT_INITIALIZED "..."
#define LOADING "Loading..."
    
#define DEFAULT_STRING_BUFFER_SIZE 16    
    
Form mainForm;

    Label lblTime;
    Label lblWeather;
    Label lblDate;
    Label lblMonth;
    Label lblLocation;

    static char _currentHour[3];
    static char _street[DEFAULT_STRING_BUFFER_SIZE];
    static char _address[DEFAULT_STRING_BUFFER_SIZE];
    static int _apiCount = 0;
    static int _timerCount = 0;

    private void mainForm_Load(Window *window) {
        
        Trace_TraceDebug("-- mainForm_Load --");
        
        strcpy(_street, NOT_INITIALIZED);
        strcpy(_address, NOT_INITIALIZED);
        
        lblDate = Label_New(GRect(0, 8, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblDate, NOT_INITIALIZED);
        Form_AddLabel(mainForm, lblDate);
        
        lblMonth = Label_New(GRect(0, 33, 144, 50), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblMonth, NOT_INITIALIZED);
        Form_AddLabel(mainForm, lblMonth);
        
        lblTime = Label_New(GRect(5, 55, 139, 50), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_BOLD_SUBSET_49);
        Label_SetText(lblTime, WATCH_DIGIT_BUFFER);
        Form_AddLabel(mainForm, lblTime);
        
        lblWeather = Label_New(GRect(0, 110, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblWeather, LOADING);
        Form_AddLabel(mainForm, lblWeather);
        
        lblLocation = Label_New(GRect(0, 134, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblLocation, NOT_INITIALIZED);
        Form_AddLabel(mainForm, lblLocation);
    }
    private void mainForm_Unload(Window *window) {
        
       Trace_TraceDebug("-- mainForm_Load --"); 
    }  
    private void mainForm_UpdateTime() {    
        
        Trace_TraceDebug("-- mainForm_UpdateTime --");
        
        time_t temp             = time(NULL);  // Get a tm structure
        struct tm *tick_time    = localtime(&temp);
        static char timeBuffer [DEFAULT_STRING_BUFFER_SIZE];
        static char dateBuffer [DEFAULT_STRING_BUFFER_SIZE];
        static char monthBuffer[DEFAULT_STRING_BUFFER_SIZE];
        char newHour[3];
        
        // Vibrate a t the beginning of each hour
        strcpy(newHour, StringFormatTime(tick_time, "%H", newHour));
        if(strcmp(_currentHour, newHour) != 0) {
            strcpy(_currentHour, newHour);
            vibes_short_pulse();
        }
            
        Label_SetText(lblTime,  StringFormatTime(tick_time, clock_is_24h_style() ? "%H:%M" : "%I:%M", timeBuffer));
        Label_SetText(lblDate,  StringFormatTime(tick_time, "%A", dateBuffer));
        Label_SetText(lblMonth, StringFormatTime(tick_time, "%B %d", monthBuffer));
    }
    /**
     * showAddress
     * Display or the current street or city, state, country info
     * is called every minute, by main timer
     */
    private void showAddress() {
        
        if(_apiCount % 2 == 0) 
            Label_SetText(lblLocation, _address);
        else 
            Label_SetText(lblLocation, _street);
        _apiCount++;
    }
    private void mainForm_EveryMinuteTimer(struct tm *tick_time, TimeUnits units_changed) {
        
        mainForm_UpdateTime();         
        showAddress(); 
        
        if((_timerCount == 0) || (tick_time->tm_min % WEATHER_REFRESH_RATE == 0)) { // Get weather update every 50 minutes (and will also get the location)
            
            jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_GET_WEATHER);
        }
        else if((_timerCount == 0) || (tick_time->tm_min % LOCATION_REFRESH_RATE == 0)) { // Get location every 12 minutes when we are not asking for weather
            
            jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_GET_LOCATION);
        }
    }
    private int CelsiusToFahrenheit(int v) {
        
        double d = (v * 9.0 / 5.0) + 32.0;
        return d;
    } 
    private void mainForm_InboxReceivedCallback(DictionaryIterator *iterator, void *context) {
      
        static char temperature[DEFAULT_STRING_BUFFER_SIZE]; // Must be static
        static char conditions [DEFAULT_STRING_BUFFER_SIZE];
        static char weather    [DEFAULT_STRING_BUFFER_SIZE];
        int requestId = -1;
        int tempCelsius = 0;
        int vibrate = 0;
    
        Tuple * t = dict_read_first(iterator);
        while(t != NULL) {
            
            switch(t->key) {
                
                case KEY_REQUEST_ID : requestId = t->value->int32;                            break;
                case KEY_TEMPERATURE: tempCelsius = t->value->int32;                          break;
                case KEY_CONDITIONS : StringFormatString(t->value->cstring, "%s", conditions);break;
                case KEY_LOCATION   : StringFormatString(t->value->cstring, "%s", _address);  break;
                case KEY_STREET     : StringFormatString(t->value->cstring, "%s", _street);   break;
                case KEY_VIBRATE    : vibrate = t->value->int32;                              break;
                default             : APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key); break;
            }
            t = dict_read_next(iterator);
        }
        
        StringFormat(AsBuffer(weather), "%dC,%dF - %s", tempCelsius, CelsiusToFahrenheit(tempCelsius), conditions);
        Label_SetText(lblWeather, weather);
        showAddress();
        
        if(vibrate) { // Api requested to vibrate to notify something to the user
             vibes_short_pulse();
        }
        
        // If we just received the response for Get Weather, not initiate Get Location
        if(requestId == KEY_REQUEST_ID_GET_WEATHER) {
            jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_GET_LOCATION);
        }
    }
    

////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////

int main(void) { 
    
    mainForm = Form_New();
    Form_Initialize(mainForm, mainForm_Load, mainForm_Unload);
    Form_Show(mainForm);
    mainForm_UpdateTime();
    jsCom_Initialize(mainForm_InboxReceivedCallback);
    Form_RegisterWatchFaceTimer(MINUTE_UNIT, mainForm_EveryMinuteTimer);    
        
    app_event_loop();
    
    Form_Destructor(mainForm);  // Also clean all associated controls
    
}
