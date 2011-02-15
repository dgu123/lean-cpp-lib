/*****************************************************/
/* lean Tags                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_TAGS_NONCONSTRUCTIBLE
#define LEAN_TAGS_NONCONSTRUCTIBLE

#include "noncopyable.h"

namespace lean
{
namespace tags
{

/// Base class that may be used to tag a specific class nonconstructible.
class nonconstructible : public noncopyable
{
private:
#ifdef CPP0X_DELETE_METHODS
	nonconstructible() = delete;
	~nonconstructible() = delete;
#else
	nonconstructible() { }
	~nonconstructible() { }
#endif
};

} // namespace

using tags::nonconstructible;

} // namespace

#endif