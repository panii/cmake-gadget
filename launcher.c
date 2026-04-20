#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h> 
#include <string.h>
#include <stdbool.h>
#include <io.h>

typedef struct {
    SDL_FRect rect;
    const char* label;
    SDL_Color color;
} ActionButton;

char base_title[1024];

const Uint64 nsPerFrame = 1000000000 / 10; // 10 FPS

void update_status(SDL_Window* window, const char* status) {
    char full_title[2048];
    if (status) {
        sprintf(full_title, "[%s] %s", status, base_title);
    } else {
        sprintf(full_title, "%s", base_title);
    }
    SDL_SetWindowTitle(window, full_title);
}

void execute_and_hold(SDL_Window* window, const char* label, const char* command) {
    char wrapped[1024];
    update_status(window, label);
    sprintf(wrapped, "cmd /c \"%s & pause\"", command);
    system(wrapped);
    update_status(window, NULL);
}

void do_run(SDL_Window* window) {
    if (_chdir("build\\Release") == 0) {
        struct _finddata_t exe_file;
        intptr_t handle;
        if ((handle = _findfirst("*.exe", &exe_file)) != -1) {
            char command[512];
            sprintf(command, "start \"\" \"%s\"", exe_file.name);
            system(command);
            _findclose(handle);
        } else {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "No executable found!", window);
        }
        _chdir("../.."); 
    } else {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Notice", "Please Build first.", window);
    }
}

int main(int argc, char* argv[]) {
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) return -1;

    char full_path[1024];
    _getcwd(full_path, sizeof(full_path));
    char* folder = strrchr(full_path, '\\');
    folder = (folder) ? folder + 1 : full_path;

    sprintf(base_title, "%s - %s", folder, full_path);

    SDL_Window* window = SDL_CreateWindow(base_title, 610, 120, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);
    
    if (!SDL_SetRenderVSync(renderer, 1)) {
        SDL_Log("VSync Error: %s", SDL_GetError());
    }

    ActionButton buttons[] = {
        {{10,  35, 190, 50}, "[1] Configure", {80, 80, 80, 255}},
        {{190 + 20, 35, 190, 50}, "[2] Build (Release)", {60, 130, 60, 255}},
        {{190*2 + 30, 35, 190, 50}, "[3] Run (Release)",   {40, 100, 200, 255}}
    };

    bool running = true;
    SDL_Event event;
    while (running) {
        Uint64 startTicks = SDL_GetTicksNS();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) running = false;
            if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.key) {
                    case SDLK_1: case SDLK_KP_1: execute_and_hold(window, "Configuring", "cmake -S . -B build"); break;
                    case SDLK_2: case SDLK_KP_2: execute_and_hold(window, "Building", "cmake --build build --config Release"); break;
                    case SDLK_3: case SDLK_KP_3: do_run(window); break;
                    case SDLK_ESCAPE: running = false; break;
                }
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float x = event.button.x, y = event.button.y;
                for (int i = 0; i < 3; i++) {
                    if (x >= buttons[i].rect.x && x <= buttons[i].rect.x + buttons[i].rect.w &&
                        y >= buttons[i].rect.y && y <= buttons[i].rect.y + buttons[i].rect.h) {
                        if (i == 0) execute_and_hold(window, "Configuring", "cmake -S . -B build");
                        if (i == 1) execute_and_hold(window, "Building", "cmake --build build --config Release");
                        if (i == 2) do_run(window);
                    }
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 30, 30, 35, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < 3; i++) {
            SDL_SetRenderDrawColor(renderer, buttons[i].color.r, buttons[i].color.g, buttons[i].color.b, 255);
            SDL_RenderFillRect(renderer, &buttons[i].rect);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            
            float text_x = buttons[i].rect.x + 15; 
            float text_y = buttons[i].rect.y + 21; 
            SDL_RenderDebugText(renderer, text_x, text_y, buttons[i].label);
        }

        SDL_RenderPresent(renderer);
        
        Uint64 runTicks = SDL_GetTicksNS() - startTicks;
        if (runTicks < nsPerFrame) {
            SDL_DelayNS(nsPerFrame - runTicks);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}