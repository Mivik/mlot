
#ifndef MIVIK_MCLI_HPP
#define MIVIK_MCLI_HPP

#include <utility>

namespace mcli {
	void init();

	void move(int, int);

	void echo(bool);

	void showCursor(bool);

	void clearScreen();

	void setTitle(const char *);

	void rawMode();

	int getKey();

	void *saveConfig();

	void loadConfig(void *);

	std::pair<int, int> size();
}

#if defined(__linux__)

#include <termio.h>
#include <unistd.h>

#include <cstdlib>
#include <cstdio>

inline void mcli::init() {}

inline void mcli::move(int x, int y) {
	printf("\033[%d;%dH", x + 1, y + 1);
	fflush(stdout);
}

inline void mcli::showCursor(bool flag) {
	if (flag) printf("\033[?25h");
	else printf("\033[?25l");
	fflush(stdout);
}

inline void mcli::echo(bool flag) {
	termios term = {};
	tcgetattr(STDIN_FILENO, &term);
	if (flag) term.c_lflag |= ICANON;
	else term.c_lflag &= ~ICANON;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

inline void mcli::clearScreen() {
	printf("\033c");
	fflush(stdout);
}

inline void mcli::setTitle(const char *title) {
	printf("\033]2;%s\007", title);
	fflush(stdout);
}

inline void mcli::rawMode() {
	termios term{};
	cfmakeraw(&term);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

inline int mcli::getKey() { return getchar(); }

inline void *mcli::saveConfig() {
	auto *term = new termios();
	tcgetattr(STDIN_FILENO, term);
	return term;
}

inline void mcli::loadConfig(void *config) {
	tcsetattr(STDIN_FILENO, TCSANOW, (termios *) config);
}

inline std::pair<int, int> mcli::size() {
	winsize size{};
	ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
	return std::make_pair(size.ws_col, size.ws_row);
}

#else
#error Unsupported platform.
#endif

#endif