from esphome import automation, pins
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@PhilippFr"]

nixie_ns = cg.esphome_ns.namespace("nixie")
NixieDisplay = nixie_ns.class_("NixieDisplay", cg.Component)

SetDisplayTextAction = nixie_ns.class_("SetDisplayTextAction", automation.Action)

CONF_DISPLAY_TEXT = "display_text"
CONF_DIGIT1_PIN = "digit1_pin"
CONF_DIGIT2_PIN = "digit2_pin"
CONF_DIGIT3_PIN = "digit3_pin"
CONF_DIGIT4_PIN = "digit4_pin"
CONF_COUNTER_RST_PIN = "counter_rst_pin"
CONF_COUNTER_CLK_PIN = "counter_clk_pin"
CONF_COUNTER_EN_PIN = "counter_en_pin"
CONF_MULTIPLEXING_SPEED = "multiplexing_speed"

CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(NixieDisplay),
            cv.Optional(
                CONF_DIGIT1_PIN, default=2
            ): pins.internal_gpio_output_pin_schema,
            cv.Optional(
                CONF_DIGIT2_PIN, default=0
            ): pins.internal_gpio_output_pin_schema,
            cv.Optional(
                CONF_DIGIT3_PIN, default=21
            ): pins.internal_gpio_output_pin_schema,
            cv.Optional(
                CONF_DIGIT4_PIN, default=20
            ): pins.internal_gpio_output_pin_schema,
            cv.Optional(
                CONF_COUNTER_RST_PIN, default=8
            ): pins.internal_gpio_output_pin_schema,
            cv.Optional(
                CONF_COUNTER_CLK_PIN, default=9
            ): pins.internal_gpio_output_pin_schema,
            cv.Optional(
                CONF_COUNTER_EN_PIN, default=10
            ): pins.internal_gpio_output_pin_schema,
            cv.Optional(
                CONF_MULTIPLEXING_SPEED, default="5000us"
            ): cv.positive_time_period_microseconds,
        }
    ),
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    digit1_pin = await cg.gpio_pin_expression(config[CONF_DIGIT1_PIN])
    cg.add(var.set_digit1_pin(digit1_pin))

    digit2_pin = await cg.gpio_pin_expression(config[CONF_DIGIT2_PIN])
    cg.add(var.set_digit2_pin(digit2_pin))

    digit3_pin = await cg.gpio_pin_expression(config[CONF_DIGIT3_PIN])
    cg.add(var.set_digit3_pin(digit3_pin))

    digit4_pin = await cg.gpio_pin_expression(config[CONF_DIGIT4_PIN])
    cg.add(var.set_digit4_pin(digit4_pin))

    counter_rst_pin = await cg.gpio_pin_expression(config[CONF_COUNTER_RST_PIN])
    cg.add(var.set_counter_rst_pin(counter_rst_pin))

    counter_clk_pin = await cg.gpio_pin_expression(config[CONF_COUNTER_CLK_PIN])
    cg.add(var.set_counter_clk_pin(counter_clk_pin))

    counter_en_pin = await cg.gpio_pin_expression(config[CONF_COUNTER_EN_PIN])
    cg.add(var.set_counter_en_pin(counter_en_pin))

    cg.add(var.set_multiplexing_speed(config[CONF_MULTIPLEXING_SPEED]))


@automation.register_action(
    "nixie.set_display_text",
    SetDisplayTextAction,
    cv.maybe_simple_value(
        {
            cv.GenerateID(): cv.use_id(NixieDisplay),
            cv.Required(CONF_DISPLAY_TEXT): cv.templatable(cv.string),
        },
        key=CONF_DISPLAY_TEXT,
    ),
)
async def nixie_set_display_text_to_code(config, action_id, template_arg, args):
    var = cg.new_Pvariable(action_id, template_arg)
    await cg.register_parented(var, config[CONF_ID])
    template_ = await cg.templatable(config[CONF_DISPLAY_TEXT], args, cg.std_string)
    cg.add(var.set_display_text(template_))
    return var
