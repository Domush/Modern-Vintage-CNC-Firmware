/**
 * Modern Vintage CNC Firmware
*/

#include "../inc/mvCNCConfig.h"

#if USE_BEEPER

#include "buzzer.h"
#include "../module/fan_control.h"
#include "../lcd/mvcncui.h"

#if ENABLED(EXTENSIBLE_UI)
  #include "../lcd/extui/ui_api.h"
#endif

Buzzer::state_t Buzzer::state;
CircularQueue<tone_t, TONE_QUEUE_LENGTH> Buzzer::buffer;
Buzzer buzzer;

/**
 * @brief Add a tone to the queue
 * @details Adds a tone_t structure to the ring buffer, will block IO if the
 *          queue is full waiting for one slot to get available.
 *
 * @param duration Duration of the tone in milliseconds
 * @param frequency Frequency of the tone in hertz
 */
void Buzzer::tone(const uint16_t duration, const uint16_t frequency/*=0*/) {
  if (!ui.buzzer_enabled) return;
  while (buffer.isFull()) {
    tick();
  }
  tone_t tone = { duration, frequency };
  buffer.enqueue(tone);
}

void Buzzer::tick() {
  if (!ui.buzzer_enabled) return;
  const millis_t now = millis();

  if (!state.endtime) {
    if (buffer.isEmpty()) return;

    state.tone = buffer.dequeue();
    state.endtime = now + state.tone.duration;

    if (state.tone.frequency > 0) {
      #if ENABLED(EXTENSIBLE_UI) && DISABLED(EXTUI_LOCAL_BEEPER)
        CRITICAL_SECTION_START();
        ExtUI::onPlayTone(state.tone.frequency, state.tone.duration);
        CRITICAL_SECTION_END();
      #elif ENABLED(SPEAKER)
        CRITICAL_SECTION_START();
        ::tone(BEEPER_PIN, state.tone.frequency, state.tone.duration);
        CRITICAL_SECTION_END();
      #else
        on();
      #endif
    }
  }
  else if (ELAPSED(now, state.endtime)) reset();
}

#endif // USE_BEEPER
