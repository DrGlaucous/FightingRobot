# Remote Vehicle repository

This is where I put everything relating to designing a small, "antweight" combat robot.


## Firmware
Currently, the firmware is programmed in platform.io around the STM32 architecture.
The vehicle communicates to the controller using the RFM69HW radio module, which is both inexpensive and has excellent transmission range.

The "Transmitter" module decodes [PPM signals](http://flyingeinstein.com/index.php/articles/58-ppm-explained) from the controller and sends their digital values over the radio to the vehicle. There *are* recievers that output PPM directly without the need for the RFM middleman, but those are expensive and radio-specific. The extra range those modules would provide would be moot in the context of miniature combat robots. The analog transmitters additionally do not provide the customization abillity the digital transmiter has.

The work with RFM69 radios builds upon my previous project where I made an [inexpensive iClicker alternative using just the radio and an STM32](https://github.com/DrGlaucous/LittleBuddy). That repository better outlines how the STM32 should be connected to the RFM69 module.

---

<details>
  <summary style="font-size:100%;"><i><b>Notes for me</b></i></summary>

### Future Goals
Either use an ESP32 on the vehicle-side, or add bidirectional DShot to the STM32, so the vehicle's motor will provide telemetry.


### Mobility
With the transmitter, more flexible motion can be implemented.

Current suggestion: 3 wheeled omniwheel wheelbase

### Weapons

Rotating offset platic blade on one side, must not be below or above the robot's dirve train so it can drive upside down or rightside up.

Or long, plastic wedge attached to a servo motor for flipping other robots.



</details>









