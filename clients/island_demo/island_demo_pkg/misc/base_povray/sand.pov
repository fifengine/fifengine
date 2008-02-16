#include "textures.inc"

#include "light.inc"
#include "camera.inc"

plane { <0, 1, 0>, 0
  texture {
    pigment {
      bozo
      //granite 
      turbulence 2
      //lambda 1.5
      //omega .8
      //octaves 8
      pigment_map{ [0.0 color DarkTan]
                 [0.6 color Tan]
      }
      scale 0.3
    }
    /*
    normal {
      crackle 0.7 
      turbulence 0.25
      scale 0.3
    }*/
    normal {
      wrinkles
      turbulence 0.1
      lambda 0.5
      omega 1.0
    }
    normal {
      bumps 0.5
      scale 0.05
    }
    finish {
      ambient 0.1
      diffuse 0.9
    }
  }
}
