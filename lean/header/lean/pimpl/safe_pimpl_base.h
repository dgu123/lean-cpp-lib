/*****************************************************/
/* lean PImpl                   (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_PIMPL_SAFE_PIMPL_BASE
#define LEAN_PIMPL_SAFE_PIMPL_BASE

namespace lean
{
namespace pimpl
{

/// Base class for private implementation classes to be securely managed by smart pointers.
class safe_pimpl_base
{
public:
	/// Virtual destructor guarantees correct destruction
	/// even where the actual pimpl class is unknown.
	virtual ~safe_pimpl_base() { };
};

} // namespace

using pimpl::safe_pimpl_base;

} // namespace

#endif