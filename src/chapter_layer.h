#include "pebble.h"

typedef Layer ChapterLayer;

ChapterLayer* chapter_layer_create(GRect frame, GColor color);
void chapter_layer_destroy(ChapterLayer *layer);
void chapter_layer_set_color(ChapterLayer *layer, GColor);
