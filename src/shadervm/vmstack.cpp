#include "vmstack.h"

memory::TLPool VMStack::stackPool(&memory::PoolCreator<unsigned char, 256>::create);
