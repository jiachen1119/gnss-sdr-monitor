set(PVT_SOURCES PVTTableModel.cpp CustomTableView.cpp DopWidget.cpp)
set(PVT_HEADERS PVTTableModel.h CustomTableView.h DopWidget.h)
add_library(pvt ${PVT_SOURCES} ${PVT_HEADERS})

set(QT5_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Charts
)
target_link_libraries(pvt PUBLIC ${QT5_LIBRARIES} plot)