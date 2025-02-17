#include "ui.h"

static lv_obj_t *ui_Screen3;
static lv_obj_t *warning_img;
static lv_obj_t *warning_info;

LV_FONT_DECLARE(yellow_warning);
LV_FONT_DECLARE(big_font);

void set_warning_info(char* warning_message)
{
    lv_label_set_text(warning_info, "Warning: ");
    lv_label_ins_text(warning_info,LV_LABEL_POS_LAST,warning_message);
}

void show_warning_ui()
{
    lv_disp_load_scr(ui_Screen3);
}

void ui_screen_warning_init()
{
    ui_Screen3 = lv_obj_create(NULL, NULL);

    warning_img = lv_img_create(ui_Screen3, NULL);
    lv_img_set_src(warning_img, &yellow_warning);
    lv_img_set_zoom(warning_img, 200);
    lv_obj_set_x(warning_img, 90);
    lv_obj_set_y(warning_img, 75);
    lv_img_set_pivot(warning_img, 0, 0); /*Rotate around the top left corner*/

    warning_info = lv_label_create(ui_Screen3, NULL);
    lv_label_set_recolor(warning_info, true);
    lv_obj_set_style_local_text_font(warning_info, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &big_font);
    lv_obj_set_width(warning_info, 200);  /// 1
    lv_obj_set_height(warning_info, 150); /// 1
    lv_obj_set_x(warning_info, 0);
    lv_obj_set_y(warning_info, 250);
    lv_label_set_text(warning_info, "Warning:");
}
MSH_CMD_EXPORT(show_warning_ui, show warning screen);
