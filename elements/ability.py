import math
from elements.elem_type import ElemType

class Ability:

    def __init__(self, x, y):
        self.type = ElemType.Ability
        self.x = x
        self.y = y
        self.alive = True

    def start_ability(end_x, end_y):                                                  
        # We convert the vector (x, y) to a basis where (self.x, self.y) is the origin
        alpha_x = self.x - self.x
        alpha_y = self.y - self.y 
        # We work out the unit length vector  
        d = math.sqrt(alpha_x**2 + alpha_y**2)
        alpha_unit_x = alpha_x / d
        alpha_unit_y = alpha_y / d
        # We draw an ability in the original basis towards (end_x, end_y), updating its position every ms
        for i in int(d):
            self.x += (i*alpha_unit_x)
            self.y += (i*alpha_unit_y)
            sleep(0.1)
        self.alive = False
