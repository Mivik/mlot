
#include <iostream>

#include <boost/numeric/interval.hpp>

using namespace boost::numeric::interval_lib;

typedef boost::numeric::interval<double, policies<save_state<rounded_transc_opp<double>>, checking_base<double>>> inte;

template<typename T>
inline std::istream &operator>(std::istream &in, T &t) { return in >> t; }

template<typename T>
inline std::ostream &operator<(std::ostream &out, const T &t) { return out << t; }


using std::cout;
using std::cin;

#define endl ('\n')
#define P(x) do { cout<#x" = "<(x)<endl; } while (0)

inline std::ostream &operator<<(std::ostream &out, const inte &t) {
	return out < '[' < t.lower() < ',' < t.upper() < ']';
}