/*****************************************************/
/* lean Functional              (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_FUNCTIONAL_VARIADIC
#define LEAN_FUNCTIONAL_VARIADIC

/// @addtogroup GlobalMacros
/// @{

/// Nothing.
#define LEAN_NOTHING

/// 'False' has-*-parameter-argument.
#define LEAN_VARIADIC_NO_PARAMS
/// 'True' has-*-parameter-argument.
#define LEAN_VARIADIC_HAS_PARAMS ,

#define LEAN_VARIADIC_TEMPLATE__LEAN_VARIADIC_NO_PARAMS()
#define LEAN_VARIADIC_TEMPLATE__LEAN_VARIADIC_HAS_PARAMS(tparams) template <tparams>

/// Defines a variadic perfect-forwarding function, optionally including additional template & function parameters.
#define LEAN_VARIADIC_PERFECT_FORWARDING_XXL(fun, tparams, has_tparams, params, has_params, modifiers, body, args) \
	/* 0 */ \
	LEAN_VARIADIC_TEMPLATE__##has_tparams(tparams)	\
	fun(params) modifiers							\
	body((args))									\
	/* 1 */ \
	template <tparams has_tparams					\
			class __lean__t1						\
		>											\
	fun( params has_params							\
			__lean__t1 LEAN_FW_REF __lean__p1		\
		) modifiers									\
	body(( args has_params							\
			LEAN_FORWARD(__lean__t1, __lean__p1)	\
		))											\
	/* 2 */ \
	template <tparams has_tparams					\
			class __lean__t1,						\
			class __lean__t2						\
		>											\
	fun( params has_params							\
			__lean__t1 LEAN_FW_REF __lean__p1,		\
			__lean__t2 LEAN_FW_REF __lean__p2		\
		) modifiers									\
	body(( args has_params							\
			LEAN_FORWARD(__lean__t1, __lean__p1),	\
			LEAN_FORWARD(__lean__t2, __lean__p2)	\
		))											\
	/* 3 */ \
	template <tparams has_tparams					\
			class __lean__t1,						\
			class __lean__t2,						\
			class __lean__t3						\
		>											\
	fun( params has_params							\
			__lean__t1 LEAN_FW_REF __lean__p1,		\
			__lean__t2 LEAN_FW_REF __lean__p2,		\
			__lean__t3 LEAN_FW_REF __lean__p3		\
		) modifiers									\
	body(( args has_params							\
			LEAN_FORWARD(__lean__t1, __lean__p1),	\
			LEAN_FORWARD(__lean__t2, __lean__p2),	\
			LEAN_FORWARD(__lean__t3, __lean__p3)	\
		))											\
	/* 4 */ \
	template <tparams has_tparams					\
			class __lean__t1,						\
			class __lean__t2,						\
			class __lean__t3,						\
			class __lean__t4						\
		>											\
	fun( params has_params							\
			__lean__t1 LEAN_FW_REF __lean__p1,		\
			__lean__t2 LEAN_FW_REF __lean__p2,		\
			__lean__t3 LEAN_FW_REF __lean__p3,		\
			__lean__t4 LEAN_FW_REF __lean__p4		\
		) modifiers									\
	body(( args has_params							\
			LEAN_FORWARD(__lean__t1, __lean__p1),	\
			LEAN_FORWARD(__lean__t2, __lean__p2),	\
			LEAN_FORWARD(__lean__t3, __lean__p3),	\
			LEAN_FORWARD(__lean__t4, __lean__p4)	\
		))											\
	/* 5 */ \
	template <tparams has_tparams					\
			class __lean__t1,						\
			class __lean__t2,						\
			class __lean__t3,						\
			class __lean__t4,						\
			class __lean__t5						\
		>											\
	fun( params has_params							\
			__lean__t1 LEAN_FW_REF __lean__p1,		\
			__lean__t2 LEAN_FW_REF __lean__p2,		\
			__lean__t3 LEAN_FW_REF __lean__p3,		\
			__lean__t4 LEAN_FW_REF __lean__p4,		\
			__lean__t5 LEAN_FW_REF __lean__p5		\
		) modifiers									\
	body(( args has_params							\
			LEAN_FORWARD(__lean__t1, __lean__p1),	\
			LEAN_FORWARD(__lean__t2, __lean__p2),	\
			LEAN_FORWARD(__lean__t3, __lean__p3),	\
			LEAN_FORWARD(__lean__t4, __lean__p4),	\
			LEAN_FORWARD(__lean__t5, __lean__p5)	\
		))											\
	/* 6 */ \
	template <tparams has_tparams					\
			class __lean__t1,						\
			class __lean__t2,						\
			class __lean__t3,						\
			class __lean__t4,						\
			class __lean__t5,						\
			class __lean__t6						\
		>											\
	fun( params has_params							\
			__lean__t1 LEAN_FW_REF __lean__p1,		\
			__lean__t2 LEAN_FW_REF __lean__p2,		\
			__lean__t3 LEAN_FW_REF __lean__p3,		\
			__lean__t4 LEAN_FW_REF __lean__p4,		\
			__lean__t5 LEAN_FW_REF __lean__p5,		\
			__lean__t6 LEAN_FW_REF __lean__p6		\
		) modifiers									\
	body(( args has_params							\
			LEAN_FORWARD(__lean__t1, __lean__p1),	\
			LEAN_FORWARD(__lean__t2, __lean__p2),	\
			LEAN_FORWARD(__lean__t3, __lean__p3),	\
			LEAN_FORWARD(__lean__t4, __lean__p4),	\
			LEAN_FORWARD(__lean__t5, __lean__p5),	\
			LEAN_FORWARD(__lean__t6, __lean__p6)	\
		))											\
	/* 7 */ \
	template <tparams has_tparams					\
			class __lean__t1,						\
			class __lean__t2,						\
			class __lean__t3,						\
			class __lean__t4,						\
			class __lean__t5,						\
			class __lean__t6,						\
			class __lean__t7						\
		>											\
	fun( params has_params							\
			__lean__t1 LEAN_FW_REF __lean__p1,		\
			__lean__t2 LEAN_FW_REF __lean__p2,		\
			__lean__t3 LEAN_FW_REF __lean__p3,		\
			__lean__t4 LEAN_FW_REF __lean__p4,		\
			__lean__t5 LEAN_FW_REF __lean__p5,		\
			__lean__t6 LEAN_FW_REF __lean__p6,		\
			__lean__t7 LEAN_FW_REF __lean__p7		\
		) modifiers									\
	body(( args has_params							\
			LEAN_FORWARD(__lean__t1, __lean__p1),	\
			LEAN_FORWARD(__lean__t2, __lean__p2),	\
			LEAN_FORWARD(__lean__t3, __lean__p3),	\
			LEAN_FORWARD(__lean__t4, __lean__p4),	\
			LEAN_FORWARD(__lean__t5, __lean__p5),	\
			LEAN_FORWARD(__lean__t6, __lean__p6),	\
			LEAN_FORWARD(__lean__t7, __lean__p7)	\
		))											\
	/* 8 */ \
	template <tparams has_tparams					\
			class __lean__t1,						\
			class __lean__t2,						\
			class __lean__t3,						\
			class __lean__t4,						\
			class __lean__t5,						\
			class __lean__t6,						\
			class __lean__t7,						\
			class __lean__t8						\
		>											\
	fun( params has_params							\
			__lean__t1 LEAN_FW_REF __lean__p1,		\
			__lean__t2 LEAN_FW_REF __lean__p2,		\
			__lean__t3 LEAN_FW_REF __lean__p3,		\
			__lean__t4 LEAN_FW_REF __lean__p4,		\
			__lean__t5 LEAN_FW_REF __lean__p5,		\
			__lean__t6 LEAN_FW_REF __lean__p6,		\
			__lean__t7 LEAN_FW_REF __lean__p7,		\
			__lean__t8 LEAN_FW_REF __lean__p8		\
		) modifiers									\
	body(( args has_params							\
			LEAN_FORWARD(__lean__t1, __lean__p1),	\
			LEAN_FORWARD(__lean__t2, __lean__p2),	\
			LEAN_FORWARD(__lean__t3, __lean__p3),	\
			LEAN_FORWARD(__lean__t4, __lean__p4),	\
			LEAN_FORWARD(__lean__t5, __lean__p5),	\
			LEAN_FORWARD(__lean__t6, __lean__p6),	\
			LEAN_FORWARD(__lean__t7, __lean__p7),	\
			LEAN_FORWARD(__lean__t8, __lean__p8)	\
		))											\
	/* 9 */ \
	template <tparams has_tparams					\
			class __lean__t1,						\
			class __lean__t2,						\
			class __lean__t3,						\
			class __lean__t4,						\
			class __lean__t5,						\
			class __lean__t6,						\
			class __lean__t7,						\
			class __lean__t8,						\
			class __lean__t9						\
		>											\
	fun( params has_params							\
			__lean__t1 LEAN_FW_REF __lean__p1,		\
			__lean__t2 LEAN_FW_REF __lean__p2,		\
			__lean__t3 LEAN_FW_REF __lean__p3,		\
			__lean__t4 LEAN_FW_REF __lean__p4,		\
			__lean__t5 LEAN_FW_REF __lean__p5,		\
			__lean__t6 LEAN_FW_REF __lean__p6,		\
			__lean__t7 LEAN_FW_REF __lean__p7,		\
			__lean__t8 LEAN_FW_REF __lean__p8,		\
			__lean__t9 LEAN_FW_REF __lean__p9		\
		) modifiers									\
	body(( args has_params							\
			LEAN_FORWARD(__lean__t1, __lean__p1),	\
			LEAN_FORWARD(__lean__t2, __lean__p2),	\
			LEAN_FORWARD(__lean__t3, __lean__p3),	\
			LEAN_FORWARD(__lean__t4, __lean__p4),	\
			LEAN_FORWARD(__lean__t5, __lean__p5),	\
			LEAN_FORWARD(__lean__t6, __lean__p6),	\
			LEAN_FORWARD(__lean__t7, __lean__p7),	\
			LEAN_FORWARD(__lean__t8, __lean__p8),	\
			LEAN_FORWARD(__lean__t9, __lean__p9)	\
		))											\
	// End

/// Defines variadic perfect-forwarding function including additional template & function parameters.
#define LEAN_VARIADIC_PERFECT_FORWARDING_TP(fun, tparams, params, modifiers, body, args) \
	LEAN_VARIADIC_PERFECT_FORWARDING_XXL(fun, tparams, LEAN_VARIADIC_HAS_PARAMS, params, LEAN_VARIADIC_HAS_PARAMS, modifiers, body, args)

/// Defines variadic perfect-forwarding function including additional template parameters.
#define LEAN_VARIADIC_PERFECT_FORWARDING_T(fun, tparams, modifiers, body) \
	LEAN_VARIADIC_PERFECT_FORWARDING_XXL(fun, tparams, LEAN_VARIADIC_HAS_PARAMS, LEAN_NOTHING, LEAN_VARIADIC_NO_PARAMS, modifiers, body, LEAN_NOTHING)

/// Defines variadic perfect-forwarding function including additional function parameters.
#define LEAN_VARIADIC_PERFECT_FORWARDING_P(fun, params, modifiers, body, args) \
	LEAN_VARIADIC_PERFECT_FORWARDING_XXL(fun, LEAN_NOTHING, LEAN_VARIADIC_NO_PARAMS, params, LEAN_VARIADIC_HAS_PARAMS, modifiers, body, args)

/// Defines variadic perfect-forwarding function.
#define LEAN_VARIADIC_PERFECT_FORWARDING(fun, modifiers, body) \
	LEAN_VARIADIC_PERFECT_FORWARDING_XXL(fun, LEAN_NOTHING, LEAN_VARIADIC_NO_PARAMS, LEAN_NOTHING, LEAN_VARIADIC_NO_PARAMS, modifiers, body, LEAN_NOTHING)

/// @}

#endif