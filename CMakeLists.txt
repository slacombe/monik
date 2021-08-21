cmake_minimum_required(VERSION 3.20)
project(monik)

set(CMAKE_CXX_STANDARD 11)


add_definitions(-DTRANSPOSITION -DNULL_MOVE -DUSE_SEE -DTRANSPOSITION -DICSTALK)

add_executable(monik monik.cpp attaque.cpp edit.cpp init.cpp quies.cpp test.cpp
        board.cpp engine.cpp iteration.cpp racine.cpp time.cpp
        book.cpp entree.cpp make.cpp repetition.cpp transposition.cpp
        chargeur.cpp eval.cpp monik.cpp search.cpp unmake.cpp
        chess.cpp genmove.cpp next.cpp sortie.cpp utile.cpp
        debug.cpp hung.cpp nextracine.cpp system.cpp valide.cpp
        controller/Controller.cpp controller/ControllerFactory.cpp
        logger/log.cpp
        protocol/NoGuiProtocol.cpp protocol/ProtocolFactory.cpp
        ui/UIController.cpp ui/UIControllerFactory.cpp)
