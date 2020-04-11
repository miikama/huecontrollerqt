
# HueControllerQT

```HueControllerQT``` is a minimalist desktop application written in ```C++``` and ```QT``` for Linux that you can use to control you Philips Hue smart lights through the Hue bridge.

<p align="center">
    <img height="400px" src="media/EntryScreen.png">
</p>


A statically linked binary of the application compiled on Ubuntu 18.04 can be found under the releases. This means you can download the binary and run it. Nothing extra required. See the [Installation](#installation) for adding an autorun entry for starting the app on computer start-up.

Please let me know if you try it and it does not work :)

The application is currently built against static Qt libraries (v5.12). Since Qt is a cross-platform framework, there should be a minimal effort to compile and run the application on Windows, but haven't done so myself.

## Packaging

See package.sh which implements required steps for packaging our application. It currently:

- Downloads the QT libraries and compiles them as static libraries.
- Build our app as static app linked agains the built QT libraries.
- Add the built app 'HueControllerQT' to installation directory
- Create a HueControllerQT.tar at the project root.

This has steps following instructions at
1. https://wiki.qt.io/Building_Qt_5_from_Git
2. https://doc.qt.io/qt-5/linux-deployment.html

NOTE: Building all the libraries will take some time...

## Installation

After building the application by using package.sh, a statically linked application binary should be under 'install' directory. To install the app on your system

```bash
cd install
./install.sh
```

This copies the app binary under ```/usr/local/bin``` and creates a autorun entry at ```~/.config/autorun/HueControllerQT.desktop```. This will automatically start the app on startup.


## General api description

Mainwindow has HueApi.

Api owns light objects and emits light events.

Mainwindow can set the state of different lights directly from the instantiated Light objects or through LightWidget->Light.

Light objects just call their HueApi instance to actually modify the light state.

HueApi tries to perform the api action by sending a POST request to the Hue Bridge.

On succeed, HueApi updates all of its light objects and then emits LightsUpdated event. MainWindow listens to this event.




