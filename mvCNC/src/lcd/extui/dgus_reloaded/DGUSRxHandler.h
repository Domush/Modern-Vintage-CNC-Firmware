/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

#include "DGUSDisplay.h"
#include "definition/DGUS_VP.h"

namespace DGUSRxHandler {

  void ScreenChange(DGUS_VP &, void *);

  #if ENABLED(SDSUPPORT)
    void Scroll(DGUS_VP &, void *);
    void SelectFile(DGUS_VP &, void *);
    void CNCFile(DGUS_VP &, void *);
  #endif

  void CNCAbort(DGUS_VP &, void *);
  void PrintPause(DGUS_VP &, void *);
  void CNCResume(DGUS_VP &, void *);

  void Feedrate(DGUS_VP &, void *);
  void Flowrate(DGUS_VP &, void *);
  void BabystepSet(DGUS_VP &, void *);
  void Babystep(DGUS_VP &, void *);

  void TempPreset(DGUS_VP &, void *);
  void TempTarget(DGUS_VP &, void *);
  void TempCool(DGUS_VP &, void *);

  void Steppers(DGUS_VP &, void *);

  void ZOffset(DGUS_VP &, void *);
  void ZOffsetStep(DGUS_VP &, void *);
  void ZOffsetSetStep(DGUS_VP &, void *);

  void MoveToPoint(DGUS_VP &, void *);

  void Probe(DGUS_VP &, void *);
  void DisableABL(DGUS_VP &, void *);

  void FilamentSelect(DGUS_VP &, void *);
  void FilamentLength(DGUS_VP &, void *);
  void FilamentMove(DGUS_VP &, void *);

  void Home(DGUS_VP &, void *);
  void Move(DGUS_VP &, void *);
  void MoveStep(DGUS_VP &, void *);
  void MoveSetStep(DGUS_VP &, void *);

  void GcodeClear(DGUS_VP &, void *);
  void GcodeExecute(DGUS_VP &, void *);

  void ResetEEPROM(DGUS_VP &, void *);

  void SettingsExtra(DGUS_VP &, void *);

  void PIDSelect(DGUS_VP &, void *);
  void PIDSetTemp(DGUS_VP &, void *);
  void PIDRun(DGUS_VP &, void *);

  #if ENABLED(POWER_LOSS_RECOVERY)
    void PowerLossAbort(DGUS_VP &, void *);
    void PowerLossResume(DGUS_VP &, void *);
  #endif

  void WaitAbort(DGUS_VP &, void *);
  void WaitContinue(DGUS_VP &, void *);

  void FanSpeed(DGUS_VP &, void *);

  void Volume(DGUS_VP &, void *);

  void Brightness(DGUS_VP &, void *);

  void Debug(DGUS_VP &, void *);

  void StringToExtra(DGUS_VP &, void *);

  template<typename T>
  void IntegerToExtra(DGUS_VP &vp, void *data_ptr) {
    if (!vp.size || !vp.extra) return;
    switch (vp.size) {
      default: return;
      case 1: {
        const uint8_t data = *(uint8_t*)data_ptr;
        *(T*)vp.extra = (T)data;
        break;
      }
      case 2: {
        const uint16_t data = Swap16(*(uint16_t*)data_ptr);
        *(T*)vp.extra = (T)data;
        break;
      }
      case 4: {
        const uint32_t data = dgus_display.SwapBytes(*(uint32_t*)data_ptr);
        *(T*)vp.extra = (T)data;
        break;
      }
    }
  }

}