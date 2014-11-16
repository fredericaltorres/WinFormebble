/*
 * A Pebble Watchface that control a PowerPoint presentation
 * using the winRemoteControl api
 * Demo for Code Camp 22, MA, USA
 * (C) Torres Frederic 2014
 * Licence: MIT
 */
#include <pebble.h>  
#include <pebble_fonts.h> 
#include "WinFormebble.h"

// Api data returned from JavaScript World to C world
// Dictionary properties used in communication with Pebble iOS App
// Metadata defined in project settings    
#define KEY_REQUEST_ID 0
    // Values for KEY_REQUEST_ID
    #define KEY_REQUEST_ID_NEXT 0
    #define KEY_REQUEST_ID_PREVIOUS 1
    #define KEY_REQUEST_ID_QUIT 2    
    #define KEY_REQUEST_ID_OK = 3
    #define KEY_REQUEST_ID_GET_IP = 4    
#define KEY_COMPUTER_IP 1 // Also use as the id for the Pebble local storage property
    
// Data Refresh Rate
#define DEFAULT_STRING_BUFFER_SIZE 16    

// Strings and messages
#define WATCH_DIGIT_BUFFER "00:00"
    
#define WATCH_MAX_WIDTH 144    
    
Form mainForm;

    Label lblTime;
    Label lblIp;
    Label lblAction;
    Label lblPage;

    int _pageCounter = 1;
    
    private char * GetComputerIp() {
        
        static char computerIpBuffer [DEFAULT_STRING_BUFFER_SIZE];
        char * computerIp = localDB()->GetString(KEY_COMPUTER_IP, computerIpBuffer, DEFAULT_STRING_BUFFER_SIZE);
        return computerIp;
    }
    private void butSelect_Click(ClickRecognizerRef recognizer, void *context) {
        
        Label_SetText(lblAction, "Quit");
        jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_QUIT);
    }
    private void butUp_Click(ClickRecognizerRef recognizer, void *context) {
        
        Label_SetText(lblAction, "Previous");
        jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_PREVIOUS);
        _pageCounter--;
        if(_pageCounter == 0)
            _pageCounter = 1;
    }
    private void butDown_Click(ClickRecognizerRef recognizer, void *context) {
        
        Label_SetText(lblAction, "Next");
        jsCom_SendIntMessage(KEY_REQUEST_ID, KEY_REQUEST_ID_NEXT);
        _pageCounter++;
    }
    private void DisplayIp() {
        
        static char ipDisplayBuffer [DEFAULT_STRING_BUFFER_SIZE*2];
        StringFormat(AsBuffer(ipDisplayBuffer), "Ip:%s", GetComputerIp());
        Label_SetText(lblIp, ipDisplayBuffer);
        Trace_TraceDebug("C world ComputerIp:%s", ipDisplayBuffer);
    }
    private void mainForm_Load(Window *window) {
        
        int y = 10;
        lblTime = Label_New(GRect(0, y, WATCH_MAX_WIDTH, 50), WhiteBackground, GTextAlignmentCenter, FONT_KEY_GOTHIC_24_BOLD);
        Label_SetText(lblTime, WATCH_DIGIT_BUFFER);
        Form_AddLabel(mainForm, lblTime);
        y += 30;
        
        lblIp = Label_New(GRect(0, y, WATCH_MAX_WIDTH, 50), WhiteBackground, GTextAlignmentCenter, FONT_KEY_GOTHIC_24);
        Label_SetText(lblIp, "Ip:");
        Form_AddLabel(mainForm, lblIp);
        y += 30;
        
        lblAction = Label_New(GRect(0, y, WATCH_MAX_WIDTH, 50), WhiteBackground, GTextAlignmentCenter, FONT_KEY_GOTHIC_24_BOLD);
        Label_SetText(lblAction, "None");
        Form_AddLabel(mainForm, lblAction);
        y += 30;
        
        lblPage = Label_New(GRect(0, y, WATCH_MAX_WIDTH, 50), WhiteBackground, GTextAlignmentCenter, FONT_KEY_GOTHIC_24_BOLD);
        Label_SetText(lblPage, "Page: 1");
        Form_AddLabel(mainForm, lblPage); 
        y += 30;
        
        DisplayIp();
        
        Form_RegisterButtonHandlers(mainForm, butSelect_Click, butUp_Click, butDown_Click);
    }
    private void mainForm_Unload(Window *window) {
        
    }  
    private void mainForm_UpdateTime() {    
        
        struct tm *tick_time   = DateTime_Now();
        static char timeBuffer [DEFAULT_STRING_BUFFER_SIZE];
        Label_SetText(lblTime,  StringFormatTime(tick_time, "%T", timeBuffer));
    }
    private void mainForm_Timer(struct tm *tick_time, TimeUnits units_changed) {
        
        mainForm_UpdateTime();                 
    }
    private void  mainForm_InboxReceivedCallback(DictionaryIterator *iterator, void *context) {
        static char pageCounterBuffer [DEFAULT_STRING_BUFFER_SIZE];
        Tuple * t = dict_read_first(iterator);
        while(t != NULL) {
            
            switch(t->key) {
                case KEY_REQUEST_ID :
                    Label_SetText(lblPage, StringFormatInt(_pageCounter, "Page: %d", pageCounterBuffer)); 
                break;
                case KEY_COMPUTER_IP:  
                    localDB()->SetString(KEY_COMPUTER_IP, t->value->cstring);
                    Trace_TraceDebug("C world ComputerIp:%s", GetComputerIp());
                    DisplayIp();
                break;
                default: APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key); break;
            }
            t = dict_read_next(iterator);
        }
    }
  
int main(void) { 
    
    mainForm = Form_New();
    Form_Initialize(mainForm, mainForm_Load, mainForm_Unload);
    Form_Show(mainForm);
    mainForm_UpdateTime();
    jsCom_Initialize(mainForm_InboxReceivedCallback);
    Form_RegisterWatchFaceTimer(SECOND_UNIT, mainForm_Timer);    
        
    app_event_loop();
    
    Form_Destructor(mainForm);  // Also clean all associated controls
}
