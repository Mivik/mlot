
#ifndef MIVIK_MLOT_STRUCTS_HPP
#define MIVIK_MLOT_STRUCTS_HPP

#include <functional>
#include <iostream>
#include <utility>
#include <tuple>

#include "common.hpp"
#include "math.hpp"

typedef const std::function<bool(const inte &, const inte &)> &graph_t;
typedef const std::function<void(int, int, char)> &plotter_t;

template<typename T>
struct point {
	T x, y;

	point() = default;

	point(T x, T y) : x(x), y(y) {}

	inline friend std::ostream &operator<<(std::ostream &out, const point<T> &t) {
		return out < '(' < t.x < ',' < t.y < ')';
	}
};

template<typename T>
struct region {
	point<T> lb, rt;

	region() = default;

	region(const point<T> &lb, const point<T> &rt) : lb(lb), rt(rt) {}

	region(T left, T bottom, T right, T top) : lb(point<T>(left, bottom)), rt(point<T>(right, top)) {}

	inline T width() const { return length(lb.x, rt.x); }

	inline T height() const { return length(lb.y, rt.y); }

	inline bool empty() const {
		return lb.x > rt.x || lb.y > rt.y;
	}

	inline bool overlap(graph_t plotter) const {
		return plotter(inte(lb.x, rt.x), inte(lb.y, rt.y));
	}

	inline friend std::ostream &operator<(std::ostream &out, const region<T> &t) {
		return out < '[' < t.lb < ' ' < t.rt < ']';
	}
};

struct plot_region {
	region<int> display;
	region<double> real;

	plot_region() = default;

	plot_region(const region<int> &display, const region<double> &real) : display(display), real(real) {}

	inline plot_region subregion(const region<int> &range) const {
		const double rw = real.width() / display.width(), rh = real.height() / display.height();
		return plot_region(range, region<double>(
				point<double>(real.lb.x + rw * (range.lb.x - display.lb.x),
							  real.lb.y + rh * (range.lb.y - display.lb.y)),
				point<double>((real.lb.x + rw * length(display.lb.x, range.rt.x)),
							  (real.lb.y + rh * length(display.lb.y, range.rt.y)))));
	}

	inline std::tuple<plot_region, plot_region, plot_region, plot_region> split() const {
		const int sx = display.lb.x, sy = display.lb.y;
		const int ex = display.rt.x, ey = display.rt.y;
		const int cx = (sx + ex) >> 1;
		const int cy = (sy + ey) >> 1;
		return std::make_tuple(
				subregion(region<int>(sx, sy, cx, cy)),
				subregion(region<int>(cx + 1, sy, ex, cy)),
				subregion(region<int>(sx, cy + 1, cx, ey)),
				subregion(region<int>(cx + 1, cy + 1, ex, ey))
		);
	}
};

#endif
