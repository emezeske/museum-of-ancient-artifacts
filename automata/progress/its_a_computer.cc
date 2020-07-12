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
    if (((program >> (24 + i)) & 0x1) == 0x0) {
      continue;
    }
    const int dx = clamp(x + neighbors[i].first, W);
    const int dy = clamp(y + neighbors[i].second, H);
    const int px_neighbor = pixels[buffer][dx][dy];
    const int neighbor_program = (px_neighbor >> (i * 3)) & 0x7;
    switch (neighbor_program) {
      case 0:
        result |= px_neighbor;
        break;
      case 1:
        result &= ~px_neighbor;
        break;
      case 2:
        result ^= px_neighbor;
        break;
      case 3:
        result = ~result;
        break;
      case 4:
        result = (result >> 1) | (result << 31);
        break;
      case 5:
        result = (result << 1) | (result >> 31);
        break;
      case 6:
        result = (result << 2) | (result >> 30);
        break;
      case 7:
        result = (result >> 2) | (result << 30);
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
  memset(pixels, 0x21, D * W * H * sizeof(Uint32));
  int back = 0;
  int mid = 1;
  int front = 2;
  pixels[mid][W/2][H/2] = 0xF3010005;
  pixels[back][W/2+1][H/2] = 0x3F090f05;

  bool run = true;
  while (run) {
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        const int px_original = pixels[mid][x][y];
        int px_next = run_program(x, y, px_original, mid);
        px_next = run_program(x, y, px_next, back);
        pixels[front][x][y] = px_next;
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
