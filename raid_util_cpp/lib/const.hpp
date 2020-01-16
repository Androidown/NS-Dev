#ifndef LIB_CONST_H
#define LIB_CONST_H

#include <vector>
#include <iostream>

#define BASE_SEED 0x82A2B175229D6A5B
#define U32 0xFFFFFFFF
#define u64 uint64_t

const std::vector<std::string> NATURES = {
    "Hardy", "Lonely", "Brave", "Adamant", "Naughty", "Bold", "Docile", "Relaxed",
    "Impish", "Lax", "Timid", "Hasty", "Serious", "Jolly", "Naive", "Modest",
    "Mild", "Quiet", "Bashful", "Rash", "Calm", "Gentle", "Sassy", "Careful", "Quirky"
};

typedef struct {
    int iv_cnt;
    bool allow_hidden;
    bool random_gender;
    u64 gender_ratio;
} PMInfo;

#endif  // LIB_CONST_H
