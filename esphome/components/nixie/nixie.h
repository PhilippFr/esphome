#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/automation.h"
#include <esp_timer.h>

namespace esphome {
namespace nixie {

class NixieDisplay : public Component {
 public:
  void set_digit1_pin(InternalGPIOPin *pin) { digit1_pin_ = pin; }
  void set_digit2_pin(InternalGPIOPin *pin) { digit2_pin_ = pin; }
  void set_digit3_pin(InternalGPIOPin *pin) { digit3_pin_ = pin; }
  void set_digit4_pin(InternalGPIOPin *pin) { digit4_pin_ = pin; }
  void set_counter_rst_pin(InternalGPIOPin *pin) { counter_rst_pin_ = pin; }
  void set_counter_clk_pin(InternalGPIOPin *pin) { counter_clk_pin_ = pin; }
  void set_counter_en_pin(InternalGPIOPin *pin) { counter_en_pin_ = pin; }

  void setup() override;
  void dump_config() override;

  void IRAM_ATTR update_display_timer_callback();

  void set_display_text(const std::string &text);
  void set_multiplexing_speed(uint32_t speed) { multiplexing_speed_ = speed; }

 protected:
  void update_display();
  void set_counter(int value);
  void delay_microseconds_fast(uint32_t us);

  GPIOPin *digit1_pin_;
  GPIOPin *digit2_pin_;
  GPIOPin *digit3_pin_;
  GPIOPin *digit4_pin_;
  GPIOPin *counter_rst_pin_;
  GPIOPin *counter_clk_pin_;
  GPIOPin *counter_en_pin_;

  uint32_t multiplexing_speed_;
  esp_timer_handle_t display_timer_;
  int active_digit_ = 0;
  std::string display_text_;
  int digits_[4] = {0, 0, 0, 0};
};

template<typename... Ts> class SetDisplayTextAction : public Action<Ts...>, public Parented<NixieDisplay> {
 public:
  TEMPLATABLE_VALUE(std::string, display_text)

  void play(Ts... x) override {
    auto display_text = this->display_text_.value(x...);
    this->parent_->set_display_text(display_text);
  }
};

}  // namespace nixie
}  // namespace esphome
