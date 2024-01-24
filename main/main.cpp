#include "system_.hpp"

extern "C" void app_main(void)
{
    __attribute__((unused)) auto sys = System::getInstance(); // Create the system singleton object...

    // Upon return, main_task stack memory will be reclaimed.
}
