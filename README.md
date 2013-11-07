CryVR
=====

CryVR push CryEngine to virtual reality !

CryVR gives CryEngine (since 3.5.4 both x86 & x64) virtual reality possibilities such as :
  
  * Connections : VRPN integration (Tracker / Analog / Button), UDP connections (console control / message sender & listener)
  
  * Controlers : Wiiuse lib near to full integration (Wiimote, Nunchuk, Balance Board, Classic and Guitar Hero controler. Custom setup nodes), Android device control, Flystick or any other device with VRPN
      
  * Camera : 3D coordinates to 2D coordinates, Simple asymetric camera setup, Cave system setup
  
  * Trackers : Vicon datastream (direct connect or VRPN)
      
  * Network : Dead Reckoning Entity 
  
  * Filters : One euro filter






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







