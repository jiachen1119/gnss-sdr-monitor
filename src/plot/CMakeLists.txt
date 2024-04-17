set(PLOT_SOURCES CustomChartView.cpp ThreadForPlot.cpp CustomChartViewForMultiCNo.cpp)
set(PLOT_HEADERS CustomChartView.h ThreadForPlot.h CustomChartViewForMultiCNo.h)
add_library(plot ${PLOT_SOURCES} ${PLOT_HEADERS})

set(QT5_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Charts
)
target_link_libraries(plot PUBLIC ${QT5_LIBRARIES} Boost::boost)