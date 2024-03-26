set(LIB_SOURCES CustomChannelView.cpp CustomTableView.cpp)
set(LIB_HEADERS Variance.h PVTStruct.h ChannelStruct.h CustomChannelView.h CustomTableView.h)
add_library(lib ${LIB_SOURCES} ${LIB_HEADERS})

set(QT5_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Charts
)

target_link_libraries(lib PUBLIC ${QT5_LIBRARIES})