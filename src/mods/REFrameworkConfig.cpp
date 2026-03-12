#include "../REFramework.hpp"

#include "REFrameworkConfig.hpp"

std::shared_ptr<REFrameworkConfig>& REFrameworkConfig::get() {
     static std::shared_ptr<REFrameworkConfig> instance{std::make_shared<REFrameworkConfig>()};
     return instance;
}

std::optional<std::string> REFrameworkConfig::on_initialize() {
    namespace fs = std::filesystem;
    fonts.clear();
    fonts.push_back("DEFAULT");

    const auto fonts_path = REFramework::get_persistent_dir() / "reframework" / "fonts";
    fs::create_directories(fonts_path);

    for (const auto& entry : fs::directory_iterator(fonts_path)) {
        if (fs::is_regular_file(entry)) {
            const auto ext = entry.path().extension();
            if (ext == ".otf" || ext == ".ttf") {
                fonts.push_back(entry.path().filename().string());
            }
        }
    }

    m_font_file = ModComboString::create(generate_name("FontFile"), fonts, "DEFAULT");

    g_framework->set_font(m_font_file->value());

    m_options.push_back(*m_font_file);

    return Mod::on_initialize();
}

void REFrameworkConfig::on_draw_ui() {
    if (!ImGui::CollapsingHeader("配置")) {
        return;
    }

    ImGui::TreePush("配置");

    bool changed = false;

    changed |= m_menu_key->draw("打开菜单按键");
    changed |= m_show_cursor_key->draw("显示指针轨迹按键");
    changed |= m_remember_menu_state->draw("记忆菜单的打开/关闭状态");
    changed |= m_always_show_cursor->draw("打开菜单时绘制指针轨迹");

    if (m_font_file->draw("字体")) {
        g_framework->set_font(m_font_file->value());
        changed = true;
    }

    if (m_font_size->draw("字体大小")) {
        g_framework->set_font_size(m_font_size->value());
        changed = true;
    }

    if (changed) {
        g_framework->request_save_config();
    }

    ImGui::TreePop();
}

void REFrameworkConfig::on_frame() {
    if (m_show_cursor_key->is_key_down_once()) {
        m_always_show_cursor->toggle();
    }
}

void REFrameworkConfig::on_config_load(const utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_load(cfg);
    }

    if (m_remember_menu_state->value()) {
        g_framework->set_draw_ui(m_menu_open->value(), false);
    }
    
    g_framework->set_font(m_font_file->value());
    g_framework->set_font_size(m_font_size->value());
}

void REFrameworkConfig::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}
