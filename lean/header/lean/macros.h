/*****************************************************/
/* lean Macros                  (c) Tobias Zirr 2011 */
/*****************************************************/

#ifndef LEAN_MACROS
#define LEAN_MACROS

/// Appends token b to token a.
#define LEAN_JOIN(a, b) a##b
/// Appends token b to token a, evaluating macros first.
#define LEAN_JOIN_VALUES(a, b) LEAN_JOIN(a, b)

/// Quotes the given expression.
#define LEAN_QUOTE(expr) #expr
/// Quotes the given value, evaluating macros first.
#define LEAN_QUOTE_VALUE(value) LEAN_QUOTE(value)

#endif