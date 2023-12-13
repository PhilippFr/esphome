#include "nanospec_mca.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace nanospec_mca {

static const char *const TAG = "nanospec_mca";

const char ASCII_CR = 0x0D;

static const uint8_t RE_INIT_TIMEOUT = 20;

static const std::string NanoSpec_Mca_ADC_D = "adcd";  // Sent 30ms before ADC ON
static const std::string NanoSpec_Mca_ADC_ON = "adcon";
static const std::string NanoSpec_Mca_ADC_OFF = "adcoff";
static const std::string NanoSpec_Mca_ADC_S = "adcs";  // Sent 30ms after ADC OFF

enum State {
  WAITING_FIRST_BYTE,
  WAITING_SECOND_BYTE,
};

uint32_t last_serial_data_time = 0;

uint8_t previousByte = 0x00;
State state_ = WAITING_FIRST_BYTE;

std::array<uint16_t, BUCKET_SIZE> count_list;
uint16_t count = 0;

void NanospecMcaComponent::send_cmd_(const std::string &message) {
  ESP_LOGD(TAG, "S: %s", message.c_str());
  this->write_str(message.c_str());
  this->write_byte(ASCII_CR);
}

void NanospecMcaComponent::initialize_nanospec() {
  ESP_LOGI(TAG, "Initializing NanoSPEC MCA");
  this->send_cmd_(NanoSpec_Mca_ADC_OFF);
  delay_microseconds_safe(30000);
  this->send_cmd_(NanoSpec_Mca_ADC_S);
  delay_microseconds_safe(2000000);
  this->send_cmd_(NanoSpec_Mca_ADC_D);
  delay_microseconds_safe(30000);
  this->send_cmd_(NanoSpec_Mca_ADC_ON);
}

void NanospecMcaComponent::setup() {
  this->initialize_nanospec();
  this->last_serial_data_time = millis();  // Record the time of initialization
}

void NanospecMcaComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "nanpspec_mca:");
  ESP_LOGCONFIG(TAG, "  Bucket-Size: %d", BUCKET_SIZE);
  // LOG_SENSOR("  ", "cpm", this->cpm_sensor_);
  // LOG_UPDATE_INTERVAL(this);
  this->check_uart_settings(9600);
}

void NanospecMcaComponent::loop() {
  while (this->available() != 0) {
    uint8_t data;
    this->read_byte(&data);

    if (state_ == WAITING_FIRST_BYTE && (data & 0xC0) == 0x80) {
      // Start of payload detected
      this->payload_ = (data & 0x3F);
      this->state_ = WAITING_SECOND_BYTE;
    } else if (state_ == WAITING_SECOND_BYTE && (data & 0xC0) == 0xC0) {
      // End of payload detected
      this->last_serial_data_time = millis();  // Update the time when serial data is received
      this->payload_ |= (data & 0x3F) << 6;
      this->state_ = WAITING_FIRST_BYTE;

      // Store the count_channel in the array
      this->count_list[this->count] = payload_;

      // Increment count and check if it reached 10
      this->count++;

      if (this->count >= BUCKET_SIZE) {
        // Trigger the custom action with the array of count_channel
        this->on_count_callback_.call(this->count_list);
        // Reset count and array after triggering callback
        this->count = 0;
        memset(&this->count_list, 0, sizeof(this->count_list));
      }
    } else {
      // Invalid byte, reset state
      this->state_ = WAITING_FIRST_BYTE;
    }
  }
  // Check if there has been no serial data for RE_INIT_TIMEOUT seconds
  if (millis() - this->last_serial_data_time > RE_INIT_TIMEOUT * 1000) {
    // Re-initialize NanoSPEC MCA after 10 seconds of no serial data
    ESP_LOGW(TAG, "No count packet received for %d seconds, re-initializing!", RE_INIT_TIMEOUT);
    this->initialize_nanospec();
    this->last_serial_data_time = millis();  // Record the time of initialization
  }
}

}  // namespace nanospec_mca
}  // namespace esphome
