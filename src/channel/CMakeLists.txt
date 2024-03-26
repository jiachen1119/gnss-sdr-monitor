set(CHANNEL_SOURCES ChannelTableModel.cpp CustomChannelView.cpp
        LedDelegate.cpp DopplerDelegate.cpp ConstellationDelegate.cpp Cn0Delegate.cpp
)
set(CHANNEL_HEADERS ChannelTableModel.h CustomChannelView.h
        LedDelegate.h DopplerDelegate.h ConstellationDelegate.h Cn0Delegate.h
)
add_library(channel ${CHANNEL_SOURCES} ${CHANNEL_HEADERS})

set(QT5_LIBRARIES
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::PrintSupport
        Qt5::Charts
)
target_link_libraries(channel PUBLIC ${QT5_LIBRARIES} Boost::boost)