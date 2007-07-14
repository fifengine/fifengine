#include "colors.inc"
#include "textures.inc"
#include "stones.inc"

#include "camera.inc"
#include "light.inc"

plane { <0, 1, 0>, 0
  texture {
    pigment {
      checker Gold, Silver
      //color Tan
      //color Copper
    }
    //normal {bumps 0.7 scale 0.05}
  }
}

box {
  <-1, 0,   -1>,  // Near lower left corner
  < 1, 1,  1>   // Far upper right corner
  texture {
    T_Stone25     // Pre-defined from stones.inc
    scale 4       // Scale by the same amount in all
          // directions
  }
}

cylinder {
  <4, 0, 3>,     // Center of one end
    <4, 3, 3>,     // Center of other end
    0.1            // Radius
      open           // Remove end caps
      texture { T_Stone23 scale 4 }
}
