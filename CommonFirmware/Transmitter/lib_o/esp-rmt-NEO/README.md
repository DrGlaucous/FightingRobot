### ESP-RMT NEO
This library gives the latest RMT support to the ESP32 arduino framework.

The arduino dev environment is much more flexible than espidf, but when it comes to cutting-edge features and updates, it often lags behind.
I needed to use some of the new features of the esp32's RMT hardware that came with the espidf v5 update. Espidf is now newer than that, but the arduino core still (at the time of this library, at least) uses espidf 4.4.5. (PlatformIO is even further behind, which doesn't help things).
So, until this library's features are available in the actual backend, I just cobbled together the bare features I needed in order to get better RMT support for my projects.

#### Pitfalls:
Some of the preprocessor `#defines` that this library uses to enable features come from the new (5.0.0) version of espidf. As a result, some features are incorrectly disabled. To mitigate this, I put some extra definitions in `/src/clk_tree_defs.h`. The definitions I'm including only cover the generic "esp32" flavor of controller. I got these definitions from `framework-espidf/components/soc/esp32/include/soc/soc_caps.h`
*This may result in compatibility issues! (enable what you need. you shouldn't need to touch it if you have one of the many basic ESP32 devkits)*

Additionally, everything in `./src/driver/hal` is also specific to the generic esp32. To change what ESP this is compatible with, change the contents of `./src/driver/hal` and use the defs from the proper board (you can get these from the official espidf release). They're found in `framework-espidf/components/hal/BOARDTYPE/include/hal/`.

Everywhere I made a change, I put `//ALTER` at the end of the line. You can use this to see what I changed or make fixes where needed.

I'm sure this could be done more smoothly, but I got what I need, and hopefully shouldn't have to wait too long until this library is completely redundant.
