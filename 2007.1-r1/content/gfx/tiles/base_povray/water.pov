#include "textures.inc"

#include "light.inc"
#include "camera.inc"

plane { <0, 1, 0>, 0
  texture {
    pigment {
      bozo
      //granite 
      turbulence 3
      //lambda 1.5
      //omega .8
      //octaves 8
      pigment_map{ [0.0 color DarkTurquoise]
                 [0.6 color NewMidnightBlue]
                 [0.9 color MidnightBlue]
      }
      scale 0.2
    }
    /*
    normal {
      crackle 0.7 
      turbulence 0.25
      scale 0.3
    }*/
    normal {
      wrinkles
      turbulence 0.9
      omega 1.0
    }
    normal {
      bumps 0.3
      scale 0.05
    }
    finish {
      ambient 0.1
      diffuse 0.9
    }
  }
}
