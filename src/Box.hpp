#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

class Box{
    public:
        Box();

        void appear(SDL_Renderer *renderer, int size);
        void disappear(SDL_Renderer *renderer, int size);

        void draw(SDL_Renderer *renderer, int x = 0, int y = 0, int w = 0, int h = 0, bool alt_vals = false);

        int width, height;
        int posx, posy;
        int val;

        SDL_Color color;

    private:
        int x, y, w, h;
        int cont_appear, cont_disappear;
};
