/*
 * WinFormebble Library
 * A C# and Winform friendly library for the developing for the Pebble watch
 *
 * (C) Torres Frederic 2014
 *
 * Licence: MIT
 */
#include <pebble.h>
#include "WinFormebble.h"
    
private int __WinFormebble__UniqueInt = 0;

private int __WinFormebble__GetNewUniqueInt() {
   
    __WinFormebble__UniqueInt += 1;
    return __WinFormebble__UniqueInt;
}
    
    
/*
 *  Watch Face Unique Timer
 */    

static TickHandler _WatchFaceTimer_Hanlder = NULL;

static void __WatchFaceTimer_Hanlder__(struct tm *tick_time, TimeUnits units_changed) {
    
    if(_WatchFaceTimer_Hanlder != NULL) {
        _WatchFaceTimer_Hanlder(tick_time, units_changed);
    }
}
void Form_RegisterWatchFaceTimer(TimeUnits tick_units, TickHandler handler) {
    
    _WatchFaceTimer_Hanlder = handler;
    tick_timer_service_subscribe(tick_units, __WatchFaceTimer_Hanlder__); // Register with TickTimerService
}
void Form_UnregisterWatchFaceTimer() {
    
    tick_timer_service_unsubscribe();
}

/*
 * App Timer
 */

/*
AppTimer * AppTimer_Register(uint32_t timeout_ms, AppTimerCallback callback, void * callback_data) {
    
    _AppTimer_Hanlder = callback;
    _AppTimer_Timeout = timeout_ms;
    return app_timer_register(_AppTimer_Timeout, __AppTimer_Hanlder__, callback_data);
}
void AppTimer_Unregister(AppTimer * timerHandle) {
    
    app_timer_cancel(timerHandle);
}*/


/*
 *     Form Methods
 */    
ControlInfo* ControlInfo_NewInstance(ControlTypeEnum controlType, void* control) {

	ControlInfo* i;
	i              = (ControlInfo*)malloc(sizeof(ControlInfo));
	i->ControlType = controlType;
	i->Control     = control;
	return i;
}
Form Form_New() {
    Form f = (Form)malloc(sizeof(FormStruct));
    f->_controlInfos = ControlInfo_New();
    f->WindowHandle = window_create();
    return f;
}
void Form_Initialize(Form form, WindowHandler load, WindowHandler unload) {
    
    window_set_window_handlers(form->WindowHandle, (WindowHandlers)  {
        .load   = load,
        .unload = unload
    });
    window_stack_push(form->WindowHandle, true);
}
void Form_Show(Form form) {
    
    window_stack_push(form->WindowHandle, true);
}
void Form_Destructor(Form form) { 
    
    for(int i=0; i<ControlInfo_GetLength(form->_controlInfos); i++) {
        
        ControlInfo *ci = ControlInfo_Get(form->_controlInfos, i);
        if(ci->ControlType == CONTROL_TYPE_LABEL) {
            Label_Destructor((TextLayer*)ci->Control);
        }
    }
    ControlInfo_Destructor(form->_controlInfos);
    window_destroy(form->WindowHandle);
    
    if(_WatchFaceTimer_Hanlder != NULL) {
        Form_UnregisterWatchFaceTimer();
    }
    free(form);
}
void Form_AddLabel(Form form, TextLayer * label) {
    
    layer_add_child(window_get_root_layer(form->WindowHandle), text_layer_get_layer(label));
    ControlInfo_Push(form->_controlInfos, ControlInfo_NewInstance(CONTROL_TYPE_LABEL, (void*)label));
}

static AppTimerCallback _AppTimer_Handler = NULL;
uint32_t                _AppTimer_Timeout = 0;
bool                    _AppTimer_On      = false;

static void __AppTimer_Hanlder__(void * data) {
    
    if((_AppTimer_On) && (_AppTimer_Handler != NULL)) {
        _AppTimer_Handler(data);
        if(_AppTimer_On) {
            app_timer_register(_AppTimer_Timeout, __AppTimer_Hanlder__, NULL);
        }
    }
}
Timer Form_StartTimer(Form form, uint32_t timeout_ms, AppTimerCallback callback/*, void * callback_data*/) {
    
    _AppTimer_Handler = callback;
    _AppTimer_Timeout = timeout_ms;
    return Form_ResumeTimer(NULL);
    //return app_timer_register(timeout_ms, callback, callback_data);
}
Timer Form_StopTimer(Timer timer) {    
    _AppTimer_On = false;
    //if(timer != NULL) { app_timer_cancel(timer); }
    return NULL;
}
Timer Form_ResumeTimer(Timer timer) {
    _AppTimer_On = true;
    return app_timer_register(_AppTimer_Timeout, __AppTimer_Hanlder__, NULL);
}
bool Form_IsTimerEnabled(Timer timer) {
    return _AppTimer_On;
}

/*
 * Font Methods
 *
 
System Font -- https://developer.getpebble.com/blog/2013/07/24/Using-Pebble-System-Fonts/

    FONT_KEY_FONT_FALLBACK
    FONT_KEY_GOTHIC_14
    FONT_KEY_GOTHIC_14_BOLD
    FONT_KEY_GOTHIC_18
    FONT_KEY_GOTHIC_18_BOLD
    FONT_KEY_GOTHIC_24
    FONT_KEY_GOTHIC_24_BOLD
    FONT_KEY_GOTHIC_28
    FONT_KEY_GOTHIC_28_BOLD
    FONT_KEY_BITHAM_30_BLACK
    FONT_KEY_BITHAM_42_BOLD
    FONT_KEY_BITHAM_42_LIGHT
    FONT_KEY_BITHAM_42_MEDIUM_NUMBERS
    FONT_KEY_BITHAM_34_MEDIUM_NUMBERS
    FONT_KEY_BITHAM_34_LIGHT_SUBSET
    FONT_KEY_BITHAM_18_LIGHT_SUBSET
    FONT_KEY_ROBOTO_CONDENSED_21
    FONT_KEY_ROBOTO_BOLD_SUBSET_49
    FONT_KEY_DROID_SERIF_28_BOLD
*/

GFont Font_Load(uint32_t resourceFontId) {
    
    return fonts_load_custom_font(resource_get_handle(resourceFontId));
}

GFont Font_LoadSystem(const char *font_key) {
    
    return fonts_get_system_font(font_key);    
}

/*
 * Buttons Handlers
 */

static ClickHandler __Button__Select__ClickHandler = NULL;
static ClickHandler __Button__Down__ClickHandler   = NULL;
static ClickHandler __Button__Up__ClickHandler     = NULL;

static void __Form__butSelect_Click(ClickRecognizerRef recognizer, void *context) {
    
    if(__Button__Select__ClickHandler != NULL) 
        __Button__Select__ClickHandler(recognizer, context);
}
static void __Form__butUp_Click(ClickRecognizerRef recognizer, void *context) {
    
    if(__Button__Up__ClickHandler != NULL) 
        __Button__Up__ClickHandler(recognizer, context);    
}
static void __Form__butDown_Click(ClickRecognizerRef recognizer, void *context) {
    
    if(__Button__Down__ClickHandler != NULL) 
        __Button__Down__ClickHandler(recognizer, context);    
}
static void __Form__ButHandlerConfigProvider(void *context) {
    
    window_single_click_subscribe(BUTTON_ID_SELECT, __Form__butSelect_Click);
    window_single_click_subscribe(BUTTON_ID_UP, __Form__butUp_Click);
    window_single_click_subscribe(BUTTON_ID_DOWN, __Form__butDown_Click);
}
void Form_RegisterButtonHandlers(Form form, ClickHandler selectClickHandler, ClickHandler upClickHandler, ClickHandler downClickHandler) {
    
    window_set_click_config_provider(form->WindowHandle, __Form__ButHandlerConfigProvider);
    if(selectClickHandler != NULL) {
        __Button__Select__ClickHandler = selectClickHandler;
    }
    if(upClickHandler != NULL) {
        __Button__Up__ClickHandler = upClickHandler;
    }
    if(downClickHandler != NULL) {
        __Button__Down__ClickHandler = downClickHandler;
    }
}

/*
 *     Label Methods
 */    

TextLayer * Label_New(GRect frame, BackGroundColorType backGroundType, GTextAlignment alignment, const char *fontName) {
    
    TextLayer *label;
    label = text_layer_create(frame);
    if(backGroundType == BlackBackground) {
        text_layer_set_background_color(label, GColorBlack);
        text_layer_set_text_color(label, GColorClear);        
    }
    else {
        text_layer_set_background_color(label, GColorWhite);
        text_layer_set_text_color(label, GColorBlack);                
    }    
    text_layer_set_text_alignment(label, alignment);
    
    if(fontName != NULL) {
        Label_SetSystemFont(label, fontName);
    }    
    return label;
}
/**
 Label_SetText
 @text the text to set. Note that the text buffer must remain allocated. It must be a static
 */
void Label_SetText(TextLayer * label, const char * text) {
    
    text_layer_set_text(label, text);
}
void Label_Destructor(TextLayer * label) {
    
    text_layer_destroy(label);  
}
void Label_SetFont(TextLayer * label, GFont font) {
    
    text_layer_set_font(label, font);
}

void Label_SetSystemFont(TextLayer * label, const char *fontName) {
    
    text_layer_set_font(label, Font_LoadSystem(fontName));
}


/*
 *  Menu
 */

#define _Menu__MaxEntry     8
#define _Menu__EntryMaxLen  48
static char _Menu__Caption[_Menu__MaxEntry][_Menu__EntryMaxLen];
static int _Menu__Counter = 0;

MenuLayerSelectCallback __Menu__MenuLayerSelectCallback = NULL;

void Menu_Add(char * entry) {
    
    strcpy(_Menu__Caption[_Menu__Counter], entry);
    _Menu__Counter++;
}
void __Menu__draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
    
    menu_cell_basic_draw(ctx, cell_layer, _Menu__Caption[cell_index->row], "", NULL);
}
uint16_t __Menu__num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
    return _Menu__Counter;
}
void __Menu__select_click_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    
    if(__Menu__MenuLayerSelectCallback != NULL) {
        __Menu__MenuLayerSelectCallback(menu_layer, cell_index, callback_context);
    }
}
MenuLayer * Menu_New(Form form/*, MenuLayerSelectCallback menuLayerSelectCallback*/) {
    
    _Menu__Counter                  = 0;
    //__Menu__MenuLayerSelectCallback = menuLayerSelectCallback;
    MenuLayer * menu_layer          = menu_layer_create(GRect(0, 0, 144, 168 - 16));
    
    menu_layer_set_click_config_onto_window(menu_layer, form->WindowHandle);
    
    MenuLayerCallbacks callbacks = {
        .draw_row     = (MenuLayerDrawRowCallback) __Menu__draw_row_callback,
        .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback) __Menu__num_rows_callback,
        .select_click = (MenuLayerSelectCallback) __Menu__select_click_callback
    };
    menu_layer_set_callbacks(menu_layer, NULL, callbacks); 
    layer_add_child(window_get_root_layer(form->WindowHandle), menu_layer_get_layer(menu_layer));
    return menu_layer;
}
void Menu_Destructor(MenuLayer *menuLayer) {
    
    menu_layer_destroy(menuLayer);
}

/*
 * Trace
 */    

//#define Trace_TraceError(fmt, args...) 
//#define Trace_TraceInformation(fmt, args...) 
//#define Trace_TraceWarning(fmt, args...) 
//#define Trace_TraceDebug(fmt, args...)  
//#define Trace_TraceInformationVerbose(fmt, args...) 

/*
    Tutorials
    http://ninedof.wordpress.com/pebble-sdk-tutorial/
    Images
    Pebble Image https://github.com/pebble/PebbleUI
*/


   /** 
	 * ControlInfo Clode File
	 * Array Class: ControlInfo
	 * Generated Code based on the darray.c library 
	 */

	DArray*      ControlInfo_New       ()                                           { return darray_init();                               }
	void         ControlInfo_Push      (DArray *array, ControlInfo *s)              { darray_push(array, s);                              }
	ControlInfo* ControlInfo_Pop       (DArray *array)                              { return (ControlInfo *)darray_pop(array);        }        
	ControlInfo* ControlInfo_Get       (DArray *array, int index)                   { return (ControlInfo *)darray_get(array, index); } 
	void         ControlInfo_Set       (DArray *array, int index, ControlInfo *s)   { darray_set(array, index, s);                        }
	void         ControlInfo_Destructor(DArray *array)                              { darray_free(array);                                 }
    int          ControlInfo_GetLength (DArray *array)                              { return array->last; }
            