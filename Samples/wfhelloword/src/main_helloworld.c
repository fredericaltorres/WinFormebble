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
    
Form mainForm;

    Label lblMsg;

    event mainForm_Load(Window *window) {
        
        lblMsg = Label_New(GRect(0, 8, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblMsg, "Hello World");
        Form_AddLabel(&mainForm, lblMsg);
    }
    event mainForm_Unload(Window *window) {
       
    }


int main(void) { 
    
    Form_New(&mainForm, mainForm_Load, mainForm_Unload);
    app_event_loop();
    Form_Destructor(&mainForm);  // Also clean all associated controls
}
*/