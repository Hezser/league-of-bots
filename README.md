# League of Bots

## To-do

* Make logical updates each ms, not each frame!

## The game

Yes, this is a LoL inspired game, nice guess. It is aimed to be a purely competitive 5v5 MOBA, each player controlling a bot and its abilities. However, the gameplay, objectives and gamemodes will be quite different from other MOBAs like LoL. We'll talk about that soon. As the game is developed, this file will grow into a more informative and transparent wiki. Once something is in the code and functional enough to play, it will be explained here. **Work in progress!**

## Elements

These are the dynamic graphical elements that may appear on the screen (not counting static/fixed elements such as walls). They can be individually identified as objects in the code. Each element is a subclass of the class Elem. Elem has the following attributes:
* ElemType type: tells you which type of element (the ones defined below) it is.
* std::mutex mutex: the mutex that protects concurrent access to the element.
* bool alive: tells you if the element should be displayed.
* std::vector<int> coord: these are the x,y coordinates of the element.
* Team team: the team it belongs to, which could be neutral, red or blue.

The main elements and their attributes are:
* Bot:
  * Attributes:
    * std::vector<Ability> abilities: the set of abilities the bot can use, ordered by their keybindings (q, w, e, r, etc.).
    * MovementManager movementManager: this object controls the movement of the bot.
  * Methods:
    * void move\_towards(std::vector(int))
    * void move\_to(std::vector(int))
    * void update(): makes a unit update (position, etc.), to be called each frame.
    * void use\_ability(int key): use an ability.
* Ability:
  * Attributes:
    * time\_t cd: the cooldown of the ability.
    * std::chrono::steady\_clock::time\_point last\_used: the time at which the ability was last used.
    * ActionManager action\_manager: controls the ability effects.
  * Methods:
    * void update(): makes a unit update each frame.

Each different ability and bot will have its own class which will inherit from these two classes. Right now, we are only considering one type of bot and four abilities to keep the development process simple.

## Helpers

* class MovementManager: manages the movements of a bot, adding different requests of movement from different souces to determine the movement of the bot at each ms.
  * Attributes:
    * std::vector<Move> moves: the currently requested moves.
    * std::vector<float> unit\_target: the amount of space the bot will travel each ms. It's calculated from the requested moves.
  * Methods:
    * void request(Move): registers a request for a move.
    * void update(): makes a unit move.
* struct Move:
  * std::vector<int> target: the target x,y coordinates.
  * std::vector<float> unit\_target: the unit distance made each ms towards the target.
  * int units: the number of units to be traversed to reach the target.
* class ActionManager: manages the action of an ability, updating each ms.

## Systems
Not much innovation here. The following systems will work together to make the game playable:
#### I/O
Running as an individual thread on each player's machine, it collects the user input through the mouse and keyboard. 
#### Logic
#### Physics
Mostly comprised of the collision detection system.
#### Rendering
#### Graphics
#### DBs and services
TBD
#### Network infrastructure
TBD
