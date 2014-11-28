#include <boost/test/unit_test.hpp>
#include <underwater_simulator/Dummy.hpp>

using namespace underwater_simulator;

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    underwater_simulator::DummyClass dummy;
    dummy.welcome();
}
