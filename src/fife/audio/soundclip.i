// SPDX-License-Identifier: LGPL-2.1-or-later
// SPDX-FileCopyrightText: 2005 - 2026 Fifengine contributors

%module fife
%{
#include "audio/soundclip.h"
#include "util/base/sharedptr.h"
%}

%include "util/resource/resource.i"

namespace FIFE {

	class SoundClip : public IResource {
	public:

		~SoundClip();
		bool isStream();
	};

	typedef SharedPtr<SoundClip> SoundClipPtr;
	%template(SharedSoundClipPointer) SharedPtr<SoundClip>;
}
