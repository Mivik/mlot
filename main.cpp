
#include <signal.h>

#include <functional>
#include <cmath>
#include <queue>

#include "structs.hpp"
#include "common.hpp"
#include "mcli.hpp"
#include "plot.hpp"

int width, height;

struct equation_graph {
	using equation_t = std::function<inte(const inte &, const inte &)>;
	equation_t equation;
	equation_graph(const equation_t& equation): equation(equation) {}
	inline bool operator()(const inte &x, const inte &y) const { return boost::numeric::zero_in(equation(x, y)); }
};

struct console_plotter {
	inline void operator()(int x, int y, char c) {
		mcli::move(height - y - 1, x);
		putchar(c);
	}
};

void *originConfig;

inline void restoreConfig() { mcli::loadConfig(originConfig); }

double cx, cy;
void windowChangeListener(int) {
	auto pair = mcli::size();
	width = pair.first;
	height = pair.second;
	cx = -pair.first;
	cy = -pair.second;
}

int main() {
	signal(SIGWINCH, windowChangeListener);
	mcli::init();
	originConfig = mcli::saveConfig();
	atexit(restoreConfig);
	mcli::echo(false);
	mcli::clearScreen();
	mcli::rawMode();
	windowChangeListener(0);
	double S = 50;
	double speed = 2;
	for (;;) {
		plot(
				plot_region(region<int>(0, 0, width-1, height-1),
							region<double>(cx / S / 2, cy / S, (cx + width * 2) / S / 2,
										   (cy + height * 2) / S)),
				/*equation_graph([](const inte &x, const inte &y) {
					return exp(sin(x)+cos(y))-sin(exp(x+y));
				}),*/
				[](const inte& x, const inte& y) {
					return x.upper()>=y.lower();
				},
				console_plotter()
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