#include "chapter_layer.h"

typedef struct ChapterLayerData {
	GColor color;

	// Cached points for dots
	GPoint points[8];

	// Cached location for numbers
	GRect twelve;
	GRect three;
	GRect six;
	GRect nine;
} ChapterLayerData;

static void draw_text(GContext *ctx, const char* text, const GRect box) {
	graphics_draw_text(
		ctx,
		text,
		fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21),
		box,
		GTextOverflowModeWordWrap,
		GTextAlignmentCenter,
		NULL
	);
}

static GSize get_content_size(const char* text, const GRect box) {
	return graphics_text_layout_get_content_size(
		text,
		fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21),
		box,
		GTextOverflowModeWordWrap,
		GTextAlignmentCenter
	);
}

static void chapter_layer_update_proc(Layer *layer, GContext *ctx) {
	ChapterLayerData *data = layer_get_data(layer);

	graphics_context_set_stroke_color(ctx, data->color);
	graphics_context_set_fill_color(ctx, data->color);
	graphics_context_set_text_color(ctx, data->color);

	for (int i = 0, l = ARRAY_LENGTH(data->points); i < l; i++) {
		graphics_fill_circle(ctx, data->points[i], 2);
	}

	draw_text(ctx, "12", data->twelve);
	draw_text(ctx, "3", data->three);
	draw_text(ctx, "6", data->six);
	draw_text(ctx, "9", data->nine);
}

ChapterLayer* chapter_layer_create(GRect frame, GColor color) {
	ChapterLayerData *data;
	Layer *layer = layer_create_with_data(frame, sizeof *data);
	data = layer_get_data(layer);

	data->color = color;

	GPoint center = grect_center_point(&frame);
	int16_t x = center.x;
	int16_t y = center.y;

	GPoint points[] = {
		GPoint(x + 33, y + 57),
		GPoint(x + 57, y + 33),
		GPoint(x + 57, y - 33),
		GPoint(x + 33, y - 57),
		GPoint(x - 33, y - 57),
		GPoint(x - 57, y - 33),
		GPoint(x - 57, y + 33),
		GPoint(x - 33, y + 57)
	};
	memcpy(&data->points, &points, sizeof data->points);

	GRect bounds = layer_get_frame(layer);

	// Cache calculation of position of the letters
	GSize size = get_content_size("12", bounds);
	data->twelve = GRect(center.x - (size.w / 2), center.y - 66 - (size.h / 2) - 2, size.w, size.h);

	size = get_content_size("3", bounds);
	data->three = GRect(center.x + 66 - (size.w / 2), center.y - (size.h / 2) - 2, size.w, size.h);

	size = get_content_size("6", bounds);
	data->six = GRect(center.x - (size.w / 2), center.y + 66 - (size.h / 2) - 2, size.w, size.h);

	size = get_content_size("9", bounds);
	data->nine = GRect(center.x - 66 - (size.w / 2), center.y - (size.h / 2) - 2, size.w, size.h);

	layer_set_update_proc(layer, chapter_layer_update_proc);

	return layer;
}

void chapter_layer_destroy(ChapterLayer *layer) {
	layer_destroy(layer);
}

void chapter_layer_set_color(ChapterLayer *layer, GColor color) {
	ChapterLayerData *data = layer_get_data(layer);

	data->color = color;

	layer_mark_dirty(layer);
}
