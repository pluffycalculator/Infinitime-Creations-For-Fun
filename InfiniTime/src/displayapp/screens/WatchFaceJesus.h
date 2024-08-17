#pragma once

#include <lvgl/src/lv_core/lv_obj.h>
#include <chrono>
#include <cstdint>
#include <memory>
#include "displayapp/screens/Screen.h"
#include "components/datetime/DateTimeController.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include "displayapp/screens/BatteryIcon.h"
#include "utility/DirtyValue.h"
#include "displayapp/widgets/StatusIcons.h"
#include "displayapp/apps/Apps.h"

namespace Pinetime {
  namespace Controllers {
    class Settings;
    class Battery;
    class Ble;
    class NotificationManager;
    class HeartRateController;
    class MotionController;
  }

  namespace Applications {
    namespace Screens {

      class WatchFaceJesus : public Screen {
      public:
        WatchFaceJesus(Controllers::DateTime& dateTimeController,
                       const Controllers::Battery& batteryController,
                       const Controllers::Ble& bleController,
                       Controllers::NotificationManager& notificationManager,
                       Controllers::Settings& settingsController,
                       Controllers::HeartRateController& heartRateController,
                       Controllers::MotionController& motionController);
        ~WatchFaceJesus() override;
        
        void Refresh() override;

      private:
        uint8_t sHour, sMinute, sSecond;

        Utility::DirtyValue<uint8_t> batteryPercentRemaining {0};
        Utility::DirtyValue<bool> isCharging {};
        Utility::DirtyValue<bool> bleState {};
        Utility::DirtyValue<uint32_t> stepCount {};
        Utility::DirtyValue<uint8_t> heartbeat {};
        Utility::DirtyValue<bool> heartbeatRunning {};
        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>> currentDateTime;
        Utility::DirtyValue<bool> notificationState {false};
        Utility::DirtyValue<std::chrono::time_point<std::chrono::system_clock, std::chrono::days>> currentDate;
        
        lv_style_t shadow_style;
        
        lv_point_t minute_point_shadow[2];
        lv_point_t hour_point_shadow[2];
        lv_point_t second_point_shadow[2];

        lv_obj_t* minute_shadow;
        lv_obj_t* hour_shadow;
        lv_obj_t* second_shadow;
        
        lv_obj_t* heartbeatIcon;
        lv_obj_t* heartbeatValue;
        lv_obj_t* stepIcon;
        lv_obj_t* stepValue;
        lv_obj_t* notificationIcon;

        lv_obj_t* minor_scales;
        lv_obj_t* major_scales;
        lv_obj_t* large_scales;
        lv_obj_t* twelve;

        lv_obj_t* hour_body;
        lv_obj_t* minute_body;
        lv_obj_t* second_body;

        lv_point_t hour_point[2];
        lv_point_t minute_point[2];
        lv_point_t second_point[2];

        lv_style_t hour_line_style;
        lv_style_t minute_line_style;
        lv_style_t second_line_style;

        lv_obj_t* label_date_day;
        lv_obj_t* plugIcon;
        lv_obj_t* bleIcon;

        BatteryIcon batteryIcon;

        Controllers::DateTime& dateTimeController;
        const Controllers::Battery& batteryController;
        const Controllers::Ble& bleController;
        Controllers::NotificationManager& notificationManager;
        Controllers::Settings& settingsController;
        Controllers::HeartRateController& heartRateController;
        Controllers::MotionController& motionController;

        lv_task_t* taskRefresh;

        void UpdateClock();
        void SetBatteryIcon();
      };
    }

    template <>
    struct WatchFaceTraits<WatchFace::Jesus> {
      static constexpr WatchFace watchFace = WatchFace::Jesus;
      static constexpr const char* name = "Jesus";

      static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::WatchFaceJesus(controllers.dateTimeController,
                                            controllers.batteryController,
                                            controllers.bleController,
                                            controllers.notificationManager,
                                            controllers.settingsController,
                                            controllers.heartRateController,
                                            controllers.motionController);
      };

      static bool IsAvailable(Pinetime::Controllers::FS& /*filesystem*/) {
        return true;
      }
    };
  }
}

