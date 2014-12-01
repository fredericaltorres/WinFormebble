WinFormebble
------------

# Overview

WinFormebble is a C librarary to program the Pebble smart watch
that is friendly to C# and WinForm developer.

WinFormebble, use the memory manager library name [MemoryM](https://github.com/fredericaltorres/MemoryM).

Text associated to label control are completely managed by MemoryM and
do not not need to be freed.

The method Form_Destructor() will free all control associated to the form.



**** This library is in ALPHA mode.***

# Contact

    Creator: FredericALTorres@gmail.com
    http://forums.getpebble.com/discussion/18161/pebble-for-c-and-winform-developer?new=1

# To Do

- Implement a mini memory manager
- Associate to Label a buffer managed by the memory manager
- Add Image Control
- Add MessageBox Yes/no support
- Label associated with Form, should be really local to the form (I am not shure how)
    - Support multiple dialog with the same label name

# Api Overview

```C

// Form -----------------------------------------------------------

Form  Form_New();
void  Form_Initialize(Form form, WindowHandler load, WindowHandler unload);
void  Form_Show(Form form);
void  Form_Destructor(Form form);

void  Form_AddLabel(Form form, TextLayer * label);
void  Form_RegisterButtonHandlers(Form form, ClickHandler selectClickHandler, 
                                  ClickHandler upClickHandler, ClickHandler downClickHandler);

Timer Form_StartTimer(Form form, uint32_t timeout_ms, AppTimerCallback callback);
Timer Form_StopTimer(Timer timer);
Timer Form_ResumeTimer(Timer timer);
bool  Form_IsTimerEnabled(Timer timer);
void  Form_RegisterWatchFaceTimer(TimeUnits tick_units, TickHandler handler);
void  Form_UnregisterWatchFaceTimer();

GRect Form_GetRectangle(Form form);
void  Form_SetPaintEvent(Form form, LayerUpdateProc paintEvent);
void  Form_ReDraw(Form form);

// Font -----------------------------------------------------------

GFont Font_Load(uint32_t resourceFontId);
GFont Font_LoadSystem(const char * font_key);

// Label -----------------------------------------------------------

TextLayer * Label_New(GRect frame, BackGroundColorType backGroundType, 
                      GTextAlignment alignment, const char * fontName);
void Label_SetText(TextLayer * label, const char * text);
void Label_Destructor(TextLayer * label);
void Label_SetFont(TextLayer * label, GFont font);
void Label_SetSystemFont(TextLayer * label, const char * fontName);

// Menu -----------------------------------------------------------

MenuLayer * Menu_New(Form form);
void Menu_Add(char * entry);
void Menu_Destructor(Menu menu);

// Trace -----------------------------------------------------------

#define Trace_TraceError(fmt, args...) 
#define Trace_TraceInformation(fmt, args...)
#define Trace_TraceWarning(fmt, args...) 
#define Trace_TraceDebug(fmt, args...)  
#define Trace_TraceInformationVerbose(fmt, args...)

// Local Storge On The Watch ---------------------------------------

localDB()->GetBool(int key);
localDB()->GetInt(int key);
localDB()->GetString(int key, char* buffer, int maxBuffer);
localDB()->SetBool(int key, bool val);
localDB()->SetInt(int key, int val);
localDB()->SetString(int key, char* val);
localDB()->Delete(int key);
```




# Samples

## Hello World Sample
```C
#include <pebble.h>
#include <pebble_fonts.h>
#include "WinFormebble.h"
    
#define WATCH_DIGIT_BUFFER "00:00:00"    
    
Form mainForm;

    Label lblMsg;
   
    private void mainForm_Load(Window *window) {
        
        lblMsg = Label_New(GRect(0, 20, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Form_AddLabel(mainForm, lblMsg);
        Label_SetText(lblMsg, "Hello World");
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
```


## Hello World With Timer Sample
```C
#include <pebble.h>
#include <pebble_fonts.h>
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
*/
```


## Hello World With Timer And Buttons Sample
```C
#include <pebble.h>
#include <pebble_fonts.h>
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
    private char * StopStartTimer() {
        
        if(Form_IsTimerEnabled(_timer)) {
            Form_StopTimer(_timer);
            return "Stopped!";
        }
        else {
            Form_ResumeTimer(_timer);
            return "Started!";
        }
    }
    private void butUp_Click(ClickRecognizerRef recognizer, void *context) {
        
        Label_SetText(lblMsg, StopStartTimer());
    }
    private void butDown_Click(ClickRecognizerRef recognizer, void *context) {
        
        Label_SetText(lblMsg, StopStartTimer());
    }
    private void mainForm_Load(Window *window) {
        
        lblMsg = Label_New(GRect(0, 20, 144, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Form_AddLabel(mainForm, lblMsg);
        Label_SetText(lblMsg, "Hello World");
               
        lblTime = Label_New(GRect(0, 60, 139, 25), WhiteBackground, GTextAlignmentCenter, FONT_KEY_ROBOTO_CONDENSED_21);
        Form_AddLabel(mainForm, lblTime);
        Label_SetText(lblTime, WATCH_DIGIT_BUFFER);
        
        Form_RegisterButtonHandlers(mainForm, NULL, butUp_Click, butDown_Click);
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

```