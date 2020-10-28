#define PJON_INCLUDE_TSA true

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include <chrono>
#include <thread>

#include "spdlog/spdlog.h"

#include <boost/program_options.hpp>

// This needs to be the last thing you include for some fucked up reason...
#include <PJONThroughSerial.h>

using namespace std::string_literals;
using namespace std::chrono_literals;
namespace po = boost::program_options;

/// This needs to be in the same file beacause of the
/// *terrible* way PJON_LINUX_Interface is written
class serial_raii
{
private:
    int _fh = 0;
    serial_raii(int fh) { _fh = fh; }

public:
    //Disable default constructors. We only want there to be 1 instance only.
    serial_raii() = delete;
    serial_raii(const serial_raii &) = delete;

    /// Throws runtime_error on failure
    static serial_raii init(const char *file, int baud)
    {
        auto fh = serialOpen(file, baud);

        if (fh == -1)
        {
            spdlog::error("Couldnt open file {}", file);
            throw std::runtime_error("Couldn't open serial file");
        }

        return serial_raii(fh);
    }
    ~serial_raii()
    {
        spdlog::debug("Closing serial file handle: {}", _fh);
        close(_fh);
    }
    int get_file_handle() { return _fh; }
};

uint8_t id = 0;

const uint32_t BAUD = 1'000'000;
const std::string SERIAL_FILE = "/dev/ttyS1"; // todo:

void receiver_function(uint8_t *payload,
                       uint16_t length,
                       const PJON_Packet_Info &packet_info)
{
    /* Make use of the payload before sending something, the buffer where payload points to is
     overwritten when a new message is dispatched */
    std::stringstream ss;
    ss << "payload :" << std::hex;
    for (auto i = 0; i < length; i++)
    {
        ss << payload[i];
    }
    ss << " packet info: ";
    ss << packet_info.header;

    spdlog::info(ss.str());
};

int main(int argc, char *argv[])
{
    spdlog::info("Command line arguments:");
    for (auto i = 0; i < argc; i++)
    {
        spdlog::info("\t{}", argv[i]);
    }

    po::options_description desc("Command line arguments");

    // We can't use uint8 for id because it gets treated as a char
    // Check and cast later on...
    desc.add_options()("id", po::value<int>()->required(), "Bus ID to use for PJON")("ll", po::value<std::string>(),
                                                                                     "Log levels: trace, debug, info, warning, error, critical, off");

    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        spdlog::debug("notify");
    }
    catch (std::exception &e)
    {
        spdlog::error(e.what());
        return -1;
    }

    int id_ = vm["id"].as<int>();
    if (id_ < 0)
    {
        spdlog::error("id {} is negative!");
        throw std::runtime_error("Bad id");
    }
    if (id_ > 255)
    {
        spdlog::error("id {} is too large!");
        throw std::runtime_error("Bad id");
    }

    id = (uint8_t)id_;

    spdlog::info("Setting bus id: {}", id);

    PJONThroughSerial bus(id);
    bus.set_communication_mode(PJON_HALF_DUPLEX);

    spdlog::debug("Bus id: {}", bus.device_id()); // Get device id

    std::string ll = vm["ll"].empty() ? "trace"s : vm["ll"].as<std::string>();

    spdlog::info("Log level: {}", ll);

    try
    {
        spdlog::set_level(spdlog::level::from_str(ll));
    }
    catch (std::exception &e)
    {
        spdlog::error("Set incorrect log level: '{}', defaulting to 'trace'", ll);
    }

    spdlog::trace("Setting reciever function");

    bus.set_receiver(receiver_function);

    spdlog::info("Opening serial");
    try
    {
        serial_raii serial = serial_raii::init(SERIAL_FILE.c_str(), BAUD);

        spdlog::trace("Setting serial");
        bus.strategy.set_serial((uint16_t)serial.get_file_handle());
        bus.strategy.set_baud_rate(BAUD);

        spdlog::info("Opening bus");
        bus.begin();

        while (true)
        {
            spdlog::trace("Sending \"hello\"");
            bus.send(45, "hello", 5);

            spdlog::trace("bus.update()");
            bus.update();

            spdlog::trace("bus.receive()");

            switch (bus.receive(1000))
            {
            case PJON_BUSY:
                spdlog::warn("PJON_BUSY");
                break;
            case PJON_ACK:
                spdlog::info("PJON_ACK");
                break;
            case PJON_NAK:
                spdlog::warn("PJON_NAK");
                break;
            case PJON_FAIL:
                spdlog::warn("PJON_FAIL");
                break;
            default:
                break;
            }

            std::this_thread::sleep_for(1s);
        }

        return 0;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what();
        return -1;
    }
}