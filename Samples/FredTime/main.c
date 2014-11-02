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
#inclu
#include <pebble.h>
#include <pebble_fonts.h>
#include "WinFormebble.h"
#include "jsCom.h"

// Api data returned from JavaScript World to C world
// Dictionary properties used in communication with Pebble iOS App
// Metadata defined in project settings    
#define KEY_REQUEST_ID 0
        #define KEY_REQUEST_ID_GET_WEATHER 0
        #define KEY_REQUEST_ID_GET_LOCATION 1   
#define KEY_CONDITIONS 1
#define KEY_LOCATION 2
#define KEY_TEMPERATURE 3

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

    event mainForm_Load(Window *window) {
        
        lblDate = Label_New(GRect(0, 8, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblDate, NOT_INITIALIZED);
        Form_AddLabel(&mainForm, lblDate);
        
        lblMonth = Label_New(GRect(0, 33, 144, 50), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblMonth, NOT_INITIALIZED);
        Form_AddLabel(&mainForm, lblMonth);
        
        lblTime = Label_New(GRect(5, 55, 139, 50), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_BOLD_SUBSET_49);
        Label_SetText(lblTime, WATCH_DIGIT_BUFFER);
        Form_AddLabel(&mainForm, lblTime);
        
        lblWeather = Label_New(GRect(0, 110, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblWeather, LOADING);
        Form_AddLabel(&mainForm, lblWeather);
        
        lblLocation = Label_New(GRect(0, 134, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblLocation, NOT_INITIALIZED);
        Form_AddLabel(&mainForm, lblLocation);
    }
    event mainForm_Unload(Window *window) {
        
    }  
    void mainForm_UpdateTime() {    
        
        time_t temp             = time(NULL);  // Get a tm structure
        struct tm *tick_time    = localtime(&temp);
        static char timeBuffer [DEFAULT_STRING_BUFFER_SIZE];
        static char dateBuffer [DEFAULT_STRING_BUFFER_SIZE];
        static char monthBuffer[DEFAULT_STRING_BUFFER_SIZE];
            
        Label_SetText(lblTime,  StringFormatTime(tick_time, clock_is_24h_style() ? "%H:%M" : "%I:%M", timeBuffer));
        Label_SetText(lblDate,  StringFormatTime(tick_time, "%A", dateBuffer));
        Label_SetText(lblMonth, StringFormatTime(tick_time, "%B %d", monthBuffer));
    }
    void mainForm_EveryMinuteTimer(struct tm *tick_time, TimeUnits units_changed) {
        
        mainForm_UpdateTime();        
        if(tick_time->tm_min % 20 == 0) { // Get weather update 20 minutes
            jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_GET_WEATHER);
        }
    }
    void  mainForm_InboxReceivedCallback(DictionaryIterator *iterator, void *context) {
      
        static char temperature[DEFAULT_STRING_BUFFER_SIZE]; // Must be static
        static char conditions [DEFAULT_STRING_BUFFER_SIZE];
        static char weather    [DEFAULT_STRING_BUFFER_SIZE];
        static char location   [DEFAULT_STRING_BUFFER_SIZE];
        int requestId = -1;
    
        Tuple * t = dict_read_first(iterator);
        while(t != NULL) {
            switch(t->key) {
                case KEY_REQUEST_ID : requestId = t->value->int32;                            break;
                case KEY_TEMPERATURE: StringFormatInt(t->value->int32, "%dC", temperature);   break;            
                case KEY_CONDITIONS : StringFormatString(t->value->cstring, "%s", conditions);break;
                case KEY_LOCATION   : StringFormatString(t->value->cstring, "%s", location);  break;
                default             : APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key); break;
            }
            t = dict_read_next(iterator);
        }
        StringFormat(AsBuffer(weather), "%s - %s", temperature, conditions);
        Label_SetText(lblWeather, weather);
        Label_SetText(lblLocation, location);
        
        // If we just received the response for Get Weather, not initiate Get Location
        if(requestId == KEY_REQUEST_ID_GET_WEATHER) {
            jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_GET_LOCATION);
        }
    }

////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////

static void init() {
       
    Form_New(&mainForm, mainForm_Load, mainForm_Unload);
    mainForm_UpdateTime();
    jsCom_Initialize(mainForm_InboxReceivedCallback);
    Timer_Register(MINUTE_UNIT, mainForm_EveryMinuteTimer);

    //Trace_TraceError("HELLO WORLDISH %d", 1);
    //Trace_TraceInformation("HELLO WORLDISH %d", 2);
    //Trace_TraceWarning("HELLO WORLDISH %d", 3);
    //Trace_TraceDebug("HELLO WORLDISH %d", 4);    
    //jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_GET_WEATHER);    
    jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_GET_WEATHER);
}

static void deinit() {
    
    Form_Destructor(&mainForm);  // Also clean all associated controls
}

int main(void) { 
    
    init();
    app_event_loop();
    deinit();
}