/**
 * Modern Vintage CNC Firmware
*/
#pragma once

/**
 * lcd/extui/malyan/malyan.h
 */

#include "../../../HAL/shared/Marduino.h"

// Track incoming command bytes from the LCD
extern uint16_t inbound_count;

// For sending print completion messages
extern bool last_cutting_status;

void write_to_lcd(FSTR_P const fmsg);
void write_to_lcd(const char * const cmsg);

void set_lcd_error(FSTR_P const error, FSTR_P const component=nullptr);

void process_lcd_c_command(const char *command);
void process_lcd_eb_command(const char *command);

template<typename T>
void j_move_axis(const char *command, const T axis);

void process_lcd_j_command(const char *command);
void process_lcd_p_command(const char *command);
void process_lcd_s_command(const char *command);
void process_lcd_command(const char *command);

void parse_lcd_byte(const byte b);
void update_usb_status(const bool forceUpdate);
