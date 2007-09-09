#include <iostream>
void usage(const char* name) {
std::cout << "USAGE: " << name;
std::cout << " [ options ] (--create $out | --edit $out) --mask $mask " << std::endl;
std::cout << "                        --first $input --second $input" << std::endl;
std::cout << "" << std::endl;
std::cout << "A specialised image manipulation program with two basic modes:" << std::endl;
std::cout << "* Take a rectangle image with size of $mask from $input image and blit" << std::endl;
std::cout << "  it into $out, working on those pixels where the mask is white. Bits" << std::endl;
std::cout << "  corresponding to blackness in the mask will be set to invisible" << std::endl;
std::cout << "  (alpha = 0) in the output." << std::endl;
std::cout << "* Take two rectangles of size of $mask from two input images and blit" << std::endl;
std::cout << "  them into $out; if a mask pixel is 0, the dest pixel is set to black" << std::endl;
std::cout << "  with alpha=0, if a mask pixel is 128, the corresponding pixel from" << std::endl;
std::cout << "  --second $input is used, while mask pixel 255 uses the colour from " << std::endl;
std::cout << "  --first $input (both times alpha set to 255)." << std::endl;
std::cout << "" << std::endl;
std::cout << "Notes:" << std::endl;
std::cout << "* Input images should have 24 bits (== NO ALPHA), at least after" << std::endl;
std::cout << "  loading with IMG_Load(...)." << std::endl;
std::cout << "* The mask images need to be 8 bit grayscale." << std::endl;
std::cout << "* The output is always 32 bit png; this images needs to be of a certain" << std::endl;
std::cout << "  dimension: w has to be a multiple of 80 and h a multiple of 36!" << std::endl;
std::cout << "  These numbers come from the f2 tile size (and thus the $mask images)," << std::endl;
std::cout << "  but aren't hardcoded beyond the default values (160x72)." << std::endl;
std::cout << "" << std::endl;
std::cout << " If you use --create $out and can specify (both!) --width and --height." << std::endl;
std::cout << "* You can specify into which --row and --column to blit (in $out) using" << std::endl;
std::cout << "  steps of size of $mask." << std::endl;
std::cout << "* You can specify offsets (--xoff, --yoff) into the $input image; the" << std::endl;
std::cout << "  defaults are 0, 0." << std::endl;
std::cout << "" << std::endl;
std::cout << "  If you use two input images, strange things may happen is they are" << std::endl;
std::cout << "  not of the same size; probably even stranger when using offsets" << std::endl;
std::cout << "  (haven't tried this so far)..." << std::endl;
}
