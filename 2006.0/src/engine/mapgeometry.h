#ifndef MAPGEOMETRY_H
#define MAPGEOMETRY_H

/*
	This implements the sometimes confusing calculations,
	that are needed in fo2s map geometry.
		-phoku
*/

static const unsigned int TILE_WIDTH = 80;
static const unsigned int TILE_HEIGHT = 36;

static const unsigned int X_DELTA_X = 48; // if we move 1 to the right we need to add this to the last xpos
static const unsigned int X_DELTA_Y = 12; // if we move 1 to the right we need to subtract this from the last ypos
static const unsigned int Y_DELTA_X = 32; // if we move 1 tile down we need to add this to the last xpos
static const unsigned int Y_DELTA_Y = 24; // if we move 1 tile down we need to add this to the last ypos

class MapGeometry;
class RenderContext  {
	private:
		int x_offset;
		int y_offset;
	public:
		RenderContext(int _xoff,int _yoff ) : x_offset(_xoff),y_offset(_yoff) {};

		SDL_Rect createTileTarget(int x, int y) const {
			SDL_Rect retval;
			retval.x = x * X_DELTA_X + y * Y_DELTA_X + x_offset;
			retval.y = y * Y_DELTA_Y - x * X_DELTA_Y + y_offset;
			retval.w = X_DELTA_X + Y_DELTA_X;
			retval.h = X_DELTA_Y + Y_DELTA_Y;
			return retval;
		};

		SDL_Rect createObjectTarget( int x, int y) const {
			SDL_Rect retval;
			retval.x = 4816 - (((x+1)/2)*32 + (x/2)*16 - y*16) + x_offset;
			retval.y = (y*12 + (x/2)*12) - 1080 + y_offset;
			return retval;
		};
		void shift(int x,int y) {
			x_offset += x;
			y_offset += y;
		};
	private:
		friend class MapGeometry;
};

class MapGeometry {
	public:
		static RenderContext createContext(const SDL_Rect& viewport, int xpos, int ypos) {
			return RenderContext(
				viewport.x - viewport.w - xpos * X_DELTA_X - ypos * Y_DELTA_X,
				viewport.y - ypos * Y_DELTA_Y + xpos * X_DELTA_Y );
		};
};

#endif //RENDER_CONTEXT_H
