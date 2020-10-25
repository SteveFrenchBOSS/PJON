#pragma once

// PJON library
#include <inttypes.h>
#include <stdlib.h>
#include <string>
#include <cstddef>

namespace BOSS
{
    class PJONSerial
    {
    public:
        static PJONSerial Init(const std::string &file, int baud);

        int dataAvail();

        char getChar();

        std::byte getByte();

        std::string getString();

        ~PJONSerial();

    private:
        int fh = 0;
    };
} // namespace BOSS
