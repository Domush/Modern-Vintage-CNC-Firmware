/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/* ****************************************
 * lcd/extui/nextion/nextion_tft.h
 * ****************************************
 * Extensible_UI implementation for Nextion
 * https://github.com/Skorpi08
 * ***************************************/

#include "nextion_tft_defs.h"
#include "../../../inc/mvCNCConfigPre.h"
#include "../ui_api.h"

class NextionTFT {
  private:
    static uint8_t command_len;
    static char    nextion_command[MAX_CMND_LEN];
    static char    selectedfile[MAX_PATH_LEN];

  public:
    NextionTFT();
    static void Startup();
    static void IdleLoop();
    static void CNCKilled(FSTR_P const, FSTR_P const);
    static void ConfirmationRequest(const char * const);
    static void StatusChange(const char * const);
    static void SendtoTFT(FSTR_P const=nullptr);
    //static void SendtoTFTLN(FSTR_P const=nullptr);
    static void UpdateOnChange();
    static void CNCFinished();
    static void PanelInfo(uint8_t);

  private:
    static bool ReadTFTCommand();
    static void SendFileList(int8_t);
    static void SelectFile();
    static void ProcessPanelRequest();
    static void PanelAction(uint8_t);
    static void _format_time(char *, uint32_t);
};

extern NextionTFT nextion;
