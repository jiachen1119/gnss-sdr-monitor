set(LIB_SOURCE PVTStruct.h Variance.h CustomChannelView.cpp CustomTableView.cpp)
add_library(LIB ${LIB_SOURCE})

set(QT5_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Charts
)
target_link_libraries(LIB PUBLIC ${QT5_LIBRARIES})