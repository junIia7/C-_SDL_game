#include "shape.h"

#include <cstring>

extern SDL_Renderer * gRenderer;

Shape_Rectangle::Shape_Rectangle(int x, int y, int width, int height):stat_graph_element() {
    rect.x = x;
    rect.y = y;
    rect.w = width;
    rect.h = height;
}

Shape_Rectangle::~Shape_Rectangle() {
    free();
}

void Shape_Rectangle::render(int x, int y, int w, int h) {
    SDL_RenderFillRect(gRenderer, &rect);
}
