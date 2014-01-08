#include <pebble.h>
#include "hands_layer.h"
#include "chapter_layer.h"

enum {
	FARTS_KEY_BG = 0,
	FARTS_KEY_HAND_STROKE = 1,
	FARTS_KEY_HAND_FILL = 2,
	FARTS_KEY_CONFIG = 100
};

typedef struct Config {
	int8_t background_color;
	int8_t hand_stroke_color;
	int8_t hand_fill_color;
} __attribute__((__packed__)) Config;

static Window *window;
static HandsLayer *hands_layer;
static ChapterLayer *chapter_layer;

static Config config = {
	.background_color = GColorBlack,
	.hand_stroke_color = GColorWhite,
	.hand_fill_color = GColorBlack
};

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

static int8_t get_tuple_value(Tuple *tuple) {
	if (tuple->length == 1) {
		return tuple->value->int8;
	}
	if (tuple->length == 2) {
		return tuple->value->int16;
	}
	return tuple->value->int32;
}

static void handle_inbox_received(DictionaryIterator *iter, void *context) {
	Tuple *background_tuple = dict_find(iter, FARTS_KEY_BG);
	Tuple *stroke_tuple = dict_find(iter, FARTS_KEY_HAND_STROKE);
	Tuple *fill_tuple = dict_find(iter, FARTS_KEY_HAND_FILL);
	int8_t tuple_value;

	if (background_tuple && background_tuple->type == TUPLE_INT) {
		tuple_value = get_tuple_value(background_tuple);
		if (tuple_value != config.background_color) {
			config.background_color = tuple_value;
			chapter_layer_set_color(chapter_layer, config.background_color == GColorBlack ? GColorWhite : GColorBlack);
			window_set_background_color(window, config.background_color);
		}
	}
	if (stroke_tuple && stroke_tuple->type == TUPLE_INT) {
		tuple_value = get_tuple_value(stroke_tuple);
		if (tuple_value != config.hand_stroke_color) {
			config.hand_stroke_color = tuple_value;
			hands_layer_set_stroke_color(hands_layer, config.hand_stroke_color);
		}
	}
	if (fill_tuple && fill_tuple->type == TUPLE_INT) {
		tuple_value = get_tuple_value(fill_tuple);
		if (tuple_value != config.hand_fill_color) {
			config.hand_fill_color = tuple_value;
			hands_layer_set_fill_color(hands_layer, config.hand_fill_color);
		}
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
	if (persist_exists(FARTS_KEY_CONFIG)) {
		persist_read_data(FARTS_KEY_CONFIG, &config, sizeof(config));
	}
	else {
		// Request from phone
		DictionaryIterator *iter;
		app_message_outbox_begin(&iter);

		Tuplet value = TupletInteger(FARTS_KEY_CONFIG, 1);
		dict_write_tuplet(iter, &value);

		app_message_outbox_send();
	}
}

static void persist_save(void) {
	persist_write_data(FARTS_KEY_CONFIG, &config, sizeof(config));
}

static void init(void) {
	app_message_init();
	persist_init();

	window = window_create();
	window_stack_push(window, true);
	window_set_background_color(window, config.background_color);

	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	time_t now = time(NULL);

	// chapters
	chapter_layer = chapter_layer_create(bounds, config.background_color == GColorBlack ? GColorWhite : GColorBlack);
	layer_add_child(window_layer, chapter_layer);

	// hands
	hands_layer = hands_layer_create(bounds, config.hand_fill_color, config.hand_stroke_color,
			&HOUR_HAND_POINTS, &MINUTE_HAND_POINTS, &SECOND_HAND_POINTS);
	hands_layer_update(hands_layer, localtime(&now));
	layer_add_child(window_layer, hands_layer);

	tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
}

static void deinit(void) {
	persist_save();
	tick_timer_service_unsubscribe();
	hands_layer_destroy(hands_layer);
	chapter_layer_destroy(chapter_layer);
	window_destroy(window);
}

int main(void) {
	init();

	APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

	app_event_loop();
	deinit();
}
