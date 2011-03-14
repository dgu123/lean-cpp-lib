#pragma once

#include <string>
#include <lean/properties/property.h>

class test_property_driven
{
private:
	std::string m_name;
	int m_id;
	float m_weight;

public:
	test_property_driven(const std::string &name, int id)
		: m_name(name),
		m_id(id),
		m_weight(1.0f) { }

	void setName(const std::string &name) { m_name = name; }
	std::string getName() const { return m_name; }

	void setID(int id) { m_id = id; }
	int getID() const { return m_id; }

	void setWeight(float weight) { m_weight = weight; }
	float getWeight() const { return m_weight; }

	void setInts(const int *ints, size_t count) { }
	void getInts(int *ints, size_t count) const { }
};
