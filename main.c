/** This file includes code from:
 * nuklear/example/canvas.c
 * nuklear/demo/sdl_opengl3/main.c
**/

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
#include "lib/nuklear.h"
#include "lib/nuklear_sdl_gl3.h"

#include "chp8.h"
#include "chp8_gui.h"

#include "defines.h"

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 800

#define MAX_VERTEX_MEMORY  512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

struct nk_canvas {
    struct nk_command_buffer *painter;
    struct nk_vec2 item_spacing;
    struct nk_vec2 panel_padding;
    struct nk_style_item window_background;
};

static void
canvas_begin(struct nk_context *ctx, struct nk_canvas *canvas, nk_flags flags,
    int x, int y, int width, int height, struct nk_color background_color)
{
    /* save style properties which will be overwritten */
    canvas->panel_padding = ctx->style.window.padding;
    canvas->item_spacing = ctx->style.window.spacing;
    canvas->window_background = ctx->style.window.fixed_background;

    /* use the complete window space and set background */
    ctx->style.window.spacing = nk_vec2(0,0);
    ctx->style.window.padding = nk_vec2(0,0);
    ctx->style.window.fixed_background = nk_style_item_color(background_color);

    /* create/update window and set position + size */
    flags = flags & ~NK_WINDOW_DYNAMIC;
    nk_window_set_bounds(ctx, "Window", nk_rect(x, y, width, height));
    nk_begin(ctx, "Window", nk_rect(x, y, width, height), NK_WINDOW_NO_SCROLLBAR|flags);

    /* allocate the complete window space for drawing */
    {struct nk_rect total_space;
    total_space = nk_window_get_content_region(ctx);
    nk_layout_row_dynamic(ctx, total_space.h, 1);
    nk_widget(&total_space, ctx);
    canvas->painter = nk_window_get_canvas(ctx);}
}

static void
canvas_end(struct nk_context *ctx, struct nk_canvas *canvas)
{
    nk_end(ctx);
    ctx->style.window.spacing = canvas->panel_padding;
    ctx->style.window.padding = canvas->item_spacing;
    ctx->style.window.fixed_background = canvas->window_background;
}

int main(int argc, char* argv[])
{
    /* Platform */
    SDL_Window *win;
    SDL_GLContext glContext;
    int win_width, win_height;
    int running = 1;

    struct nk_canvas canvas;
    int ticks = 0;

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
    struct nk_font *proggy = nk_font_atlas_add_from_file(atlas, "lib/ProggyTiny.ttf", 10, 0);
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

            if (evt.type == SDL_KEYDOWN || evt.type == SDL_KEYUP) {
                bool down = evt.type == SDL_KEYDOWN;
                switch (evt.key.keysym.sym) {
                    case SDLK_1: chp8_setkey(emu, 0x1, down); break;
                    case SDLK_2: chp8_setkey(emu, 0x2, down); break;
                    case SDLK_3: chp8_setkey(emu, 0x3, down); break;
                    case SDLK_4: chp8_setkey(emu, 0xC, down); break;

                    case SDLK_q: chp8_setkey(emu, 0x4, down); break;
                    case SDLK_w: chp8_setkey(emu, 0x5, down); break;
                    case SDLK_e: chp8_setkey(emu, 0x6, down); break;
                    case SDLK_r: chp8_setkey(emu, 0xD, down); break;

                    case SDLK_a: chp8_setkey(emu, 0x7, down); break;
                    case SDLK_s: chp8_setkey(emu, 0x8, down); break;
                    case SDLK_d: chp8_setkey(emu, 0x9, down); break;
                    case SDLK_f: chp8_setkey(emu, 0xE, down); break;

                    case SDLK_z: chp8_setkey(emu, 0xA, down); break;
                    case SDLK_x: chp8_setkey(emu, 0x0, down); break;
                    case SDLK_c: chp8_setkey(emu, 0xB, down); break;
                    case SDLK_v: chp8_setkey(emu, 0xF, down); break;

                    default: break;
                }
            }
            
            nk_sdl_handle_event(&evt);
        }
        nk_input_end(ctx);

        /* Draw */

        // Draw Canvas
        canvas_begin(ctx, &canvas, NK_WINDOW_NO_INPUT, CANVAS_X, CANVAS_Y,
            CANVAS_WIDTH, CANVAS_HEIGHT, nk_rgb(32,32,32));
        {
            if (ticks == 0) {
                for (int row = 0; row < DISPLAY_ROWS; row++)
                for (int col = 0; col < DISPLAY_COLS; col++) {
                    if (!emu->display[row][col])
                        continue;

                    nk_fill_rect(canvas.painter, nk_rect(CANVAS_X+col*CANVAS_PX_SIZE,
                        CANVAS_Y+row*CANVAS_PX_SIZE,CANVAS_PX_SIZE,CANVAS_PX_SIZE),
                        0, nk_rgb(255, 0, 0));
                }
            }
            canvas_end(ctx, &canvas);
        }

        switch (chp8_debug_window(ctx, emu, status)) {
            case PAUSE:     status = PAUSED;        break;
            case STEP:      chp8_singlestep(emu);   break;
            case CONTINUE:  status = RUNNING;       break;
            case RESET:     chp8_reset(emu);        break;
            case NONE:                              break;
        }

        //chp8_display_window(ctx, emu);
        chp8_code_window(ctx, emu);
        chp8_memory_window(ctx, emu);
        chp8_keypad_window(ctx, emu);

        SDL_GetWindowSize(win, &win_width, &win_height);
        glViewport(0, 0, win_width, win_height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg.r, bg.g, bg.b, bg.a);        

        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

        SDL_GL_SwapWindow(win);

        if (status == RUNNING)
            chp8_singlestep(emu);

        ticks++;
        if (ticks > CANVAS_REFRESH)
            ticks = 0;
    }

cleanup:
    chp8_destroy(emu);
    nk_sdl_shutdown();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}

