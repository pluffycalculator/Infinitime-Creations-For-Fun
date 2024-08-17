#pragma once
#include <cstddef>
#include <cstdint>

namespace Pinetime {
  namespace Applications {
    enum class Apps : uint8_t {
      None,
      Launcher,
      Clock,
      SysInfo,
      FirmwareUpdate,
      FirmwareValidation,
      NotificationsPreview,
      Notifications,
      Timer,
      Alarm,
      FlashLight,
      BatteryInfo,
      Music,
      Paint,
      Paddle,
      Twos,
      HeartRate,
      Navigation,
      StopWatch,
      Metronome,
      Motion,
      Steps,
      Dice,
      TapCounter,
      TicTacToe,
      Weather,
      PassKey,
      QuickSettings,
      Settings,
      SettingWatchFace,
      SettingTimeFormat,
      SettingWeatherFormat,
      SettingDisplay,
      SettingWakeUp,
      SettingSteps,
      SettingSetDateTime,
      SettingChimes,
      SettingShakeThreshold,
      SettingBluetooth,
      Error
    };

    enum class WatchFace : uint8_t {
      Digital,
      Analog,
      PineTimeStyle,
      Terminal,
      Infineat,
      CasioStyleG7710,
      Jesus,
    };

    template <Apps>
    struct AppTraits {};

    template <WatchFace>
    struct WatchFaceTraits {};

    template <Apps... As>
    struct TypeList {
      static constexpr size_t Count = sizeof...(As);
    };

    using UserAppTypes = TypeList<Apps::StopWatch,Apps::Timer,Apps::Steps,Apps::HeartRate,Apps::Music,Apps::Paint,Apps::Paddle,Apps::Twos,Apps::Dice,Apps::Metronome,Apps::TapCounter>;

    template <WatchFace... Ws>
    struct WatchFaceTypeList {
      static constexpr size_t Count = sizeof...(Ws);
    };

    using UserWatchFaceTypes = WatchFaceTypeList<WatchFace::Analog,WatchFace::Digital,WatchFace::Jesus>;

    static_assert(UserWatchFaceTypes::Count >= 1);
  }
}
