/*****************************************************/
/* lean Time                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_TIME_HIGHRES_TIMER
#define LEAN_TIME_HIGHRES_TIMER

#include "../lean.h"

namespace lean
{
namespace time
{

/// High-resolution timer class.
class highres_timer
{
private:
	uint8 m_frequency;
	uint8 m_time;

public:
	/// Constructs a new timer from the current system time.
	highres_timer();

	/// Updates the reference time stored by this timer.
	void tick();

	/// Gets the time that has elapsed since the last tick.
	double seconds() const;
	/// Gets the time that has elapsed since the last tick.
	double milliseconds() const;
};

} // namespace

using time::highres_timer;

} // namespace

#ifdef LEAN_INLINING
#include "source/highres_timer.cpp"
#endif

#endif