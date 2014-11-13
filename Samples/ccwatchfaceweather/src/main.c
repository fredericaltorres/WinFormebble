/*
 * A Pebble Watchface that show times and weather
 * Demo for Code Camp 22, MA, USA
 * (C) Torres Frederic 2014
 * Licence: MIT
 */
#include <pebble.h>  
#include <pebble_fonts.h>
#include "WinFormebble.h"
#include "jsCom.h"

// Api data returned from JavaScript World to C world
// Dictionary properties used in communication with Pebble iOS App
// Metadata defined in project settings    
#define KEY_REQUEST_ID 0
#define KEY_REQUEST_ID_GET_WEATHER 0
#define KEY_CONDITIONS 1
#define KEY_TEMPERATURE 2
    
// Data Refresh Rate
#define DEFAULT_STRING_BUFFER_SIZE 16    

// Strings and messages
#define WATCH_DIGIT_BUFFER "00:00"
#define NOT_INITIALIZED "..."
#define LOADING_WEATHER "Loading weather"
#define LOADING_LOCATION "Loading location"
    
#define WEATHER_REFRESH_RATE 3 // Every 3 minutes    
#define WATCH_MAX_WIDTH  144
    
    
Form mainForm;

    Label lblTime;
    Label lblWeather;
    Label lblDate;
    Label lblMonth;

    int _timerCallCount = 0;

    private void mainForm_Load(Window *window) {
        
        int y = 10;
        lblDate = Label_New(GRect(0, y, WATCH_MAX_WIDTH, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblDate, NOT_INITIALIZED);
        Form_AddLabel(mainForm, lblDate);
        y += 30;
        
        lblMonth = Label_New(GRect(0, y, WATCH_MAX_WIDTH, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblMonth, NOT_INITIALIZED);
        Form_AddLabel(mainForm, lblMonth);
        y += 20;
        
        lblTime = Label_New(GRect(0, y, WATCH_MAX_WIDTH, 50), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_BOLD_SUBSET_49);
        Label_SetText(lblTime, WATCH_DIGIT_BUFFER);
        Form_AddLabel(mainForm, lblTime);
        y += 60;
        
        lblWeather = Label_New(GRect(0, y, WATCH_MAX_WIDTH, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblWeather, LOADING_WEATHER);
        Form_AddLabel(mainForm, lblWeather);
    }
    private void mainForm_Unload(Window *window) {
        
    }  
    private void mainForm_UpdateTime() {    
        
        struct tm *tick_time   = DateTime_Now();
        static char timeBuffer [DEFAULT_STRING_BUFFER_SIZE];
        static char dateBuffer [DEFAULT_STRING_BUFFER_SIZE];
        static char monthBuffer[DEFAULT_STRING_BUFFER_SIZE];
        
        Trace_TraceDebug("-- mainForm_UpdateTime time:%s", StringFormatTime(tick_time, "%H:%M", timeBuffer));
            
        Label_SetText(lblTime,  StringFormatTime(tick_time, "%H:%M", timeBuffer));
        Label_SetText(lblDate,  StringFormatTime(tick_time, "%A"   , dateBuffer));
        Label_SetText(lblMonth, StringFormatTime(tick_time, "%B %d", monthBuffer));
    }
    private void mainForm_EveryMinuteTimer(struct tm *tick_time, TimeUnits units_changed) {
        
        mainForm_UpdateTime();                 
        // Request the weather the first time we call (on start) or every 3 minutes
        if((_timerCallCount == 0) || (tick_time->tm_min % WEATHER_REFRESH_RATE == 0)) { // Get weather update every 3 minutes (and will also get the location)
            
            jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_GET_WEATHER);
        }
        _timerCallCount++;
    }
    private int CelsiusToFahrenheit(int v) {
        
        double d = (v * 9.0 / 5.0) + 32.0;
        return d;
    } 
    private void  mainForm_InboxReceivedCallback(DictionaryIterator *iterator, void *context) {
      
        static char weather    [DEFAULT_STRING_BUFFER_SIZE];
        static char conditions [DEFAULT_STRING_BUFFER_SIZE*2];
        int tempCelsius = 0;
        Tuple * t = dict_read_first(iterator);
        while(t != NULL) {
            
            switch(t->key) {
                case KEY_REQUEST_ID : break;
                case KEY_TEMPERATURE: tempCelsius = t->value->int32; break;
                case KEY_CONDITIONS : StringFormatString(t->value->cstring, "%s", conditions);break;
                default             : APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key); break;
            }
            t = dict_read_next(iterator);
        }
        StringFormat(AsBuffer(weather), "%dC,%dF - %s", tempCelsius, CelsiusToFahrenheit(tempCelsius), conditions);
        Label_SetText(lblWeather, weather);
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
