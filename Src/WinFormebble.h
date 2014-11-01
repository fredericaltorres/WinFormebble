/*
 * WinFormebble
 * Wrapper around the pebble API, more friendly to C#, WinForm and Windows developers.
 */
#include <pebble.h>
#include "WinFormebble.Utils.h"  
    
/*
    
*/    
 
//#define class(type) static type   
//#define classy(type, var) static type #var;
#define event static void   
#define Label static TextLayer *
#define Menu MenuLayer *
    
// Form -----------------------------------------------------------
typedef struct {
    
    Window    *WindowHandle;
    Vector    _vectorLabels;
} Form;

void Form_New(Form *form, WindowHandler load, WindowHandler unload);
void Form_Destructor(Form *form);
void Form_AddLabel(Form *form, TextLayer * label);
void Form_RegisterButtonHandlers(Form *form, ClickHandler selectClickHandler, ClickHandler upClickHandler, ClickHandler downClickHandler);

// Font -----------------------------------------------------------

GFont Font_Load(uint32_t resourceFontId);
GFont Font_LoadSystem(const char *font_key);

// Label -----------------------------------------------------------

typedef enum {
    BlackBackground = 0,  
    WhiteBackground = 1,  
} BackGroundColorType;

TextLayer * Label_New(GRect frame, BackGroundColorType backGroundType, GTextAlignment alignment, const char *fontName);
void Label_SetText(TextLayer * label, const char * text);
void Label_Destructor(TextLayer * label);
void Label_SetFont(TextLayer * label, GFont font);
void Label_SetSystemFont(TextLayer * label, const char *fontName);


/*
 *  Unique Timer
 */    
void Timer_Register(TimeUnits tick_units, TickHandler handler);

void Menu_Add(char * entry);
MenuLayer * Menu_New(Form *form);
void Menu_Destructor(Menu menu);

/*
 * Trace
 */    

#define Trace_TraceError(fmt, args...)  app_log(APP_LOG_LEVEL_ERROR, __FILE_NAME__, __LINE__, fmt, ## args)
#define Trace_TraceInformation(fmt, args...)  app_log(APP_LOG_LEVEL_INFO, __FILE_NAME__, __LINE__, fmt, ## args)
#define Trace_TraceWarning(fmt, args...)  app_log(APP_LOG_LEVEL_WARNING, __FILE_NAME__, __LINE__, fmt, ## args)
#define Trace_TraceDebug(fmt, args...)  app_log(APP_LOG_LEVEL_DEBUG, __FILE_NAME__, __LINE__, fmt, ## args)
#define Trace_TraceInformationVerbose(fmt, args...)  app_log(APP_LOG_LEVEL_DEBUG_VERBOSE, __FILE_NAME__, __LINE__, fmt, ## args)



