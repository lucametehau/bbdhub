#pragma once

namespace BBD {

class Color {
private:
  bool m_color;

public:
  constexpr Color() = default;
  constexpr Color(uint8_t color) : m_color(color) {}

  operator uint8_t() const { return m_color; }

  /// flips the color
  /// \return
  Color flip() { return Color(!m_color); }
};

namespace Colors {
constexpr Color BLACK = Color(0);
constexpr Color WHITE = Color(1);
} // namespace Colors

} // namespace BBD