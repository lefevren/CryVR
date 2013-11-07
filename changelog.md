CryVR
=======================



Version 0.2 (first release to come)  :
-----------------

Stable Wiimote Part Release & VRPN consideration

- Wii devices full integration (10x nodes)
- Customisable Wii nodes CPU dependancies
- Wii devices as a Plugin for CryEngine 3.5.4
- VRPN as a Plugin for CryEngine 3.5.4


Version 0.16
-------------------
Current Debug version : GuitarHeroWork !

- Beta Guitar Hero 3 controler integration
- Bluetooth stack, Aspect ratio and IR sensivity settings


Version 0.15
-------------------
BalanceBoardWork !

- Balance Board integration
- Balance Board [x,y] {-1;1} "gravity center" output value
- Balance Board automatic [0,0] calibration system
- Balance Board weight output
- Balance Board AllInOne Node
- Beta Classic Controler integration
- Classic Controler AllInOne node
- Removed motion sensing option from WiimoteManager node (Motion Sensing set to false by default)


Version 0.11 
-----------------

WiiWork !

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

- Project creation
- VRPN tracker and wiimote (2x nodes) 
- Asymetric camera setup (3x nodes) (not user friendly, not usable for the moment)
- Wiimote controler (8x nodes)
- 3D to 2D coordinates (1x node) 
- One euro filter integration (deleting noise for float, vec2, vec3 and quaternion) (4x nodes)
