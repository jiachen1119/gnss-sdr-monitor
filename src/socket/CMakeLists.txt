aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOCKET_SOURCE)
add_library(SOCKET_LIB ${SOCKET_SOURCE})

set(QT5_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Network
)
target_link_libraries(SOCKET_LIB PUBLIC ${QT5_LIBRARIES})