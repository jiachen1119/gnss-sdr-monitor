set(SOCKET_SOURCES SocketGnss.cpp SocketPVT.cpp)
set(SOCKET_HEADERS SocketGnss.h SocketPVT.h)
add_library(SOCKET ${SOCKET_SOURCES} ${SOCKET_HEADERS})

include_directories()
set(QT5_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Network
)

target_link_libraries(socket PUBLIC ${QT5_LIBRARIES})