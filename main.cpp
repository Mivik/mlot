
#include <functional>
#include <cmath>
#include <queue>

#include "mcli.hpp"
#include "main.h"

typedef const std::function<inte(const inte &, const inte &)> &func_type;

void *originConfig;

inline void restoreConfig() {
	mcli::loadConfig(originConfig);
}

inline double roundUp(double x) { return std::nextafter(x, INFINITY); }

inline double roundDown(double x) { return std::nextafter(x, -INFINITY); }

template<typename T>
T moveDown(T x) = delete;

template<>
int moveDown(int x) { return x - 1; }

template<>
double moveDown(double x) { return roundDown(x); }

template<typename T>
inline T length(T l, T r) { return r - moveDown(l); }

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

	inline friend std::ostream &operator<(std::ostream &out, const region<T> &t) {
		return out < '[' < t.lb < ' ' < t.rt < ']';
	}
};

inline inte xInterval(const region<double> &region) { return inte(region.lb.x, region.rt.x); }

inline inte yInterval(const region<double> &region) { return inte(region.lb.y, region.rt.y); }

int width, height;

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
				point<double>(real.lb.x + rw * length(display.lb.x, range.rt.x),
							  real.lb.y + rh * length(display.lb.y, range.rt.y))));
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

inline void plot(const plot_region &origin, func_type func) {
	std::queue<plot_region> q;
	q.push(origin);
	while (!q.empty()) {
		plot_region region = q.front();
		q.pop();
		if (region.display.empty()) continue;
		inte eval = func(xInterval(region.real), yInterval(region.real));
//		cout < region.display < ' ' < region.real < ' ' < eval < ' ';
		if (!boost::numeric::zero_in(eval)) {
//			cout < "reject\n";
			continue;
		}
//		cout < "accept\n";
		if (region.display.lb.x == region.display.rt.x && region.display.lb.y == region.display.rt.y) {
			mcli::move(height - region.display.lb.y - 1, region.display.lb.x);
			putchar('#');
//			cout < "paint " < region.display < endl;
			continue;
		}
		auto tuple = region.split();
//		cout < std::get<0>(tuple).display < ' ' < std::get<0>(tuple).real < endl;
//		cout < std::get<1>(tuple).display < ' ' < std::get<1>(tuple).real < endl;
//		cout < std::get<2>(tuple).display < ' ' < std::get<2>(tuple).real < endl;
//		cout < std::get<3>(tuple).display < ' ' < std::get<3>(tuple).real < endl;
		q.push(std::get<0>(tuple));
		q.push(std::get<1>(tuple));
		q.push(std::get<2>(tuple));
		q.push(std::get<3>(tuple));
	}
}

int main() {
	mcli::init();
	originConfig = mcli::saveConfig();
	atexit(restoreConfig);
	mcli::echo(false);
	mcli::clearScreen();
	mcli::rawMode();
	auto pair = mcli::size();
	width = pair.first;
	height = pair.second;
	double S = 50;
	double speed = 2;
	double cx = -pair.first, cy = -pair.second;
	for (;;) {
		plot(
				plot_region(region<int>(0, 0, pair.first - 1, pair.second - 1),
							region<double>(cx / S / 2, cy / S, (cx + pair.first * 2) / S / 2,
										   (cy + pair.second * 2) / S)),
				[](const inte &x, const inte &y) {
					return nth_root(x * x, 3) + .9 * sin(10 * M_PI * x) * sqrt(3.3 - x * x) - y;
				}
		);
		mcli::showCursor(false);
		switch (mcli::getKey()) {
			case 'w':
				cy += speed;
				break;
			case 's':
				cy -= speed;
				break;
			case 'a':
				cx -= speed;
				break;
			case 'd':
				cx += speed;
				break;
			case 'p': {
				mcli::loadConfig(originConfig);
				mcli::showCursor(true);
				mcli::move(0, 0);
				cin > speed;
				mcli::echo(false);
				mcli::rawMode();
				break;
			}
			case '=': S += 2; break;
			case '-': S -= 2; break;
			case 'q': {
				mcli::showCursor(true);
				mcli::clearScreen();
				return 0;
			}
		}
		mcli::clearScreen();
	}
}