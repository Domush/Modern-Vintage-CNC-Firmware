/**
 * Modern Vintage CNC Firmware
*/

#include "../inc/mvCNCConfig.h"

#if ENABLED(HOST_ACTION_COMMANDS)

//#define DEBUG_HOST_ACTIONS

#include "host_actions.h"

#if ENABLED(ADVANCED_PAUSE_FEATURE)
  #include "pause.h"
  #include "../gcode/queue.h"
#endif

#if HAS_FILAMENT_SENSOR
  #include "runout.h"
#endif

HostUI hostui;

flag_t HostUI::flag;

void HostUI::action(FSTR_P const fstr, const bool eol) {
  if (!flag.bits) return;
  PORT_REDIRECT(SerialMask::All);
  SERIAL_ECHOPGM("//action:");
  SERIAL_ECHOF(fstr);
  if (eol) SERIAL_EOL();
}

#ifdef ACTION_ON_KILL
  void HostUI::kill() { action(F(ACTION_ON_KILL)); }
#endif
#ifdef ACTION_ON_PAUSE
  void HostUI::pause(const bool eol/*=true*/) { action(F(ACTION_ON_PAUSE), eol); }
#endif
#ifdef ACTION_ON_PAUSED
  void HostUI::paused(const bool eol/*=true*/) { action(F(ACTION_ON_PAUSED), eol); }
#endif
#ifdef ACTION_ON_RESUME
  void HostUI::resume() { action(F(ACTION_ON_RESUME)); }
#endif
#ifdef ACTION_ON_RESUMED
  void HostUI::resumed() { action(F(ACTION_ON_RESUMED)); }
#endif
#ifdef ACTION_ON_CANCEL
  void HostUI::cancel() { action(F(ACTION_ON_CANCEL)); }
#endif
#ifdef ACTION_ON_START
  void HostUI::start() { action(F(ACTION_ON_START)); }
#endif

#if ENABLED(G29_RETRY_AND_RECOVER)
  #ifdef ACTION_ON_G29_RECOVER
    void HostUI::g29_recover() { action(F(ACTION_ON_G29_RECOVER)); }
  #endif
  #ifdef ACTION_ON_G29_FAILURE
    void HostUI::g29_failure() { action(F(ACTION_ON_G29_FAILURE)); }
  #endif
#endif

#ifdef SHUTDOWN_ACTION
  void HostUI::shutdown() { action(F(SHUTDOWN_ACTION)); }
#endif

#if ENABLED(HOST_PROMPT_SUPPORT)

  PromptReason HostUI::host_prompt_reason = PROMPT_NOT_DEFINED;

  PGMSTR(CONTINUE_STR, "Continue");
  PGMSTR(DISMISS_STR, "Dismiss");

  #if HAS_RESUME_CONTINUE
    extern bool wait_for_user;
  #endif

  void HostUI::notify(const char * const cstr) {
    if (!flag.bits) return;
    PORT_REDIRECT(SerialMask::All);
    action(F("notification "), false);
    SERIAL_ECHOLN(cstr);
  }

  void HostUI::notify_P(PGM_P const pstr) {
    if (!flag.bits) return;
    PORT_REDIRECT(SerialMask::All);
    action(F("notification "), false);
    SERIAL_ECHOLNPGM_P(pstr);
  }

  void HostUI::prompt(FSTR_P const ptype, const bool eol/*=true*/) {
    if (!flag.bits) return;
    PORT_REDIRECT(SerialMask::All);
    action(F("prompt_"), false);
    SERIAL_ECHOF(ptype);
    if (eol) SERIAL_EOL();
  }

  void HostUI::prompt_plus(FSTR_P const ptype, FSTR_P const fstr, const char extra_char/*='\0'*/) {
    if (!flag.bits) return;
    prompt(ptype, false);
    PORT_REDIRECT(SerialMask::All);
    SERIAL_CHAR(' ');
    SERIAL_ECHOF(fstr);
    if (extra_char != '\0') SERIAL_CHAR(extra_char);
    SERIAL_EOL();
  }
  void HostUI::prompt_begin(const PromptReason reason, FSTR_P const fstr, const char extra_char/*='\0'*/) {
    if (!flag.bits) return;
    prompt_end();
    host_prompt_reason = reason;
    prompt_plus(F("begin"), fstr, extra_char);
  }
  void HostUI::prompt_button(FSTR_P const fstr) { prompt_plus(F("button"), fstr); }
  void HostUI::prompt_end() { prompt(F("end")); }
  void HostUI::prompt_show() { prompt(F("show")); }

  void HostUI::_prompt_show(FSTR_P const btn1, FSTR_P const btn2) {
    if (btn1) prompt_button(btn1);
    if (btn2) prompt_button(btn2);
    prompt_show();
  }
  void HostUI::prompt_do(const PromptReason reason, FSTR_P const fstr, FSTR_P const btn1/*=nullptr*/, FSTR_P const btn2/*=nullptr*/) {
    prompt_begin(reason, fstr);
    _prompt_show(btn1, btn2);
  }
  void HostUI::prompt_do(const PromptReason reason, FSTR_P const fstr, const char extra_char, FSTR_P const btn1/*=nullptr*/, FSTR_P const btn2/*=nullptr*/) {
    prompt_begin(reason, fstr, extra_char);
    _prompt_show(btn1, btn2);
  }

  #if ENABLED(ADVANCED_PAUSE_FEATURE)
    void HostUI::filament_load_prompt() {
      const bool disable_to_continue = TERN0(HAS_FILAMENT_SENSOR, runout.filament_ran_out);
      prompt_do(PROMPT_FILAMENT_RUNOUT, F("Paused"), F("PurgeMore"),
        disable_to_continue ? F("DisableRunout") : FPSTR(CONTINUE_STR)
      );
    }
  #endif

  //
  // Handle responses from the host, such as:
  //  - Filament runout responses: Purge More, Continue
  //  - General "Continue" response
  //  - Resume CNC response
  //  - Dismissal of info
  //
  void HostUI::handle_response(const uint8_t response) {
    const PromptReason hpr = host_prompt_reason;
    host_prompt_reason = PROMPT_NOT_DEFINED;  // Reset now ahead of logic
    switch (hpr) {
      case PROMPT_FILAMENT_RUNOUT:
        switch (response) {

          case 0: // "Purge More" button
            #if BOTH(M600_PURGE_MORE_RESUMABLE, ADVANCED_PAUSE_FEATURE)
              pause_menu_response = PAUSE_RESPONSE_EXTRUDE_MORE;  // Simulate menu selection (menu exits, doesn't extrude more)
            #endif
            break;

          case 1: // "Continue" / "Disable Runout" button
            #if BOTH(M600_PURGE_MORE_RESUMABLE, ADVANCED_PAUSE_FEATURE)
              pause_menu_response = PAUSE_RESPONSE_RESUME_PRINT;  // Simulate menu selection
            #endif
            #if HAS_FILAMENT_SENSOR
              if (runout.filament_ran_out) {                      // Disable a triggered sensor
                runout.enabled = false;
                runout.reset();
              }
            #endif
            break;
        }
        break;
      case PROMPT_USER_CONTINUE:
        TERN_(HAS_RESUME_CONTINUE, wait_for_user = false);
        break;
      case PROMPT_PAUSE_RESUME:
        #if BOTH(ADVANCED_PAUSE_FEATURE, SDSUPPORT)
          extern const char M24_STR[];
          queue.inject_P(M24_STR);
        #endif
        break;
      case PROMPT_INFO:
        break;
      default: break;
    }
  }

#endif // HOST_PROMPT_SUPPORT

#endif // HOST_ACTION_COMMANDS
