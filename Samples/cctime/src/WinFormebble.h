/*
 * WinFormebble Library
 * A C# and Winform friendly library for the developing for the Pebble watch
 *
 * (C) Torres Frederic 2014
 *
 * Licence: MIT
 */
#include <pebble.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "WinFormebble.Utils.h"  
 
#define event static void   
#define Label static TextLayer *
#define Menu MenuLayer *
#define Timer AppTimer*

private int __WinFormebble__GetNewUniqueInt();

    
// Form -----------------------------------------------------------
typedef enum {
    CONTROL_TYPE_LABEL = 0,
    CONTROL_TYPE_TIMER = 1,
} ControlTypeEnum;

typedef struct {
    
    ControlTypeEnum ControlType;
    void * Control;
    
} ControlInfo;

// ControlInfo Array
DArray*      ControlInfo_New       ();
void         ControlInfo_Push      (DArray *array, ControlInfo *s);
ControlInfo* ControlInfo_Pop       (DArray *array);
ControlInfo* ControlInfo_Get       (DArray *array, int index);
void         ControlInfo_Set       (DArray *array, int index, ControlInfo *s);
void         ControlInfo_Destructor(DArray *array);
int          ControlInfo_GetLength (DArray *array);
ControlInfo* ControlInfo_NewInstance(ControlTypeEnum controlType, void * control);

typedef struct {
    
    Window    *WindowHandle;
    DArray*   _controlInfos;
    
} FormStruct;

#define Form FormStruct*

Form  Form_New();
void  Form_Initialize(Form form, WindowHandler load, WindowHandler unload);
void  Form_Show(Form form);
void  Form_Destructor(Form form);
void  Form_AddLabel(Form form, TextLayer * label);
void  Form_RegisterButtonHandlers(Form form, ClickHandler selectClickHandler, ClickHandler upClickHandler, ClickHandler downClickHandler);
Timer Form_StartTimer(Form form, uint32_t timeout_ms, AppTimerCallback callback/*, void * callback_data*/);
Timer Form_StopTimer(Timer timer);
Timer Form_ResumeTimer(Timer timer);
bool  Form_IsTimerEnabled(Timer timer);

// Font -----------------------------------------------------------

GFont Font_Load(uint32_t resourceFontId);
GFont Font_LoadSystem(const char * font_key);

// Label -----------------------------------------------------------

typedef enum {
    BlackBackground = 0,  
    WhiteBackground = 1,  
} BackGroundColorType;

TextLayer * Label_New(GRect frame, BackGroundColorType backGroundType, GTextAlignment alignment, const char * fontName);
void Label_SetText(TextLayer * label, const char * text);
void Label_Destructor(TextLayer * label);
void Label_SetFont(TextLayer * label, GFont font);
void Label_SetSystemFont(TextLayer * label, const char * fontName);


/*
 *  Watch face and app unique timer
 */
void Form_RegisterWatchFaceTimer(TimeUnits tick_units, TickHandler handler);
void Form_UnregisterWatchFaceTimer();

MenuLayer * Menu_New(Form form);
void Menu_Add(char * entry);
void Menu_Destructor(Menu menu);

/*
 * Trace
 */    

#define Trace_TraceError(fmt, args...)  app_log(APP_LOG_LEVEL_ERROR, __FILE_NAME__, __LINE__, fmt, ## args)
#define Trace_TraceInformation(fmt, args...)  app_log(APP_LOG_LEVEL_INFO, __FILE_NAME__, __LINE__, fmt, ## args)
#define Trace_TraceWarning(fmt, args...)  app_log(APP_LOG_LEVEL_WARNING, __FILE_NAME__, __LINE__, fmt, ## args)
#define Trace_TraceDebug(fmt, args...)  app_log(APP_LOG_LEVEL_DEBUG, __FILE_NAME__, __LINE__, fmt, ## args)
#define Trace_TraceInformationVerbose(fmt, args...)  app_log(APP_LOG_LEVEL_DEBUG_VERBOSE, __FILE_NAME__, __LINE__, fmt, ## args)



