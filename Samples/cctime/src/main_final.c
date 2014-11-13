/*
 * CCTime - a Pebble Watchface for Code Camp 22
 * (C) Torres Frederic 2014
 *
 * Use the library WinFormebble (https://github.com/fredericaltorres/WinFormebble)
 * Licence: MIT
 *
#include <pebble.h>  
#include <pebble_fonts.h>
#include "WinFormebble.h"
#include "jsCom.h"

// Constants
#define WATCH_DIGIT_BUFFER "00:00"   
#define CODE_CAMP_22_DAY 2014, 11, 22, 0, 0, 0        
#define DEFAULT_STRING_BUFFER_SIZE 16    
    
Form mainForm;

    Label lblDate;
    Label lblWaitTimeInDays;
    Label lblWaitTimeInHours;
    Label lblTime2;

    private void mainForm_UpdateUI() {    
                
        static char timeBuffer    [DEFAULT_STRING_BUFFER_SIZE];
        static char dateBuffer    [DEFAULT_STRING_BUFFER_SIZE];
        static char msgBufferDays [DEFAULT_STRING_BUFFER_SIZE*2];
        static char msgBufferHours[DEFAULT_STRING_BUFFER_SIZE*2];
        
        struct tm * now       = DateTime_Now();
        struct tm * eventDay  = DateTime(CODE_CAMP_22_DAY);
        int waitTimeInDays    = DateTime_Diff('d', now, eventDay);
        int waitTimeInHours   = DateTime_Diff('h', now, eventDay);
                
        Label_SetText(lblDate,  StringFormatTime(now, "%a - %b %d", dateBuffer));
        Label_SetText(lblTime2,  StringFormatTime(now, "%H:%M", timeBuffer));
        
        StringFormat(AsBuffer(msgBufferDays), "%d days to CC", waitTimeInDays);
        Label_SetText(lblWaitTimeInDays, msgBufferDays);
        
        StringFormat(AsBuffer(msgBufferHours), "%d hours to CC", waitTimeInHours); 
        Label_SetText(lblWaitTimeInHours, msgBufferHours);       
    }
    private void mainForm_EveryMinuteTimer(struct tm *tick_time, TimeUnits units_changed) {
        
        mainForm_UpdateUI();         
    }
    private void mainForm_Load(Window *window) {
         
        int y = 10;
        lblDate = Label_New(GRect(0, y, 143, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblDate, "Date");
        Form_AddLabel(mainForm, lblDate);
        y += 30;
        
        lblTime2 = Label_New(GRect(0, y, 143, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblTime2, "Time");
        Form_AddLabel(mainForm, lblTime2);
        y += 50;
        
        lblWaitTimeInDays = Label_New(GRect(0, y, 143, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblWaitTimeInDays, "Number of days");
        Form_AddLabel(mainForm, lblWaitTimeInDays);
        y += 30;
        
        lblWaitTimeInHours = Label_New(GRect(0, y, 143, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Label_SetText(lblWaitTimeInHours, "Number of hours");
        Form_AddLabel(mainForm, lblWaitTimeInHours);
        y += 30;
        
        Form_RegisterWatchFaceTimer(MINUTE_UNIT, mainForm_EveryMinuteTimer);
    }
    private void mainForm_Unload(Window *window) {
        
    }  
  


int main(void) { 
    
    mainForm = Form_New();
    Form_Initialize(mainForm, mainForm_Load, mainForm_Unload);
    app_event_loop();
    Form_Destructor(mainForm);  // Clean all associated controls and un register timer
}
*/