#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace nanospec_mca {

class NanospecMcaComponent : public Component, public uart::UARTDevice {
 public:
  NanospecMcaComponent() = default;

  void setup() override;
  void loop() override;
  void dump_config() override;

  void add_on_count_callback(std::function<void(std::array<uint16_t, BUCKET_SIZE>)> callback) {
    this->on_count_callback_.add(std::move(callback));
  }

 protected:
  enum State {
    WAITING_FIRST_BYTE,
    WAITING_SECOND_BYTE,
  };

  void send_cmd_(const std::string &message);
  void initialize_nanospec();
  uint32_t last_serial_data_time;
  uint8_t payload_;
  State state_ = WAITING_FIRST_BYTE;
  std::array<uint16_t, BUCKET_SIZE> count_list;
  uint16_t count;
  CallbackManager<void(std::array<uint16_t, BUCKET_SIZE>)> on_count_callback_;
};

class NanospecMcaCountTrigger : public Trigger<std::array<uint16_t, BUCKET_SIZE>> {
 public:
  explicit NanospecMcaCountTrigger(NanospecMcaComponent *parent) {
    parent->add_on_count_callback([this](const std::array<uint16_t, BUCKET_SIZE> &channel) { this->trigger(channel); });
  }
};

}  // namespace nanospec_mca
}  // namespace esphome
