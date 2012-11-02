/*****************************************************/
/* lean Smart                   (c) Tobias Zirr 2011 */
/*****************************************************/

#pragma once
#ifndef LEAN_SMART_COMMON
#define LEAN_SMART_COMMON

#include "../lean.h"

namespace lean
{
namespace smart
{

#ifdef DOXYGEN_READ_THIS
/// Allows for the binding of existing COM object / resource references on smart pointer construction.
enum bind_reference_t
{
	bind_reference ///< Allows for the binding of existing COM object / resource references on smart pointer construction.
};
#else
/// Allows for the binding of existing COM object / resource references on smart pointer construction.
typedef consume_t bind_reference_t;
/// Allows for the binding of existing COM object / resource references on smart pointer construction.
const bind_reference_t bind_reference = consume;
#endif


} // namespace

using smart::bind_reference_t;
using smart::bind_reference;

} // namespace

#endif