/*****************************************************/
/* lean PImpl                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_PIMPL_UNSAFE_PIMPL_BASE
#define LEAN_PIMPL_UNSAFE_PIMPL_BASE

namespace lean
{
namespace pimpl
{

/// Base class for private implementation classes to be managed by smart pointers.
class unsafe_pimpl_base
{
protected:
	unsafe_pimpl_base() { };
	// Always defined, as restricted dtor access is the only reason for this class to exist
	~unsafe_pimpl_base() throw() { };
};

} // namespace

using pimpl::unsafe_pimpl_base;

} // namespace

#endif