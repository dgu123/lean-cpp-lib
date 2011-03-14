/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_SMART_CLONEABLE
#define LEAN_SMART_CLONEABLE

namespace lean
{
namespace smart
{

/// Base class that may be used to tag a specific class cloneable.
class cloneable
{
protected:
	~cloneable() throw() { };

public:
	/// Constructs and returns a clone of this cloneable.
	virtual cloneable* clone() const = 0;
	/// Destroys this cloneable.
	virtual void destroy() const = 0;
};

} // namespace

using smart::cloneable;

} // namespace

#endif