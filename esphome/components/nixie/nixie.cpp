#include "nixie.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace nixie {

static const char *const TAG = "nixie.display";

void NixieDisplay::setup() {
  this->digit1_pin_->setup();
  this->digit2_pin_->setup();
  this->digit3_pin_->setup();
  this->digit4_pin_->setup();
  this->counter_rst_pin_->setup();
  this->counter_clk_pin_->setup();
  this->counter_en_pin_->setup();

  this->digit1_pin_->digital_write(false);
  this->digit2_pin_->digital_write(false);
  this->digit3_pin_->digital_write(false);
  this->digit4_pin_->digital_write(false);
  this->counter_en_pin_->digital_write(true);
  this->counter_rst_pin_->digital_write(true);
  this->delay_microseconds_fast(5);
  this->counter_rst_pin_->digital_write(false);

  const esp_timer_create_args_t display_timer_args = {
      .callback = [](void *arg) { static_cast<NixieDisplay *>(arg)->update_display_timer_callback(); },
      .arg = this,
      .dispatch_method = ESP_TIMER_TASK,
      .name = "nixie_display_timer"};
  esp_timer_create(&display_timer_args, &this->display_timer_);
  esp_timer_start_periodic(this->display_timer_, 5000);  // 5ms
}

void NixieDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "Nixie Display");
  LOG_PIN("  Digit1: ", digit1_pin_);
  LOG_PIN("  Digit2: ", digit2_pin_);
  LOG_PIN("  Digit3: ", digit3_pin_);
  LOG_PIN("  Digit4: ", digit4_pin_);
  LOG_PIN("  Counter RST: ", counter_rst_pin_);
  LOG_PIN("  Counter CLK: ", counter_clk_pin_);
  LOG_PIN("  Counter EN: ", counter_en_pin_);
}

void NixieDisplay::set_display_text(const std::string &text) {
  this->display_text_ = text;
  for (int i = 0; i < 4; i++) {
    this->digits_[i] = this->display_text_[i] - '0';  // Subtract ASCII 48 to account for ASCII encoding 0=48 to 57=9
  }
}

void NixieDisplay::update_display_timer_callback() { this->update_display(); }

void NixieDisplay::update_display() {
  this->digit1_pin_->digital_write(false);
  this->digit2_pin_->digital_write(false);
  this->digit3_pin_->digital_write(false);
  this->digit4_pin_->digital_write(false);

  this->set_counter(this->digits_[this->active_digit_]);

  switch (this->active_digit_) {
    case 0:
      this->digit1_pin_->digital_write(true);
      break;
    case 1:
      this->digit2_pin_->digital_write(true);
      break;
    case 2:
      this->digit3_pin_->digital_write(true);
      break;
    case 3:
      this->digit4_pin_->digital_write(true);
      break;
  }

  this->active_digit_ = (this->active_digit_ + 1) % 4;
}

void NixieDisplay::set_counter(int value) {
  this->counter_rst_pin_->digital_write(true);
  this->delay_microseconds_fast(5);
  this->counter_rst_pin_->digital_write(false);

  this->counter_en_pin_->digital_write(false);
  this->delay_microseconds_fast(5);
  for (int i = 0; i < value; i++) {
    this->counter_clk_pin_->digital_write(true);
    this->delay_microseconds_fast(5);
    this->counter_clk_pin_->digital_write(false);
    this->delay_microseconds_fast(5);
  }
  this->counter_en_pin_->digital_write(true);
}

void NixieDisplay::delay_microseconds_fast(uint32_t us) {
  uint32_t start = esp_timer_get_time();
  while (esp_timer_get_time() - start < us) {
    asm volatile("nop");
  }
}

}  // namespace nixie
}  // namespace esphome
