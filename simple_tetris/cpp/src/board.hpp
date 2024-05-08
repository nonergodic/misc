#pragma once

#include <cmath> //log, ceil
#include <cassert>
#include <stdexcept> //invalid_argument
#include <array>
#include <bitset>
#include <vector>
#include <limits> //numeric_limits::max()
#include <algorithm> //max, max_element, reverse
#include <iostream> //ostream
#include <iomanip> //setw
#include <type_traits> //is_signed

#include "compact_bitset.hpp"

//Board implements a simplified 'Tetris engine' that calculates the placement of shapes
// (i.e. non-empty, rigid sets of blocks) that are dropped straight down at a given location (no
// rotations) and clears rows that are filled as a result.
//
//Placements of dropped shapes are found by using a separate heights array that tracks the top most
// filled block of each column and by comparing it to the 'bottom profile' of the dropped shape,
// i.e. the height of the lowest block of each column of the shape.
//
//Rows are represented by bitsets and are stored in a std::array, hence putting the entire game
// board linearly in memory. While this makes deleting rows expensive from an asymptotic point of
// view, cache locality and avoiding allocations is much more important for performance in all
// remotely realistic cases, especially when considering that most row deletions are likely to
// occur towards the top of the board. Note that for very large boards, std::arrays might exceed
// the stackframe size and so a switch to std::vector might become necessary.
//
//assert() is used to document given guarantees of appropriate use.
template <size_t WIDTH, size_t HEIGHT>
class Board {
  static_assert(2 <= WIDTH);
  static_assert(1 <= HEIGHT);

  //we use max/2 to prevent potential overflow issues on addition:
  template <size_t MAX_VAL>
  using FitsInUint =
    std::conditional_t<MAX_VAL <= std::numeric_limits< uint8_t>::max()/2,  uint8_t,
    std::conditional_t<MAX_VAL <= std::numeric_limits<uint16_t>::max()/2, uint16_t,
    std::conditional_t<MAX_VAL <= std::numeric_limits<uint32_t>::max()/2, uint32_t,
    std::conditional_t<MAX_VAL <= std::numeric_limits<uint64_t>::max()/2, uint64_t,
    void>>>>;

  using WidthType = FitsInUint<WIDTH>;
  using HeightType = FitsInUint<HEIGHT>;
  using RowType = std::conditional_t<WIDTH <= 64, CompactBitset<WIDTH>, std::bitset<WIDTH>>;

  public:
    //Shape could theoretically be implemented using templates and static polymorphism too for
    // potentially even better performance, but the readability trade-off (addtional template
    // parameters and having to deal with type erasure in various places) wouldn't be worth it.
    class Shape {
      public:
        //Shape's constructor ensures two things:
        // 1) Allows natural specification along the lines of:
        //     Shape{{1, 1, 0,
        //            0, 1, 1}};
        //    which means we have to internally flip row orientation
        // 2) Strips padding:
        //     auto z1 = Shape{1, 1, 0,
        //                     0, 1, 1};
        //     auto z2 = Shape{0, 0, 0, 0, 0,
        //                     0, 1, 1, 0, 0,
        //                     0, 0, 1, 1, 0,
        //                     0, 0, 0, 0, 0};
        //    then z1 == z2
        template <size_t SHAPE_WIDTH, size_t SHAPE_HEIGHT>
        explicit Shape(std::array<std::array<bool, SHAPE_WIDTH>, SHAPE_HEIGHT> const & blocks);

        auto width() const {return bottom_profile_.size();}
        auto height() const {return blocks_.size();}
        auto const & bottom_profile() const {return bottom_profile_;}
        auto const & operator[](size_t row) const {return blocks_[row];}

      private:
        std::vector<RowType> blocks_;
        std::vector<HeightType> bottom_profile_;
    };

    void drop(Shape const & shape, WidthType location);
    auto const & heights() const {return heights_;}
    auto fill_height() const {return fill_height_;}

    auto const & operator[](WidthType row) const {return rows_[row];}

  private:
    std::array<RowType, HEIGHT> rows_;
    std::array<HeightType, WIDTH> heights_;
    HeightType fill_height_;
};

template <size_t WIDTH, size_t HEIGHT>
auto & operator<<(std::ostream & os, Board<WIDTH,HEIGHT> const & board) {
  auto const fill_height = board.fill_height();
  auto const row_digits =
    fill_height == 0 ? 1 : static_cast<int>(std::ceil(std::log(fill_height)/std::log(10)));
  for (auto row = fill_height; row > 0; --row) {
    auto row_str = board[row-1].to_string(' ', 'X');
    std::reverse(row_str.begin(), row_str.end());
    os << std::setw(row_digits) << static_cast<size_t>(row)-1 << " |" << row_str << "|\n";
  }
  os << std::setw(row_digits) << " " << " |" << std::string(WIDTH, '-') << "|\n";
#ifndef NDEBUG
  os << "heights:";
  for (auto height : board.heights())
    os << " " << std::setw(row_digits) << static_cast<size_t>(height);
  os << "\n";
#endif
  return os;
}

template <size_t WIDTH, size_t HEIGHT>
template <size_t SHAPE_WIDTH, size_t SHAPE_HEIGHT>
Board<WIDTH,HEIGHT>::Shape::Shape(
  std::array<std::array<bool, SHAPE_WIDTH>, SHAPE_HEIGHT> const & blocks
) {
  static_assert(0 < SHAPE_WIDTH && SHAPE_WIDTH <= WIDTH);
  static_assert(0 < SHAPE_HEIGHT && SHAPE_HEIGHT <= HEIGHT);

  auto const shape_width = WidthType{SHAPE_WIDTH};
  auto const shape_height = HeightType{SHAPE_HEIGHT};

  //determine dimensions of shape
  auto min_width = shape_width;
  auto max_width = WidthType{};
  auto top_height = shape_height;
  auto bottom_height = HeightType{};
  for (HeightType row = 0; row < shape_height; ++row) //iterates visually top to bottom
    for (WidthType col = 0; col < shape_width; ++col)
      if (blocks[row][col]) {
        min_width = std::min(col, min_width);
        max_width = std::max(col, max_width);
        if (top_height == shape_height)
          top_height = row;
        bottom_height = row;
      }

  if (min_width > max_width)
    throw std::invalid_argument("Shapes must have at least one block");

  auto width = max_width - min_width + 1;
  auto height = bottom_height - top_height + 1;

  //reserve actually required size (i.e. minus padding)
  bottom_profile_.resize(width, HEIGHT);
  blocks_.resize(height);

  //flip row orientation and store in datamembers
  for (HeightType inverted_row = 0; inverted_row < height; ++inverted_row)
    for (WidthType col = min_width; col <= max_width ; ++col) {
      auto row = bottom_height - inverted_row;
      if (blocks[row][col]) {
        auto const shifted_col = col - min_width;
        blocks_[inverted_row].set(shifted_col);
        if (bottom_profile_[shifted_col] == HEIGHT)
          bottom_profile_[shifted_col] = inverted_row;
      }
    }
}

template <size_t WIDTH, size_t HEIGHT>
void Board<WIDTH,HEIGHT>::drop(Shape const & shape, WidthType location) {
  assert(0 <= location);
  assert(location + shape.width() <= WIDTH);

  auto placement_row = 0;
  for (WidthType col = 0; col < shape.width(); ++col) {
    if (heights_[location + col] > shape.bottom_profile()[col])
      placement_row =
        std::max(heights_[location + col] - shape.bottom_profile()[col], placement_row);
  }
  
  assert(placement_row + shape.height() <= HEIGHT);

  //place the shape on the board
  for (size_t row = 0; row < shape.height(); ++row)
    for (size_t col = 0; col < shape.width(); ++col)
      if (shape[row][col]) {
        rows_[placement_row + row].set(location + col);
        heights_[location + col] = placement_row + row + 1;
      }
  
  //determine new max height so we don't loop through rows which we know to be empty later
  fill_height_ = std::max<HeightType>(fill_height_, placement_row + shape.height());

  //now determine and delete all filled rows that are within the shape's placement window (i.e.
  // check all rows between placement_row and placement_row + shape.height())
  auto deletion_count = 0;
  for (size_t i = 0; i < shape.height(); ++i) {
    auto row = static_cast<HeightType>(placement_row + i - deletion_count);
    if (deletion_count > 0)
      rows_[row] = rows_[row + deletion_count];
    if (rows_[row].all()) {
      for (size_t col = 0; col < WIDTH; ++col) {
        if (heights_[col] > row+1)
          --heights_[col];
        else if(heights_[col] == row+1) {
          size_t look_down = row;
          //this is the second most expensive part due to strided, 'unpredictable' memory access
          for (; look_down > 0 && !rows_[look_down-1][col]; --look_down);
          heights_[col] = look_down;
        }
      }
      ++deletion_count;
    }
  }

  if (deletion_count > 0) {
    //copy rows above placement window downwards
    for (HeightType row = placement_row + shape.height(); row < fill_height_; ++row)
      rows_[row - deletion_count] = rows_[row];

    //clean up topmost lines are now empty
    for (HeightType row = fill_height_ - deletion_count; row < fill_height_; ++row)
      rows_[row].reset();
    
    //update fill_height
    fill_height_ -= deletion_count;
  }
}
