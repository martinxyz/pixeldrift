#include <array>

constexpr int tile_size_log2 = 9;
constexpr int tile_size = 1 << tile_size_log2;

class DiffusionTile
{
 private:
  std::array<std::array<uint8_t, tile_size/2>, tile_size> arr{};

};
