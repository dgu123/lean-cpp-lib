/*****************************************************/
/* lean PImpl                   (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN_PIMPL_STATIC_PIMPL
#define LEAN_PIMPL_STATIC_PIMPL

#include "../lean.h"

/// @addtogroup PImplMacros PImpl macros
/// @see lean::pimpl
/// @{

/// Makes the given class behave like an interface to a static private implementation.
#define LEAN_SIMPL_INTERFACE_BEHAVIOR(name) LEAN_INTERFACE_BEHAVIOR(name) \
		private: \
			LEAN_INLINE name() throw() { } \
			LEAN_INLINE name(const name&) throw() { }

/// Makes the given class behave like an interface to a static private implementation, supporting shared ownership.
#define LEAN_SHARED_SIMPL_INTERFACE_BEHAVIOR(name) LEAN_SHARED_INTERFACE_BEHAVIOR(name) \
		private: \
			LEAN_INLINE name() throw() { } \
			LEAN_INLINE name(const name&) throw() { }

/// Defines a local reference to the private implementation of the given type and name.
#define LEAN_EXIMPL(t, m, w) t &m = static_cast<t&>(w);
/// Defines a local reference to the private implementation 'm' of type 'M'.
#define LEAN_EXIMPL_M(w) LEAN_EXIMPL(M, m, w)
/// Defines a local reference to the private implementation 'm' of type 'M'.
#define LEAN_EXIMPL_CM(w) LEAN_EXIMPL(const M, m, w)

/// Defines a local reference to the private implementation of the given type and name.
#define LEAN_SIMPL(t, m) t &m = static_cast<t&>(*this);
/// Defines a local reference to the private implementation 'm' of type 'M'.
#define LEAN_SIMPL_M LEAN_SIMPL(M, m)
/// Defines a local reference to the private implementation 'm' of type 'M'.
#define LEAN_SIMPL_CM LEAN_SIMPL(const M, m)

/// @}

#endif