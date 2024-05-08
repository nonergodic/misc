#include <iostream>
#include <cassert>

#include "board.hpp"
#include "shapes.hpp"

using BoardType = Board<10, 100>;

auto extra_shapes() {
  auto shapes = default_shapes<BoardType>();

  shapes.emplace('c', Array2d<4,3>{1, 1, 1, 1,
                                   1, 0, 0, 1,
                                   1, 0, 0, 1});

  shapes.emplace('b', Array2d<6,6>{1, 1, 1, 1, 1, 1,
                                   1, 0, 0, 0, 0, 1,
                                   1, 0, 1, 1, 0, 1,
                                   1, 0, 1, 1, 0, 1,
                                   1, 0, 0, 0, 0, 1,
                                   1, 1, 1, 1, 1, 1});

  shapes.emplace('a', Array2d<5,1>{1, 1, 0, 1, 1});

  shapes.emplace('s', Array2d<5,3>{0, 0, 0, 0, 1,
                                   0, 0, 0, 1, 0,
                                   1, 1, 0, 1, 0});

  shapes.emplace('d', Array2d<2,2>{1, 1,
                                   0, 1});

  shapes.emplace('f', Array2d<1,6>{1,
                                   1,
                                   1,
                                   1,
                                   1,
                                   1,});

  shapes.emplace('x', Array2d<1,1>{1});

  shapes.emplace('v', Array2d<3,8>{0, 0, 0,
                                   0, 1, 0,
                                   0, 1, 0,
                                   0, 1, 0,
                                   0, 1, 0,
                                   0, 1, 0,
                                   0, 1, 0,
                                   0, 0, 0,});
  
  return shapes;
}

int main() {
  auto board = BoardType{};
  auto shapes = extra_shapes();

  auto input = std::string{};
  std::cout << "valid shape characters:";
  for (auto const & [shape_char, _] : shapes)
    std::cout << " " << shape_char;
  std::cout << "\n";
  std::cout << "Enter shape (letter) and location (number) - type quit to quit" << std::endl;
  while (true) {
    std::cin >> input;
    if (input == "quit")
      break;
    
    if (input.size() < 2) {
      std::cout << "invalid input, expected at least 2 chars, try again" << std::endl;
      continue;
    }
    auto shape_it = shapes.find(input[0]);
    if (shape_it == shapes.end()) {
      std::cout << "unknown shape letter, try again" << std::endl;
      continue;
    }

    try {
      auto location = std::stoi(input.substr(1));

      board.drop(shape_it->second, location);

      std::cout << board << std::endl;
    }
    catch (std::exception & e) {
      std::cout << "caught exception while parsing positon: " << e.what() << std::endl;
      std::cout << "try again";
    }
  }
}