#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lc3instrbuilder.h"


uint16_t build_instruction(uint8_t , uint8_t, uint8_t, uint16_t);


uint16_t op_3_i(uint8_t i, uint8_t p, uint8_t q, int8_t r, int8_t logicflag) {
    if ((logicflag) && ((uint8_t) r > 0x001F)) {
        perror("Immediate operand overflow");
        exit(EXIT_FAILURE);
    }
    if ((q > 0x0007) || ((uint8_t) r > 0x003F)) {
        perror("Operand overflow");
        exit(EXIT_FAILURE);
    }
    return build_instruction(i,p,(q & 0x0007), r);
}

uint16_t op_3_r(uint8_t i, uint8_t p, uint8_t q, uint8_t r) {
    if (r > 0x0007) {
        perror("Register overflow");
        exit(EXIT_FAILURE);
    }
    return op_3_i(i,p,q, ((int8_t) r & 0x0007),0);
}

uint16_t op_2_i(uint8_t i, uint8_t p, int16_t q) {
    if (q > 0x01FF) {			// il cast a uint dava problemi con il branch
        perror("Operand overflow");
        exit(EXIT_FAILURE);
    }
    return build_instruction(i,p,0,(q & 0x01FF));

}


uint16_t op_2_r(uint8_t i, uint8_t p, uint8_t q) {
    return op_3_i(i,p,q, 0x003F,0);
}

uint16_t branch(uint8_t p, int16_t q) {
    if (!(p == 0x1) && !(p == 0x2) && !(p == 0x4)) {
        perror("Invalid branch instruction definition");
        exit(EXIT_FAILURE);
    }
    return op_2_i(0,p,q);
}

uint16_t op_1_i(uint8_t i, int16_t q) {
    if (i == 4 && q < 2047) return ((9 << 11) | q) & 0xFFFF; /*JSR*/
    if (i == 15 && q <= 255) return build_instruction(i,0,0,q); /*TRAP*/
    perror("1-operand immediate instruction build fail");
    exit(EXIT_FAILURE);
}

uint16_t op_1_r(uint8_t i, uint16_t q) {
    return op_3_i(i,0,q,0,0);
}

uint16_t build_instruction(uint8_t i, uint8_t p, uint8_t q, uint16_t r) {
    if ((i > 0x000F) || (p > 0x0007)) {
        perror("Operand overflow");
        exit(EXIT_FAILURE);
    }
    //return (((i << 12)  & 0xF000) | ((p << 9) & 0x0700)  | ((q << 6) & 0x01C0) | r) & 0xFFFF; // verificare se lo shift fa bit extend, quindi scegliere
    return (((i & 0x000F) << 12) | ((p & 0x0007) << 9)  | (q << 6) | r) & 0xFFFF;
}
