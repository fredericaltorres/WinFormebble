/*
 * Hello Word App for the Pebble with WinFormebble
 * 
 * (C) Torres Frederic 2014
 * Use the library WinFormebble (https://github.com/fredericaltorres/WinFormebble)
 * Licence: MIT
 *
 */
#include <pebble.h>
#include <pebble_fonts.h>

#define Trace_TraceDebugOn  1 // WinFormebble Trace Modes
#define Form_TraceOn        1

#include "WinFormebble.h"
    
#define WATCH_DIGIT_BUFFER "00:00:00"    
    
Form mainForm;
    
    Label lblMsg;
    Label lblTime;
    Timer _timer = NULL;
    
    private void _timer_Tick(void * data)  {
        
        struct tm * now   = memoryM()->NewDate();
        char * timeFormat = memoryM()->FormatDateTime(now, "%H:%M:%S");
        Label_SetText(lblTime, timeFormat);
        memoryM()->Free(timeFormat);
    }
    private void mainForm_Load(Window *window) {
        
        lblMsg = Label_New(GRect(0, 20, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Form_AddLabel(mainForm, lblMsg);
        Label_SetText(lblMsg, "Hello World");
               
        lblTime = Label_New(GRect(0, 60, 139, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Form_AddLabel(mainForm, lblTime);
        Label_SetText(lblTime, WATCH_DIGIT_BUFFER);
        
        _timer = Form_StartTimer(mainForm, 1000, _timer_Tick);
    }
    private void mainForm_Unload(Window *window) {
        
    }

int main(void) { 
 
    mainForm = Form_New();
    Form_Initialize(mainForm, mainForm_Load, mainForm_Unload);
    Form_Show(mainForm);
        
    app_event_loop();
    
    Form_Destructor(mainForm);  // Also clean all associated controls    
    memoryM()->FreeAll();
}
