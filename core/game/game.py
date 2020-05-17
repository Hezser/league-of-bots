from __future__ import with_statement
from elements.bot import Bot
from core import graphical
from time import sleep
import threading
import pygame

elems = []
elems_lock = threading.Lock()
bot = None

def run():
    # Instantiate the graphical environment
    graphical_thread = threading.Thread(target=graphical.run, args=(elems, elems_lock), daemon=True)
    graphical_thread.start()
    # Create the bot
    bot = Bot(0, 0)
    elems.append(bot)
    # Listen to user input
    input_thread = threading.Thread(target=listen_input, args=(), daemon=True)
    input_thread.start()
    # Remove unused graphical elements
    garbage_collector = threading.Thread(target=collect_garbage, args=(), daemon=True)
    garbage_collector.start()
    graphical_thread.join()

def listen_input():
    while True:
        ev = pygame.event.get()
        for event in ev:
            # Right click
            if event.type == pygame.MOUSEBUTTONUP and event.button == 3:
                pos = pygame.mouse.get_pos()
                # bot.move_towards(pos[0], pos[1])
                bot.move(pos[0], pos[1])
            elif event.type == pygame.KEYUP and event.key == pygame.K_q:
                pos = pygame.mouse.get_pos()
                with elems_lock:
                    elems.append(bot.use_ability(pos[0], pos[1]))

def collect_garbage():
    while True:
        with elems_lock:
            for elem in elems:
                if not elem.alive:
                    elems.remove(elem)
            # Collect garbage every frame
        sleep(0.017)
