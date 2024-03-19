aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PVT_SOURCE)
add_library(PVT_LIB ${PVT_SOURCE})

set(PROTOBUF_SOURCE ${CMAKE_BINARY_DIR}/src)
include_directories(${PROTOBUF_SOURCE})

set(QT5_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Charts
)
target_link_libraries(PVT_LIB PUBLIC ${QT5_LIBRARIES})