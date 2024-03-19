aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} STYLES_SOURCE)
add_library(STYLES ${STYLES_SOURCE})

set(QT5_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
)
target_link_libraries(STYLES PUBLIC ${QT5_LIBRARIES})