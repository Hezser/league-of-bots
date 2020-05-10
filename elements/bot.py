from elements.elem_type import ElemType
from elements.ability import Ability
from helpers.exceptions import CoordinateError
import datetime
import threading

class Bot:
    
    def __init__(self, x, y):
        self.type = ElemType.Bot
        self.x = x
        self.y = y
        self.cd = 3000
        self.last_ability = 0
        self.alive = True

    def translate(window, x, y):
        new_x = self.x + x
        new_y = self.y + new_y
        if new_x < 0 or new_x >= board.width or new_y < 0 or new_y >= board.height:
            raise CoordinateError('The desired translation is not within the board')
        self.x = new_x
        self.y = new_y

    def move(window, x, y):
        if x < 0 or x >= board.width or y < 0 or y >= board.height:
            raise CoordinateError('The desired movement is not within the board')
        self.x = x
        self.y = y

    def use_ability(window, end_x, end_y):
        if end_x < 0 or end_x >= board.width or end_y < 0 or end_y >= board.height:
            raise CoordinateError('The desired ability end point is not within the board')
        now = datetime.now().microsecond
        if now - self.last_ability > self.cd:
            self.last_ability = now
            ability = Ability(self.x, self.y)
            ability_thread = threading.Thread(target=ability.start_ability, args=(window, end_x, end_y), daemon=True)
            ability_thread.start()
            return ability
        return None
