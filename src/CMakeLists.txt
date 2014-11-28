rock_library(underwater_simulator
    SOURCES Dummy.cpp
    HEADERS Dummy.hpp)

rock_executable(underwater_simulator_bin Main.cpp
    DEPS underwater_simulator)

