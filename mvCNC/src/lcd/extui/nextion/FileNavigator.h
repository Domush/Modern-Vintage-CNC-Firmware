/**
 * Modern Vintage CNC Firmware
 * Copyright (c) 2022 Edward Webber [https://github.com/Domush/mvCNC-Modern-Vintage-CNC-Firmware]

 * Based on Marlin and grbl.
 */
#pragma once

/* ****************************************
 * lcd/extui/nextion/FileNavigator.cpp
 * ****************************************
 * Extensible_UI implementation for Nextion
 * https://github.com/Skorpi08
 * ***************************************/

#include "nextion_tft_defs.h" // for MAX_PATH_LEN
#include "../ui_api.h"

using namespace ExtUI;

class FileNavigator {
  public:
    FileNavigator();
    static void  reset();
    static void  getFiles(uint16_t);
    static void  upDIR();
    static void  changeDIR(char *);
    static void  refresh();
    static char* getCurrentFolderName();
  private:
    static FileList filelist;
    static char     currentfoldername[MAX_PATH_LEN];
    static uint16_t lastindex;
    static uint8_t  folderdepth;
    static uint16_t currentindex;
};

extern FileNavigator filenavigator;
