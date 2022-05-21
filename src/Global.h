#pragma once
#include <cryptopp/Integer.h>

namespace Global
{
    extern const CryptoPP::byte ClientHash[0x20];

    extern const CryptoPP::Integer p;
    extern const CryptoPP::Integer q;
    extern const CryptoPP::Integer g;

    extern const CryptoPP::byte KeyHeader[0x180];

    extern const CryptoPP::byte HeartbeatKey[0x40];
} // namespace Global
