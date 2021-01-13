#include <stdlib/Concepts.h>

struct Con {
	bool& operator==(const Con&) const
	{
		bool aa;
		return aa;
	}
	bool& operator!=(const Con&) const
	{
		bool aa;
		return aa;
	}
}

void func(IslvalueReference auto t)
{
}
int main2(int argc, char** argv)
{
	return 0;
}
