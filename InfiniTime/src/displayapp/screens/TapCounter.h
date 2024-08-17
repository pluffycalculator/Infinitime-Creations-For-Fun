#pragma once

#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Label.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/screens/ScreenList.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"

class TapCounterModel;

namespace Pinetime {
    namespace Applications {
        namespace Screens {
            class TapCounter : public Screen {
            public:
                TapCounter(Controllers::MotorController& motorController,
                           Controllers::DateTime& dateTimeController,
                           Controllers::FS& fs);
                ~TapCounter() override;
                void Vibrate();
                void Refresh() override;
                bool OnTouchEvent(TouchEvents event) override;
                bool OnButtonPushed() override;
            private:
                Controllers::MotorController& motorController;
                Controllers::DateTime& dateTimeController;
                Controllers::FS& fs;
                Controllers::Timer motorTimer;

                std::unique_ptr<TapCounterModel> model;
                std::chrono::seconds startTime;

                lv_obj_t* numberLabel;
                bool firstEventRejected{false};
            };

        }

        template <>
        struct AppTraits<Apps::TapCounter> {
            static constexpr Apps app = Apps::TapCounter;
            static constexpr const char* icon = Screens::Symbols::one;
            static Screens::Screen* Create(AppControllers& controllers) {
                return new Screens::TapCounter(controllers.motorController,
                                               controllers.dateTimeController,
                                               controllers.filesystem);
            };
        };
    }
}

