#pragma once
#include <vector>

struct D2Q9
{
  static constexpr float wt[9] = {4.0f / 9.0f,
    1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f,
    1.0f / 36.0f, 1.0f / 36.0f, 1.0f / 36.0f, 1.0f / 36.0f};
  static constexpr int ex[9] = {0, 1, 0, -1, 0, 1, -1, -1, 1};
  static constexpr int ey[9] = {0, 0, 1, 0, -1, 1, 1, -1, -1};
};

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

inline float compute_feq(const int& iDir, const float& rho, const float& u, const float& v)
{
  float ue = u * D2Q9::ex[iDir] + v * D2Q9::ey[iDir];
  float uu = u * u + v * v;
  float feq = D2Q9::wt[iDir] * rho * (1.0f + 3.0f * ue + 4.5f * ue * ue - 1.5f * uu);
  return feq;
}
