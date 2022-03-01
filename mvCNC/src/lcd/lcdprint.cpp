/**
 * Modern Vintage CNC Firmware
*/

/**
 * lcdprint.cpp
 */

#include "../inc/mvCNCConfigPre.h"

#if HAS_WIRED_LCD && !HAS_GRAPHICAL_TFT && !IS_DWIN_MVCNCUI

#include "mvcncui.h"
#include "lcdprint.h"

/**
 * lcd_put_u8str_ind_P
 *
 * Print a string with an index substituted within it:
 *
 *   $ displays the clipped C-string given by the inStr argument
 *   = displays  '0'....'10' for indexes 0 - 10
 *   ~ displays  '1'....'11' for indexes 0 - 10
 *   * displays 'E1'...'E11' for indexes 0 - 10 (By default. Uses LCD_FIRST_TOOL)
 *   @ displays an axis name such as XYZUVW, or E for an ATC tool
 */
lcd_uint_t lcd_put_u8str_ind_P(PGM_P const pstr, const int8_t ind, PGM_P const inStr/*=nullptr*/, const lcd_uint_t maxlen/*=LCD_WIDTH*/) {
  const uint8_t prop = USE_WIDE_GLYPH ? 2 : 1;
  uint8_t *p = (uint8_t*)pstr;
  int8_t n = maxlen;
  while (n > 0) {
    wchar_t ch;
    p = get_utf8_value_cb(p, read_byte_rom, &ch);
    if (!ch) break;
    if (ch == '=' || ch == '~' || ch == '*') {
      if (ind >= 0) {
        if (ch == '*') { lcd_put_wchar('E'); n--; }
        if (n) {
          int8_t inum = ind + ((ch == '=') ? 0 : LCD_FIRST_TOOL);
          if (inum >= 10) {
            lcd_put_wchar('0' + (inum / 10)); n--;
            inum %= 10;
          }
          if (n) { lcd_put_wchar('0' + inum); n--; }
        }
      }
      else {
        PGM_P const b = ind == -2 ? GET_TEXT(MSG_CHAMBER) : GET_TEXT(MSG_BED);
        n -= lcd_put_u8str_max_P(b, n * (MENU_FONT_WIDTH)) / (MENU_FONT_WIDTH);
      }
      if (n) {
        n -= lcd_put_u8str_max_P((PGM_P)p, n * (MENU_FONT_WIDTH)) / (MENU_FONT_WIDTH);
        break;
      }
    }
    else if (ch == '$' && inStr) {
      n -= lcd_put_u8str_max_P(inStr, n * (MENU_FONT_WIDTH)) / (MENU_FONT_WIDTH);
    }
    else if (ch == '@') {
      lcd_put_wchar(axis_codes[ind]);
      n--;
    }
    else {
      lcd_put_wchar(ch);
      n -= ch > 255 ? prop : 1;
    }
  }
  return n;
}

// Calculate UTF8 width with a simple check
int calculateWidth(PGM_P const pstr) {
  if (!USE_WIDE_GLYPH) return utf8_strlen_P(pstr) * MENU_FONT_WIDTH;
  const uint8_t prop = 2;
  uint8_t *p = (uint8_t*)pstr;
  int n = 0;

  do {
    wchar_t ch;
    p = get_utf8_value_cb(p, read_byte_rom, &ch);
    if (!ch) break;
    n += (ch > 255) ? prop : 1;
  } while (1);

  return n * MENU_FONT_WIDTH;
}

#endif // HAS_WIRED_LCD
