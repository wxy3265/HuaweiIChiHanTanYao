cmake_minimum_required(VERSION 3.22.0)
project(HuaweiIChiHanTanYao)

set(CMAKE_CXX_STANDARD 17)

add_executable(HuaweiIChiHanTanYao main.cpp
        tanyao.h
        Map.h
        Path.h
        Point.h
        State.h
        Goods.h
        Berth.h
        Ship.h
        values.h
        RobotTest.cpp
        Robot.cpp
        Robot.h
        Map.cpp
        values.cpp
        Berth.cpp
        Goods.cpp
        path.cpp
        Path.cpp
)
