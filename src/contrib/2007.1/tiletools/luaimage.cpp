#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <string.h>
#include <stdint.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

void lua_error (lua_State *L, const char *fmt, ...);

lua_State *LL = NULL;
SDL_Surface* img = NULL;

static int get_rgb(lua_State* L) {
  int x = static_cast<int>(lua_tonumber(L, 1));
  int y = static_cast<int>(lua_tonumber(L, 2));
  uint8_t r,g,b;
  uint8_t *pixel = static_cast<uint8_t*>(img->pixels);
  pixel += img->w * img->format->BytesPerPixel * y +
    img->format->BytesPerPixel * x;
  r = *pixel; ++pixel;
  g = *pixel; ++pixel;
  b = *pixel; ++pixel;
  lua_pushnumber(L, r);
  lua_pushnumber(L, g);
  lua_pushnumber(L, b);
  return 3;
}

static int get_rgba(lua_State* L) {
  int x = static_cast<int>(lua_tonumber(L, 1));
  int y = static_cast<int>(lua_tonumber(L, 2));
  uint8_t r,g,b,a;
  uint8_t *pixel = static_cast<uint8_t*>(img->pixels);
  pixel += img->w * img->format->BytesPerPixel * y +
    img->format->BytesPerPixel * x;
  r = *pixel; ++pixel;
  g = *pixel; ++pixel;
  b = *pixel; ++pixel;
  a = *pixel;
  lua_pushnumber(L, r);
  lua_pushnumber(L, g);
  lua_pushnumber(L, b);
  lua_pushnumber(L, a);
  return 4;
}

static int set_rgb(lua_State* L) {
  int x = static_cast<int>(lua_tonumber(L, 1));
  int y = static_cast<int>(lua_tonumber(L, 2));
  uint8_t r = static_cast<uint8_t>(lua_tonumber(L, 3));
  uint8_t g = static_cast<uint8_t>(lua_tonumber(L, 4));
  uint8_t b = static_cast<uint8_t>(lua_tonumber(L, 5));
  uint8_t *pixel = static_cast<uint8_t*>(img->pixels);
  pixel += img->w * img->format->BytesPerPixel * y +
    img->format->BytesPerPixel * x;
  *pixel = r; ++pixel;
  *pixel = g; ++pixel;
  *pixel = b; 
  return 0;
}

static int set_rgba(lua_State* L) {
  int x = static_cast<int>(lua_tonumber(L, 1));
  int y = static_cast<int>(lua_tonumber(L, 2));
  uint8_t r = static_cast<uint8_t>(lua_tonumber(L, 3));
  uint8_t g = static_cast<uint8_t>(lua_tonumber(L, 4));
  uint8_t b = static_cast<uint8_t>(lua_tonumber(L, 5));
  uint8_t a = static_cast<uint8_t>(lua_tonumber(L, 6));
  uint8_t *pixel = static_cast<uint8_t*>(img->pixels);
  pixel += img->w * img->format->BytesPerPixel * y +
    img->format->BytesPerPixel * x;
  *pixel = r; ++pixel;
  *pixel = g; ++pixel;
  *pixel = b; ++pixel;
  *pixel = a;
  return 0;
}

void at_exit() {
  if (LL != NULL)
    lua_close(LL);
  if (img != NULL)
    SDL_FreeSurface(img);
}

extern void usage(const char*);
extern int write_png(const char *filename, SDL_Surface& surface);

int main(int argc, char* argv[]) {

  if (argc == 1) {
    usage(argv[0]);
    return 1;
  }
  if (strncmp(argv[1], "-h", 2) == 0) {
    usage(argv[0]);
    return 0;
  }
  if (argc < 3 || argc > 4) {
    usage(argv[0]);
    return 1;
  }
   
  std::string scriptfile = argv[1];
  std::string imagefile  = argv[2];
  std::string outfile = (argc == 4) ? argv[3] : "";
  
  if (imagefile.size() == 0) {
    usage(argv[0]);
    return 1;
  }
  if (scriptfile.size() == 0) {
    usage(argv[0]);
    return 1;
  }
  
  img = IMG_Load(imagefile.c_str());
  if (img == NULL) {
    std::cerr << "Error: could not load image: " << imagefile << std::endl;
    return 1;
  }
  
  LL = lua_open();
  if (LL == NULL) {
    std::cerr << "Error: could not create lua state???" << std::endl;
    return 1;
  }
  
  luaopen_base(LL);
  luaopen_io(LL);
  luaopen_string(LL);
  luaopen_math(LL);

  lua_pushnumber(LL, img->w);
  lua_setglobal(LL, "width");
  lua_pushnumber(LL, img->h);
  lua_setglobal(LL, "height");
  lua_pushnumber(LL, img->format->BitsPerPixel);
  lua_setglobal(LL, "bpp");

  lua_pushcfunction(LL, get_rgb);
  lua_setglobal(LL, "get_rgb");
  lua_pushcfunction(LL, get_rgba);
  lua_setglobal(LL, "get_rgba");
  lua_pushcfunction(LL, set_rgb);
  lua_setglobal(LL, "set_rgb");
  lua_pushcfunction(LL, set_rgba);
  lua_setglobal(LL, "set_rgba");

  SDL_LockSurface(img);
  atexit(at_exit); 
  
  if (luaL_loadfile(LL, scriptfile.c_str()) || lua_pcall(LL, 0, 0, 0)) {
    std::cout << "Error parsing " << scriptfile << " : " << std::string(lua_tostring(LL,-1));
    SDL_UnlockSurface(img);
    return 1;
  }
  
  SDL_UnlockSurface(img);
  if (outfile.size() > 0) {
    bool saveAsPNG = false;
    if ((outfile.find(".png") > -1) ||
      (outfile.find(".PNG") > -1))
      saveAsPNG = true;
    if (saveAsPNG) {
      write_png(outfile.c_str(), *img);
    }
    else {
      SDL_SaveBMP(img, outfile.c_str());
    }
  }
  else {
    std::cerr << "ALPHA-Warning: refusing to overwrite original image \n\t\tdefine an output file" << std::endl;
  }
  
  return 0;
}
