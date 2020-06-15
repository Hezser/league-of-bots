#include <vector>
#include "../logic/elements/elem.hpp"

typedef std::vector<Elem*> Collision;

class CollisionDetectionSystem {
    public:
        // Meant to be used a posteriori
        static std::vector<Collision> detect(std::vector<Elem*> elems);
    private:
        // Static class
        CollisionDetectionSystem() {}
};

class CollisionResolutionSystem {
    public:
        static void resolve(std::vector<Collision> collisions);
    private:
        // Static class
        CollisionResolutionSystem() {}
};
