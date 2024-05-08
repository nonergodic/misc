use crate::common::*;

use std::collections::HashMap;

#[derive(Debug)]
pub struct Shape {
  blocks: Vec<Vec<HeightType>>,
  height: HeightType
}

impl Shape {
  pub fn new<const WIDTH: HeightType, const HEIGHT: HeightType>(
    bitmap: &[[bool; WIDTH as usize]; HEIGHT as usize]
  ) -> Self 
  //static assert that WIDTH and HEIGHT are valid
  where [(); (WIDTH > 0) as usize * (HEIGHT > 0) as usize -1]: {
    Shape {
      blocks: (|| {
        let mut ret = Vec::with_capacity(WIDTH as usize);
        ret.resize(WIDTH as usize, Vec::with_capacity(HEIGHT as usize));
        //push blocks in bottom to top order
        for (row_index, row) in bitmap.iter().enumerate().rev() {
          for (col_index, square) in row.iter().enumerate() {
            if *square {
              ret[col_index].push(HEIGHT - row_index as HeightType)
            }
          }
        }
        ret
      })(),
      height: HEIGHT,
    }
  }

  pub fn width(&self) -> WidthType {
    self.blocks.len() as WidthType
  }

  pub fn height(&self) -> HeightType {
    self.height
  }

  pub fn blocks(&self) -> &Vec<Vec<HeightType>> {
    &self.blocks
  }
}

pub fn standard_shapes() -> HashMap<char, Shape> {
  let x = true;
  let o = false;
  let mut shapes = HashMap::new();
  
  shapes.insert('I', Shape::new::<4,1>(&[[x,x,x,x]]));
  shapes.insert('Q', Shape::new::<2,2>(&[[x, x],
                                        [x, x]]));

  shapes.insert('Z', Shape::new::<3,2>(&[[x, x, o],
                                        [o, x, x]]));

  shapes.insert('S', Shape::new::<3,2>(&[[o, x, x],
                                        [x, x, o]]));

  shapes.insert('T', Shape::new::<3,2>(&[[x, x, x],
                                        [o, x, o]]));

  shapes.insert('L', Shape::new::<2,3>(&[[x, o],
                                        [x, o],
                                        [x, x]]));

  shapes.insert('J', Shape::new::<2,3>(&[[o, x],
                                        [o, x],
                                        [x, x]]));
  shapes
}

//available in interactive mode
pub fn extra_shapes() -> HashMap<char, Shape> {
  let x = true;
  let o = false;
  let mut shapes = standard_shapes();
  shapes.insert('a', Shape::new::<1,1>(&[[x]]));
  shapes.insert('s', Shape::new::<2,2>(&[[x, x],
                                         [x, o]]));
  shapes.insert('d', Shape::new::<2,2>(&[[x, x],
                                         [o, x]]));
  shapes.insert('f', Shape::new::<1,6>(&[[x],
                                         [x],
                                         [x],
                                         [x],
                                         [x],
                                         [x]]));
  shapes.insert('q', Shape::new::<5,1>(&[[x, x, o, x, x]]));
  shapes.insert('w', Shape::new::<5,2>(&[[o, o, o, x, x],
                                         [x, x, o, o, o]]));
  shapes.insert('e', Shape::new::<5,2>(&[[x, x, o, o, o],
                                          [o, o, o, x, x]]));
  shapes.insert('r', Shape::new::<3,1>(&[[x, x, x]]));
  shapes
}

#[cfg(test)]
mod tests {
  use super::*;

  #[test]
  fn basic_shape_functionality() {
    let x = true;
    let o = false;
    let shape = Shape::new::<4,3>(&[[o, x, o, x],
                                    [x, o, o, x],
                                    [o, x, o, x]]);
    assert_eq!(*shape.blocks(), vec![vec![2 as u8], vec![1,3], vec![], vec![1,2,3]]);
    assert_eq!(shape.width(), 4);
    assert_eq!(shape.height(), 3);
  }
}