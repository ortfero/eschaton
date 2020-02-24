#pragma once



#include <stdint.h>
#include <chrono>
#include <ostream>

#ifndef HAS_UNCAUGHT_EXCEPTIONS
#define HAS_UNCAUGHT_EXCEPTIONS 1
#endif
#include <date/date.h>




namespace eschaton {


using system_time = std::chrono::system_clock::time_point;
using system_duration = std::chrono::system_clock::duration;

template<typename D> uint64_t to(system_time const& tp) noexcept {
  using namespace std::chrono;
  return uint64_t(floor<D>(tp.time_since_epoch()).count());
}

template<typename D> system_time from(uint64_t units) noexcept {
  return system_time{D{units}};
}

template<typename D> uint64_t current() noexcept {
  return to<D>(std::chrono::system_clock::now());
}

template<typename D> uint64_t elapsed(uint64_t from) noexcept {
  return current<D>() - from;
}

template<typename D>
bool is_occured(system_time const& tp, date::time_of_day<D> const& tod) noexcept {
  using namespace std::chrono;
  auto const daypoint = floor<date::days>(tp);
  return floor<D>(tp - daypoint) == tod;
}


struct datetime {
  using year_month_day = date::year_month_day;
  using time_of_day = date::time_of_day<std::chrono::microseconds>;
  
  static datetime utc() noexcept {
    return from_system_time(std::chrono::system_clock::now());    
  }
  
  static datetime from_system_time(system_time const& tp) noexcept {
    using date::floor;
    auto const dp = floor<date::days>(tp);
    year_month_day const ymd = dp;
    time_of_day const tod{floor<std::chrono::microseconds>(tp - dp)};
    return datetime{ymd, tod};
  }
  
  static datetime from_microseconds(uint64_t micros) noexcept {
    return from_system_time(eschaton::from_microseconds(micros));
  }
  
  static datetime from_milliseconds(uint64_t millis) noexcept {
    return from_system_time(eschaton::from_milliseconds(millis));
  }
  
  static datetime from_seconds(uint64_t seconds) noexcept {
    return from_system_time(eschaton::from_seconds(seconds));
  }
  
  static datetime from_date(date::year y, date::month m, date::day d) noexcept {
    return datetime{y/m/d, time_of_day{}};
  }
    
  datetime() noexcept = default;
  datetime(datetime const&) noexcept = default;
  datetime& operator = (datetime const&) noexcept = default;
  bool ok() const noexcept { return ymd_.ok(); }
  bool operator ! () const noexcept { return !ok(); }
  
  year_month_day const& date_only() const noexcept { return ymd_; }
  time_of_day const& time_only() const noexcept { return tod_; }
  int year() const noexcept { return int(ymd_.year()); }
  unsigned month() const noexcept { return unsigned(ymd_.month()); }
  unsigned day() const noexcept { return unsigned(ymd_.day()); }
  date::weekday weekday() const noexcept { return date::weekday{ymd_}; }
  int hours() const noexcept { return tod_.hours().count(); }
  int minutes() const noexcept { return int(tod_.minutes().count()); }
  int seconds() const noexcept { return int(tod_.seconds().count()); }
  int microseconds() const noexcept { return int(tod_.subseconds().count()); }
  
private:
  year_month_day ymd_;
  time_of_day tod_;

  datetime(year_month_day const& ymd, time_of_day const& tod) noexcept
      : ymd_(ymd), tod_(tod)
  { }
  
  template<typename charT, typename traits> friend
  std::basic_ostream<charT, traits>&
    operator<<(std::basic_ostream<charT, traits>& os, datetime const& dt) {
      os << dt.ymd_ << ' ' << dt.tod_;
      return os;
    }
}; // datetime


} // eschaton
