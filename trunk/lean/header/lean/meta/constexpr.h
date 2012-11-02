/*****************************************************/
/* lean Meta                    (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN_META_CONSTEXPR
#define LEAN_META_CONSTEXPR

namespace lean
{
namespace meta
{

/// Helper struct that may be used to pass constant integers (compile-time literals).
template <int ConstIntExpr>
struct ce_int
{
	/// Integer literal.
	static const int value = ConstIntExpr;
};

} // namespace

using meta::ce_int;

} // namespace

#endif