#pragma once
#include <vector>


struct DomainBoundaryMask
{
  static constexpr uint16_t right = (1 << 1) | (1 << 5) | (1 << 8);
  static constexpr uint16_t left = (1 << 3) | (1 << 6) | (1 << 7);
  static constexpr uint16_t up = (1 << 2) | (1 << 5) | (1 << 6);
  static constexpr uint16_t down = (1 << 4) | (1 << 7) | (1 << 8);
};

inline std::vector<uint16_t> build_boundary_mask(const int& width, const int& height)
{
  // 1 buffer each side
  auto getPtId = [width](int x, int y)
  {
    return (y + 1) * (width + 2) + (x + 1);
  };

  std::vector<uint16_t> boundaryMask((width + 2) * (height * 2), 0);
  auto markBoundary = [&boundaryMask, getPtId](int xStart, int xEnd, int yStart, int yEnd, uint64_t maskValue)
  {
    for (int y = yStart; y < yEnd; ++y)
    {
      for (int x = xStart; x < xEnd; ++x)
      {
        uint16_t mask = 0;
        mask |= maskValue;
        boundaryMask[getPtId(x, y)] = mask;
      }
    }
  };

  markBoundary(0, 1, 0, height, DomainBoundaryMask::right);
  markBoundary(width - 1, width, 0, height, DomainBoundaryMask::left);
  markBoundary(0, width, 0, 1, DomainBoundaryMask::up);
  markBoundary(0, width, height - 1, height, DomainBoundaryMask::down);
  return boundaryMask;
}
