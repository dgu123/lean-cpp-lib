/*****************************************************/
/* lean Memory                  (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_MEMORY_ALIGNMENT
#define LEAN_MEMORY_ALIGNMENT

#include "../lean.h"

#ifdef LEAN0X_NO_ALIGN
	
	#ifdef _MSC_VER
		#ifndef alignas
			/// Emulated alignas using MSVC storage class specification.
			#define alignas(alignment) __declspec( align(alignment) )
		#endif
		#ifndef alignof
			/// Emulated alignof using MSVC-specific alignof operator extension.
			#define alignof(type) __alignof(type)
		#endif
	#else
		#error Unknown compiler, alignment specifiers unavailable.
	#endif

#endif

namespace lean
{
namespace memory
{
	/// Checks whether the given alignment is a valid power of two.
	template <size_t Alignment>
	struct check_alignment
	{
		/// Specifies whether the given alignment is a valid power of two.
		static const bool valid = false;
	};

	template <> struct check_alignment<1> { static const bool valid = true; };
	template <> struct check_alignment<2> { static const bool valid = true; };
	template <> struct check_alignment<4> { static const bool valid = true; };
	template <> struct check_alignment<8> { static const bool valid = true; };
	template <> struct check_alignment<16> { static const bool valid = true; };
	template <> struct check_alignment<32> { static const bool valid = true; };
	template <> struct check_alignment<64> { static const bool valid = true; };
	template <> struct check_alignment<128> { static const bool valid = true; };

	/// Aligns the given unsigned integer on the given alignment boundaries.
	template <size_t Alignment, class Integer>
	LEAN_INLINE Integer align_integer(Integer integer)
	{
		LEAN_STATIC_ASSERT_MSG_ALT(check_alignment<Alignment>::valid,
			"Alignment is required to be power of two.", Alignment_is_required_to_be_power_of_two);

		integer += (Alignment - 1);
		return integer & ~(Alignment - 1);
	}

	/// Aligns the given pointer on the given alignment boundaries.
	template <size_t Alignment, class Value>
	LEAN_INLINE Integer align(Value *pointer)
	{
		return reinterpret_cast<Value*>(
			align_integer<Alignment>( reinterpret_cast<uintptr_t>(pointer) ) );
	}

	/// Aligns the given unsigned integer on the given alignment boundaries, incrementing it at least by one.
	template <size_t Alignment, class Integer>
	LEAN_INLINE Integer upper_align_integer(Integer integer)
	{
		LEAN_STATIC_ASSERT_MSG_ALT(check_alignment<Alignment>::valid,
			"Alignment is required to be power of two.", Alignment_is_required_to_be_power_of_two);

		integer += Alignment;
		return integer & ~(Alignment - 1);
	}

	/// Aligns the given pointer on the given alignment boundaries, incrementing it at least by one.
	template <size_t Alignment, class Value>
	LEAN_INLINE Integer upper_align(Value *pointer)
	{
		return reinterpret_cast<Value*>(
			upper_align_integer<Alignment>( reinterpret_cast<uintptr_t>(pointer) ) );
	}

	/// Aligns derived classes according to the given alignment template argument when instances are created on the stack.
	template <size_t Alignment>
	struct stack_aligned
	{
		LEAN_STATIC_ASSERT_MSG_ALT(false, "Alignment is required to be power of two.", Alignment_is_required_to_be_power_of_two);
	};

	template <> alignas(1) struct stack_aligned<1> { };
	template <> alignas(2) struct stack_aligned<2> { };
	template <> alignas(4) struct stack_aligned<4> { };
	template <> alignas(8) struct stack_aligned<8> { };
	template <> alignas(16) struct stack_aligned<16> { };
	template <> alignas(32) struct stack_aligned<32> { };
	template <> alignas(64) struct stack_aligned<64> { };
	template <> alignas(128) struct stack_aligned<128> { };

} // namespace

using memory::check_alignment;
using memory::stack_aligned;

} // namespace

#endif