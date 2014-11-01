#include <pebble.h>
#include "WinFormebble.h"
#include <pebble_fonts.h>

Form mainForm;

    Menu mainMenu;

    event mainMenu_Click(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
        
        vibes_short_pulse();
    }    
    event mainForm_Load(Window *window) {
        
        mainMenu = Menu_New(&mainForm, mainMenu_Click);
        Menu_Add("AA");
        Menu_Add("BB");
        Menu_Add("CC");
    }
    event mainForm_UnLoad(Window *window) {
        
        Menu_Destructor(mainMenu);
    }

////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////

static void init() {
       
    Form_New(&mainForm, mainForm_Load, mainForm_UnLoad);
}
static void deinit() {    
    
    Form_Destructor(&mainForm);  // Also clean all associated controls
}
int main(void) { 
    
    init();
    app_event_loop();
    deinit();
}