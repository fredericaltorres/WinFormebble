/*
 * FredTime - a Pebble Watchface
 * 
 * (C) Torres Frederic 2014
 *
 * Display the following:
 * - Day, Month, DayOfMonth
 * - Time hh:mm:ss
 * - Current minute progress 
 * - Info, display alternate
 * -    Current weather
 * -    Address
 * -    City
 *
 * Use the library WinFormebble (https://github.com/fredericaltorres/WinFormebble)
 *
 * Licence: MIT
 *
 */
#include <pebble.h>  
#include <pebble_fonts.h>

#define Trace_TraceDebugOn  1 // WinFormebble Trace Modes
//#define Form_TraceOn        1
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
#define SHOWINFO_REFRESH_RATE 3  // seconds

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
    InfoType_WatchColor = 5,
    InfoType_FirmwareVersion = 6,

    InfoType_Max     = 7,
    InfoType_Min     = 0

} InfoType;

Form mainForm;

    Label lblTime;
    Label lblInfo;
    Label lblDate;
    
    char * _street                    = NULL;
    char * _address                   = NULL;
    char * _conditions                = NULL;
    int    _currentHour               = -1;
    int    _apiCount                  = 0;
    int    _timerCount                = 0;
    int    _lastMinute                = -1;
    int    _currentSecond             = 0;
    bool   _javaScriptCommunicationOn = true;

    InfoType _infoType = InfoType_Min;

    private void mainForm_Load(Window *window) {
        
        Trace_TraceDebug("--- mainForm_Load --");

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
    private void mainForm_Paint(Layer *layer, GContext *ctx) {

        graphics_context_set_fill_color(ctx, GColorBlack);

        int drawY      = 100;
        int drawXStart = 12; // To center the line
        int drawXLen   = 60*2;
        int drawYLen   = 7;

        //GRect r = GRect(drawXStart, drawY, drawXStart+drawXLen, drawY+drawYLen);
        int border = 3;
        graphics_draw_rect(ctx, GRect(drawXStart-border, drawY-border, drawXLen+border, drawYLen+border));
        
        GPoint start   = GPoint(drawXStart, drawY);
        GPoint end     = GPoint(drawXStart, drawY);
        end.x          = drawXStart+(_currentSecond * 2);

        for(int i=0; i<4; i++) {
            graphics_draw_line(ctx, start, end);
            start.y += 1;
            end.y   += 1;
        }
    }
    private void mainForm_Unload(Window *window) {
        
       Trace_TraceDebug("-- mainForm_Unload --"); 
    }
   
    private void ShowInfo() {

        char* firmware              = NULL;
        char* watchColor            = NULL;
        char * info                 = NULL;
        static char * dotString     = "...";
        static char * oktring       = "Ok?";

        switch(_infoType) {

            case InfoType_Street     : info = _street;     break;
            case InfoType_Address    : info = _address;    break;
            case InfoType_Weather    : info = _conditions; break;
            case InfoType_Dot        : info = dotString;   break;
            case InfoType_Ok         : info = oktring;     break;
            case InfoType_WatchColor :
                watchColor = watch()->GetColor();
                info       = memoryM()->Format("%s watch", watchColor);
            break;
            case InfoType_FirmwareVersion:
                firmware   = watch()->GetFirmwareVersion();
                info       = memoryM()->Format("%s Firmware", firmware);
            break;
            case InfoType_Max        : break; // Required by the compiler 
        }
        Label_SetText(lblInfo, info);

        _infoType += 1;
        if(_infoType == InfoType_Max) {
            _infoType = InfoType_Min;
        }
        if((watchColor != NULL)||(firmware != NULL)) { // Clean if we allocated some string to display the watch color
            memoryM()->FreeMultiple(3, watchColor, firmware, info);
        }
    }
    private void mainForm_EveryMinuteTimer(struct tm *tick_time, TimeUnits units_changed) {
        
        char * dateBuffer;
        
        // Vibrate at the beginning of each hour
        if (tick_time->tm_year != _currentHour) {
            _currentHour = tick_time->tm_year;
            vibes_short_pulse();
        }
        // Set Day, Month, DayOfMonth
        Label_SetText(lblDate, (dateBuffer  = memoryM()->FormatDateTime(tick_time, "%a %b %d")));
        memoryM()->Free(dateBuffer);
        Trace_TraceDebug("-- mainForm_EveryMinuteTimer -- memoryM:[%d/%d]", memoryM()->GetCount(), memoryM()->GetMemoryUsed());
                
        if(_javaScriptCommunicationOn) {

            if((_timerCount == 0) || (tick_time->tm_min % WEATHER_REFRESH_RATE == 0)) { // Get weather update every 50 minutes (and will also get the location)
                
                jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_GET_WEATHER);
            }
            else if((_timerCount == 1) || (tick_time->tm_min % LOCATION_REFRESH_RATE == 0)) { // Get location every 12 minutes when we are not asking for weather
                
                jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_GET_LOCATION);
            }
        }
        _timerCount++;
    }
    private void mainForm_EverySecondTimer(struct tm *tick_time, TimeUnits units_changed) {

        _currentSecond = tick_time->tm_sec;

        char * timeBuffer = memoryM()->FormatDateTime(tick_time, "%H:%M:%S");

        Label_SetText(lblTime, timeBuffer);

        memoryM()->Free(timeBuffer); 

        if(_lastMinute != tick_time->tm_min) {

            _lastMinute = tick_time->tm_min;
            mainForm_EveryMinuteTimer(tick_time, units_changed);
        }
        if(tick_time->tm_sec % SHOWINFO_REFRESH_RATE == 0) {

            ShowInfo();
        }
    }
    private int CelsiusToFahrenheit(int v) {
        
        double d = (v * 9.0 / 5.0) + 32.0;
        return d;
    } 
    private void mainForm_InboxReceivedCallback(DictionaryIterator *iterator, void *context) {
      
        char conditions[64];
        //memset(conditions, 0, sizeof(conditions));

        int tempCelsius    = 0;
        int vibrate        = 0;
        int requestId      = -1;
    
        Tuple * t = dict_read_first(iterator);
        while(t != NULL) {

            //Trace_TraceDebug("-- mainForm_InboxReceivedCallback Looping:%d", (int)t->key);

            switch(t->key) {
                
                case KEY_REQUEST_ID : requestId   = t->value->int32;                                        break;
                case KEY_TEMPERATURE: tempCelsius = t->value->int32;                                        break;
                case KEY_CONDITIONS : strcpy(conditions, t->value->cstring);                                break;
                case KEY_LOCATION   : _address    = memoryM()->ReNewString(t->value->cstring, _address);    break;
                case KEY_STREET     : _street     = memoryM()->ReNewString(t->value->cstring, _street);     break;
                case KEY_VIBRATE    : vibrate     = t->value->int32;                                        break;

                default             : APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key); break;
            }
            t = dict_read_next(iterator);
        }

        // For the weather we need 2 info temp and condition so we have to do the formating
        // outside of the while lopp
        if(requestId == KEY_REQUEST_ID_GET_WEATHER) {

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
    
    jsCom_Initialize(mainForm_InboxReceivedCallback);
    Form_RegisterWatchFaceTimer(SECOND_UNIT, mainForm_EverySecondTimer);

    // The paint event is automatically call on the watchface timer
    // No need to call Form_ReDraw()
    Form_SetPaintEvent(mainForm, mainForm_Paint);
        
    app_event_loop();
    
    Form_Destructor(mainForm);  // Also clean all associated controls    
    //memoryM()->FreeAll();
}
