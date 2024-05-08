#![allow(incomplete_features)]
#![feature(generic_const_exprs)]
#![feature(int_log)]

use std::{env, io, debug_assert};

mod common;
mod shape;
mod board;

use shape::{standard_shapes, extra_shapes};
use board::{Board, STANDARD_BOARD_WIDTH, STANDARD_BOARD_HEIGHT} ;

fn interactive_mode() {
  let shapes = extra_shapes();
  print!("Shape characters:");
  for (shape_char, _) in shapes.iter() {
    print!(" {}", shape_char);
  }
  println!();
  let mut board = Board::new(STANDARD_BOARD_WIDTH, STANDARD_BOARD_HEIGHT);
  println!("Enter a shape character and a location - return = quit");
  loop {
    let mut line = String::new();  
    io::stdin().read_line(&mut line).expect("Failed to read line");
    if line.len() == 1 {
      break;
    }
    if line.len() < 3 {
      println!("invalid input, expected a letter and a number, try again");
      continue;
    }
    let (shape_char, location) = (line.chars().next().unwrap(), line[1..line.len()-1].parse());
    if !shapes.contains_key(&shape_char) {
      println!("unknown shape character");
      continue;
    }
    if location.is_err() {
      println!("invalid location");
      continue;
    }
    board.drop(&shapes[&shape_char], location.unwrap());
    println!("{}",board);  
  }
}

fn processing_mode() {
  let shapes = standard_shapes();
  loop {
    let mut line = String::new();
    io::stdin().read_line(&mut line).expect("Failed to read line");
    if line.len() == 0 {
      break;
    }
    let mut board = Board::new(STANDARD_BOARD_WIDTH, STANDARD_BOARD_HEIGHT);
    for action in line.split(",") {
      debug_assert!(action.len() >= 2);
      let (shape_char, location) = (action.chars().next().unwrap(), action[1..2].parse().unwrap());
      board.drop(&shapes[&shape_char], location);
    }
    println!("{}", board.fill_height());
  }
}

fn main() {
  //poor man's clif parser:
  if env::args().find(|arg| arg.eq("-I")).is_some() {
    interactive_mode();
  }
  else {
    processing_mode();
  }
}
