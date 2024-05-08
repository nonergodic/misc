use std::debug_assert;
use std::cmp::max;
use std::fmt;

use crate::{
  common::*,
  shape::Shape
};

pub const STANDARD_BOARD_WIDTH: WidthType = 10;
pub const STANDARD_BOARD_HEIGHT: HeightType = 100;

#[derive(Debug)]
pub struct Board {
  blocks: Vec<Vec<HeightType>>,
  row_fill_count: Vec<WidthType>,
}

impl Board {
  pub fn new(width: WidthType, height: HeightType) -> Self {
    let mut ret = Board {
      blocks: Vec::with_capacity(width as usize),
      row_fill_count: Vec::with_capacity(height as usize),
    };
    ret.blocks.resize(width as usize, Vec::with_capacity(height as usize));
    ret.row_fill_count.resize(height as usize, 0);
    ret
  }

  pub fn drop(&mut self, shape: &Shape, location: WidthType) {
    debug_assert!(location + shape.width() <= self.width());
    //1. first we find where to place the shape
    let drop_columns = location as usize..(location+shape.width()) as usize;
    //this monster iterates over the columns of drop_columns and shape and
    let placement_row =
      self.blocks[drop_columns.clone()].iter()
      //gets the highest square of each relevant column on the board
      .map(|board_col| *board_col.last().unwrap_or(&0))
      //and zip it with
      .zip(shape.blocks().iter()
        //the lowest square of each column of the shape (or self.height for empty shape columns)
        .map(|shape_col| *shape_col.first().unwrap_or(&self.height())))
      //determines where the shape would be placed if it was only constrained by the current column
      .map(|(lhs, rhs)| lhs as isize - rhs as isize +1)
      //take the highest constraining row
      .fold(0, |acc, val| max(acc, val)) as HeightType;
    
    debug_assert!(placement_row as usize + shape.height() as usize <= self.height() as usize);
    
    //2. next we place the shape and update Board's datamembers accordingly
    let zipped_columns = shape.blocks().iter().zip(self.blocks[drop_columns].iter_mut());
    for (shape_col, board_col) in zipped_columns {
      for shape_square_height in shape_col.iter() {
        let board_square_height = placement_row + shape_square_height;
        board_col.push(board_square_height);
        self.row_fill_count[(board_square_height-1) as usize] += 1;
      }
    }

    //3. then we find all rows that are full and have to be cleared
    let maybe_delete_range = (placement_row as usize)..(placement_row+shape.height()) as usize;
    let rows_to_delete: Vec<_> = self.row_fill_count[maybe_delete_range].iter().enumerate()
      .filter(|(_, & fill_count)| fill_count == self.width())
      .map(|(row, _)| row + placement_row as usize).collect();
    
    //4. and finally, we delete them if there are any
    if rows_to_delete.len() > 0 {
      //copy and reduce heights from any blocks above
      for col in self.blocks.iter_mut() {
        let start_index = col.binary_search(&((rows_to_delete[0]+1) as HeightType)).unwrap();
        let end_index = col.len() - rows_to_delete.len();
        let mut delete_count = 1;
        for row_index in start_index..end_index {
          while delete_count < rows_to_delete.len() &&
            row_index + delete_count == rows_to_delete[delete_count] {
            delete_count += 1;
          }
          col[row_index] = col[row_index + delete_count] - delete_count as HeightType;
        }
        //handle topmost rows that dropped
        col.resize(col.len() - rows_to_delete.len(), 0);
      }
      //adjust row_fill_count accordingly
      for (delete_count, row) in rows_to_delete.iter().enumerate() {
        self.row_fill_count.remove(*row - delete_count);
      }
      self.row_fill_count.resize(self.row_fill_count.len() + rows_to_delete.len(), 0);
    }
    
  }

  pub fn width(&self) -> WidthType {
    self.blocks.len() as WidthType
  }

  pub fn height(&self) -> HeightType {
    self.row_fill_count.len() as HeightType
  }

  pub fn fill_height(&self) -> HeightType {
    self.blocks.iter().fold(0, |acc, height| max(acc, *height.last().unwrap_or(&0)))
  }
}

impl fmt::Display for Board {
  fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
    let fill_height = self.fill_height();
    let row_digits = ((if fill_height == 0 {0} else {fill_height.log10()}) + 1) as usize;
    let mut height_iters: Vec<_> = self.blocks.iter().map(|col| col.iter().rev().peekable()).collect();
    for row in (0..fill_height).rev() {
      write!(f, "{:row_digits$} |", {row})?;
      for col_height_it in height_iters.iter_mut() {
        if col_height_it.peek().map_or(false, |&&h| h == row+1) {
          col_height_it.next();
          write!(f, "x")?;
        }
        else {
          write!(f, " ")?;
        }
      }
      writeln!(f, "| {}", self.row_fill_count[row as usize])?;
    }
    writeln!(f, "{}{}", " ".repeat(row_digits+1), "-".repeat(self.width() as usize + 2))
  }
}

#[cfg(test)]
mod tests {
  use crate::shape::standard_shapes;

use super::*;

  use test_case::test_case;

  #[test_case("", 0; "empty")]
  #[test_case("Q0", 2; "Q0")]
  #[test_case("Q0,Q1", 4; "Q0,Q1")]
  #[test_case("Q0,Q2,Q4,Q6,Q8", 0; "Q0,Q2,Q4,Q6,Q8")]
  #[test_case("Q0,Q2,Q4,Q6,Q8,Q1", 2; "Q0,Q2,Q4,Q6,Q8,Q1")]
  #[test_case("Q0,Q2,Q4,Q6,Q8,Q1,Q1", 4; "Q0,Q2,Q4,Q6,Q8,Q1,Q1")]
  #[test_case("I0,I4,Q8", 1; "I0,I4,Q8")]
  #[test_case("I0,I4,Q8,I0,I4", 0; "I0,I4,Q8,I0,I4")]
  #[test_case("L0,J2,L4,J6,Q8", 2; "L0,J2,L4,J6,Q8")]
  #[test_case("L0,Z1,Z3,Z5,Z7", 2; "L0,Z1,Z3,Z5,Z7")]
  #[test_case("T0,T3", 2; "T0,T3")]
  #[test_case("T0,T3,I6,I6", 1; "T0,T3,I6,I6")]
  #[test_case("I0,I6,S4", 1; "I0,I6,S4")]
  #[test_case("T1,Z3,I4", 4; "T1,Z3,I4")]
  #[test_case("L0,J3,L5,J8,T1", 3; "L0,J3,L5,J8,T1")]
  #[test_case("L0,J3,L5,J8,T1,T6", 1; "L0,J3,L5,J8,T1,T6")]
  #[test_case("L0,J3,L5,J8,T1,T6,J2,L6,T0,T7", 2; "L0,J3,L5,J8,T1,T6,J2,L6,T0,T7")]
  #[test_case("L0,J3,L5,J8,T1,T6,J2,L6,T0,T7,Q4", 1; "L0,J3,L5,J8,T1,T6,J2,L6,T0,T7,Q4")]
  #[test_case("S0,S2,S4,S6", 8; "S0,S2,S4,S6")]
  #[test_case("S0,S2,S4,S5,Q8,Q8,Q8,Q8,T1,Q1,I0,Q4", 8; "S0,S2,S4,S5,Q8,Q8,Q8,Q8,T1,Q1,I0,Q4")]
  #[test_case("L0,J3,L5,J8,T1,T6,S2,Z5,T0,T7", 0; "L0,J3,L5,J8,T1,T6,S2,Z5,T0,T7")]
  #[test_case("Q0,I2,I6,I0,I6,I6,Q2,Q4", 3; "Q0,I2,I6,I0,I6,I6,Q2,Q4")]
  fn check_drop_sequence(drop_sequence: &str, expected_fill_height: HeightType) {
    let shapes = standard_shapes();
    let mut board = Board::new(STANDARD_BOARD_WIDTH, STANDARD_BOARD_HEIGHT);
    for action in drop_sequence.split(",") {
      if action.len() < 2 {
        break;
      }
      let (shape_char, location) = (action.chars().next().unwrap(), action[1..2].parse().unwrap());
      board.drop(&shapes[&shape_char], location);
    }
    assert_eq!(board.fill_height(), expected_fill_height);
  }
}
