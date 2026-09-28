#pragma once

#include <cstdint>


constexpr uint32_t PackColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return (a << 24) | (g << 16) | (b << 8) | r;
}

namespace Color
{
    constexpr uint32_t White = PackColor(255, 255, 255, 255);
    constexpr uint32_t Black = PackColor(0, 0, 0, 255);
    constexpr uint32_t Red = PackColor(255, 0, 0, 255);
}
