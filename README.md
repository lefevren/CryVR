CryVR (Current version : 0.16)
=====

CryVR push CryEngine to virtual reality !

CryVR gives CryEngine (since 3.5.4 both x86 & x64) virtual reality possibilities such as :
  
  * Connections : VRPN integration (Tracker / Analog / Button), UDP connections (console control / message sender & listener)
  
  * Controlers : Wiiuse lib near to full integration (Wiimote, Nunchuk, Balance Board, Classic and Guitar Hero controler. Custom setup nodes), Android device control, Flystick or any other device with VRPN
      
  * Camera : 3D coordinates to 2D coordinates, Simple asymetric camera setup, Cave system setup
  
  * Trackers : Vicon datastream (direct connect or VRPN)
      
  * Network : Dead Reckoning Entity 
  
  * Filters : One euro filter

  * Android & IOS : CryEngine ingame control application (console, custom events, 2x XY analogs, GUI elements)



Included library
================
- Wiiuse integration (x86 and x64 lib and dll in vs2012 version)
- VRPN library integration (x86 and x64 lib and dll in vs2012 version)


Infos
======

- See changelog to know implemented functions.

[Video](http://www.youtube.com/watch?v=0e7RbiY0b60)


More
====

- One euro filter : Algorithm based on the C++ version of [One euro filter] (http://www.lifl.fr/~casiez/1euro/). This filter help to smooth raw data from VRPN server or Wiimote controler. 4x nodes to smooth float, vec2, vec3 and quaternion values.
- Wiimote nodes :  Wiiuse 0.14 near to full integration. Full custom event detection, wiimote & nunchuk, Classic, Balance Board and Guitar Hero integration.
- VRPN : Need a VRPN server to connect. If the server bring wiimote controler, then VRPNWiimote can be used. 
- UDP connections based onRenEvo Software & Designs UDP listener and sender nodes.
- Wiimote event detection can be really heavy. Wiimote generates event quickly. Detect all events could be FPS killer (something like 10% worst)
With good configuration you can get enough events to make it real time response without noticable fps drop down. Depends on system performance.

Wiimote Flownodes :
------------------

Wiimote Manager : Needed as the first active node. Initialize connection with bluetooth stack. 
Wiimote Balance Board / Classic / Guitar Hero / Controler nodes : For now, just one at a time. Use this to retrieve devices values inside your flowgraph.
Wiimote Battery Level : Retreive battery level of the connected device.
Wiimote IR : Activate / Deactivate / Get IR state.
Wiimote Rumble : Toogle Rumble for the given device.
Wiimote Motion Sensing : Activate or deactivate motion sensing for the connected device (off by default)
Wiimote Setup : Setup wiimote detection events.





