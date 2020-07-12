#include <SDL2/SDL.h>
#include <vector>
#include <iostream>

const Uint32 W = 512;
const Uint32 H = 512;
const Uint32 D = 3;

using std::vector;
using std::pair;

vector<pair<int, int> > neighbors{
  {-1, 1},  {0, 1},  {1, 1},
  {-1, 0},           {1, 0},
  {-1, -1}, {0, -1}, {1, -1}
};

int clamp(int v, int max) {
  if (v == -1) return max - 1;
  if (v == max) return 0;
  return v;
}

Uint32 pixels[D][W][H];

int run_program(int x, int y, int program, int buffer) {
  int result = program;
  for (int i = 0; i < neighbors.size(); ++i) {
    //if (((program >> (24 + i)) & 0x1) == 0x0) {
    //  continue;
    //}
    const int dx = clamp(x + neighbors[i].first, W);
    const int dy = clamp(y + neighbors[i].second, H);
    const int neighbor = pixels[buffer][dx][dy];
    const int subprogram = (program >> (i * 3)) & 0x7;
    switch (subprogram) {
      case 0:
        result |= neighbor;
        break;
      case 1:
        result &= neighbor;
        break;
      case 2:
        result ^= neighbor;
        break;
      case 3:
        result = neighbor;
        break;
      case 4:
        result = ~neighbor;
        break;
      case 5:
        result = (result << 7) | (result >> 25);
        break;
      case 6:
        result = (result << 2) | (result >> 30);
        break;
      case 7:
        result &= ~(0x7 << (i * 3));
        result |= neighbor & (0x7 << (i * 3));
        break;
       default:
        exit(1);
        break;
    }
  }
  return result;
}

int main(int argc, char *argv[]) {
  SDL_Init(SDL_INIT_EVERYTHING);
  SDL_Window *screen = SDL_CreateWindow("My Game Window",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    W, H, SDL_WINDOW_OPENGL);

  SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 0);
  SDL_RenderSetLogicalSize(renderer, W, H);

  SDL_Texture *texture = SDL_CreateTexture(
      renderer, SDL_PIXELFORMAT_RGB888,
      SDL_TEXTUREACCESS_STREAMING, W, H);
  for (int d = 0; d < D; ++d ) {
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        pixels[d][x][y] = d * x * y;
      }
    }
  }
  int back = 0;
  int mid = 1;
  int front = 2;

  bool run = true;
  while (run) {
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        int program = pixels[mid][x][y];
        program = run_program(x, y, program, back);
        program = run_program(x, y, program, mid);
        pixels[front][x][y] = program;
      }
    }

    SDL_UpdateTexture(texture, NULL, pixels[front], W * sizeof(Uint32));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    back = (back + 1) % D;
    mid = (mid + 1) % D;
    front = (front + 1) % D;

    SDL_Event event;
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
      run = false;
    }
  }
  SDL_Quit();
  return 0;
}
