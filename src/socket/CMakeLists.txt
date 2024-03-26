set(SOCKET_SOURCE SocketGnss.cpp SocketGnss.h)
add_library(SOCKET ${SOCKET_SOURCE})

include_directories()
set(QT5_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Network
)

target_link_libraries(socket PUBLIC ${QT5_LIBRARIES})