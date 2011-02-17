/*****************************************************/
/* lean Tags                    (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_TAGS_CLONEABLE
#define LEAN_TAGS_CLONEABLE

namespace lean
{
namespace cloneable
{

/// Base class that may be used to tag a specific class cloneable.
class cloneable
{
public:
	/// Constructs and returns a clone of this cloneable.
	virtual cloneable* clone() const = nullptr;
	/// Destroys this cloneable.
	virtual void destroy() const = nullptr;
};

} // namespace

using tags::cloneable;

} // namespace

#endif