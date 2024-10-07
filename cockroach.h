#ifndef COCKROACH_H
#define COCKROACH_H

#include <string>

#include "upravl_po_ekr.h"
#include "shape.h"

struct data{
    char alias[25];

    unsigned int ParticipNumb;

    unsigned int WinNumb;
};

class running_cockroach : public upravl_perem_po_ekr_el, public Shape_Rectangle
{
    private:
        struct data info;

    public:

        running_cockroach(char alias[25] = "", unsigned int ParticipNumb = 0, unsigned int WinNumb = 0);

        void render(int x = 0, int y = 0, int w = 100, int h = 100) override;

        void move() override;

        int getPosX();

        bool loadMedia(std::string textureText, SDL_Color Color = {0, 0, 0});

        void readData(const char * path, char index);

        void GameOver(char a = 0);

        struct data getData() {
            return info;
        }

        void setInfo(char name[25], unsigned int part = 0, unsigned int win = 0) {
            strcpy(info.alias, name);
            info.ParticipNumb = part;
            info.WinNumb = win;
        }
};

#endif //COCKROACH_H
