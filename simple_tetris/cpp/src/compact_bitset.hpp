#pragma once

#include <cstdint>
#include <cassert>
#include <string>
#include <limits>

//CompactBitset emulates the relevant parts of std::bitset interface used by Board.
// Unlike std::bitset which internally uses an array of unsigned longs, CompactBitset wraps the
// smallest possible unsigned type that can hold SIZE bits, giving a more compact representation.
template <uint8_t SIZE>
class CompactBitset {
  static_assert(SIZE > 0);
  //we'd use boost::integer here if we had it
  using UintType = std::conditional_t<SIZE <=  8,  uint8_t,
                   std::conditional_t<SIZE <= 16, uint16_t,
                   std::conditional_t<SIZE <= 32, uint32_t,
                   std::conditional_t<SIZE <= 64, uint64_t,
                   void>>>>;

  public:
    bool operator[](uint8_t pos) const {
      assert(pos < SIZE);
      return static_cast<bool>(bits_ & (UintType{1} << pos));
    }

    void set(uint8_t pos) {
      assert(pos < SIZE);
      bits_ |= UintType{1} << pos;
    }

    bool all() const {
      auto constexpr filled =
        static_cast<UintType>((UintType{0}-1)) >> (std::numeric_limits<UintType>::digits - SIZE);
      return bits_ == filled;
    }

    void reset() {bits_ = 0;}

    std::string to_string(char zero = '0', char one = '1') const {
      auto ret = std::string(SIZE, 0);
      for (uint8_t i = 0; i < SIZE; ++i)
        //use right to left representation, like std::bitset
        ret[i] = (*this)[SIZE-1-i] ? one : zero;
      return ret;
    }

  private:
    UintType bits_;
};
