import QtQuick 2.0
import QtPositioning 5.5
import QtLocation 5.6
//! [Imports]

Rectangle {
    anchors.fill: parent

    //! [Initialize Plugin]
    Plugin {
        id: myPlugin
        name: "mapboxgl"
        PluginParameter {
            name: "mapboxgl.access_token"
            value: "pk.eyJ1IjoiamlhY2hlbjExMTkiLCJhIjoiY2x0NDJsamd5MWJsaTJqcXgwcnluMHZtaSJ9.bouv2PXrlGugcA-zTuWoXw"
        }
    }
    Map {
        id: map
        anchors.fill: parent
        plugin: myPlugin;
        center: QtPositioning.coordinate(60.170448, 24.942046)//helsen
        zoomLevel: 15


    }
}