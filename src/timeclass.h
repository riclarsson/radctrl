#ifndef timeclass_h
#define timeclass_h

#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <thread>

/** A duration of time, 1 full tick should be 1 second */
using TimeStep = std::chrono::duration<double>;

/** Class to handle time */
class Time {
 private:
  std::chrono::system_clock::time_point mtime;

 public:
  using InternalTimeStep = decltype(mtime)::duration;

  // Version will be updated when C++20 datetime is available... Version 1 will
  // still assume local time at that time
  static constexpr long Version() noexcept { return 1; }

  // Construction
  Time() : mtime(std::chrono::system_clock::now()) {}
  explicit Time(std::time_t t)
      : mtime(std::chrono::system_clock::from_time_t(t)) {}
  explicit Time(std::tm t) : Time(std::mktime(&t)) {}

  // Data
  const std::chrono::system_clock::time_point &Data() const { return mtime; }

  // Conversions
  std::time_t toTimeT() const {
    return std::chrono::system_clock::to_time_t(mtime);
  }
  std::tm toStruct() const {
    std::time_t x = toTimeT();
    std::tm *y = std::localtime(&x);
    return *y;
  }
  std::tm toGMTStruct() const {
    std::time_t x = toTimeT();
    std::tm *y = std::gmtime(&x);
    return *y;
  }
  TimeStep seconds_into_day() const {
    std::tm x = toStruct();
    return TimeStep(x.tm_hour * 3600 + x.tm_min * 60 + x.tm_sec +
                    PartOfSecond());
  }
  InternalTimeStep EpochTime() const { return mtime.time_since_epoch(); }

  // Operations
  InternalTimeStep operator-(const Time &t) const { return mtime - t.mtime; }
  bool operator<(const Time &t) const { return mtime < t.mtime; }
  bool operator==(const Time &t) const { return mtime == t.mtime; }
  template <typename T, typename R>
  Time &operator+=(const std::chrono::duration<T, R> &dt) {
    mtime += std::chrono::duration_cast<InternalTimeStep>(dt);
    return *this;
  }
  template <typename T, typename R>
  Time &operator-=(const std::chrono::duration<T, R> &dt) {
    mtime -= std::chrono::duration_cast<InternalTimeStep>(dt);
    return *this;
  }
  template <typename T, typename R>
  Time operator+(const std::chrono::duration<T, R> &dt) const {
    return (Time(*this) += dt);
  }
  template <typename T, typename R>
  Time operator-(const std::chrono::duration<T, R> &dt) const {
    return (Time(*this) -= dt);
  }

  // helpers
  TimeStep::rep Seconds() const {
    return std::chrono::duration_cast<TimeStep>(mtime.time_since_epoch())
        .count();
  }
  void Seconds(TimeStep::rep x) { operator+=(TimeStep(x - Seconds())); }
  TimeStep::rep PartOfSecond() const { return std::fmod(Seconds(), 1.0); }

  friend std::ostream &operator<<(std::ostream &os, const Time &t) {
    std::tm x = t.toStruct();

    // Deal with seconds
    char sec[2 + 1 + 9 + 100];
    TimeStep::rep seconds = TimeStep::rep(x.tm_sec) + t.PartOfSecond();
    sprintf(sec, "%.9lf", seconds);

    // Print based on std::tm specs
    return os << 1900 + x.tm_year << '-' << std::setfill('0') << std::setw(2)
              << 1 + x.tm_mon << '-' << std::setfill('0') << std::setw(2)
              << x.tm_mday << ' ' << std::setfill('0') << std::setw(2)
              << x.tm_hour << ':' << std::setfill('0') << std::setw(2)
              << x.tm_min << ':' << std::setfill('0') << std::setw(12) << sec;
  }

  friend std::istream &operator>>(std::istream &is, Time &t) {
    std::string ymd, hms;
    is >> ymd >> hms;

    std::vector<std::string> YMD(3), HMS(3);
    auto firstminus = std::find(ymd.begin(), ymd.end(), '-');
    auto secondminus = std::find(firstminus + 1, ymd.end(), '-');
    auto firstcolon = std::find(hms.begin(), hms.end(), ':');
    auto secondcolon = std::find(firstcolon + 1, hms.end(), ':');
    YMD[0] = std::string{ymd.begin(), firstminus};
    YMD[1] = std::string{firstminus + 1, secondminus};
    YMD[2] = std::string{secondminus + 1, ymd.end()};
    HMS[0] = std::string{hms.begin(), firstcolon};
    HMS[1] = std::string{firstcolon + 1, secondcolon};
    HMS[2] = std::string{secondcolon + 1, hms.end()};

    // FIXME: C++20 has much better calendar handling
    std::tm x;
    x.tm_year = std::stoi(YMD[0]) - 1900;
    x.tm_mon = std::stoi(YMD[1]) - 1;
    x.tm_mday = std::stoi(YMD[2]);
    x.tm_hour = std::stoi(HMS[0]);
    x.tm_min = std::stoi(HMS[1]);
    x.tm_sec = 0;
    x.tm_isdst = -1;

    t = Time(x) + TimeStep(std::stod(HMS[2]));

    return is;
  }

  friend void Sleep(Time t);
};  // Time

/** Debug output for duration */
inline std::ostream &operator<<(std::ostream &os, const TimeStep &dt) {
  return os << dt.count() << " seconds";
}

inline void Sleep(double duration) {
  std::this_thread::sleep_for(TimeStep(duration));
}

inline void Sleep(Time t) { std::this_thread::sleep_until(t.mtime); }

#endif  // timeclass_h
