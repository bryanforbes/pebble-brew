#include "chapter_layer.h"

typedef struct ChapterLayerData {
	GPoint center;

	GColor color;
} ChapterLayerData;

static void chapter_layer_update_proc(Layer *layer, GContext *ctx) {
	ChapterLayerData *data = layer_get_data(layer);

	graphics_context_set_stroke_color(ctx, data->color);
	graphics_context_set_fill_color(ctx, data->color);

	//graphics_draw_circle(ctx, data->center, 66);

	int16_t x = data->center.x;
	int16_t y = data->center.y;

	// 1
	graphics_fill_circle(ctx, GPoint(x + 33, y + 57), 2);
	// 2
	graphics_fill_circle(ctx, GPoint(x + 57, y + 33), 2);

	// 4
	graphics_fill_circle(ctx, GPoint(x + 57, y - 33), 2);
	// 5
	graphics_fill_circle(ctx, GPoint(x + 33, y - 57), 2);

	// 7
	graphics_fill_circle(ctx, GPoint(x - 33, y - 57), 2);
	// 8
	graphics_fill_circle(ctx, GPoint(x - 57, y - 33), 2);

	// 10
	graphics_fill_circle(ctx, GPoint(x - 57, y + 33), 2);
	// 11
	graphics_fill_circle(ctx, GPoint(x - 33, y + 57), 2);

	// 12, 3, 6, 9
	// TODO: Change these to text or paths
	graphics_fill_circle(ctx, GPoint(data->center.x - 66, data->center.y), 4);
	graphics_fill_circle(ctx, GPoint(data->center.x + 66, data->center.y), 4);
	graphics_fill_circle(ctx, GPoint(data->center.x, data->center.y - 66), 4);
	graphics_fill_circle(ctx, GPoint(data->center.x, data->center.y + 66), 4);
}

ChapterLayer* chapter_layer_create(GRect frame, GColor color) {
	ChapterLayerData *data;
	Layer *layer = layer_create_with_data(frame, sizeof *data);
	data = layer_get_data(layer);

	data->center = grect_center_point(&frame);
	data->color = color;

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
