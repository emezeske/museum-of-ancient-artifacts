#include <SDL2/SDL.h>
#include <vector>
#include <iostream>

const Uint32 W = 512;
const Uint32 H = 512;
const Uint32 D = 3;

using std::vector;
using std::pair;

vector<pair<int, int> > around{
  {-1, 1},  {0, 1},  {1, 1},
  {-1, 0},           {1, 0},
  {-1, -1}, {0, -1}, {1, -1}
};

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
  Uint32 pixels[D][W][H]{0};
  int back = 0;
  int mid = 1;
  int front = 2;
  pixels[back][W/3][H/5] = 0x9236541;
  pixels[back][W/3][H/3] = 0x1236541;
  pixels[mid][W/3][H/2] = 0x3238549;
  pixels[mid][W/4][H/1] = 0x1111111;
  pixels[back][W/4][H/1] = 0x1111121;
  pixels[back][W/4+1][H/1] = 0x1111121;

  bool run = true;
  while (run) {
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        int newpx = pixels[mid][x][y];
        for (int i = 0; i < around.size(); ++i) {
          if ((newpx >> (24+i)) & 0x1 == 0) {
            continue;
          }
          int bits = (newpx >> (i * 3)) & 0x7;
          int dx = x + around[i].first;
          if (dx == -1) dx = W-1;
          if (dx == W) dx = 0;
          int dy = y + around[i].second;
          if (dy == -1) dy = H-1;
          if (dy == H) dy = 0;
          int dpx = pixels[back][dx][dy] & pixels[mid][dx][dy];
          switch (bits) {
            case 0:
              newpx |= dpx;
              break;
            case 1:
              newpx = (newpx >> 1) | (newpx << 31);
              break;
            case 2:
              newpx = (newpx << 1) | (newpx >> 31);
              break;
            case 3:
              newpx = (newpx >> 3) | (newpx << 29);
              break;
            case 4:
              newpx = (newpx << 3) | (newpx >> 29);
              break;
            case 5:
              newpx = ~newpx;
              //newpx = ~dpx;
              break;
            case 6:
              newpx &= dpx;
              break;
            case 7:
              newpx ^= dpx;
              break;
             default:
              exit(1);
              break;
          }
        }
        pixels[front][x][y] = newpx;
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
