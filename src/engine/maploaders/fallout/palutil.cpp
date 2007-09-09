#include "palutil.h"
#include "log.h"
#include <string.h>
#include "mffalloutpal.h"
#include "imagecache.h"
#include "animation.h"
#include "debugutils.h"

int _ln2(int pot)
{
	for(int i =0; i != 8; ++i)
		if( pot == (1 << i) )
			return i;
	return -1;
};

namespace FIFE {

	PALAnimation::PALAnimation(int id, uint8_t* data,int width,int height,int gamma) : m_id(id),m_data(data) {
		//Log("palanim") << "PAL Animation with id " << id << " and name " << getName() << " generated.";
		m_animation=0;
		m_gamma = gamma;
		m_width = width;
		m_height=height;
	};
	PALAnimation::~PALAnimation() {
		delete[] m_data;
	}

	void PALAnimation::setWidth(int width){
		//Log("palanim") <<  "width " << width;
		m_width = width;
	};
	void PALAnimation::setHeight(int height){
		//Log("palanim") <<  "height " << height;
		m_height = height;
	};

	bool PALAnimation::isValid() const {
		return  _ln2(m_id) != -1;
	};

	std::string PALAnimation::getName() const {
		return PALUtil::instance()->m_pal_animations[m_id].name;
	}
	Animation* PALAnimation::generateAnimation()
	{
		//Log("palanim") << "Generating Animation " << getName() << " with w,h= " 
		//		<< m_width << ", " << m_height;
		struct PALUtil::pal_animation p = PALUtil::instance()->m_pal_animations[m_id];
		Animation *anim = new Animation( p.nr_elements );
		anim->setFrameDuration( p.speed );

		ImageCache* imgCache = ImageCache::instance();
		for(int i=0; i != p.nr_elements ; ++i) {
			if( m_animation ){
				anim->setFrame(i, m_frames[i], m_shifts_x[i], m_shifts_y[i] );
			} else {
				int x_shift,y_shift;
				SDL_Surface* surface = 0;
				makeFrame(i, surface, x_shift, y_shift );
				if( surface == 0 ){
					PANIC_PRINT("surface == NULL in generateAnimation() !!");
				};
				size_t imgId = imgCache->addImage( surface );
				anim->setFrame(i, imgId, x_shift, y_shift );

				m_frames.push_back( imgId );
				m_shifts_x.push_back(x_shift);
				m_shifts_y.push_back(y_shift);
			};
		};
		anim->setCurrentFrame(0);
// 		Log("palanim") << "DONE";
		m_animation = anim;
		return anim;
	};
	void PALAnimation::makeFrame(int nframe, SDL_Surface* &surface, int& x_shift, int& y_shift)
	{
		//Log("palanim") << "makeFrame( " << nframe << " )";
		struct PALUtil::pal_animation p = PALUtil::instance()->m_pal_animations[m_id];
		x_shift = y_shift = 0;
		surface = 0;
		uint8_t* data = m_data;

		MFFalloutPAL palette("color.pal");


		surface = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, m_width, 
			m_height, 32, 0xff000000, 0x00ff0000, 0x0000ff00,0x000000ff);// 0x000000ff);
		if( surface == 0 ){
			PANIC_PRINT("sdl surface == null");
		};
		SDL_SetAlpha(surface, SDL_SRCALPHA,  0x000000ff);
		SDL_LockSurface(surface);
		uint32_t* pixeldata = static_cast<uint32_t*>(surface->pixels);
		for (uint32_t i = 0; i < static_cast<uint32_t>(m_height*m_width); ++i) {
			uint8_t index = *(data);
			if (index == 0) {
				*pixeldata = 0x00000000;
			} else if ( index >= p.pal_offset && index - p.pal_offset <= p.nr_elements) {
				index = index - p.pal_offset;
				index = (index + nframe) % p.nr_elements;
				double factor = 1.0;
				if( m_id & ( 1 | 8 ) ) // Slime & Shoreline
					factor = 4.0 / m_gamma;
				uint8_t red = uint8_t(p.color_table[3*index]/factor) & 0xff;
				uint8_t green =  uint8_t(p.color_table[3*index + 1]/factor) & 0xff;
				uint8_t blue =  uint8_t(p.color_table[3*index + 2]/factor) & 0xff;
			
				*pixeldata = (red << 24) | (green << 16) | (blue << 8) | 0xff;
			} else {
				uint8_t red = std::min(palette.getRed(index) * m_gamma, 0xff) & 0xff;
				uint8_t green = std::min(palette.getGreen(index) * m_gamma, 0xff) & 0xff;
				uint8_t blue = std::min(palette.getBlue(index) * m_gamma, 0xff) & 0xff;
			
				*pixeldata = (red << 24) | (green << 16) | (blue << 8) | 0xff;
			}
			++data;
			++pixeldata;
		};
		SDL_UnlockSurface(surface);

	};

	PALUtil::PALUtil() { init(); };
	PALUtil::~PALUtil() {};
	PALUtil::PALUtil(const PALUtil&){};
	PALUtil& PALUtil::operator=(const PALUtil&) {
		return *this;
	}


// Hard coded PAL Animations ...

/*
uint8_t PAL_Slime[] = { 0, 108, 0, 11, 115, 7, 27, 123, 15, 43, 131, 27 };                             // Slime
uint8_t PAL_Monitors[] = { 107, 107, 111, 99, 103, 127, 87, 107, 143, 0, 147, 163, 107, 187, 255 };    // Monitors
uint8_t PAL_FireSlow[] = { 255, 0, 0, 215, 0, 0 , 147, 43, 11, 255, 119, 0, 255, 59, 0 };              // Slow fire
uint8_t PAL_FireFast[] = { 71, 0, 0, 123, 0, 0, 179, 0, 0, 123, 0, 0, 71, 0, 0 };                      // Fast fire
uint8_t PAL_Shoreline[] = { 83, 63, 43, 75, 59, 43, 67, 55, 39, 63, 51, 39, 55, 47, 35, 51, 43, 35 };  // Shoreline
uint8_t  PAL_BlinkingRed[] = { 252,0,0 };   
uint8_t PAL_PalOffsets[] = { 229, 233, 238, 243,248, 254 };

std::string PAL_Names[] = { "Slime", "Monitors", "FireSlow", "FireFast", "Shoreline", "BlinkingRed" };
int PAL_NrElements[] = { 4,6,5,5,5,1};
int PAL_Speeds[] = {200,200,200,142,100,33};
uint8_t* PAL_ColorTables [] = {
 PAL_Slime, PAL_Monitors,PAL_FireSlow,PAL_FireFast,PAL_Shoreline,PAL_BlinkingRed 
};
*/

// Fire often is used combined (fast & slow Fire) ... since we currently CAN'T display this,
// since we'd need to decompose the images itself, we simple combine the fire parts.

uint8_t PAL_Slime[] = { 0, 108, 0, 11, 115, 7, 27, 123, 15, 43, 131, 27 };                             // Slime
uint8_t PAL_Monitors[] = { 107, 107, 111, 99, 103, 127, 87, 107, 143, 0, 147, 163, 107, 187, 255 };    // Monitors
uint8_t PAL_FireSlow[] = { 255, 0, 0, 215, 0, 0 , 147, 43, 11, 255, 119, 0, 255, 59, 0 ,
			 71, 0, 0, 123, 0, 0, 179, 0, 0, 123, 0, 0, 71, 0, 0 };                      // Slow&Fast fire
uint8_t PAL_Shoreline[] = { 83, 63, 43, 75, 59, 43, 67, 55, 39, 63, 51, 39, 55, 47, 35, 51, 43, 35 };  // Shoreline
uint8_t  PAL_BlinkingRed[] = { 252,0,0 };   
uint8_t PAL_PalOffsets[] = { 229, 233, 238,248, 254 };

std::string PAL_Names[] = { "Slime", "Monitors", "FireSlow", "Shoreline", "BlinkingRed" };
int PAL_NrElements[] = { 4,6,10,5,1};
int PAL_Speeds[] = {200,200,180,100,33};
uint8_t* PAL_ColorTables [] = {
 PAL_Slime, PAL_Monitors,PAL_FireSlow,PAL_Shoreline,PAL_BlinkingRed 
};

	void PALUtil::init() {
		Log("palutil") << "Init PAL Animation reference tables.";
		memset(m_table,0,256);

		struct pal_animation p;
		uint8_t id = 1; 
		for(int i =0; i != 5; ++i){
			p.name = PAL_Names[i];
			p.speed = PAL_Speeds[i];
			p.nr_elements = PAL_NrElements[i];
			p.pal_offset = PAL_PalOffsets[i];
			p.color_table = PAL_ColorTables[i];
			m_pal_animations[id] = p;
			memset(m_table + p.pal_offset, id, p.nr_elements);
			id = id << 1;
		};


	};

};
