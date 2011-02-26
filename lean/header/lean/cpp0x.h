/*****************************************************/
/* lean C++0x Config            (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_CPP0X
#define LEAN_CPP0X

// Disable all C++0x features by default when working with older C++ standards
#if (201100L > __cplusplus)
	#define LEAN0X_NO_NULLPTR
	#define LEAN0X_NO_RVALUE_REFERENCES
	#define LEAN0X_NO_STATIC_ASSERT
	#define LEAN0X_NO_DECLTYPE
	#define LEAN0X_NO_AUTO
	#define LEAN0X_NO_ALIGNMENT
	#define LEAN0X_NO_DELETE_METHODS;
#endif

// Allows for pre-0x testing
#ifndef LEAN0X_DISABLE

// Enable Visual Studio 2010 C++0x features
#if (_MSC_VER >= 1600) && defined(_MSC_EXTENSIONS)
	#undef LEAN0X_NO_NULLPTR
	#undef LEAN0X_NO_RVALUE_REFERENCES
	#undef LEAN0X_NO_STATIC_ASSERT
	#undef LEAN0X_NO_DECLTYPE
	#undef LEAN0X_NO_AUTO
#endif

// Fix missing nullptr
#if defined(LEAN0X_NO_NULLPTR) && !defined(nullptr)
	#define nullptr 0
#endif

// Emulate static_assert
#if defined(LEAN0X_NO_STATIC_ASSERT) && !defined(static_assert)

	namespace lean
	{
		struct static_assertion_error;

		namespace impl
		{
			template <bool Trigger, class Error>
			struct trigger_static_assertion_error
			{
				typedef Error error_type;
			};

			template <class Error>
			struct trigger_static_assertion_error<false, Error>
			{
				typedef Error error_type;
				typedef char error[sizeof(error_type)];
			};
		}
		
	}

	/// Emulate static_assert
	#define static_assert(expr, msg) typedef \
		::lean::impl::trigger_static_assertion_error<(expr), static_assertion_error>::error_type \
		LEAN_TOKEN_CONCAT(static_assertion_error_, __LINE__);

	/// Static assertion incorporating the given message TYPE in a compiler error on failure.
	#define LEAN_STATIC_ASSERT(expr, msg, msgtype) typedef \
		::lean::impl::trigger_static_assertion_error<(expr), msgtype>::error_type \
		LEAN_TOKEN_CONCAT(static_assertion_error_, __LINE__);

#else
	/// Static assertion incorporating the given message in a compiler error on failure.
	#define LEAN_STATIC_ASSERT(expr, msg, msgtype) static_assert(expr, msg)
#endif

// Automatically include utility for move semantics
#ifndef LEAN0X_NO_RVALUE_REFERENCES
	#include <utility>
#endif

#endif

#define CPP0X_MOVE_SEMANTICS
#define CPP0X_DELETE_METHODS
#define CPP0X_STATIC_ASSERT

#endif