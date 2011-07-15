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

			namespace lean
			{
				namespace memory
				{
					namespace impl
					{
						/// Workaround for Visual Studio bug: Alignment of some template classes not evaluated until size has been queried
						template <size_t Size, size_t Alignment>
						struct alignof_fix
						{
							LEAN_STATIC_ASSERT(Size != 0U && Alignment != 0U);
							static const size_t alignment = Alignment;
						};
					}
				}
			}

			/// Emulated alignof using MSVC-specific alignof operator extension.
			#define alignof(type) ::lean::memory::impl::alignof_fix<sizeof(type), __alignof(type)>::alignment
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

#ifndef DOXYGEN_SKIP_THIS

	template <> struct check_alignment<1> { static const bool valid = true; };
	template <> struct check_alignment<2> { static const bool valid = true; };
	template <> struct check_alignment<4> { static const bool valid = true; };
	template <> struct check_alignment<8> { static const bool valid = true; };
	template <> struct check_alignment<16> { static const bool valid = true; };
	template <> struct check_alignment<32> { static const bool valid = true; };
	template <> struct check_alignment<64> { static const bool valid = true; };
	template <> struct check_alignment<128> { static const bool valid = true; };

#endif

	/// (Negatively) aligns the given unsigned integer on the given alignment boundaries.
	template <size_t Alignment, class Integer>
	LEAN_INLINE Integer nalign_integer(Integer integer)
	{
		LEAN_STATIC_ASSERT_MSG_ALT(check_alignment<Alignment>::valid,
			"Alignment is required to be power of two.", Alignment_is_required_to_be_power_of_two);

		// Widen BEFORE complement, otherwise higher-order bits might be lost
		return integer & ~static_cast<typename int_type<sign_class::no_sign, sizeof(Integer)>::type>(Alignment - 1);
	}

	/// (Negatively) aligns the given pointer on the given alignment boundaries.
	template <size_t Alignment, class Value>
	LEAN_INLINE Value* nalign(Value *pointer)
	{
		return reinterpret_cast<Value*>(
			nalign_integer<Alignment>( reinterpret_cast<uintptr_t>(pointer) ) );
	}

	/// Aligns the given unsigned integer on the given alignment boundaries.
	template <size_t Alignment, class Integer>
	LEAN_INLINE Integer align_integer(Integer integer)
	{
		LEAN_STATIC_ASSERT_MSG_ALT(check_alignment<Alignment>::valid,
			"Alignment is required to be power of two.", Alignment_is_required_to_be_power_of_two);

		integer += (Alignment - 1);
		// Widen BEFORE complement, otherwise higher-order bits might be lost
		return integer & ~static_cast<typename int_type<sign_class::no_sign, sizeof(Integer)>::type>(Alignment - 1);
	}

	/// Aligns the given pointer on the given alignment boundaries.
	template <size_t Alignment, class Value>
	LEAN_INLINE Value* align(Value *pointer)
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
		// Widen BEFORE complement, otherwise higher-order bits might be lost
		return integer & ~static_cast<typename int_type<sign_class::no_sign, sizeof(Integer)>::type>(Alignment - 1);
	}

	/// Aligns the given pointer on the given alignment boundaries, incrementing it at least by one.
	template <size_t Alignment, class Value>
	LEAN_INLINE Value* upper_align(Value *pointer)
	{
		return reinterpret_cast<Value*>(
			upper_align_integer<Alignment>( reinterpret_cast<uintptr_t>(pointer) ) );
	}

	/// Aligns derived classes according to the given alignment template argument when instances are created on the stack.
	/** @remarks MSC adds padding to make the size of aligned structures a multiple of their alignment, make sure to specify
	  * this base class first to allow for empty base class optimization. */
	template <size_t Alignment>
	struct stack_aligned
	{
		// Always checked, therefore use static_assert with care
		LEAN_STATIC_ASSERT_MSG_ALT(Alignment & ~Alignment, // = false, dependent
			"Alignment is required to be power of two.",
			Alignment_is_required_to_be_power_of_two);
	};

#ifndef DOXYGEN_SKIP_THIS

#ifdef _MSC_VER
	// MSC adds padding to make the size of aligned structures a multiple of their alignment...
	#pragma warning(push)
	#pragma warning(disable : 4324)
#endif

	template <> struct alignas(1) stack_aligned<1> { };
	template <> struct alignas(2) stack_aligned<2> { };
	template <> struct alignas(4) stack_aligned<4> { };
	template <> struct alignas(8) stack_aligned<8> { };
	template <> struct alignas(16) stack_aligned<16> { };
	template <> struct alignas(32) stack_aligned<32> { };
	template <> struct alignas(64) stack_aligned<64> { };
	template <> struct alignas(128) stack_aligned<128> { };

#ifdef _MSC_VER
	#pragma warning(pop)
#endif

#endif

} // namespace

using memory::check_alignment;
using memory::stack_aligned;

} // namespace

#endif