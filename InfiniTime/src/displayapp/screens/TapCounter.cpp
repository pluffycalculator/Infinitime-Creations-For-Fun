#include "displayapp/screens/TapCounter.h"
#include "components/motor/MotorController.h"
#include "components/datetime/DateTimeController.h"
#include "components/timer/Timer.h"
#include <string>

class TapCounterModel {
public:
    TapCounterModel() : count(0) {}

    void increase() { count++; }
    void decrease() { count--; }
    int getCount() const { return count; }

private:
    int count;
};

namespace Pinetime::Applications::Screens {

    static void TimerCallback(TimerHandle_t xTimer) {
        auto* obj = static_cast<TapCounter*>(pvTimerGetTimerID(xTimer));
        obj->Vibrate();
    }

    TapCounter::TapCounter(Controllers::MotorController& motorController,
                           Controllers::DateTime& dateTimeController,
                           Controllers::FS& fs)
    : motorController(motorController),
      dateTimeController(dateTimeController),
      fs(fs),
      motorTimer(this, TimerCallback),
      model(std::make_unique<TapCounterModel>()),
      startTime(dateTimeController.Uptime()) {
        const auto mainContainer = lv_cont_create(lv_scr_act(), nullptr);
        lv_obj_set_style_local_bg_color(mainContainer, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
        lv_obj_set_style_local_border_color(mainContainer, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GRAY);
        lv_obj_set_style_local_border_width(mainContainer, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_cont_set_fit(mainContainer, LV_FIT_PARENT);
        
        lv_cont_set_layout(mainContainer, LV_LAYOUT_OFF);
        
        numberLabel = lv_label_create(mainContainer, nullptr);
        lv_label_set_text_static(numberLabel, "0");
        lv_obj_set_style_local_text_font(numberLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &jetbrains_mono_extrabold_compressed);
        lv_obj_set_style_local_text_color(numberLabel, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
        lv_obj_align(numberLabel, mainContainer, LV_ALIGN_CENTER, 0, 0);

        Refresh();
    }

    TapCounter::~TapCounter() {
        lv_obj_clean(lv_scr_act());
    }

    void TapCounter::Vibrate() {
        motorController.RunForDuration(30);
    }

bool TapCounter::OnTouchEvent(TouchEvents event) {
    if (event == TouchEvents::Tap && !firstEventRejected) {
        firstEventRejected = true;
        return false;
    }

    if (event == TouchEvents::DoubleTap ||
                   event == TouchEvents::SwipeUp) {
        model->increase();
        motorController.RunForDuration(30);
        Refresh();
        return true;
    } else if (event == TouchEvents::LongTap ||
                   event == TouchEvents::SwipeDown) {
        model->decrease();
        motorController.RunForDuration(30);
        Refresh();
        return true;
    }

    return false;
}


    bool TapCounter::OnButtonPushed() {
        // Handle button pushed event if necessary
        return true;
    }

    void TapCounter::Refresh() {
        lv_label_set_text_fmt(numberLabel, "%d", model->getCount());
    }
}

