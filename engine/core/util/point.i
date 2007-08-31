%{
#include "util/point.h"
%}

namespace FIFE {

	template <typename T> class PointType {
	public:
		T x;
		T y;
		explicit PointType(T _x = 0, T _y = 0);
		PointType<T> operator+(const PointType<T>& p) const;
		PointType<T> operator-(const PointType<T>& p) const;
		PointType<T>& operator+=(const PointType<T>& p);
		PointType<T>& operator-=(const PointType<T>& p);
		PointType<T> operator*(const long& i) const;
		PointType<T> operator/(const long& i) const;
		bool operator==(const PointType<T>& p) const;
		bool operator!=(const PointType<T>& p) const;
		T length() const;
	};
	template<typename T>
	std::ostream& operator<<(std::ostream& os, const PointType<T>& p);

	typedef PointType<int> Point;
	typedef PointType<double> DoublePoint;

	%template(Point) PointType<int>;
	%template(DoublePoint) PointType<double>;
}
