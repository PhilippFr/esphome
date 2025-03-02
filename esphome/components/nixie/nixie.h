#include "esphome/components/display/display.h"
#include "esphome/core/component.h"
#include "esphome/core/log.h"
#include "esp_timer.h"

namespace esphome {
namespace nixie {

class NixieDisplay : public display::Display, public Component {
 public:
  void setup() override;
  void update() override;

  void IRAM_ATTR update_display_timer_callback();

  void set_display_text(const std::string &text);

 protected:
  void draw_pixel_at(int x, int y, Color color) override;
  void update_display();
  void set_counter(int value);
  void delay_microseconds_fast(uint32_t us);

 private:
  static const int DIGIT1_PIN = 2;
  static const int DIGIT2_PIN = 0;
  static const int DIGIT3_PIN = 21;
  static const int DIGIT4_PIN = 20;
  static const int COUNTER_RST_PIN = 8;
  static const int COUNTER_CLK_PIN = 9;
  static const int COUNTER_EN_PIN = 10;

  esp_timer_handle_t display_timer_;
  int active_digit_ = 0;
  std::string display_text_;
  int digits_[4] = {0, 0, 0, 0};
};

}  // namespace nixie
}  // namespace esphome