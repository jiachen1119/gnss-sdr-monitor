# generate the header file needed and set auto skip generation on
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${CMAKE_SOURCE_DIR}/src/protobuf/gnss_synchro.proto)
set_property(SOURCE ${PROTO_SRCS} PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE ${PROTO_HDRS} PROPERTY SKIP_AUTOGEN ON)

protobuf_generate_cpp(PROTO_SRCS2 PROTO_HDRS2 ${CMAKE_SOURCE_DIR}/src/protobuf/monitor_pvt.proto)
set_property(SOURCE ${PROTO_SRCS2} PROPERTY SKIP_AUTOGEN ON)
set_property(SOURCE ${PROTO_HDRS2} PROPERTY SKIP_AUTOGEN ON)

set(SOCKET_SOURCES SocketGnss.cpp SocketPVT.cpp monitor_pvt_wrapper.cpp)
set(SOCKET_HEADERS SocketGnss.h SocketPVT.h ${PROTO_SRCS} ${PROTO_SRCS2} monitor_pvt_wrapper.h)

add_library(socket ${SOCKET_SOURCES} ${SOCKET_HEADERS})

set(QT5_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Network
        Qt5::Positioning
)

target_link_libraries(socket PUBLIC ${QT5_LIBRARIES} protobuf::libprotobuf)
target_include_directories(socket PUBLIC ${CMAKE_CURRENT_BINARY_DIR})