#include "pebble.h"

typedef Layer HandsLayer;

HandsLayer* hands_layer_create(GRect frame, GColor background, GColor fill, GColor stroke,
							   const GPathInfo *hour_path_info,
							   const GPathInfo *minute_path_info,
							   const GPathInfo *second_path_info);

void hands_layer_destroy(HandsLayer *layer);
void hands_layer_update(HandsLayer *layer, const struct tm *time);
void hands_layer_set_colors(HandsLayer *layer, GColor background, GColor fill, GColor stroke);
void hands_layer_set_fill_color(HandsLayer *layer, GColor color);
void hands_layer_set_stroke_color(HandsLayer *layer, GColor color);
