import pygame
from elements.elem_type import ElemType
from time import sleep

# Dimensions
WIDTH  = 500
HEIGHT = 500

# Colors
GREEN = (85, 222, 95)
RED = (239, 7, 7)
LIGHT = (232, 235, 239)
DARK = (125, 135, 150)

def init():
    pygame.init()
    window = pygame.display.set_mode((WIDTH, HEIGHT))
    return window

def run(elems, elems_lock):
    window = init()
    while True:
        with elems_lock:
            update(window, elems)
        # Around 60 fps (would need a timer to properly measure fps due to the execution time of the drawing)
        sleep(0.017)

def update(window, elems):
    draw_board(window)
    for elem in elems:
        if elem.type == ElemType.Bot:
            draw_bot(window, elem.x, elem.y)
        elif elem.type == ElemType.Ability:
            draw_ability(window, elem.x, elem.y)
    pygame.display.update()

def draw_board(window):
    pygame.draw.rect(window, DARK, (0, 0, WIDTH, HEIGHT))

def draw_bot(window, x, y):
    pygame.draw.rect(window, LIGHT, (x, y, 20, 20))

def draw_ability(window, x, y):
    pygame.draw.rect(window, GREEN, (x, y, 5, 5))


