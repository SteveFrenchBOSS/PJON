// For printf used below
#include <stdio.h>
// PJON library
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "PJONThroughSerial.h"
#include <cstddef> //std::byte

using namespace std::string_literals;

const uint8_t ID = 45;

const uint32_t BAUD = 115200;
const std::string serial_file = "/dev/ttySO1"; // todo:

PJONThroughSerial bus(ID);

namespace BOSS
{
    class PJONSerial
    {
    public:
        static PJONSerial Init(std::string &file, uint8_t baud)
        {
            PJONSerial ser;

            auto fh = serialOpen(file.c_str(), baud);

            if (fh == -1)
            {
                throw std::runtime_error(("Couldn't open serial file "s + file).c_str());
            }

            ser.fh = fh;

            return ser;
        }

        int dataAvail()
        {
            return serialDataAvailable(fh);
        }

        char getChar()
        {
            return serialGetCharacter(fh);
        }

        std::byte getByte()
        {
            std::byte b{(uint8_t)serialGetCharacter(fh)};
            return b;
        }

        std::string getString()
        {
            auto avail = dataAvail();
            std::string ret(avail, '\0');
            for (auto i = 0; i < avail; i++)
            {
                ret[i] = getChar();
            }
            return ret;
        }

        ~PJONSerial()
        {
            close(fh);
        }

    private:
        int fh = 0;
    };
} // namespace BOSS

int main()
{
}