#include "Vtarunes_top.h"
#include "verilated.h"
#include "verilated_vcd_c.h"
#include <SDL2/SDL.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    
    Verilated::traceEverOn(true);

    Vtarunes_top* dut = new Vtarunes_top;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("wave.vcd");

    // SDL2の初期化
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    // ウィンドウサイズ: 256x240を2倍スケーリング -> 512x480
    const int SCREEN_WIDTH = 256;
    const int SCREEN_HEIGHT = 240;
    const int SCALE = 2;
    const int WINDOW_WIDTH = SCREEN_WIDTH * SCALE;
    const int WINDOW_HEIGHT = SCREEN_HEIGHT * SCALE;

    SDL_Window* window = SDL_CreateWindow("tarunes",
                                          100,
                                          100,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!texture) {
        fprintf(stderr, "SDL_CreateTexture Error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    // ピクセルバッファ (ARGB8888)
    uint32_t pixel_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
    memset(pixel_buffer, 0, sizeof(pixel_buffer));

    dut->clk = 0;
    dut->rst = 0;
    dut->controller1_btns = 0;
    dut->controller2_btns = 0;

    const int RESET_CYCLES = 4;
    int frame_count = 0;
    int prev_scanline = 0;
    bool running = true;
    uint8_t controller1_btns = 0;

    // メインループ
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                bool pressed = event.type == SDL_KEYDOWN;
                uint8_t mask = 0;
                switch (event.key.keysym.sym) {
                    case SDLK_z:      mask = 1 << 0; break; // A
                    case SDLK_x:      mask = 1 << 1; break; // B
                    case SDLK_RSHIFT: mask = 1 << 2; break; // Select
                    case SDLK_RETURN: mask = 1 << 3; break; // Start
                    case SDLK_UP:     mask = 1 << 4; break;
                    case SDLK_DOWN:   mask = 1 << 5; break;
                    case SDLK_LEFT:   mask = 1 << 6; break;
                    case SDLK_RIGHT:  mask = 1 << 7; break;
                    case SDLK_ESCAPE: running = false; break;
                    default: break;
                }

                if (mask != 0) {
                    if (pressed) {
                        controller1_btns |= mask;
                    } else {
                        controller1_btns &= ~mask;
                    }
                }
            }
        }
        dut->controller1_btns = controller1_btns;
        dut->controller2_btns = 0;

        // 1サイクルシミュレーション
        for (int half = 0; half < 2; half++) {
            if (Verilated::time() == RESET_CYCLES) {
                dut->rst = 1;
            }

            dut->clk = half;
            dut->eval();
            tfp->dump(Verilated::time());
            Verilated::timeInc(1);
        }

        // 有効ピクセル領域かチェック
        int scanline = dut->scanline;
        int cycle = dut->cycle;
        int pixel_r = dut->pixel_r;
        int pixel_g = dut->pixel_g;
        int pixel_b = dut->pixel_b;

        if (cycle < SCREEN_WIDTH && scanline < SCREEN_HEIGHT) {
            // 直接RGB値を使用
            // ARGB8888形式に変換 (Alpha=0xFF, R, G, B)
            pixel_buffer[scanline][cycle] = 0xFF000000 | 
                                           ((pixel_r & 0xFF) << 16) |
                                           ((pixel_g & 0xFF) << 8) |
                                           (pixel_b & 0xFF);
        }

        // フレームカウント: scanlineが0に戻ったら1フレーム完了
        if (prev_scanline == 261 && scanline == 0) {
            frame_count++;
            printf("Frame %d completed\n", frame_count);

            // ピクセルバッファをテクスチャにコピーして表示
            SDL_UpdateTexture(texture, NULL, pixel_buffer, SCREEN_WIDTH * sizeof(uint32_t));
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
        prev_scanline = scanline;
    }

    printf("Simulation finished after %d frames\n", frame_count);

    // クリーンアップ
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    tfp->close();
    delete dut;
    delete tfp;
    return 0;
}
