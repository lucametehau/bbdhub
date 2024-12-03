#pragma once

namespace BBD {
    class Color {
    private:
        uint8_t m_color;
        
    public:
        constexpr Color() = default;
        constexpr Color(uint8_t color) : m_color(color) {}

        operator uint8_t() const {
            return m_color;
        }
    };

    namespace Colors {
        constexpr Color BLACK = Color(0);
        constexpr Color WHITE = Color(1);
    };
};