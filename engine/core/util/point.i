%{
#include "util/point.h"
%}

namespace FIFE {

	class Point {
		public:
			%immutable;
			int32_t x;
			int32_t y;
			%mutable;
			
			explicit Point(int x = 0, int y = 0);

			/** Vector addition
			 */
			Point operator+(const Point& p) const;
			/** Vector substraction
			 */
			Point operator-(const Point& p) const;

			/** Vector inplace addition
			 */
			Point& operator+=(const Point& p);
			/** Vector inplace substraction
			 */
			Point& operator-=(const Point& p);

			/** Scalar multiplikation with an integer value
			 */
			Point operator*(const long& i) const;

			/** Scalar division with an integer value
			 */
			Point operator/(const long& i) const;

			/** Equality comparision
			 */
			bool operator==(const Point& p) const;

			/** Equality comparision
			 */
			bool operator!=(const Point& p) const;

			/** Return length
			 */
			uint32_t length() const;
	};

	/** Print coords of the Point to a stream
	 */
	std::ostream& operator<<(std::ostream& os, const Point& p);

}
