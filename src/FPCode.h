//
//  echoprint-codegen
//  Copyright 2011 The Echo Nest Corporation. All rights reserved.
//

#ifndef FPCODE_H
#define FPCODE_H

#include <stdint.h>

struct FPCode {
    FPCode();
    FPCode(FPCode const & copy);
    FPCode(uint32_t f, uint32_t c);
    uint32_t frame;
    uint32_t code;
};

#endif /* FPCODE_H */
