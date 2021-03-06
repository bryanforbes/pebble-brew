#include "hands_layer.h"

typedef struct HandsLayerData {
	GPoint center;

	GColor background;
	GColor fill;
	GColor stroke;

	GPath *hour_hand;
	GPath *minute_hand;
	GPath *second_hand;

	int current_hours;
	int current_minutes;
	int current_seconds;

	int hours;
	int minutes;
	int seconds;
} HandsLayerData;

static void hands_layer_update_proc(Layer *layer, GContext *ctx) {
	HandsLayerData *data = layer_get_data(layer);

	graphics_context_set_fill_color(ctx, data->fill);
	graphics_context_set_stroke_color(ctx, data->stroke);

	if (data->second_hand != NULL) {
		if (data->current_seconds != data->seconds) {
			data->seconds = data->current_seconds;
			gpath_rotate_to(data->second_hand, TRIG_MAX_ANGLE * data->seconds / 60);
		}
		if (data->stroke != GColorClear) {
			gpath_draw_outline(ctx, data->second_hand);
		}
		if (data->fill != GColorClear) {
			gpath_draw_filled(ctx, data->second_hand);
		}
	}

	if (data->current_minutes != data->minutes || data->current_hours != data->hours) {
		data->minutes = data->current_minutes;
		data->hours = data->current_hours;
		gpath_rotate_to(data->minute_hand, TRIG_MAX_ANGLE * data->minutes / 60);
		gpath_rotate_to(data->hour_hand, (TRIG_MAX_ANGLE * (((data->hours % 12) * 6) + (data->minutes / 10))) / 72);
	}

	// TODO: make center circle configurable
	graphics_draw_circle(ctx, data->center, 4);

	if (data->stroke != GColorClear) {
		gpath_draw_outline(ctx, data->minute_hand);
		gpath_draw_outline(ctx, data->hour_hand);
	}
	if (data->fill != GColorClear) {
		gpath_draw_filled(ctx, data->minute_hand);
		gpath_draw_filled(ctx, data->hour_hand);
	}
}

static void set_colors(HandsLayerData *data, GColor background, GColor fill, GColor stroke) {
	data->background = background;

	if (fill == background) {
		data->fill = GColorClear;
	}
	else {
		data->fill = fill;
	}
	if (stroke == background) {
		data->stroke = GColorClear;
	}
	else {
		data->stroke = stroke;
	}
}

HandsLayer* hands_layer_create(GRect frame, GColor background, GColor fill, GColor stroke,
							   const GPathInfo *hour_path_info,
							   const GPathInfo *minute_path_info,
							   const GPathInfo *second_path_info) {
	HandsLayerData *data;
	HandsLayer *layer = layer_create_with_data(frame, sizeof *data);
	data = layer_get_data(layer);

	data->center = grect_center_point(&frame);

	set_colors(data, background, fill, stroke);

	data->hour_hand = gpath_create(hour_path_info);
	gpath_move_to(data->hour_hand, data->center);

	data->minute_hand = gpath_create(minute_path_info);
	gpath_move_to(data->minute_hand, data->center);

	if (second_path_info != NULL) {
		data->second_hand = gpath_create(second_path_info);
		gpath_move_to(data->second_hand, data->center);
	}

	layer_set_update_proc(layer, hands_layer_update_proc);

	return layer;
}

void hands_layer_destroy(HandsLayer *layer) {
	HandsLayerData *data = layer_get_data(layer);

	gpath_destroy(data->hour_hand);
	gpath_destroy(data->minute_hand);

	if (data->second_hand != NULL) {
		gpath_destroy(data->second_hand);
	}

	layer_destroy(layer);
}

void hands_layer_update(HandsLayer *layer, const struct tm *time) {
	HandsLayerData *data = layer_get_data(layer);

	data->current_hours = time->tm_hour % 12;
	data->current_minutes = time->tm_min;
	data->current_seconds = time->tm_sec;

	layer_mark_dirty(layer);
}

void hands_layer_set_colors(HandsLayer *layer, GColor background, GColor fill, GColor stroke) {
	HandsLayerData *data = layer_get_data(layer);

	set_colors(data, background, fill, stroke);

	layer_mark_dirty(layer);
}

void hands_layer_set_fill_color(HandsLayer *layer, GColor color) {
	HandsLayerData *data = layer_get_data(layer);

	data->fill = color;

	layer_mark_dirty(layer);
}

void hands_layer_set_stroke_color(HandsLayer *layer, GColor color) {
	HandsLayerData *data = layer_get_data(layer);

	data->stroke = color;

	layer_mark_dirty(layer);
}
