#include "pebble.h"

typedef Layer HandsLayer;

HandsLayer* hands_layer_create(GRect frame,
							   const GPathInfo *hour_path_info,
							   const GPathInfo *minute_path_info,
							   const GPathInfo *second_path_info);

void hands_layer_destroy(HandsLayer *layer);
void hands_layer_update(HandsLayer *layer, const struct tm *time);
