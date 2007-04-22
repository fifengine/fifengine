#ifndef PALUTIL_H
#define PALUTIL_H
#include <SDL.h>
#include "singleton.h"
#include <stdint.h>
#include <map>
#include <string>
#include <vector>

namespace FIFE {
	class Animation;

	class PALAnimation {
		public:
			PALAnimation(int id, uint8_t* data,int width,int height,int gamma);
			~PALAnimation();
			Animation* generateAnimation();
			std::string getName() const;

			void setWidth(int);
			void setHeight(int);
			bool isValid() const;

		private:
			int m_id;
			uint8_t* m_data;
			int m_width;
			int m_height;
			int m_gamma;
			Animation* m_animation;
			std::vector<size_t> m_frames;
			std::vector<int16_t> m_shifts_x;
			std::vector<int16_t> m_shifts_y;


			void makeFrame(int nframe, SDL_Surface*&surface,int&x_shift,int&y_shift);
	};

	class PALUtil : public Singleton<PALUtil> {
		public:
			uint8_t checkPixel(uint8_t px);

		private:
			void init();

			uint8_t m_table[256];
			struct pal_animation {
				std::string name;
				int nr_elements;
				int pal_offset;
				uint8_t* color_table; // max 6 elements
				int speed; // in ms
			};
			typedef std::map<uint8_t,struct pal_animation> type_pal_animations;
			type_pal_animations m_pal_animations;

			SINGLEFRIEND(PALUtil);
			friend class PALAnimation;
	};

	inline
	uint8_t PALUtil::checkPixel(uint8_t px) {
		return m_table[px];
	};

}; // FIFE

#endif
