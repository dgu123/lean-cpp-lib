/*****************************************************/
/* lean C++0x Config            (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CPP0X
#define LEAN_CPP0X

#include "macros.h"

/// @addtogroup CPP0X C++0x-related macros
/// @{

#ifdef DOXYGEN_READ_THIS
	/// @ingroup GlobalSwitches
	/// Define this to disable all C++0x features
	#define LEAN0X_DISABLE
#endif

// Disable all C++0x features by default when working with older C++ standards
#if (201100L > __cplusplus) || defined(LEAN0X_DISABLE)
	/// Indicates that built-in nullptr is not available.
	#define LEAN0X_NO_NULLPTR
	/// Indicates that r-value references are not available.
	#define LEAN0X_NO_RVALUE_REFERENCES
	/// Indicates that built-in static_assert is not available.
	#define LEAN0X_NO_STATIC_ASSERT
	/// Indicates that built-in alignment modifiers are not available.
	#define LEAN0X_NO_ALIGN
	/// Indicates that default / delete method specifiers not available.
	#define LEAN0X_NO_DELETE_METHODS
#endif

#ifndef LEAN0X_DISABLE

	// Enable Visual Studio 2010 C++0x features
	#if (_MSC_VER >= 1600) && defined(_MSC_EXTENSIONS)
		#undef LEAN0X_NO_NULLPTR
		#undef LEAN0X_NO_RVALUE_REFERENCES
		#undef LEAN0X_NO_STATIC_ASSERT
	#endif

#endif

// Fix missing nullptr
#if defined(LEAN0X_NO_NULLPTR) && !defined(nullptr)
	#define nullptr 0
#endif

// Emulate static_assert
#if defined(LEAN0X_NO_STATIC_ASSERT) && !defined(static_assert)

#ifndef DOXYGEN_SKIP_THIS
	namespace lean
	{
		struct static_assertion_error;

		namespace impl
		{
			// Accepts literal to circumvent the requirement of typename (which dependens on context)
			template <bool Triggered>
			struct emit_static_assertion_error;

			template <bool Assertion, class Error>
			struct trigger_static_assertion_error
			{
				static const bool triggered = false;
			};

			template <class Error>
			struct trigger_static_assertion_error<false, Error>
			{
				// Defines literal instead of type to circumvent the requirement of typename (which dependens on context)
				static const bool triggered = sizeof(Error) || true;
			};
		}
	}
#endif
	
	/// Static assertion triggering a compiler error on failure.
	#define LEAN_STATIC_ASSERT(expr) typedef \
		::lean::impl::emit_static_assertion_error< \
			::lean::impl::trigger_static_assertion_error<(expr), ::lean::static_assertion_error>::triggered \
		> LEAN_JOIN_VALUES(static_assertion_error_, __LINE__)

	/// Static assertion incorporating the given message in a compiler error on failure.
	#define LEAN_STATIC_ASSERT_MSG(expr, msg) typedef \
		::lean::impl::emit_static_assertion_error< \
			::lean::impl::trigger_static_assertion_error<(expr), ::lean::static_assertion_error>::triggered \
		> LEAN_JOIN_VALUES(static_assertion_error_, __LINE__)

	/// Static assertion incorporating either the given message or the given type name in a compiler error on failure.
	#define LEAN_STATIC_ASSERT_MSG_ALT(expr, msg, msgtype) struct static_assertion_error__##msgtype; typedef \
		::lean::impl::emit_static_assertion_error< \
			::lean::impl::trigger_static_assertion_error<(expr), static_assertion_error__##msgtype>::triggered \
		> LEAN_JOIN_VALUES(static_assertion_error_, __LINE__)

	// Emulate static_assert
	#define static_assert(expr, msg) LEAN_STATIC_ASSERT_MSG(expr, msg)

#else
	/// Static assertion triggering a compiler error on failure.
	#define LEAN_STATIC_ASSERT(expr) static_assert(expr, #expr)
	
	/// Static assertion incorporating the given message in a compiler error on failure.
	#define LEAN_STATIC_ASSERT_MSG(expr, msg) static_assert(expr, msg)
	
	/// Static assertion incorporating either the given message or the given incomplete type in a compiler error on failure.
	#define LEAN_STATIC_ASSERT_MSG_ALT(expr, msg, msgtype) static_assert(expr, msg)
#endif

// Automatically include utility for move semantics
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	#include <utility>
#endif

/// @}

#ifdef DOXYGEN_READ_THIS
	// Re-enable move semantics for documentation
	#undef LEAN0X_DISABLE LEAN0X_NO_RVALUE_REFERENCES
#endif

#endif