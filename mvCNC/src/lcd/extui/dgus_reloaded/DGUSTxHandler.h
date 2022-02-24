/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include "DGUSDisplay.h"
#include "definition/DGUS_VP.h"

namespace DGUSTxHandler {

  #if ENABLED(SDSUPPORT)
    void SetFileControlState(int, bool);
    void FileType(DGUS_VP &);
    void FileName(DGUS_VP &);
    void ScrollIcons(DGUS_VP &);
    void SelectedFileName(DGUS_VP &);
  #endif

  void PositionZ(DGUS_VP &);
  void Ellapsed(DGUS_VP &);
  void Percent(DGUS_VP &);
  void StatusIcons(DGUS_VP &);

  void Flowrate(DGUS_VP &);

  void TempMax(DGUS_VP &);

  void StepperStatus(DGUS_VP &);

  void StepIcons(DGUS_VP &);

  void ABLDisableIcon(DGUS_VP &);
  void ABLGrid(DGUS_VP &);

  void FilamentIcons(DGUS_VP &);

  void BLTouch(DGUS_VP &);

  void PIDIcons(DGUS_VP &);
  void PIDKp(DGUS_VP &);
  void PIDKi(DGUS_VP &);
  void PIDKd(DGUS_VP &);

  void BuildVolume(DGUS_VP &);
  void TotalPrints(DGUS_VP &);
  void FinishedPrints(DGUS_VP &);
  void CNCTime(DGUS_VP &);
  void LongestPrint(DGUS_VP &);
  void FilamentUsed(DGUS_VP &);

  void WaitIcons(DGUS_VP &);

  void FanSpeed(DGUS_VP &);

  void Volume(DGUS_VP &);

  void Brightness(DGUS_VP &);

  void ExtraToString(DGUS_VP &);
  void ExtraPGMToString(DGUS_VP &);

  template<typename T>
  void ExtraToInteger(DGUS_VP &vp) {
    if (!vp.size || !vp.extra) return;
    switch (vp.size) {
      default: return;
      case 1: {
        const uint8_t data = (uint8_t)(*(T*)vp.extra);
        dgus_display.Write((uint16_t)vp.addr, data);
        break;
      }
      case 2: {
        const uint16_t data = (uint16_t)(*(T*)vp.extra);
        dgus_display.Write((uint16_t)vp.addr, Swap16(data));
        break;
      }
      case 4: {
        const uint32_t data = (uint32_t)(*(T*)vp.extra);
        dgus_display.Write((uint16_t)vp.addr, dgus_display.SwapBytes(data));
        break;
      }
    }
  }

  template<typename T, uint8_t decimals>
  void ExtraToFixedPoint(DGUS_VP &vp) {
    if (!vp.size || !vp.extra) return;
    switch (vp.size) {
      default: return;
      case 1: {
        const uint8_t data = dgus_display.ToFixedPoint<T, uint8_t, decimals>(*(T*)vp.extra);
        dgus_display.Write((uint16_t)vp.addr, data);
        break;
      }
      case 2: {
        const uint16_t data = dgus_display.ToFixedPoint<T, uint16_t, decimals>(*(T*)vp.extra);
        dgus_display.Write((uint16_t)vp.addr, Swap16(data));
        break;
      }
      case 4: {
        const uint32_t data = dgus_display.ToFixedPoint<T, uint32_t, decimals>(*(T*)vp.extra);
        dgus_display.Write((uint16_t)vp.addr, dgus_display.SwapBytes(data));
        break;
      }
    }
  }

}
