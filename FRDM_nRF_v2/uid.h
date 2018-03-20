// -*- C++ -*-
// FRDL-KL25z unique device ID and hashing functions

#if !defined(UID_H)
#define UID_H

// FNV-32-1a hashing algorithm
constexpr
uint32_t hash_fnv32(uint32_t const* Beg, uint32_t const* End);

constexpr
uint16_t hash16(uint32_t x)
{
   return (x & 0xFFFF) ^ (x >> 16);
}

// 80-bit UID
uint32_t const UniqueID80[3] = { SIM->UIDMH, SIM->UIDML, SIM->UIDL };

// a 16-bit hash of the 80-bit UID
uint16_t const UniqueID16 = hash16(hash_fnv32(&UniqueID80[0], &UniqueID80[3]));

uint32_t const UniqueID32 = hash_fnv32(&UniqueID80[0], &UniqueID80[3]);

#include "uid.tcc"

#endif
