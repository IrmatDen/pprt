#include "vmstack.h"

memory::UCharPool VMStack::stackPool(&memory::UCharPoolCreator<256>::create);
