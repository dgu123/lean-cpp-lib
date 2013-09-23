#pragma once

#include <lean/smart/resource.h>

class test_resource : public lean::resource<>
{
private:
	int *m_destrCount;

public:
	test_resource(int *constrCount, int *destrCount)
		: m_destrCount(destrCount)
	{
		++*constrCount;
	}
	~test_resource()
	{
		++*m_destrCount;
	}

	void destroy()
	{
		delete this;
	}
};
