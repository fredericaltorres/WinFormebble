/*
 * Hello Word App for the Pebble with WinFormebble
 * 
 * (C) Torres Frederic 2014
 *
 * Use the library WinFormebble (https://github.com/fredericaltorres/WinFormebble)
 *
 * Licence: MIT
 *
 *
#include <pebble.h>
#include <pebble_fonts.h>
#include "WinFormebble.h"
    
#define WATCH_DIGIT_BUFFER "00:00:00"    
    
Form mainForm;

    Label lblMsg;
    Label lblTime;
    Timer _timer;
    
    event timer_callback(void * data) {
        
        static char timeBuffer [8+1];
        struct tm *tick_time = DateTime_Now();
        
        Label_SetText(lblTime, StringFormatTime(tick_time, "%T", timeBuffer));
        _timer = AppTimer_Register(1000, timer_callback, NULL);
    }
    event mainForm_Load(Window *window) {
        
        lblMsg = Label_New(GRect(0, 20, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblMsg, "Hello World");
        Form_AddLabel(&mainForm, lblMsg);
               
        lblTime = Label_New(GRect(0, 60, 139, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblTime, WATCH_DIGIT_BUFFER);
        Form_AddLabel(&mainForm, lblTime);
        
        _timer = AppTimer_Register(1000, timer_callback, NULL);
    }
    event mainForm_Unload(Window *window) {
        
    }
  

int main(void) { 
    
    Form_New(&mainForm, mainForm_Load, mainForm_Unload);
    app_event_loop();
    Form_Destructor(&mainForm);  // Also clean all associated controls
}
*/