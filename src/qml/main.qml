import QtQuick 2.0
import QtPositioning 5.5
import QtLocation 5.6
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
//! [Imports]

Rectangle {
    anchors.fill: parent
    //! [Initialize Plugin]

    Map {
        id: map
        anchors.fill: parent
        plugin: myPlugin;
        center: QtPositioning.coordinate(32.0584359, 118.7888270)
        zoomLevel: 19

        Plugin {
            id: myPlugin
            name: "mapboxgl"
            PluginParameter {
                name: "mapboxgl.access_token"
                value: "pk.eyJ1IjoiamlhY2hlbjExMTkiLCJhIjoiY2x0NDJsamd5MWJsaTJqcXgwcnluMHZtaSJ9.bouv2PXrlGugcA-zTuWoXw"
            }
        }

        ColumnLayout
        {
            CheckBox // Follow CheckBox.
            {
                id: follow
                checked: true
                text: "Follow"
            }
            CheckBox // Follow CheckBox.
            {
                id: show_path
                checked: true
                text: "Show Path"
            }
        }

        MapPolyline // Path
        {
            line.width: 3
            line.color: "red"
            opacity: 0.3
            path: monitor_pvt_wrapper_.path
            visible: show_path.checked
        }

        MapQuickItem // Current position
        {
            id: myPosition
            coordinate: monitor_pvt_wrapper_.position

            anchorPoint.x: icon.width/2
            anchorPoint.y: icon.height/2

            sourceItem: Rectangle
            {
                id: icon
                width: 10
                height: 10
                radius: width/2
                color: "red"
            }
        }

        Connections
        {
            target: myPosition
            function onCoordinateChanged()
            {
                if (follow.checked){
                    map.center = monitor_pvt_wrapper_.position;
                }
            }
        }
    }
}