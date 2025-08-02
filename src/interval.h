#ifndef INTERVAL_H
#define INTERVAL_H

class interval {
    public:
	double min, max;

	interval() : min(+infinity), max(-infinity) {}

	interval(double min, double max) : min(min), max(max) {}

	interval(const interval& a, const interval& b) {
	    min = (a.min <= b.min) ? a.min : b.min;
	    max = (a.max >= b.max) ? a.max : b.max;
	}

	// 구간 크기
	double size() const {
	    return max - min;
	}

	// x가 구간 안에 있는지? (양끝 포함)
	bool contains(double x) const {
	    return min <= x && x <= max;
	}

	// x가 구간 안에 있는지? (양끝 제외)
	bool surrounds(double x) const {
	    return min < x && x < max;
	}

	// x가 구간 안에 있는 경우에만 x 리턴
	// min 또는 max 경계를 넘어가면 경계값으로 설정
	double clamp(double x) const {
	    if (x < min) return min;
	    if (x > max) return max;
	    return x;
	}

	// 구간의 범위를 delta만큼 증가 -> 양 끝을 delta/2만큼 늘림
	interval expand(double delta) const {
	    double padding = delta / 2.0f;
	    return interval(min - padding, max + padding);
	}

	static const interval empty, universe;
};

const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, infinity);

#endif