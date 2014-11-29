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
#define LOCATION_REFRESH_RATE 10 // minutes    

// Strings and messages
#define WATCH_DIGIT_BUFFER "00:00:00"
#define NOT_INITIALIZED "..."
    
#define DEFAULT_STRING_BUFFER_SIZE 16    

typedef enum {

    InfoType_Street  = 0,
    InfoType_Address = 1,
    InfoType_Weather = 2,
    InfoType_Dot     = 3,
    InfoType_Ok      = 4,

    InfoType_Max     = 5,
    InfoType_Min     = 0

} InfoType;

Form mainForm;

    Label lblTime;
    Label lblInfo;
    Label lblDate;

    static char _currentHour[3];
    static char * _street     = NULL;
    static char * _address    = NULL;
    static char * _conditions = NULL;
    static int    _apiCount   = 0;
    static int    _timerCount = 0;
    static int   _lastMinute  = -1;

    InfoType _infoType = InfoType_Min;


    private void mainForm_Load(Window *window) {
        
        Trace_TraceDebug("-- mainForm_Load --");

        lblDate = Label_New(GRect(0, 8, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_GOTHIC_24_BOLD);
        Form_AddLabel(mainForm, lblDate);
        Label_SetText(lblDate, NOT_INITIALIZED);
                
        lblTime = Label_New(GRect(5, 55, 139, 50), WhiteBackground, GTextAlignmentCenter, FONT_KEY_GOTHIC_28_BOLD);
        Form_AddLabel(mainForm, lblTime);
        Label_SetText(lblTime, WATCH_DIGIT_BUFFER);
        
        lblInfo = Label_New(GRect(0, 110, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_GOTHIC_24_BOLD);
        Form_AddLabel(mainForm, lblInfo);
        Label_SetText(lblInfo, NOT_INITIALIZED);
        
    }
    private void mainForm_Unload(Window *window) {
        
       Trace_TraceDebug("-- mainForm_Unload --"); 
    }  
    private void mainForm_UpdateTime(struct tm *now) {    

        char * dateBuffer;
        char * newHour;
        
        Trace_TraceDebug("-- mainForm_UpdateTime -- memoryM:[%d/%d]", memoryM()->GetCount(), memoryM()->GetMemoryUsed());
        
        // Vibrate at the beginning of each hour
        newHour = memoryM()->FormatDateTime(now, "%H");
        if(strcmp(_currentHour, newHour) != 0) {
            strcpy(_currentHour, newHour);
            vibes_short_pulse();
        }
        Label_SetText(lblDate,  (dateBuffer  = memoryM()->FormatDateTime(now, "%a %b %d")));
        memoryM()->FreeMultiple(2, newHour, dateBuffer);
    }
    /**
     * showInfo
     */
    private void ShowInfo() {

        char * info = NULL;
        static char * dotString = "...";
        static char * oktring = "Ok?";

        switch(_infoType) {
            case InfoType_Street  : info = _street;     break;
            case InfoType_Address : info = _address;    break;
            case InfoType_Weather : info = _conditions; break;
            case InfoType_Dot     : info = dotString;   break;
            case InfoType_Ok      : info = oktring;   break;
            case InfoType_Max     :  break; // Required by the compiler 
        }
        Label_SetText(lblInfo, info);

        _infoType += 1;
        if(_infoType == InfoType_Max) {
            _infoType = InfoType_Min;
        }
    }
    private void mainForm_EveryMinuteTimer(struct tm *tick_time, TimeUnits units_changed) {
        
        mainForm_UpdateTime(tick_time);
                
        if((_timerCount == 0) || (tick_time->tm_min % WEATHER_REFRESH_RATE == 0)) { // Get weather update every 50 minutes (and will also get the location)
            
            jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_GET_WEATHER);
        }
        else if((_timerCount == 1) || (tick_time->tm_min % LOCATION_REFRESH_RATE == 0)) { // Get location every 12 minutes when we are not asking for weather
            
            jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_GET_LOCATION);
        }
        _timerCount++;
    }
    private void mainForm_EverySecondTimer(struct tm *tick_time, TimeUnits units_changed) {

        char * timeBuffer;
        Label_SetText(lblTime,  (timeBuffer = memoryM()->FormatDateTime(tick_time, "%T")));
        memoryM()->Free(timeBuffer);

        if(_lastMinute != tick_time->tm_min) {

            _lastMinute = tick_time->tm_min;
            mainForm_EveryMinuteTimer(tick_time, units_changed);
        }
        if(tick_time->tm_sec % 3 == 0) {
            ShowInfo();
        }
    }
    private int CelsiusToFahrenheit(int v) {
        
        double d = (v * 9.0 / 5.0) + 32.0;
        return d;
    } 
    private void mainForm_InboxReceivedCallback(DictionaryIterator *iterator, void *context) {
      
        Trace_TraceDebug("-- mainForm_InboxReceivedCallback START"); 

        char conditions [64];
        memset(conditions, 0, sizeof(conditions));

        int tempCelsius    = 0;
        int vibrate        = 0;
    
        Tuple * t = dict_read_first(iterator);
        while(t != NULL) {

            Trace_TraceDebug("-- mainForm_InboxReceivedCallback Looping:%d", (int)t->key);

            switch(t->key) {
                
                case KEY_REQUEST_ID : /*requestId   = t->value->int32;*/                                    break;
                case KEY_TEMPERATURE: tempCelsius = t->value->int32;                                        break;
                case KEY_CONDITIONS : strcpy(conditions, t->value->cstring);                                break;
                case KEY_LOCATION   : _address    = memoryM()->ReNewString(t->value->cstring, _address);    break;
                case KEY_STREET     : _street     = memoryM()->ReNewString(t->value->cstring, _street);     break;
                case KEY_VIBRATE    : vibrate     = t->value->int32;                                        break;

                default             : APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key); break;
            }
            t = dict_read_next(iterator);
        }

        if(strlen(conditions)) {

            memoryM()->Free(_conditions);
            _conditions = memoryM()->Format("%dC, %dF - %s", tempCelsius, CelsiusToFahrenheit(tempCelsius), conditions);
        }

        if(vibrate) { // Api requested to vibrate to notify something to the user
             vibes_short_pulse();
        }
    }
    

////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////

int main(void) { 
    
    mainForm = Form_New();
    Form_Initialize(mainForm, mainForm_Load, mainForm_Unload);
    Form_Show(mainForm);
    //mainForm_UpdateTime();
    jsCom_Initialize(mainForm_InboxReceivedCallback);
    Form_RegisterWatchFaceTimer(SECOND_UNIT, mainForm_EverySecondTimer);
        
    app_event_loop();
    
    Form_Destructor(mainForm);  // Also clean all associated controls    
    memoryM()->FreeAll();
}
