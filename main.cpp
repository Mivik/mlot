
#include <functional>
#include <cmath>
#include <queue>

#include "structs.hpp"
#include "mcli.hpp"

typedef const std::function<bool(const inte &, const inte &)> &plotter_t;

struct equation_plotter {
	using equation_t = std::function<inte(const inte &, const inte &)>;
	equation_t equation;
	equation_plotter(const equation_t& equation): equation(equation) {}
	inline bool operator()(const inte &x, const inte &y) const { return boost::numeric::zero_in(equation(x, y)); }
};

void *originConfig;

inline void restoreConfig() { mcli::loadConfig(originConfig); }

inline void plot(const plot_region &origin, plotter_t plotter) {
	std::queue<plot_region> q;
	q.push(origin);
	while (!q.empty()) {
		plot_region region = q.front();
		q.pop();
		if (region.display.empty()) continue;
		if (!plotter(xInterval(region.real), yInterval(region.real))) continue;
		if (region.display.lb.x == region.display.rt.x && region.display.lb.y == region.display.rt.y) {
			mcli::move(height - region.display.lb.y - 1, region.display.lb.x);
			putchar('#');
			continue;
		}
		auto tuple = region.split();
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
				equation_plotter([](const inte &x, const inte &y) {
					return sin(x*x+y*y)-cos(x*y);
				})
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