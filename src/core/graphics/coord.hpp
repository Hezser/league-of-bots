/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef COORD_HPP
#define COORD_HPP

#include <cstdint>

namespace adamant {
namespace graphics {

typedef struct Coord {
    int_fast16_t x;
    int_fast16_t y;
} Coord;

}  // namespace graphics
}  // namespace adamant

#endif
