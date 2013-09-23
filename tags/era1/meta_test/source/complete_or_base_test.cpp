#include "stdafx.h"
#include <lean/meta/conditional.h>

BOOST_AUTO_TEST_SUITE( complete_or_base )

namespace Types
{
	enum T
	{
		Complete,
		Incomplete
	};
}

struct Base
{
	static const Types::T value = Types::Incomplete;
};
struct Complete : public Base
{
	static const Types::T value = Types::Complete;
};
struct Incomplete;

BOOST_AUTO_TEST_CASE( complete )
{
	BOOST_CHECK_EQUAL( (lean::complete_type_or_base<Complete, Base>::type::value), Types::Complete );
	BOOST_CHECK( (lean::complete_type_or_base<Complete, Base>::is_complete) );
}

BOOST_AUTO_TEST_CASE( incomplete )
{
	BOOST_CHECK_EQUAL( (lean::complete_type_or_base<Incomplete, Base>::type::value), Types::Incomplete );
	BOOST_CHECK( !(lean::complete_type_or_base<Incomplete, Base>::is_complete) );
}

BOOST_AUTO_TEST_SUITE_END()
