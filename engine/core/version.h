/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#ifndef FIFE_VERSION_H
#define FIFE_VERSION_H

/** These version numbers should be checked and updated
 * as part of the release process for FIFE.
 */
#ifndef FIFE_MAJOR_VERSION
	#define FIFE_MAJOR_VERSION 0
#endif
#ifndef FIFE_MINOR_VERSION
	#define FIFE_MINOR_VERSION 3
#endif
#ifndef FIFE_PATCH_VERSION
	#define FIFE_PATCH_VERSION 4
#endif

/** Types
 *  0 = none (pre-release info is not appended to the version in this case)
 *  1 = alpha
 *  2 = beta
 *  3 = rc
 */
#ifndef FIFE_PRERELEASE_TYPE
	#define FIFE_PRERELEASE_TYPE 0
#endif
#ifndef FIFE_PRERELEASE_VERSION
	#define FIFE_PRERELEASE_VERSION 0
#endif

/***************************************************************************
 * Do not update anything below this line!
 ***************************************************************************/

#define FIFE_STR(s)			# s
#define FIFE_XSTR(s)		FIFE_STR(s)

#define FIFE_DOT(a,b)		a.b
#define FIFE_XDOT(a,b)		FIFE_DOT(a,b)

#define FIFE_PLUS(a,b)		a+b
#define FIFE_XPLUS(a,b)		FIFE_PLUS(a,b)

#define FIFE_MINUS(a,b)		a-b
#define FIFE_XMINUS(a,b)	FIFE_MINUS(a,b)

#if FIFE_PRERELEASE_TYPE==1
	#define FIFE_PRERELEASE alpha
#elif FIFE_PRERELEASE_TYPE==2
 	#define FIFE_PRERELEASE beta
#elif FIFE_PRERELEASE_TYPE==3
	#define FIFE_PRERELEASE rc
#endif

#if FIFE_PRERELEASE_VERSION>0
	#ifdef FIFE_PRERELEASE
		#define FIFE_PRERELEASE_STR \
			FIFE_XDOT( \
				FIFE_PRERELEASE, \
				FIFE_PRERELEASE_VERSION \
			)
	#endif
#endif

#define FIFE_VERSION \
	FIFE_XDOT( \
		FIFE_XDOT(FIFE_MAJOR_VERSION, FIFE_MINOR_VERSION), \
		FIFE_PATCH_VERSION \
	)

#ifdef FIFE_PRERELEASE_STR
	#define FIFE_VERSION_STRING \
		FIFE_XMINUS( \
			FIFE_VERSION, \
			FIFE_PRERELEASE_STR \
		)
#endif
#ifdef FIFE_GIT_HASH
	#ifndef FIFE_VERSION_STRING
		#define FIFE_VERSION_STRING \
			FIFE_XPLUS( \
				FIFE_VERSION, \
				FIFE_GIT_HASH \
			)
	#else
		#undef FIFE_VERSION_STRING
		#ifdef FIFE_PRERELEASE_STR
			#define FIFE_VERSION_STRING \
				FIFE_XMINUS( \
					FIFE_VERSION, \
					FIFE_XPLUS( \
						FIFE_PRERELEASE_STR, \
						FIFE_GIT_HASH \
					) \
				)
		#else
			#define FIFE_VERSION_STRING \
				FIFE_XPLUS( \
					FIFE_VERSION, \
					FIFE_GIT_HASH \
				)
		#endif
	#endif
#else
	#define FIFE_GIT_HASH ""
#endif


// This is an actual release
#ifndef FIFE_VERSION_STRING
	#define FIFE_VERSION_STRING FIFE_VERSION
#endif

/** All FIFE related code is in this namespace.
 *  We currently only use one namespace for all our code with the exception of
 *  the gcn namespace which is used for our custom widgets.
 */
namespace FIFE {
	inline const char* getVersion() {
		return FIFE_XSTR(FIFE_VERSION_STRING);
	}

	inline int getMajor() {
		return FIFE_MAJOR_VERSION;
	}

	inline int getMinor() {
		return FIFE_MINOR_VERSION;
	}

	inline int getPatch() {
		return FIFE_PATCH_VERSION;
	}

	inline const char* getHash() {
		return FIFE_XSTR(FIFE_GIT_HASH);
	}
} //FIFE

//cleanup
#undef FIFE_STR
#undef FIFE_XSTR
#undef FIFE_DOT
#undef FIFE_XDOT
#undef FIFE_PLUS
#undef FIFE_XPLUS
#undef FIFE_MINUS
#undef FIFE_XMINUS
#undef FIFE_VERSION_STRING
#undef FIFE_VERSION
#undef FIFE_PRERELEASE
#undef FIFE_PRERELEASE_STR

#endif //FIFE_VERSION_H

