
#ifndef MIVIK_MLOT_PLOT_HPP
#define MIVIK_MLOT_PLOT_HPP

#include <cassert>
#include <queue>
#include <tuple>

#include "structs.hpp"

inline void plot(const plot_region &origin, graph_t graph, plotter_t plotter) {
	const char* PATTERN = ".-*#";

	using std::get;

	std::queue<plot_region> q;
	q.push(origin);
	while (!q.empty()) {
		plot_region region = q.front();
		q.pop();
		if (region.display.empty()) continue;
		if (!region.real.overlap(graph)) continue;
		if (region.display.lb.x == region.display.rt.x && region.display.lb.y == region.display.rt.y) {
			auto tuple = region.split();
			int cnt = 0;
			cnt += get<0>(tuple).real.overlap(graph);
			cnt += get<1>(tuple).real.overlap(graph);
			cnt += get<2>(tuple).real.overlap(graph);
			cnt += get<3>(tuple).real.overlap(graph);
			assert(cnt);
			plotter(region.display.lb.x, region.display.lb.y, PATTERN[cnt]);
			continue;
		}
		auto tuple = region.split();
		q.push(get<0>(tuple));
		q.push(get<1>(tuple));
		q.push(get<2>(tuple));
		q.push(get<3>(tuple));
	}
}

#endif
