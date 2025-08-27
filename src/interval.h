#ifndef INTERVAL_H
#define INTERVAL_H

class Interval {
public:
	double min, max;

	Interval() : min(+infinity), max(-infinity) {}

	Interval(double min, double max) : min(min), max(max) {}

	Interval(const Interval& a, const Interval& b) {
	    min = (a.min <= b.min) ? a.min : b.min;
	    max = (a.max >= b.max) ? a.max : b.max;
	}

	// 구간 크기
	double Size() const {
	    return max - min;
	}

	// x가 구간 안에 있는지? (양끝 포함)
	bool Contains(double x) const {
	    return min <= x && x <= max;
	}

	// x가 구간 안에 있는지? (양끝 제외)
	bool Surrounds(double x) const {
	    return min < x && x < max;
	}

	// x가 구간 안에 있는 경우에만 x 리턴
	// min 또는 max 경계를 넘어가면 경계값으로 설정
	double Clamp(double x) const {
	    if (x < min) return min;
	    if (x > max) return max;
	    return x;
	}

	// 구간의 범위를 delta만큼 증가 -> 양 끝을 delta/2만큼 늘림
	Interval Expand(double delta) const {
	    double padding = delta / 2.0f;
	    return Interval(min - padding, max + padding);
	}

	static const Interval empty, universe;
};

const Interval Interval::empty = Interval(+infinity, -infinity);
const Interval Interval::universe = Interval(-infinity, infinity);

Interval operator+(const Interval& ival, double displacement) {
    return Interval(ival.min + displacement, ival.max + displacement);
}

Interval operator+(double displacement, const Interval& ival) {
    return ival + displacement;
}

#endif