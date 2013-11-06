CryVR
=======================


Version 0.2 (first release)  :
-----------------

Stable Wiimote Part Release & VRPN consideration

- Balance Board integration
- Balance board [x,y] {-1;1} "gravity center" output value
- Classic and Guitar Hero nodes integration
- Wiimote node less CPU dependant
- Wiimote as a Plugin for CryEngine 3.5.4
- VRPN as a Plugin for CryEngine 3.5.4
- Optional Wiimotes nodes : Bluetooth stack setup, Aspect Ratio, IR sensivity, IR position and status nodes


Version 0.11 
-----------------

Current Debug version : WiiWork !

- Fixing access memory Cryengine crashs
- Adding WiimoteSetup node 
- Fixing Nunchuk & Balance board detection (force Motion sensing off during connection)
- Setup default nodes balanced configuration
- Code optimization
- Full event detection setup (timeouts thresholds and angles detection)
- Roll, pitch and yaw events outputs only one time / frame


Version 0.1 
----------------------

Initial Debug for CryEngine 3.5.4 (32/64 bit).

- VRPN tracker and wiimote (2x nodes) 
- Asymetric camera setup (3x nodes) (not user friendly, not usable for the moment)
- Wiimote controler (8x nodes)
- 3D to 2D coordinates (1x node) 
- One euro filter integration (deleting noise for float, vec2, vec3 and quaternion) (4x nodes)
