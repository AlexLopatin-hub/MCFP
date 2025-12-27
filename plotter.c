#include <stdio.h>
#include <SDL2/SDL.h>
#include "tinyexpr.h"

#define WIDTH 800
#define HEIGHT 600
#define SCALAR 50   // pixels per unit

void draw_grid(SDL_Renderer *renderer, double scalar) {
    int i;
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
    SDL_RenderDrawLine(renderer, 0, HEIGHT / 2, WIDTH, HEIGHT / 2);
    SDL_RenderDrawLine(renderer, WIDTH / 2, 0, WIDTH / 2,  HEIGHT);

    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    if (scalar > 10) {
        // x unit segments
        for (int x = 1; x < WIDTH / scalar / 2; ++x) {
            i = x * scalar;
            SDL_RenderDrawLine(renderer, WIDTH / 2 + i, 0, WIDTH / 2 + i, HEIGHT);
            SDL_RenderDrawLine(renderer, WIDTH / 2 - i, 0, WIDTH / 2 - i, HEIGHT);
        }
        // y unit segments
        for (int y = 1; y < HEIGHT / scalar / 2; ++y) {
            i = y * scalar;
            SDL_RenderDrawLine(renderer, 0, HEIGHT / 2 + i, WIDTH, HEIGHT / 2 + i);
            SDL_RenderDrawLine(renderer, 0, HEIGHT / 2 - i, WIDTH, HEIGHT / 2 - i);
        }
    }
}

int normalize(double n, double scalar, char* axis) {
    if (axis == "x") {
        return WIDTH / 2 + n * scalar;
    } else if (axis == "y") {
        return HEIGHT / 2 - n * scalar;
    } else return 0;
}

void draw_point_normalized(SDL_Renderer *renderer, double x, double y, double scalar) {
    int n_x = normalize(x, scalar, "x");
    int n_y = normalize(y, scalar, "y");

    SDL_RenderDrawPoint(renderer, n_x, n_y);
}

void draw_line_normalized(SDL_Renderer *renderer, double x1, double y1, double x2, double y2, double scalar) {
    int n_x1 = normalize(x1, scalar, "x");
    int n_y1 = normalize(y1, scalar, "y");
    int n_x2 = normalize(x2, scalar, "x");
    int n_y2 = normalize(y2, scalar, "y");
    
    SDL_RenderDrawLine(renderer, n_x1, n_y1, n_x2, n_y2);
}

void draw_func(SDL_Renderer *renderer, char *func, double scalar) {
    double x = 0, x_old = 0;
    double y = 0, y_old = 0;

    te_variable var[] = {{"x", &x}};

    int err;
    te_expr *expr = te_compile(func, var, 1, &err);
    if (err) {
        fprintf(stderr, "Failed to compile the expression \"%s\": %s\n", func, SDL_GetError());
        exit(-1);
    }
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    double step = 0.1 / scalar;
    for (double i = -(double)WIDTH / scalar / 2; i <= (double)WIDTH / scalar / 2; i += step) {
        x_old = x; y_old = y;
        x = i; y = te_eval(expr);

        if (fabs(y) > HEIGHT / scalar) continue;
        if (i == -(double)WIDTH / scalar / 2) continue;
        
        draw_line_normalized(renderer, x_old, y_old, x, y, scalar);
    }
    
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

    draw_grid(renderer, scalar);
    draw_func(renderer, expr, scalar);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    Uint8 app_running = 1;
    while (app_running) {
        SDL_Delay(10); 
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
                    draw_grid(renderer, scalar);
                    draw_func(renderer, expr, scalar);
                    SDL_RenderPresent(renderer);
                    break;
            }
        }
    }
}
