cmake_minimum_required (VERSION 3.8)
project(HuaweiIChiHanTanYao)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

if (!WIN32)
    link_libraries(pthread rt m)
endif (!WIN32)

set(CMAKE_EXE_LINKER_FLAGS "-static")

AUX_SOURCE_DIRECTORY(. src)
ADD_EXECUTABLE(main ${src}
        Path.cpp
        mission.cpp)

#add_executable(main main.cpp
#        tanyao.h
#        Map.h
#        Path.h
#        Point.h
#        State.h
#        Goods.h
#        Berth.h cre
#        Ship.h
#        values.h
##        RobotTest.cpp
#        Robot.cpp
#        Robot.h
#        Map.cpp
#        values.cpp
#        Berth.cpp
#        tanyao.cpp
#        Ship.cpp
# )
