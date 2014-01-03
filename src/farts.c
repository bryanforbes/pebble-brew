#include <pebble.h>

enum {
	FARTS_KEY_TEXT = 0
};

static Window *window;
static TextLayer *text_layer;
static char text[128];

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Tick");
}

static void handle_inbox_received(DictionaryIterator *iter, void *context) {
	Tuple *text_tuple = dict_find(iter, FARTS_KEY_TEXT);

	if (text_tuple) {
		strncpy(text, text_tuple->value->cstring, 128);
		persist_write_string(FARTS_KEY_TEXT, text);
		text_layer_set_text(text_layer, text);
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Log: %s", text);
	}
}

static void handle_inbox_dropped(AppMessageResult reason, void *context) {
}

static void app_message_init(void) {
	app_message_register_inbox_received(handle_inbox_received);
	app_message_register_inbox_dropped(handle_inbox_dropped);
	app_message_open(64, 64);
}

static void persist_init(void) {
	if (persist_exists(FARTS_KEY_TEXT)) {
		persist_read_string(FARTS_KEY_TEXT, text, 128);
	}
}

static void persist_save(void) {
	persist_write_data(FARTS_KEY_TEXT, text, 128);
}

static void init(void) {
	window = window_create();
	window_stack_push(window, true);
	window_set_background_color(window, GColorBlack);

	text_layer = text_layer_create(GRect(29, 54, 144-40, 168-54));
	text_layer_set_text_color(text_layer, GColorWhite);
	text_layer_set_background_color(text_layer, GColorClear);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));

	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	layer_add_child(window_layer, text_layer_get_layer(text_layer));

	persist_init();
	app_message_init();
	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

static void deinit(void) {
	tick_timer_service_unsubscribe();
	text_layer_destroy(text_layer);
	window_destroy(window);
}

int main(void) {
	init();

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

	app_event_loop();
	deinit();
}
