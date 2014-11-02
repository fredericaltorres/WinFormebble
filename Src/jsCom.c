#include <pebble.h>
#include "jsCom.h"
    
static AppMessageInboxReceived _received_callback;

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    
    if(_received_callback != NULL) { // Call user callback if defined
        _received_callback(iterator, context);
    }
}
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}
static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    //APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void jsCom_Initialize(AppMessageInboxReceived received_callback) {
    
    _received_callback = received_callback;
    
    // Register internal callback
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    
    // Open AppMessage
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}
void jsCom_SendMessage(uint8_t key) {
    
     jsCom_SendIntMessage(key, 0);
}
void jsCom_SendIntMessage(uint8_t key, uint8_t cmd) {
    
	DictionaryIterator *iter;
 	app_message_outbox_begin(&iter);
 	
 	Tuplet value = TupletInteger(key, cmd);
 	dict_write_tuplet(iter, &value);
 	
 	app_message_outbox_send();
}
bool jsCom_SendStringMessage(int key, char *text) {
    
    if ((key == -1) && (text == NULL)) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "no data to send");
        // well, the "nothing" that was sent to us was queued, anyway ...
        return true;
    }
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);
    if (iter == NULL) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "null iter");
        return false;
    }
    Tuplet tuple = (text == NULL) ? TupletInteger(key, 1) : TupletCString(key, text);
    dict_write_tuplet(iter, &tuple);
    dict_write_end(iter);
    
    app_message_outbox_send();
    return true;
}

