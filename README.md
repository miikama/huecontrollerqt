

## General api description

Mainwindow has HueApi.

Api owns light objects and emits light events.

Mainwindow can set the state of different lights directly from the instantiated Light objects or through LightWidget->Light.

Light objects just call their HueApi instance to actually modify the light state.

HueApi tries to perform the api action by sending a POST request to the Hue Bridge.

On succeed, HueApi updates all of its light objects and then emits LightsUpdated event. MainWinown listens to this event.

## Packaging

See package.sh which implements required steps for packaing our application.

This has steps following instructions at
1. https://wiki.qt.io/Building_Qt_5_from_Git
2. https://doc.qt.io/qt-5/linux-deployment.html


