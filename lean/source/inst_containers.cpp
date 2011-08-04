#include "lean_internal/stdafx.h"

#define LEAN_CONTAINERS_TEST_INSTANTIATION
#include "lean/containers/containers.h"

#include <vector>
#include <map>

namespace lean
{
namespace containers
{
	struct test_value
	{
		int i;

		void move(const test_value&r) { i = r.i; }
	};
	bool operator ==(const test_value &a, const test_value &b) { return a.i == b.i; }
	bool operator <(const test_value &a, const test_value &b) { return a.i < b.i; }

	// Simple vector
	template class simple_vector<int, simple_vector_policies::pod>;
	template class simple_vector<int, simple_vector_policies::inipod>;
	template class simple_vector<int, simple_vector_policies::semipod>;
	template class simple_vector<int, simple_vector_policies::nonpod>;

	template class simple_vector<test_value, simple_vector_policies::pod>;
	template class simple_vector<test_value, simple_vector_policies::inipod>;
	template class simple_vector<test_value, simple_vector_policies::semipod>;
	template class simple_vector<test_value, simple_vector_policies::nonpod>;

	// Simple hash map
	template class simple_hash_map<int, int, simple_hash_map_policies::nonpod>;
	template class simple_hash_map<int, int, simple_hash_map_policies::semipodkey>;
	template class simple_hash_map<int, int, simple_hash_map_policies::semipod>;
	template class simple_hash_map<int, int, simple_hash_map_policies::podkey>;
	template class simple_hash_map<int, int, simple_hash_map_policies::podkey_semipod>;
	template class simple_hash_map<int, int, simple_hash_map_policies::pod>;

	template class simple_hash_map<int, test_value, simple_hash_map_policies::nonpod>;
	template class simple_hash_map<int, test_value, simple_hash_map_policies::semipodkey>;
	template class simple_hash_map<int, test_value, simple_hash_map_policies::semipod>;
	template class simple_hash_map<int, test_value, simple_hash_map_policies::podkey>;
	template class simple_hash_map<int, test_value, simple_hash_map_policies::podkey_semipod>;
	template class simple_hash_map<int, test_value, simple_hash_map_policies::pod>;

	// Accumulation vector
	template class accumulation_vector< std::vector<test_value>, default_reallocation_policy< std::vector<test_value> > >;
	template class accumulation_vector< std::vector<test_value>, move_reallocation_policy< std::vector<test_value> > >;

	// Not fully supported
//	template class accumulation_vector< simple_vector<test_value>, default_reallocation_policy< simple_vector<test_value> > >;
//	template class accumulation_vector< simple_vector<test_value>, move_reallocation_policy< simple_vector<test_value> > >;

	// Accumulation map
	template class accumulation_map< std::map<int, test_value> >;
	template class accumulation_map< std::map<int, test_value> >;

	// Any
	template class any_value<int>;
	template class any_value<test_value>;

	template test_value* any_cast(any*);
	template const test_value* any_cast(const any*);
	template volatile test_value* any_cast(volatile any*);
	template const volatile test_value* any_cast(const volatile any*);

	template test_value& any_cast(any&);
	template const test_value& any_cast(const any&);
	template volatile test_value& any_cast(volatile any&);
	template const volatile test_value& any_cast(const volatile any&);

} // namespace
} // namespace