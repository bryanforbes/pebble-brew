#include <pebble.h>
#include "hands_layer.h"

enum {
	FARTS_KEY_TEXT = 0
};

static Window *window;
static TextLayer *text_layer;
static HandsLayer *hands_layer;
static char text[128];

static const GPathInfo SECOND_HAND_POINTS = {
	.num_points = 5,
	.points = (GPoint []) {
		{ 2, -10 },
		{ 4, -55 },
		{ 0, -60 },
		{ -4, -55 },
		{ -2, -10 }
	}
};

static const GPathInfo MINUTE_HAND_POINTS = {
	.num_points = 5,
	.points = (GPoint []) {
		{ 2, -10 },
		{ 4, -50 },
		{ 0, -55 },
		{ -4, -50 },
		{ -2, -10 }
	}
};

static const GPathInfo HOUR_HAND_POINTS = {
	.num_points = 5,
	.points = (GPoint []) {
		{ 2, -10 },
		{ 4, -40 },
		{ 0, -45 },
		{ -4, -40 },
		{ -2, -10 }
	}
};

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	hands_layer_update(hands_layer, tick_time);
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

	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	// text
	text_layer = text_layer_create(GRect(29, 54, 144-40, 168-54));
	text_layer_set_text_color(text_layer, GColorWhite);
	text_layer_set_background_color(text_layer, GColorClear);
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(text_layer));

	time_t now = time(NULL);

	// hands
	hands_layer = hands_layer_create(bounds,
			&HOUR_HAND_POINTS, &MINUTE_HAND_POINTS, &SECOND_HAND_POINTS);
	hands_layer_update(hands_layer, localtime(&now));
	layer_add_child(window_layer, hands_layer);

	persist_init();
	app_message_init();
	tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

static void deinit(void) {
	tick_timer_service_unsubscribe();
	text_layer_destroy(text_layer);
	hands_layer_destroy(hands_layer);
	window_destroy(window);
}

int main(void) {
	init();

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

	app_event_loop();
	deinit();
}
