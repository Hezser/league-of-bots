import exceptions.CoordinateError
import datetime
import threading
import math

class Bot:

    cd = 3000
    last_ability = 0

    def __init__(self, board):
        self.x = 0
        self.y = 0
        self.board = board

    def translate(x, y):
        new_x = self.x + x
        new_y = self.y + new_y
        if new_x < 0 or new_x >= board.width or new_y < 0 or new_y >= board.height:
            raise CoordinateError('The desired translation is not within the board')
        self.x = new_x
        self.y = new_y
        core.graphical.draw_bot(board, self.x, self.y)

    def move(x, y):
        if x < 0 or x >= board.width or y < 0 or y >= board.height:
            raise CoordinateError('The desired movement is not within the board')
        self.x = x
        self.y = y
        core.graphical.draw_bot(board, self.x, self.y)

    def use_ability(end_x, end_y):
        if end_x < 0 or end_x >= board.width or end_y < 0 or end_y >= board.height:
            raise CoordinateError('The desired ability end point is not within the board')
        now = datetime.now().microsecond
        if now - last_ability > cd:
            last_ability = now
            ability = threading.Thread(target=start_ability, args=(end_x, end_y), daemon=True)
            ability.start()

    def start_ability(end_x, end_y):
        # We convert the vector (x, y) to a basis where (self.x, self.y) is the origin
        alpha_x = x - self.x
        alpha_y = y - self.y
        # We work out the unit length vector
        d = math.sqrt(alpha_x**2 + alpha_y**2)
        alpha_unit_x = alpha_x / d
        alpha_unit_y = alpha_y / d
        # We draw an ability in the original basis towards (end_x, end_y), updating its position every ms
        for i in int(d):
            core.graphical.draw_ability(board, self.x + (i*alpha_unit_x), self.y + (i*alpha_unit_y))

