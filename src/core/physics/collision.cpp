#import "collision.hpp"
#include <vector>

/* class CollisionDetector */

std::vector<Collision> CollisionDetector::detect(std::vector<Elem*> elems) {
    /* 1. Broad phase: check for collisions of bounding spheres of each elem, 
     * by calculating the vector from the center (coord) of elem1 to the center
     * of elem2, calculating the magnitude of that vector and checking if the 
     * magnitude is less than the sums of the radius of the two bounding spheres
     * 2. Narrow phase: for each pair of elems whose bounding spheres collide,
     * accurately check for collisions using the Separating Axis Theorem (SAT)
     * algorithm on the shape of each elem
     * 3. This all means that we need to implement two more members of the class
     * Elem: bounding_sphere and shape (IMPORTANT: shape needs to be a convex
     * polygon or a collection of them for the SAT algorithm to work) */
    return {};
}
