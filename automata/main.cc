#include <SDL2/SDL.h>
#include <vector>
#include <iostream>

const Uint32 W = 512;
const Uint32 H = 512;
const Uint32 D = 2;

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

struct Cell {
  Cell(Uint32 _v) :
    v(_v),
    n((_v >> 24) & 0xff),
    r((_v >> 16) & 0xff),
    g((_v >> 8) & 0xff),
    b(_v & 0xff) {
  }

  Cell(Uint32 _n, Uint32 _r, Uint32 _g, Uint32 _b) :
    v((_n << 24) | (_r << 16) | (_g << 8) | _b),
    n(_n),
    r(_r),
    g(_g),
    b(_b) {
  }

  int SubProgram(int i) const {
    return (v >> (i * 3)) & 0x7;
  }

  bool PokeNeighbor(int i) const {
    return ((n >> i) & 0x1);
  }

  const int v;
  const int n;
  const int r;
  const int g;
  const int b;
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

  Uint32 pixels[D][W][H];
  //for (int d = 0; d < D; ++d ) {
  //  for (int x = 0; x < W; ++x) {
  //    for (int y = 0; y < H; ++y) {
  //      pixels[d][x][y] = 0xFF000000 | ((d+1) * x ^ (x * y) ^ y);
  //    }
  //  }
  //}
  for (int d = 0; d < D; ++d ) {
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        if (x > W/2 - 8 && x < W/2 + 8 & y > H/2 - 8 && y < H/2 + 8)
          pixels[d][x][y] = 0xFFD2B456;
        else
          pixels[d][x][y] = 0x00654321;
          //pixels[d][x][y] = x * y;
      }
    }
  }
  int back = 0;
  int front = 1;

  bool run = true;
  // TODO some kind of equilibrium?

  while (run) {
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        const int program = pixels[back][x][y];
        for (int i = 0; i < neighbors.size(); ++i) {
          const Cell cell(program);
          if (!cell.PokeNeighbor(i)) {
            continue;
          }
          const int dx = clamp(x + neighbors[i].first, W);
          const int dy = clamp(y + neighbors[i].second, H);
          Uint32& neighbor = pixels[front][dx][dy];
          switch (2) {
          //switch (cell.SubProgram(i)) {
            case 0:
              neighbor |= program;
              break;
            case 1:
              neighbor = ~neighbor ^ ~program;
              break;
            case 2:
              neighbor ^= program;
              break;
            case 3:
              neighbor ^= ~program;
              break;
            case 4:
              neighbor = ~neighbor ^ program;
              break;
            case 5:
              // pointless
              neighbor &= 0xF0F0F0F0;
              neighbor ^= program & 0x0F0F0F0F;
              break;
            case 6:
              neighbor &= 0x0F0F0F0F;
              neighbor ^= program & 0xF0F0F0F0;
              break;
            case 7:
              neighbor &= ~(0x7 << (i * 3));
              neighbor |= program & (0x7 << (i * 3));
              break;
             default:
              exit(1);
              break;
          }
        }
      }
    }
    // blur kernel
    for (int x = 0; x < W; ++x) {
      for (int y = 0; y < H; ++y) {
        const Cell cell(pixels[front][x][y]);
        const int weight = 4;
        double r = weight*cell.r, g = weight*cell.g, b = weight*cell.b;
        for (int i = 0; i < neighbors.size(); ++i) {
          const int dx = clamp(x + neighbors[i].first, W);
          const int dy = clamp(y + neighbors[i].second, H);
          const Cell neighbor(pixels[front][dx][dy]);
          r += neighbor.r; g += neighbor.g; b += neighbor.b;
        }
        r /= 8+weight; g /= 8+weight; b /= 8+weight;
        const Cell result(cell.n, r, g, b);
        pixels[front][x][y] = result.v;
      }
    }


    SDL_UpdateTexture(texture, NULL, pixels[front], W * sizeof(Uint32));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    back = (back + 1) % D;
    front = (front + 1) % D;

    SDL_Event event;
    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
      run = false;
    }
  }
  SDL_Quit();
  return 0;
}
