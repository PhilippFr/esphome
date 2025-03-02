#include "nixie.h"

namespace esphome {
  namespace nixie {
  
  void NixieDisplay::setup() {
    pinMode(DIGIT1_PIN, OUTPUT);
    pinMode(DIGIT2_PIN, OUTPUT);
    pinMode(DIGIT3_PIN, OUTPUT);
    pinMode(DIGIT4_PIN, OUTPUT);
    pinMode(COUNTER_RST_PIN, OUTPUT);
    pinMode(COUNTER_CLK_PIN, OUTPUT);
    pinMode(COUNTER_EN_PIN, OUTPUT);
  
    digitalWrite(DIGIT1_PIN, LOW);
    digitalWrite(DIGIT2_PIN, LOW);
    digitalWrite(DIGIT3_PIN, LOW);
    digitalWrite(DIGIT4_PIN, LOW);
    digitalWrite(COUNTER_EN_PIN, HIGH);
    digitalWrite(COUNTER_RST_PIN, HIGH);
    this->delay_microseconds_fast(5);
    digitalWrite(COUNTER_RST_PIN, LOW);
  
    const esp_timer_create_args_t display_timer_args = {
        .callback = [](void *arg) { static_cast<NixieDisplay *>(arg)->update_display_timer_callback(); },
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "nixie_display_timer"};
    esp_timer_create(&display_timer_args, &this->display_timer_);
    esp_timer_start_periodic(this->display_timer_, 5000); // 5ms
  }
  
  void NixieDisplay::update() {
    this->update_display();
  }
  
  void NixieDisplay::update_display_timer_callback() {
    this->update_display();
  }
  
  void NixieDisplay::set_display_text(const std::string &text) {
    this->display_text_ = text;
    if (this->display_text_.length() > 4) {
      this->display_text_ = this->display_text_.substr(0, 4);
    }
  
    for (int i = 0; i < 4; i++) {
      if (i < this->display_text_.length() && isdigit(this->display_text_[i])) {
        this->digits_[i] = this->display_text_[i] - '0';
      } else {
        this->digits_[i] = 0;
      }
    }
  }
  
  void NixieDisplay::update_display() {
    digitalWrite(DIGIT1_PIN, LOW);
    digitalWrite(DIGIT2_PIN, LOW);
    digitalWrite(DIGIT3_PIN, LOW);
    digitalWrite(DIGIT4_PIN, LOW);
  
    this->set_counter(this->digits_[this->active_digit_]);
  
    switch (this->active_digit_) {
      case 0:
        digitalWrite(DIGIT1_PIN, HIGH);
        break;
      case 1:
        digitalWrite(DIGIT2_PIN, HIGH);
        break;
      case 2:
        digitalWrite(DIGIT3_PIN, HIGH);
        break;
      case 3:
        digitalWrite(DIGIT4_PIN, HIGH);
        break;
    }
  
    this->active_digit_ = (this->active_digit_ + 1) % 4;
  }
  
  void NixieDisplay::set_counter(int value) {
    digitalWrite(COUNTER_RST_PIN, HIGH);
    this->delay_microseconds_fast(5);
    digitalWrite(COUNTER_RST_PIN, LOW);
  
    for (int i = 0; i < value; i++) {
      digitalWrite(COUNTER_EN_PIN, LOW);
      this->delay_microseconds_fast(5);
      digitalWrite(COUNTER_CLK_PIN, HIGH);
      this->delay_microseconds_fast(5);
      digitalWrite(COUNTER_CLK_PIN, LOW);
      digitalWrite(COUNTER_EN_PIN, HIGH);
    }
  }
  
  void NixieDisplay::delay_microseconds_fast(uint32_t us) {
    uint32_t start = esp_timer_get_time();
    while (esp_timer_get_time() - start < us) {
      asm volatile("nop");
    }
  }
  
  void NixieDisplay::draw_pixel_at(int x, int y, Color color){
      //This display doesn't use x,y coordinates, so this method is not really used.
      //But it must be implemented.
  }
  
  }  // namespace nixie
  }  // namespace esphome