/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef MAP_SIZE_HPP
#define MAP_SIZE_HPP

#include <cstdint>

namespace adamant {
namespace graphics {

typedef struct MapSize {
    int_least16_t x;
    int_least16_t y;
} MapSize;

}  // namespace graphics
}  // namespace adamant

#endif
