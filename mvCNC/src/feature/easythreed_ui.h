/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

class EasythreedUI {
  public:
    static void init();
    static void run();

  private:
    static void blinkLED();
    static void loadButton();
    static void printButton();
};

extern EasythreedUI easythreed_ui;
