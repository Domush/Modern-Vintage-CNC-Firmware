/**
 * Webber Ranch CNC Firmware
*/

#include "../../../inc/WRCNCConfig.h"

#if HAS_SPI_TFT

#include "tft_spi.h"

SPIClass TFT_SPI::SPIx(1);

void TFT_SPI::Init() {
  #if PIN_EXISTS(TFT_RESET)
    OUT_WRITE(TFT_RESET_PIN, HIGH);
    delay(100);
  #endif

  #if PIN_EXISTS(TFT_BACKLIGHT)
    OUT_WRITE(TFT_BACKLIGHT_PIN, HIGH);
  #endif

  SET_OUTPUT(TFT_DC_PIN);
  SET_OUTPUT(TFT_CS_PIN);
  WRITE(TFT_DC_PIN, HIGH);
  WRITE(TFT_CS_PIN, HIGH);

  /**
   * STM32F1 APB2 = 72MHz, APB1 = 36MHz, max SPI speed of this MCU if 18Mhz
   * STM32F1 has 3 SPI ports, SPI1 in APB2, SPI2/SPI3 in APB1
   * so the minimum prescale of SPI1 is DIV4, SPI2/SPI3 is DIV2
   */
  #if 0
    #if SPI_DEVICE == 1
     #define SPI_CLOCK_MAX SPI_CLOCK_DIV4
    #else
     #define SPI_CLOCK_MAX SPI_CLOCK_DIV2
    #endif
    uint8_t  clock;
    uint8_t spiRate = SPI_FULL_SPEED;
    switch (spiRate) {
     case SPI_FULL_SPEED:    clock = SPI_CLOCK_MAX ;  break;
     case SPI_HALF_SPEED:    clock = SPI_CLOCK_DIV4 ; break;
     case SPI_QUARTER_SPEED: clock = SPI_CLOCK_DIV8 ; break;
     case SPI_EIGHTH_SPEED:  clock = SPI_CLOCK_DIV16; break;
     case SPI_SPEED_5:       clock = SPI_CLOCK_DIV32; break;
     case SPI_SPEED_6:       clock = SPI_CLOCK_DIV64; break;
     default:                clock = SPI_CLOCK_DIV2;  // Default from the SPI library
    }
  #endif

  #if TFT_MISO_PIN == BOARD_SPI1_MISO_PIN
    SPIx.setModule(1);
  #elif TFT_MISO_PIN == BOARD_SPI2_MISO_PIN
    SPIx.setModule(2);
  #endif
  SPIx.setClock(SPI_CLOCK_MAX_TFT);
  SPIx.setBitOrder(MSBFIRST);
  SPIx.setDataMode(SPI_MODE0);
}

void TFT_SPI::DataTransferBegin(uint16_t DataSize) {
  SPIx.setDataSize(DataSize);
  SPIx.begin();
  WRITE(TFT_CS_PIN, LOW);
}

uint32_t TFT_SPI::GetID() {
  uint32_t id;
  id = ReadID(LCD_READ_ID);
  if ((id & 0xFFFF) == 0 || (id & 0xFFFF) == 0xFFFF)
    id = ReadID(LCD_READ_ID4);
  return id;
}

uint32_t TFT_SPI::ReadID(uint16_t Reg) {
  uint32_t data = 0;

  #if PIN_EXISTS(TFT_MISO)
    uint8_t d = 0;
    SPIx.setDataSize(DATASIZE_8BIT);
    SPIx.setClock(SPI_CLOCK_DIV64);
    SPIx.begin();
    WRITE(TFT_CS_PIN, LOW);
    WriteReg(Reg);

    LOOP_L_N(i, 4) {
      SPIx.read((uint8_t*)&d, 1);
      data = (data << 8) | d;
    }

    DataTransferEnd();
    SPIx.setClock(SPI_CLOCK_MAX_TFT);
  #endif

  return data >> 7;
}

bool TFT_SPI::isBusy() { return false; }

void TFT_SPI::Abort() { DataTransferEnd(); }

void TFT_SPI::Transmit(uint16_t Data) { SPIx.transfer(Data); }

void TFT_SPI::TransmitDMA(uint32_t MemoryIncrease, uint16_t *Data, uint16_t Count) {
  DataTransferBegin(DATASIZE_16BIT);
  WRITE(TFT_DC_PIN, HIGH);
  SPIx.dmaSend(Data, Count, MemoryIncrease);
  DataTransferEnd();
}

#endif // HAS_SPI_TFT