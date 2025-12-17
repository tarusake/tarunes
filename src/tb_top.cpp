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

    const int RESET_CYCLES = 4;
    const int MAX_FRAMES = 10;  // 10フレームで終了
    int frame_count = 0;
    int prev_scanline = 0;
    bool running = true;

    // メインループ
    while (running && frame_count < MAX_FRAMES) {

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
