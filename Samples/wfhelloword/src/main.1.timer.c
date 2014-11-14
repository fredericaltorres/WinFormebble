/*
 * Hello Word App for the Pebble with WinFormebble
 * 
 * (C) Torres Frederic 2014
 * Use the library WinFormebble (https://github.com/fredericaltorres/WinFormebble)
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
    Timer tmrMain;
    
    private void tmrMain_Tick(void * data)  {
        
        static char timeBuffer [8+1];
        struct tm *tick_time = DateTime_Now();
        
        // Format %T -> HH:MM:SS -- http://www.cplusplus.com/reference/ctime/strftime
        Label_SetText(lblTime, StringFormatTime(tick_time, "%T", timeBuffer));
    }
    private void mainForm_Load(Window *window) {
        
        lblMsg = Label_New(GRect(0, 20, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblMsg, "Hello World");
        Form_AddLabel(mainForm, lblMsg);
               
        lblTime = Label_New(GRect(0, 60, 139, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblTime, WATCH_DIGIT_BUFFER);
        Form_AddLabel(mainForm, lblTime);
        
        tmrMain = Form_StartTimer(mainForm, 1000, tmrMain_Tick);
    }
    private void mainForm_Unload(Window *window) {
        
    }
  

int main(void) { 
    mainForm = Form_New();
    Form_Initialize(mainForm, mainForm_Load, mainForm_Unload);
    Form_Show(mainForm);
    app_event_loop();
    Form_Destructor(mainForm);  // Also clean all associated controls
}
*/