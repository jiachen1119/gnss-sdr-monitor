aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PLOT_SOURCE)
add_library(PLOT ${PLOT_SOURCE})

set(QT5_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Charts
)
target_link_libraries(PLOT PUBLIC ${QT5_LIBRARIES})