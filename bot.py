import exceptions.CoordinateError

class Bot:
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

    def move(x, y):
        if x < 0 or x >= board.width or y < 0 or y >= board.height:
            raise CoordinateError('The desired movement is not within the board')
        self.x = x
        self.y = y
