// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%{
#ifdef HAVE_MOVIE
#include "video/movie/moviedecoder.h"
#include "video/movie/movieplayer.h"
#endif
%}

#ifdef HAVE_MOVIE

namespace FIFE {

	/** Demuxes a movie container and decodes its video stream to RGBA. */
	class MovieDecoder {
	public:
		MovieDecoder(const std::string& path);
		~MovieDecoder();

		uint32_t getWidth() const;
		uint32_t getHeight() const;
		double getFrameRate() const;
		double getDuration() const;
		bool nextFrame();
		bool seek(double seconds);
		double getFrameTimestamp() const;
		const uint8_t* getFrameData() const;
	};

	/** Plays a movie into a persistent image, one frame at a time. */
	class MoviePlayer {
	public:
		MoviePlayer();
		~MoviePlayer();

		void load(const std::string& path);
		void unload();
		bool isLoaded() const;
		void rewind();
		void stop();
		void play(double currentTime);
		void update(double currentTime);
		void setLooping(bool looping);
		bool isLooping() const;
		bool isFinished() const;
		ImagePtr getFrameImage() const;
		uint32_t getWidth() const;
		uint32_t getHeight() const;
		double getDuration() const;
		bool hasAudio() const;
		SoundClipPtr getSoundClip();
	};

}

// MovieSource and MovieAudioDecoder are deliberately not wrapped: both are only
// constructible from a shared MovieSource, and exposing std::shared_ptr here would
// add SWIG stdlib support the rest of the interface does not use. Go through
// MoviePlayer or Engine.playMovie() instead.

#endif // HAVE_MOVIE