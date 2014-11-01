/*
 * WinFormebble
 * Wrapper around the pebble API, more friendly to C# and Win Form developers
 */
#include <pebble.h>
#include "WinFormebble.h"

/*
 *     Form Methods
 */    
void Form_New(Form *form, WindowHandler load, WindowHandler unload) {
        
    //form->_labelCounter = 0;    
    vector_init(&form->_vectorLabels);
    
    form->WindowHandle = window_create();
    window_set_window_handlers(form->WindowHandle, (WindowHandlers)  {
        .load   = load,
        .unload = unload
    });
    window_stack_push(form->WindowHandle, true);
}
void Form_Destructor(Form *form) {    
    int i;
    // Free the labels
    for(i=0; i<form->_vectorLabels.size; i++) {       
       Label_Destructor((TextLayer*)vector_get(&form->_vectorLabels, i));
    }
    window_destroy(form->WindowHandle);
    vector_free(&form->_vectorLabels);
}
void Form_AddLabel(Form *form, TextLayer * label) {
    
    layer_add_child(window_get_root_layer(form->WindowHandle), text_layer_get_layer(label));
    vector_append(&form->_vectorLabels, (int)label);
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
void Form_RegisterButtonHandlers(Form *form, ClickHandler selectClickHandler, ClickHandler upClickHandler, ClickHandler downClickHandler) {
    
    window_set_click_config_provider(form->WindowHandle, __Form__ButHandlerConfigProvider);
    __Button__Select__ClickHandler = selectClickHandler;
    __Button__Up__ClickHandler     = upClickHandler;
    __Button__Down__ClickHandler   = downClickHandler;
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
 *  Unique Timer
 */    

static TickHandler _Timer_Hanlder = NULL;

static void __Timer_Hanlder__(struct tm *tick_time, TimeUnits units_changed) {
    
    if(_Timer_Hanlder != NULL) {
        _Timer_Hanlder(tick_time, units_changed);
    }
}
void Timer_Register(TimeUnits tick_units, TickHandler handler) {
    
    _Timer_Hanlder = handler;
    tick_timer_service_subscribe(tick_units, __Timer_Hanlder__); // Register with TickTimerService
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
MenuLayer * Menu_New(Form * form/*, MenuLayerSelectCallback menuLayerSelectCallback*/) {
    
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