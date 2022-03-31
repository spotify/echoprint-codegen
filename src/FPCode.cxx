//
//  echoprint-codegen
//  Copyright 2011 The Echo Nest Corporation. All rights reserved.
//


#include "FPCode.h"

FPCode::FPCode()
    : frame(0), code(0) {}

FPCode::FPCode(FPCode const & copy)
    : frame( copy.frame), code( copy.code) {}

FPCode::FPCode(uint32_t f, uint32_t c)
    : frame(f), code(c) {}

