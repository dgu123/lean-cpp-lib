/*****************************************************/
/* lean Tags                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_TAGS_NONCOPYABLE
#define LEAN_TAGS_NONCOPYABLE

namespace lean
{
namespace tags
{

/// Base class that may be used to tag a specific class noncopyable.
class noncopyable
{
private:
#ifdef CPP0X_DELETE_METHODS
	noncopyable(const noncopyable&) = delete;
	noncopyable& operator =(const noncopyable&) = delete;
#else
	noncopyable(const noncopyable&);
	noncopyable& operator =(const noncopyable&);
#endif

protected:
	noncopyable() { }
	~noncopyable() { }
};

} // namespace

using tags::noncopyable;

} // namespace

#endif