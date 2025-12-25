#include <stdio.h>
#include <SDL2/SDL.h>
#include "tinyexpr.h"

#define WIDTH 600
#define HEIGHT 400
#define SCALAR 50   // pixels per unit

void draw_grid(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 100);
    SDL_RenderDrawLine(renderer, 0, HEIGHT / 2, WIDTH, HEIGHT / 2);
    SDL_RenderDrawLine(renderer, WIDTH / 2, 0, WIDTH / 2,  HEIGHT);
    SDL_RenderPresent(renderer);
}

void draw_point_normalized(SDL_Renderer *renderer, double x, double y, double scalar) {
    int n_x = WIDTH / 2 + x * scalar;
    int n_y = HEIGHT / 2 - y * scalar;

    SDL_RenderDrawPoint(renderer, n_x, n_y);
}

void draw_func(SDL_Renderer *renderer, char *func, double scalar) {
    double x;
    te_variable var[] = {{"x", &x}};

    int err;
    te_expr *expr = te_compile(func, var, 1, &err);
    if (err) {
        fprintf(stderr, "Failed to compile the expression \"%s\": %s\n", func, SDL_GetError());
        exit(-1);
    }
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    double y;
    for (x = -(float)WIDTH / scalar / 2; x <= (float)WIDTH / scalar / 2; x += 0.1 / scalar) {
        y = te_eval(expr);
        draw_point_normalized(renderer, x, y, scalar);
    }
    SDL_RenderPresent(renderer);
    te_free(expr);
}

int main(int argc, char *argv[]) {
    if (3 != argc) {
        fprintf(stderr, "Usage: %s <function> <scalar>\n", argv[0]);
        exit(-1);
    }
    char *expr = argv[1];
    double scalar = atof(argv[2]);

    SDL_Init( SDL_INIT_EVERYTHING );

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    int result = SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
    if (result) {
        fprintf(stderr, "Failed to create window and renderer: %s\n", SDL_GetError());
        exit(-1);
    }

    draw_grid(renderer);
    draw_func(renderer, expr, scalar);

    SDL_Event event;
    Uint8 app_running = 1;
    while (app_running) {
        // SDL_Delay(10); 
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    app_running = 0;
                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0 ) {
                        scalar *= 1.11;
                    } else if (event.wheel.y < 0) {
                        scalar *= 0.9;
                    }
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
                    SDL_RenderClear(renderer);
                    draw_grid(renderer);
                    draw_func(renderer, expr, scalar);
                    break;
            }
        }
    }
}
