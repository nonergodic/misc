#include <iostream>
#include <cassert>

#include "board.hpp"
#include "shapes.hpp"

using BoardType = Board<10, 100>;

int main() {
  //we could use a std::deque and std::async here to parallelize the computation (probably
  // only helpful for long lines)
  auto shapes = default_shapes<BoardType>();
  auto line = std::string{};
  while (std::getline(std::cin, line)) {
    auto board = BoardType{};
    for (size_t i = 0; i < line.size(); ++i) {
      auto shape_char = line[i++];
      auto shape_it = shapes.find(shape_char);
      assert(shape_it != shapes.end());
      auto const & shape = shape_it->second;
      assert('0' <= line[i] && line[i] <= '9');
      auto location = line[i++] - '0';
      board.drop(shape, location);
    }
    std::cout << static_cast<unsigned>(board.fill_height()) << "\n";
  }

  return EXIT_SUCCESS;
}
