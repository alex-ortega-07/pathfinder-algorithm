#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

class Button{
    public:
        Button(int posx, int posy, int width, int height, const char *text, TTF_Font *font, SDL_Color color_text);

        void create(SDL_Renderer *renderer);
        int clicked(int posx, int posy);

        void set_color(SDL_Color color);
        int is_color(SDL_Color  color);

    private:
        int posx, posy;
        int width, height;

        const char *text;

        TTF_Font *font;

        SDL_Color color_text;
};
