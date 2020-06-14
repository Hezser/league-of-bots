#include <vector>
#include "../logic/elements/elem.hpp"

typedef std::vector<Elem*> Collision;

class CollisionDetector {
    public:
        // Meant to be used a posteriori
        static std::vector<Collision> detect(std::vector<Elem*> elems);
    private:
        // Static class
        CollisionDetector() {}
};
