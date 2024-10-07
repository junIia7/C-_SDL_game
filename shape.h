#ifndef SHAPE_H
#define SHAPE_H

#include "stat_graph_el.h"

class Shape_Rectangle : public stat_graph_element
{
    private:
        SDL_Rect rect; // SDL структура для хранения прямоугольника

    public:
        Shape_Rectangle(int x = 0, int y = 0, int width = 10, int height = 10);

        ~Shape_Rectangle();

        void render(int x = 0, int y = 0, int w = 100, int h = 100) override;
};

#endif // SHAPE_H
