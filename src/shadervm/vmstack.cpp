#include "vmstack.h"


struct StackPoolCreator
{
	static memory::AlignedPool* create()
	{
		return new memory::AlignedPool(sizeof(unsigned char) * 256);
	}
};


VMStack::StackPoolImpl	VMStack::stackPool(&StackPoolCreator::create);
