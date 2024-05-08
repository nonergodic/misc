#include <iostream>

#include "board.hpp"
#include "shapes.hpp"

using BoardType = Board<10, 100>;

//in absence of a proper testing framework
int main() {
  auto testcases = std::vector<std::pair<std::string, size_t>>{
    {"", 0},
    {"Q0", 2},
    {"Q0,Q1", 4},
    {"Q0,Q2,Q4,Q6,Q8", 0},
    {"Q0,Q2,Q4,Q6,Q8,Q1", 2},
    {"Q0,Q2,Q4,Q6,Q8,Q1,Q1", 4},
    {"I0,I4,Q8", 1},
    {"I0,I4,Q8,I0,I4", 0},
    {"L0,J2,L4,J6,Q8", 2},
    {"L0,Z1,Z3,Z5,Z7", 2},
    {"T0,T3", 2},
    {"T0,T3,I6,I6", 1},
    {"I0,I6,S4", 1},
    {"T1,Z3,I4", 4},
    {"L0,J3,L5,J8,T1", 3},
    {"L0,J3,L5,J8,T1,T6", 1},
    {"L0,J3,L5,J8,T1,T6,J2,L6,T0,T7", 2},
    {"L0,J3,L5,J8,T1,T6,J2,L6,T0,T7,Q4", 1},
    {"S0,S2,S4,S6", 8},
    {"S0,S2,S4,S5,Q8,Q8,Q8,Q8,T1,Q1,I0,Q4", 8},
    {"L0,J3,L5,J8,T1,T6,S2,Z5,T0,T7", 0},
    {"Q0,I2,I6,I0,I6,I6,Q2,Q4", 3}
  };

  auto shapes = default_shapes<BoardType>();
  for (auto const & [drops, expected] : testcases) {
    auto board = BoardType{};
    for (size_t i = 0; i < drops.size(); i += 3)
      board.drop(shapes.find(drops[i])->second, drops[i+1] - '0');
    
    auto result = static_cast<size_t>(board.fill_height());
    if (result == expected)
      std::cout << "OK ...";
    else
      std::cout << "FAILED ... expected: " << expected << " but got: " << result;
    std::cout << " " << drops << "\n";
  }
}
