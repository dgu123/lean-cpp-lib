/*****************************************************/
/* lean Tags                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_TAGS_NONCOPYABLE
#define LEAN_TAGS_NONCOPYABLE

#include "../cpp0x.h"

namespace lean
{
namespace tags
{

/// Base class that may be used to tag a specific class noncopyable.
class noncopyable
{
private:
#ifndef LEAN0X_NO_DELETE_METHODS
	noncopyable(const noncopyable&) = delete;
	noncopyable& operator =(const noncopyable&) = delete;
#else
	noncopyable(const noncopyable&);
	noncopyable& operator =(const noncopyable&);
#endif

protected:
	noncopyable() { }
#ifndef LEAN_OPTIMIZE_DEFAULT_DESTRUCTOR
	~noncopyable() throw() { }
#endif
};

/// Base class that may be used to tag a specific class nonassignable.
class nonassignable
{
private:
#ifndef LEAN0X_NO_DELETE_METHODS
	nonassignable& operator =(const nonassignable&) = delete;
#else
	nonassignable& operator =(const nonassignable&);
#endif

protected:
	nonassignable() { }
	nonassignable(const nonassignable&) { }
#ifndef LEAN_OPTIMIZE_DEFAULT_DESTRUCTOR
	~nonassignable() throw() { }
#endif
};

} // namespace

using tags::noncopyable;
using tags::nonassignable;

} // namespace

#endif