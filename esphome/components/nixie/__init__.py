# _init_.py
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome.const import CONF_ID

CODEOWNERS = ["@PhilippFr"]

nixie_ns = cg.esphome_ns.namespace("nixie")
NixieDisplay = nixie_ns.class_("NixieDisplay", cg.Component, display.DisplayBuffer)

CONFIG_SCHEMA = display.FULL_DISPLAY_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(NixieDisplay),
    }
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await display.register_display(var, config)