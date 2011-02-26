/*****************************************************/
/* lean Macros                  (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_MACROS
#define LEAN_MACROS

#define LEAN_JOIN_TOKENS_IMPL(a, b) a##b
/// Appends token b to token a.
#define LEAN_JOIN_TOKENS(a, b) LEAN_JOIN_TOKENS_IMPL(a, b)

/// Quotes the given expression.
#define LEAN_QUOTE(expr) #expr
/// Quotes the given value (first evaluating macros).
#define LEAN_QUOTE_VALUE(value) LEAN_QUOTE(value)

#endif