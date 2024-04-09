set(ALERT_SOURCES AlertWidget.cpp)
set(ALERT_HEADERS AlertWidget.h)
add_library(alert ${ALERT_SOURCES} ${ALERT_HEADERS})

set(QT5_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
)
target_link_libraries(alert PUBLIC ${QT5_LIBRARIES} plot Boost::boost)