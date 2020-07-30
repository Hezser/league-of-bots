#include "move.hpp"

bool operator < (const Move& lhs, const Move& rhs) {
    return lhs.priority < rhs.priority;
}

bool operator > (const Move& lhs, const Move& rhs) {
    return lhs.priority > rhs.priority;
}

bool Move::GreaterComparator::operator() (const Move* lhs, const Move* rhs) {   
    return lhs->priority > rhs->priority;                   
}
