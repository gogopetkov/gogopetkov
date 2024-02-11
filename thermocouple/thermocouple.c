#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/widget.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/variable_item_list.h>

#include <furi.h>
#include <furi_hal.h>

#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>

#include "mcp960X.h"


/* 
 - 3V3 (3V3, pin 9)  = VCC
 - GND (GND, pin 18) = GND
 - SCL (C0, pin 16)  = SCL
 - SDA (C1, pin 15)  = SDA
 */


typedef enum {
    tCoupleScenesMainMenuScene,
    tCoupleScenesDisplayTemperatureScene,
    tCoupleScenesConfigThermcoupleScene,
    tCoupleScenesSceneCount,
} tCoupleScenesScene;

typedef enum {
    tCoupleScenesSubmenuView,
    tCoupleScenesWidgetView,
    tCoupleScenesConfigView,
} tCoupleScenesView;

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    TextInput* text_input;
    VariableItemList* variable_item_list;
    uint8_t variable_list_index;
    uint8_t filter_list_index ;
    uint8_t scale_list_index;
    char* user_name;
    uint8_t user_name_size;
} App;

typedef enum {
    tCoupleMainMenuDisplayTemp,
    tCoupleainMenuConfig,
} tCoupleScenesMainMenuIndex;

typedef enum {
    tCoupleMainMenuDisplayTempEvent,
    tCoupleainMenuConfigEvent,
} tCoupleScenesMainMenuEvent;

typedef enum {
    BasicScenesGreetingInputSceneSaveEvent,
} BasicScenesGreetingInputEvent;

#define THERMOCOUPLE_TYPE_COUNT 8
const char* thermocouple_type_text[THERMOCOUPLE_TYPE_COUNT] = {
    "K",
    "J",
    "T",
    "N",
    "S",
    "E",
    "B",
    "R",
};

#define THERMOCOUPLE_FILTER_COUNT 8
const char* thermocouple_filter_text[THERMOCOUPLE_FILTER_COUNT] = {
    "0",
    "1",
    "2",
    "3",
    "4",
    "5",
    "6",
    "7",
};

#define TEMPERATURE_SCALE_COUNT 2
const char* themperature_scale_text[TEMPERATURE_SCALE_COUNT] = {
    "C",
    "F",
};

void tcouple_scenes_menu_callback(void* context, uint32_t index) {
    App* app = context;
    switch(index) {
    case tCoupleMainMenuDisplayTemp:
        scene_manager_handle_custom_event(app->scene_manager, tCoupleMainMenuDisplayTempEvent);
        break;
    case tCoupleainMenuConfig:
        scene_manager_handle_custom_event(app->scene_manager, tCoupleainMenuConfigEvent);
        break;
    }
}

// main menu scene

void tcouple_scenes_main_menu_scene_on_enter(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, "Thermcouple temp");
    submenu_add_item(
        app->submenu, "Temperature", tCoupleMainMenuDisplayTemp, tcouple_scenes_menu_callback, app);
    submenu_add_item(
        app->submenu, "Config ", tCoupleainMenuConfig, tcouple_scenes_menu_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, tCoupleScenesSubmenuView);
}

bool tcouple_scenes_main_menu_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case tCoupleMainMenuDisplayTempEvent:
            scene_manager_next_scene(app->scene_manager, tCoupleScenesDisplayTemperatureScene);
            consumed = true;
            break;
        case tCoupleainMenuConfigEvent:
            scene_manager_next_scene(app->scene_manager, tCoupleScenesConfigThermcoupleScene);
            consumed = true;
            break;
        }
        break;
    default:
        break;
    }
    return consumed;
}

void tcouple_scenes_main_menu_scene_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->submenu);
}

//  display temperature scene

void tcouple_scenes_display_temperature_scene_on_enter(void* context) {
    App* app = context;
    uint8_t addr = i2c_find_device();
    
    FURI_LOG_I("device addr", " addr: %x", addr>>1);

    i2c_init_mcp9600(0xC0); 

    uint16_t hj = read_hot_junction_mcp9600(0xC0);
    double tcf = 0.0625 * hj;
    char * tc = malloc(30);
    char * cj = malloc(30);
    snprintf(tc, 31, "hot junction  %-5.2f", tcf);

    uint16_t cjt = read_cold_junction_mcp9600(0xC0);
    double tcj = 0.0625 * cjt;
    snprintf(cj, 31, "cold junction %-5.2f", tcj);

    widget_reset(app->widget);
    widget_add_string_element(
        app->widget, 15, 10, AlignLeft, AlignCenter, FontPrimary, "Temperature:");
    widget_add_string_element(
        app->widget, 10, 30, AlignLeft, AlignCenter, FontPrimary, cj);
    widget_add_string_element(
        app->widget, 10, 45, AlignLeft, AlignCenter, FontPrimary,  tc);
  
    FURI_LOG_I("thermocouple", "Fdisplay_temperature_scene_on_enter ...");
    view_dispatcher_switch_to_view(app->view_dispatcher, tCoupleScenesWidgetView);
    free(tc);
}

bool tcouple_scenes_display_temperature_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false; // event not handled.
}

void tcouple_scenes_display_temperature_scene_on_exit(void* context) {
    UNUSED(context);
}

// config thermocouple scene

void tcouple_scenes_text_input_callback(void* context) {
    // UNUSED(context);
    App* app = context;
    scene_manager_handle_custom_event(app->scene_manager, BasicScenesGreetingInputSceneSaveEvent);
}

static void tcouple_scenes_set_thermocouple_callback(VariableItem* item) {

    App* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, thermocouple_type_text[index]);
    app->variable_list_index = index;

}

static void tcouple_scenes_set_filter_callback(VariableItem* item) {

    App* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, thermocouple_filter_text[index]);
    app->filter_list_index = index;

}

static void tcouple_scenes_set_scale_callback(VariableItem* item) {

    App* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, themperature_scale_text[index]);
    app->scale_list_index = index;

}




// static void tcouple_scenes_set_thermocouple_callback(void* context) {

//     App* app = context;
    
//     uint8_t index = app->variable_item_list.variable_item_get_current_value_index;
//     variable_item_set_current_value_text(item, thermocouple_type_text[index]);
//     app->variable_list_index = index;
// }


void tcouple_scenes_config_thermcouple_scene_on_enter(void* context) {
    App* app = context;


    VariableItem *item, *item1, *item2;


    item = variable_item_list_add(
        app->variable_item_list,
        "TC Type",
        THERMOCOUPLE_TYPE_COUNT,
        tcouple_scenes_set_thermocouple_callback,
        context);

    uint8_t index = app->variable_list_index;
    variable_item_set_current_value_index(item, index);
    variable_item_set_current_value_text(item, thermocouple_type_text[index]);

    

    // VariableItem* item1;
    item1 = variable_item_list_add(
        app->variable_item_list,
        "Filter",
        THERMOCOUPLE_FILTER_COUNT,
        tcouple_scenes_set_filter_callback,
        context);

    index = app->filter_list_index;
    variable_item_set_current_value_index(item1, index);
    variable_item_set_current_value_text(item1, thermocouple_filter_text[index]);


     item2 = variable_item_list_add(
        app->variable_item_list,
        "Scale",
        TEMPERATURE_SCALE_COUNT,
        tcouple_scenes_set_scale_callback,
        context);

    index = app->scale_list_index;
    variable_item_set_current_value_index(item2, index);
    variable_item_set_current_value_text(item2, themperature_scale_text[index]);

    
    view_dispatcher_switch_to_view(app->view_dispatcher, tCoupleScenesConfigView);
}

bool tcouple_scenes_config_thermcouple_scene_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false; // event not handled.


    // App* app = context;
    // bool consumed = false;
    // if(event.type == SceneManagerEventTypeCustom) {
    //     if(event.event == BasicScenesGreetingInputSceneSaveEvent) {
    //         scene_manager_next_scene(app->scene_manager, tCoupleScenesMainMenuScene);
    //         consumed = true;
    //     }
    // }
    // return consumed;


}
void tcouple_scenes_config_thermcouple_scene_on_exit(void* context) {
    // UNUSED(context);
    App* app = context;
    variable_item_list_set_selected_item(app->variable_item_list, 0);
    variable_item_list_reset(app->variable_item_list);
}

void (*const tcouple_scenes_scene_on_enter_handlers[])(void*) = {

    tcouple_scenes_main_menu_scene_on_enter,
    tcouple_scenes_display_temperature_scene_on_enter,
    tcouple_scenes_config_thermcouple_scene_on_enter,
};

bool (*const tcouple_scenes_scene_on_event_handlers[])(void*, SceneManagerEvent) = {

    tcouple_scenes_main_menu_scene_on_event,
    tcouple_scenes_display_temperature_scene_on_event,
    tcouple_scenes_config_thermcouple_scene_on_event

};

void (*const tcouple_scenes_scene_on_exit_handlers[])(void*) = {

    tcouple_scenes_main_menu_scene_on_exit,
    tcouple_scenes_display_temperature_scene_on_exit,
    tcouple_scenes_config_thermcouple_scene_on_exit,

};

static const SceneManagerHandlers tcouple_scenes_scene_manager_handlers = {
    .on_enter_handlers = tcouple_scenes_scene_on_enter_handlers,
    .on_event_handlers = tcouple_scenes_scene_on_event_handlers,
    .on_exit_handlers = tcouple_scenes_scene_on_exit_handlers,
    .scene_num = tCoupleScenesSceneCount,
};

static bool tcouple_scene_custom_callback(void* context, uint32_t custom_event) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, custom_event);
}

bool tcouple_scene_back_event_callback(void* context) {
    furi_assert(context);
    App* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static App* app_alloc() {
    App* app = malloc(sizeof(App));
    app->user_name_size = 16;
    app->variable_list_index = 0;
    app->filter_list_index = 0;
    app->scale_list_index = 0;
    app->user_name = malloc(app->user_name_size);
    app->scene_manager = scene_manager_alloc(&tcouple_scenes_scene_manager_handlers, app);
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, tcouple_scene_custom_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, tcouple_scene_back_event_callback);
    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, tCoupleScenesSubmenuView, submenu_get_view(app->submenu));
    app->widget = widget_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, tCoupleScenesWidgetView, widget_get_view(app->widget));
    // app->text_input = text_input_alloc();
    // view_dispatcher_add_view(
    //     app->view_dispatcher, tCoupleScenesConfigView, text_input_get_view(app->text_input));

    app->variable_item_list = variable_item_list_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, tCoupleScenesConfigView, variable_item_list_get_view(app->variable_item_list));

    return app;
}

static void app_free(App* app) {
    furi_assert(app);
    view_dispatcher_remove_view(app->view_dispatcher, tCoupleScenesSubmenuView);
    view_dispatcher_remove_view(app->view_dispatcher, tCoupleScenesWidgetView);
    view_dispatcher_remove_view(app->view_dispatcher, tCoupleScenesConfigView);
    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);
    submenu_free(app->submenu);
    widget_free(app->widget);
    // text_input_free(app->text_input);
    variable_item_list_free(app->variable_item_list);
    free(app);
}

int32_t thermocouple_app(void* p) {
    UNUSED(p);
    App* app = app_alloc();

    Gui* gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(app->scene_manager, tCoupleScenesMainMenuScene);
    view_dispatcher_run(app->view_dispatcher);

    app_free(app);
    return 0;
}

