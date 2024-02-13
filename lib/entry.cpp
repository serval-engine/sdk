
#include "serval_extension.hpp"

#include <cr.h>
#include <spdlog/spdlog.h>
#include <imgui.h>

/********************************************************************************
 * Plugin entrypoint and setup
 ********************************************************************************/

static serval::Init* engine_api = nullptr;
static bool is_reloading = false;

CR_EXPORT int cr_main(cr_plugin* ctx, cr_op operation)
{
    if (engine_api == nullptr) {
        auto init = static_cast<serval::ExtensionInit*>(ctx->userdata);
        // Set spdlog logger
        spdlog::set_default_logger(init->logger);
        // Set Dear ImGUI context
        ImGui::SetCurrentContext(init->imgui_context);
        // Store the engine API for later use
        engine_api = init->engine_init_api;
        // First load is never a reload
        is_reloading = false;
    }
    switch (operation) {
        // Hot-code reloading
        case CR_LOAD:
        {
            if (is_reloading) {
                is_reloading = false;
                serval_extension_reload(*engine_api);
            } else {
                serval_extension_load(*engine_api);
            }
            break;
        }
        case CR_UNLOAD:
        {
            is_reloading = true;
            break;
        }
        // Update step
        case CR_STEP:
        {
            break;
        }
        // Close and unload module
        case CR_CLOSE:
        {
            serval_extension_unload(*engine_api);
            break;
        }
    }
    return 0;
}
