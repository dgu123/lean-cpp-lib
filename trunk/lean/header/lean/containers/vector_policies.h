/*****************************************************/
/* lean Containers              (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN_CONTAINERS_VECTOR_POLICIES
#define LEAN_CONTAINERS_VECTOR_POLICIES

#include "../lean.h"
#include "../meta/conditional.h"
#include "construction.h"

namespace lean 
{
namespace containers
{

/// Defines construction policies for vector classes.
namespace vector_policies
{
	/// Simple vector element construction policy.
	template <bool RawMove = false, bool RawCopy = false, bool NoDestruct = false, bool NoConstruct = false>
	struct policy
	{
		/// Specifies whether memory containing constructed elements may be moved as a whole, without invoking the contained elements' copy or move constructors.
		static const bool raw_move = RawMove;
		/// Specifies whether memory containing constructed elements may be copied as a whole, without invoking the contained elements' copy constructors.
		static const bool raw_copy = RawCopy;
		/// Specifies whether memory containing constructed elements may be freed as a whole, without invoking the contained elements' destructors.
		static const bool no_destruct = NoDestruct;
		/// Specifies whether memory needs to be initialized by constructing elements.
		static const bool no_construct = NoConstruct;

		/// Move construction tag matching raw_move.
		typedef typename conditional_type<raw_move, trivial_construction_t, nontrivial_construction_t>::type move_tag;
		/// Copy construction tag matching raw_copy.
		typedef typename conditional_type<raw_copy, trivial_construction_t, nontrivial_construction_t>::type copy_tag;
		/// Destruction tag matching no_destruct.
		typedef typename conditional_type<no_destruct, trivial_construction_t, nontrivial_construction_t>::type destruct_tag;
		/// Construction tag matching no_construct.
		typedef typename conditional_type<no_construct, trivial_construction_t, nontrivial_construction_t>::type construct_tag;
	};

	/// Default element construction policy.
	typedef policy<> nonpod;
	/// Semi-POD element construction policy (raw move, yet properly copied and destructed).
	typedef policy<true> semipod;
	/// Initialize-POD element construction policy (raw move, no destruction, yet properly constructed).
	typedef policy<true, true, true> inipod;
	/// POD element (no-)construction policy.
	typedef policy<true, true, true, true> pod;
}

} // namespace

namespace vector_policies = containers::vector_policies;

} // namespace

#endif