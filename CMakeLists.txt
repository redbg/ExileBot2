cmake_minimum_required(VERSION 3.5)

project(ExileBot2 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(Qt6 REQUIRED COMPONENTS WebEngineWidgets)
find_package(Qt6 REQUIRED COMPONENTS Qml)

add_executable(ExileBot2
    # main
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
    src/MainWindow.ui

    # Qt Resource
    res/Resource.qrc

    # Helper
    src/Helper.cpp
    src/Helper.h

    # Global
    src/Global.cpp
    src/Global.h

    # Account
    src/Account.cpp
    src/Account.h

    # AccountModel
    src/AccountModel.cpp
    src/AccountModel.h

    # ExileSocket
    src/ExileSocket.cpp
    src/ExileSocket.h

    # ExileClient
    src/ExileClient.cpp
    src/ExileClient.h
    
    # Character
    src/Character.h
    
)

target_link_libraries(ExileBot2 PRIVATE Qt6::Widgets)
target_link_libraries(ExileBot2 PRIVATE Qt6::Network)
target_link_libraries(ExileBot2 PRIVATE Qt6::WebEngineWidgets)
target_link_libraries(ExileBot2 PRIVATE Qt6::Qml)

# vcpkg
include("D:/vcpkg/scripts/buildsystems/vcpkg.cmake")

# CryptoPP
find_package(cryptopp CONFIG REQUIRED)
target_link_libraries(ExileBot2 PRIVATE cryptopp-static)

file(COPY scripts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY scripts DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug)
file(COPY scripts DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release)
