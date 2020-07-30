/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

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
