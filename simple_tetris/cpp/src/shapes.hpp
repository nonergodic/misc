#pragma once

#include <unordered_map>

#include "board.hpp"

template <size_t WIDTH, size_t HEIGHT>
using Array2d = std::array<std::array<bool, WIDTH>, HEIGHT>;

template <typename BoardT>
auto default_shapes() {
  auto shapes = std::unordered_map<char, typename BoardT::Shape>{};

  shapes.emplace('I', Array2d<4,1>{1, 1, 1, 1});

  shapes.emplace('Q', Array2d<2,2>{1, 1,
                                   1, 1});

  shapes.emplace('Z', Array2d<3,2>{1, 1, 0,
                                   0, 1, 1});

  shapes.emplace('S', Array2d<3,2>{0, 1, 1,
                                   1, 1, 0});

  shapes.emplace('T', Array2d<3,2>{1, 1, 1,
                                   0, 1, 0});

  shapes.emplace('L', Array2d<2,3>{1, 0,
                                   1, 0,
                                   1, 1});

  shapes.emplace('J', Array2d<2,3>{0, 1,
                                   0, 1,
                                   1, 1});

  return shapes;
}