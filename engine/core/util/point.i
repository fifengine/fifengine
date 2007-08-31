%{
#include "util/point.h"
%}

namespace FIFE {

	template <typename T> class PointType2D {
	public:
		T x;
		T y;
		explicit PointType2D(T _x = 0, T _y = 0);
		PointType2D<T> operator+(const PointType2D<T>& p) const;
		PointType2D<T> operator-(const PointType2D<T>& p) const;
		PointType2D<T>& operator+=(const PointType2D<T>& p);
		PointType2D<T>& operator-=(const PointType2D<T>& p);
		PointType2D<T> operator*(const long& i) const;
		PointType2D<T> operator/(const long& i) const;
		PointType2D<T> operator*(const double& i) const;
		PointType2D<T> operator/(const double& i) const;
		bool operator==(const PointType2D<T>& p) const;
		bool operator!=(const PointType2D<T>& p) const;
		T length() const;
	};

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const PointType2D<T>& p);

	typedef PointType2D<int> Point;
	typedef PointType2D<int> IntPoint;
	typedef PointType2D<double> DoublePoint;
	typedef PointType2D<int> IntPoint2D;
	typedef PointType2D<double> DoublePoint2D;

	%template(Point) PointType2D<int>;
	%template(DoublePoint) PointType2D<double>;

	template <typename T> class PointType3D {
	public:
		T x;
		T y;
		T z;
		explicit PointType3D(T _x = 0, T _y = 0, T _z = 0);
		PointType3D<T> operator+(const PointType3D<T>& p) const;
		PointType3D<T> operator-(const PointType3D<T>& p) const;
		PointType3D<T>& operator+=(const PointType3D<T>& p);
		PointType3D<T>& operator-=(const PointType3D<T>& p);
		PointType3D<T> operator*(const long& i) const;
		PointType3D<T> operator/(const long& i) const;
		PointType3D<T> operator*(const double& i) const;
		PointType3D<T> operator/(const double& i) const;
		bool operator==(const PointType3D<T>& p) const;
		bool operator!=(const PointType3D<T>& p) const;
		T length() const;
	};

	template<typename T>
	std::ostream& operator<<(std::ostream& os, const PointType3D<T>& p);

	typedef PointType3D<int> Point3D;
	typedef PointType3D<int> IntPoint3D;
	typedef PointType3D<double> DoublePoint3D;

	%template(Point3D) PointType3D<int>;
	%template(DoublePoint3D) PointType3D<double>;
}
