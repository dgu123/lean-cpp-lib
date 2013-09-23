// benchmark.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void hash_map_benchmark();
void com_ptr_benchmark();
void vector_benchmark();

int main()
{
	hash_map_benchmark();
	vector_benchmark();
	com_ptr_benchmark();

	return 0;
}

