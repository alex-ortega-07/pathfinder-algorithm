#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>

#include "Box.hpp"

Box::Box(){
    val = posx = posy = width = height = cont_appear = cont_disappear = 0;
    color = {255, 255, 255, 255};
}


void Box::appear(SDL_Renderer *renderer, int size){
    if(cont_appear <= size / 2){
        cont_appear++;
    }

    if(x == posx && y == posy && w == width && h == height){
        cont_disappear = 0;
    }

    x = posx + size / 2 - cont_appear + 1;
    y = posy + size / 2 - cont_appear + 1;
    h = cont_appear * 2 - 1;
    w = cont_appear * 2 - 1;

    this->draw(renderer, x, y, w, h, true);
}


void Box::disappear(SDL_Renderer *renderer, int size){
    int x = posx + cont_disappear;
    int y = posy + cont_disappear;
    int h = size - cont_disappear * 2;
    int w = size - cont_disappear * 2;

    if(cont_disappear <= size / 2){
        cont_disappear++;
    }

    else{
        x = y = w = h = 0;
        cont_appear = 0;
    }

    this->draw(renderer, x, y, w, h, true);
}


void Box::draw(SDL_Renderer *renderer, int x, int y, int w, int h, bool alt_vals){
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_Rect pixel;

    if(!alt_vals){
        pixel.x = posx;
        pixel.y = posy;
        pixel.w = width;
        pixel.h = height;
    }

    else{
        pixel.x = x;
        pixel.y = y;
        pixel.w = w;
        pixel.h = h;
    }

    SDL_RenderFillRect(renderer, &pixel);
}
