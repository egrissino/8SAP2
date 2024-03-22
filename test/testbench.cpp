

#include <sim.hpp>
#include <Bus.hpp>
#include <Logic.hpp>
#include <Buffer.hpp>
#include <Latch.hpp>
#include <Clock.hpp>
#include <Counter.hpp>
#include <Decoder3to8.hpp>
#include <AT28C64.hpp>

extern int sim_test(int argc, char** argv);

int run_test(int argc, char** argv)
{
    //
    sim_test(argc, (char**)argv);
    return 0;
}