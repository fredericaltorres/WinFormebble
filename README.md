WinFormebble
------------

# Overview

WinFormebble is a C librarary to program the Pebble smart watch
that is friendly to C# and WinForm developer.

# Api Overview

		void Form_New(Form *form, WindowHandler load, WindowHandler unload);
		void Form_Destructor(Form *form);
		void Form_AddLabel(Form *form, TextLayer * label);
		void Form_RegisterButtonHandlers(Form *form, ClickHandler selectClickHandler, ClickHandler upClickHandler, ClickHandler downClickHandler);

		GFont Font_Load(uint32_t resourceFontId);
		GFont Font_LoadSystem(const char *font_key);

		TextLayer * Label_New(GRect frame, BackGroundColorType backGroundType, GTextAlignment alignment, const char *fontName);
		void Label_SetText(TextLayer * label, const char * text);
		void Label_Destructor(TextLayer * label);
		void Label_SetFont(TextLayer * label, GFont font);
		void Label_SetSystemFont(TextLayer * label, const char *fontName);

		void Timer_Register(TimeUnits tick_units, TickHandler handler);

		MenuLayer * Menu_New(Form *form);
		void Menu_Add(char * entry);
		void Menu_Destructor(Menu menu);

		Trace_TraceError(fmt, args...) 
		Trace_TraceInformation(fmt, args...)
		Trace_TraceWarning(fmt, args...)
		Trace_TraceDebug(fmt, args...)
		Trace_TraceInformationVerbose(fmt, args...)


# Samples

## Hello World Sample
```C
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

```