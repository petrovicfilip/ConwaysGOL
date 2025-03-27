#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>

#define TRUE 1

typedef struct {
    SDL_Rect* rect;
    SDL_Color* color;
} Button;

Button* makeButton(int x, int y, int h, int w, int r, int g, int b)
{
    SDL_Rect* rect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
    rect->x = x;
    rect->y = y;
    rect->h = h;
    rect->w = w;
    SDL_Color* color = (SDL_Color*)malloc(sizeof(SDL_Color));
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = 255;
    Button* btn = (Button*)malloc(sizeof(Button));
    btn->rect = rect;
    btn->color = color;
    return btn;
}


int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Cofi was here!", 800, 600, SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        printf("Greška pri kreiranju prozora: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL) {
        printf("Greška pri kreiranju renderer-a: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Event event;
    Button* btn = makeButton(0, 0, 50, 50, 0, 255, 0);
    SDL_SetRenderDrawColor(renderer, btn->color->r, btn->color->g, btn->color->b, btn->color->a);
    SDL_RenderFillRect(renderer, btn->rect);
    int running = 1;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Boja pozadine
        SDL_RenderClear(renderer); // Prvo brisanje ekrana

        SDL_SetRenderDrawColor(renderer, btn->color->r, btn->color->g, btn->color->b, btn->color->a);
        SDL_RenderFillRect(renderer, btn->rect); // Iscrtavanje dugmeta

        SDL_RenderPresent(renderer); // Prikazivanje scene
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
