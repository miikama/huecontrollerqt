

## General api description

Mainwindow has HueApi.

Api owns light objects and emits light events.

Mainwindow can set the state of different lights directly from the instantiated Light objects or through LightWidget->Light.

Light objects just call their HueApi instance to actually modify the light state.

HueApi tries to perform the api action by sending a POST request to the Hue Bridge.

On succeed, HueApi updates all of its light objects and then emits LightsUpdated event. MainWinown listens to this event.

```


mainwindow

```
