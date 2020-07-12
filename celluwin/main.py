#/usr/bin/python

import pygame
import itertools
import sys

XS = 512
YS = 512

NEIGHBORS = [
  (-1, 1),
  (0, 1),
  (1, 1),
  (1, 0),
  (1, -1),
  (0, -1),
  (-1, -1),
  (-1, 0)]

#def affectors(color):
#  for i in xrange(0, 8):
#    if (color[0] >> i) & 0x1:
#      yield NEIGHBORS[i]

def unpack_color(i):
  return (i >> 16, i >> 8 & 0xff, i & 0xff)

def apply_neighbors(back, front, x, y):
  color = unpack_color(front[x, y])
  for dx, dy in NEIGHBORS:
    #nx, ny = (x + dx) % XS, (y + dy) % YS
    nx, ny = x + dx, y + dy
    if nx < 0 or nx >= XS or ny < 0 or ny >= YS:
      continue
    neighbor = unpack_color(back[nx, ny])
    color = ((neighbor[0] ^ 0x7e) ^ color[0], (neighbor[1] ^ 0xe7) ^ color[1], neighbor[2] ^ color[2])
  return color

def simulate(back, front):
  for x, y in itertools.product(xrange(0, XS), xrange(0, YS)):
    front[x, y] = apply_neighbors(back, front, x, y)
  front[XS/2, YS/2] = (1, 2, 3)

def main():
  pygame.init()
  screen = pygame.display.set_mode((XS, YS))
  back_surface = pygame.Surface(screen.get_size()).convert()
  front_surface = pygame.Surface(screen.get_size()).convert()

  for surface in [back_surface, front_surface]:
    surface.fill((255, 255, 255))

  while True:
    for event in pygame.event.get():
      if event.type == pygame.QUIT:
        pygame.quit()
        sys.exit()

    # Note that this is accidentally a second-order cellular automata!
    back = pygame.PixelArray(back_surface)
    front = pygame.PixelArray(front_surface)
    simulate(back, front)
    del front, back
    screen.blit(front_surface, (0, 0))
    back_surface, front_surface = front_surface, back_surface
    pygame.display.flip()

if __name__ == '__main__':
  main()
