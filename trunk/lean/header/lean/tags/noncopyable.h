/*****************************************************/
/* lean Tags                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_TAGS_NONCOPYABLE
#define LEAN_TAGS_NONCOPYABLE

#include "../lean.h"

namespace lean
{
namespace tags
{

/// Base class that may be used to tag a specific class noncopyable.
template <class Base>
class noncopyable_chain : public Base
{
private:
#ifndef LEAN0X_NO_DELETE_METHODS
	noncopyable_chain(const noncopyable_chain&) = delete;
	noncopyable_chain& operator =(const noncopyable_chain&) = delete;
#else
	noncopyable_chain(const noncopyable_chain&);
	noncopyable_chain& operator =(const noncopyable_chain&);
#endif

protected:
	noncopyable_chain() { }
#ifndef LEAN_OPTIMIZE_DEFAULT_DESTRUCTOR
	~noncopyable_chain() throw() { }
#endif
};

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
template <class Base>
class nonassignable_chain : public Base
{
private:
#ifndef LEAN0X_NO_DELETE_METHODS
	nonassignable_chain& operator =(const nonassignable_chain&) = delete;
#else
	nonassignable_chain& operator =(const nonassignable_chain&);
#endif

protected:
	nonassignable_chain() { }
	nonassignable_chain(const nonassignable_chain&) { }
#ifndef LEAN_OPTIMIZE_DEFAULT_DESTRUCTOR
	~nonassignable_chain() throw() { }
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

using tags::noncopyable_chain;
using tags::noncopyable;
using tags::nonassignable_chain;
using tags::nonassignable;

} // namespace

#endif