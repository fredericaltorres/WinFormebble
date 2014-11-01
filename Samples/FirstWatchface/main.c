/*
 * Pebble Watchface
 */
#include <pebble.h>
#include <pebble_fonts.h>
#include "WinFormebble.h"
#include "jsCom.h"
    
#include <stdarg.h>

int myfunc(int count, ...)
{
   va_list list;
   int j = 0;

   va_start(list, count); 
   for(j=0; j<count; j++)
   {
     printf("%d", va_arg(list, int));
   }

   va_end(list);

   return count;
}

// Api data returned from JavaScript World to C world
// Dictionary propertues used in communication with Pebble iOS App
#define KEY_TEMPERATURE 0
#define KEY_CONDITIONS 1

// Api Request from the C world to JavaScript World    
#define JS_API_GET_WHEATHER 0

#define WATCH_DIGIT_BUFFER  "00:00"
#define NOT_INITIALIZED "..."
#define LOADING "Loading..."
    
Form mainForm;

    Label lblTime;
    Label lblWeather;
    Label lblDate;
    Label lblMonth;

    event mainForm_Load(Window *window) {
        
        lblDate = Label_New(GRect(0, 0, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblDate, NOT_INITIALIZED);
        Form_AddLabel(&mainForm, lblDate);
        
        lblMonth = Label_New(GRect(0, 28, 144, 50), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblMonth, NOT_INITIALIZED);
        Form_AddLabel(&mainForm, lblMonth);
        
        lblTime = Label_New(GRect(5, 60, 139, 50), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_BOLD_SUBSET_49);
        Label_SetText(lblTime, WATCH_DIGIT_BUFFER);
        Form_AddLabel(&mainForm, lblTime);
        
        lblWeather = Label_New(GRect(0, 130, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblWeather, LOADING);
        Form_AddLabel(&mainForm, lblWeather);
    }
    event mainForm_Unload(Window *window) {
        
    }
  
    void mainForm_UpdateTime() {    
        
        time_t temp             = time(NULL);  // Get a tm structure
        struct tm *tick_time    = localtime(&temp);
        static char timeBuffer [06];
        static char dateBuffer [16];
        static char monthBuffer[16];
            
        Label_SetText(lblTime, StringFormatTime(tick_time, clock_is_24h_style() ? "%H:%M" : "%I:%M", timeBuffer, sizeof(timeBuffer)));        
        Label_SetText(lblDate, StringFormatTime(tick_time, "%A", dateBuffer, sizeof(dateBuffer)));                
        Label_SetText(lblMonth, StringFormatTime(tick_time, "%B %d", monthBuffer, sizeof(monthBuffer)));
    }
    void mainForm_EveryMinuteTimer(struct tm *tick_time, TimeUnits units_changed) {
        
        mainForm_UpdateTime();
        if(tick_time->tm_min % 20 == 0) { // Get weather update 20 minutes
            jsCom_SendMessage(JS_API_GET_WHEATHER);
        }
    }
    void  mainForm_InboxReceivedCallback(DictionaryIterator *iterator, void *context) {
      
        static char temperature_buffer[8]; // Store incoming information
        static char conditions_buffer[32];
        static char weather_layer_buffer[32];
    
        Tuple *t = dict_read_first(iterator); // Read first item
        while(t != NULL) {
            switch(t->key) {
                case KEY_TEMPERATURE: StringFormatInt((int)t->value->int32, "%dC", temperature_buffer, sizeof(temperature_buffer)); break;            
                case KEY_CONDITIONS : StringFormat(t->value->cstring, "%s", conditions_buffer, sizeof(conditions_buffer)); break;
                
                default             : APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key); break;
            }
            t = dict_read_next(iterator);
        }
        snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "(%s, %s)", temperature_buffer, conditions_buffer);
        Label_SetText(lblWeather, weather_layer_buffer);
    }

////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////

static void init() {
       
    Form_New(&mainForm, mainForm_Load, mainForm_Unload);
    mainForm_UpdateTime();
    Timer_Register(MINUTE_UNIT, mainForm_EveryMinuteTimer);
    jsCom_Initialize(mainForm_InboxReceivedCallback);
    
    Trace_TraceError("HELLO WORLDISH %d", 1);
    Trace_TraceInformation("HELLO WORLDISH %d", 2);
    Trace_TraceWarning("HELLO WORLDISH %d", 3);
    Trace_TraceDebug("HELLO WORLDISH %d", 4);
}

static void deinit() {
    
    Form_Destructor(&mainForm);  // Also clean all associated controls
}

int main(void) { 
  init();
  app_event_loop();
  deinit();
}