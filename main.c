/* Bad on nuklear sdl_opengl3 example */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_IMPLEMENTATION
#define NK_SDL_GL3_IMPLEMENTATION
#include "nuklear/nuklear.h"
#include "nuklear/nuklear_sdl_gl3.h"

#include "chp8.h"
#include "chp8_gui.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

int main(int argc, char* argv[])
{
    /* Platform */
    SDL_Window *win;
    SDL_GLContext glContext;
    int win_width, win_height;
    int running = 1;

    /* GUI */
    struct nk_context *ctx;
    struct nk_colorf bg;

    /* CHP8 emu */
    struct emu_t* emu;
    enum  status status = PAUSED;

    /* CHP8 setup */
    emu = chp8_init();

    if (argc < 2) {
        printf("Debes pasar el nombre de la ROM\n");
        return 1;
    }
    chp8_load_from_file(emu, argv[1]);

    /* SDL setup */
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTS);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    win = SDL_CreateWindow("CHP8",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);
    glContext = SDL_GL_CreateContext(win);
    SDL_GetWindowSize(win, &win_width, &win_height);

    /* OpenGL setup */
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glewExperimental = 1;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    ctx = nk_sdl_init(win);

    struct nk_font_atlas *atlas;
    nk_sdl_font_stash_begin(&atlas);
    struct nk_font *proggy = nk_font_atlas_add_from_file(atlas, "ProggyTiny.ttf", 10, 0);
    nk_sdl_font_stash_end();
    nk_style_set_font(ctx, &proggy->handle);

    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
    while (running)
    {
        /* Input */
        SDL_Event evt;
        nk_input_begin(ctx);
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT) goto cleanup;
            nk_sdl_handle_event(&evt);
        }
        nk_input_end(ctx);

        switch (chp8_debug_window(ctx, emu, status)) {
            case PAUSE:     status = PAUSED;        break;
            case STEP:      chp8_singlestep(emu);   break;
            case CONTINUE:  status = RUNNING;       break;
            case RESET:     chp8_reset(emu);        break;
            case NONE:                              break;
        }

        chp8_display_window(ctx, emu);
        chp8_code_window(ctx, emu);
        chp8_memory_window(ctx, emu);

        /* Draw */
        SDL_GetWindowSize(win, &win_width, &win_height);
        glViewport(0, 0, win_width, win_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg.r, bg.g, bg.b, bg.a);

        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
        SDL_GL_SwapWindow(win);

        if (status == RUNNING)
            chp8_singlestep(emu);
    }

cleanup:
    chp8_destroy(emu);
    nk_sdl_shutdown();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

