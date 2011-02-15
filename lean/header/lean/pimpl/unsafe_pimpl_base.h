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
};

} // namespace

using pimpl::unsafe_pimpl_base;

} // namespace

#endif