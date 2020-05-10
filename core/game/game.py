from elements.bot import Bot
from core import graphical
import threading

window = None
elems = []
bot = None

def run():
    # Instantiate the graphical environment
    window = graphical.init()
    graphical_thread = threading.Thread(target=graphical.start, args=(window, elems), daemon=True)
    # Create the bot
    bot = Bot(0, 0)
    elems.append(bot)
    # Listen to user input
    input_thread = threading.Thread(target=listen_input, args=(), daemon=True)
    # Remove unused graphical elements
    garbage_collector = threading.Thread(target=collect_garbage, args=(), daemon=True)

def listen_input():
    while True:
        ev = pygame.event.get()
        for event in ev:
            # Right click
            if event.type == pygame.MOUSEBUTTONUP and event.button == 3:
                pos = pygame.mouse.get_pos()
                # bot.move_towards(window, pos[0], pos[1])
                bot.move(window, pos[0], pos[1])
            elif event.type == pygame.KEYUP and event.key == pygame.K_q:
                pos = pygame.mouse.get_pos()
                elems.append(bot.use_ability(window, pos[0], pos[1]))

def collect_garbage():
    while True:
        for elem in elems:
            if not elem.alive:
                elems.remove(elem)
        # Collect garbage every frame
        sleep(0.017)
