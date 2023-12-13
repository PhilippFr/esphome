import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import automation
from esphome.components import uart

from esphome.const import (
    CONF_ID,
    CONF_TRIGGER_ID,
)

CODEOWNERS = ["@PhilippFr"]
DEPENDENCIES = ["uart"]

# CONF_CPM = "cpm"
CONF_ON_COUNT = "on_count"
CONF_BUCKET_SIZE = "bucket_size"

nanospec_mca_ns = cg.esphome_ns.namespace("nanospec_mca")
NanospecMcaComponent = nanospec_mca_ns.class_(
    "NanospecMcaComponent", uart.UARTDevice, cg.Component
)
# nanospecMcaSensor = nanospec_mca_ns.class_("NanospecMcaSensor", sensor.Sensor, cg.PollingComponent)

NanospecMcaCountTrigger = nanospec_mca_ns.class_(
    "NanospecMcaCountTrigger", automation.Trigger
)

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(NanospecMcaComponent),
            # cv.Optional(CONF_CPM): sensor.sensor_schema(
            #     unit_of_measurement="cpm",
            #     icon="radioactive",
            #     accuracy_decimals=0,
            #     device_class="radiation",
            #     state_class=STATE_CLASS_MEASUREMENT,
            # ),
            cv.Optional(CONF_BUCKET_SIZE, default=128): cv.int_range(min=1, max=65535),
            cv.Optional(CONF_ON_COUNT): automation.validate_automation(
                {
                    cv.GenerateID(CONF_TRIGGER_ID): cv.declare_id(
                        NanospecMcaCountTrigger
                    ),
                }
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(uart.UART_DEVICE_SCHEMA),
    # .extend(cv.polling_component_schema("never")),
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "nanospec_mca",
    baud_rate=9600,
    require_tx=True,
    require_rx=True,
    data_bits=8,
    parity=None,
    stop_bits=1,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)
    cg.add_define("BUCKET_SIZE", int(config[CONF_BUCKET_SIZE]))

    # if CONF_CPM in config:
    #     sens = await sensor.new_sensor(config[CONF_CPM])
    #     cg.add(var.set_cpm_sensor(sens))

    for conf in config.get(CONF_ON_COUNT, []):
        trigger = cg.new_Pvariable(conf[CONF_TRIGGER_ID], var)
        await automation.build_automation(
            trigger,
            [
                (
                    cg.RawExpression(f"std::array<uint16_t, {config[CONF_BUCKET_SIZE]}>"),
                    "channel",
                )
            ],
            conf,
        )
