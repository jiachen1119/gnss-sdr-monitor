set(STYLES_SOURCES CustomTabWidget.cpp TabBarStyle.cpp)
set(STYLES_HEADERS CustomTabWidget.h TabBarStyle.h)
add_library(styles ${STYLES_SOURCES} ${STYLES_HEADERS})

set(QT5_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
)
target_link_libraries(styles PUBLIC ${QT5_LIBRARIES} lib channel)