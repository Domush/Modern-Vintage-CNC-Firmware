/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/**
 * DWIN Mesh Viewer
 * Author: Miguel A. Risco-Castillo (MRISCOC)
 * Version: 3.9.1
 * Date: 2021/11/09
 *
 * Based on the original code provided by Creality under GPL
 */

class MeshViewerClass {
public:
  void Draw();
};

extern MeshViewerClass MeshViewer;
