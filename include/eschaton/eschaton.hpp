#pragma once


#include <cstdint>
#include <cstring>
#include <ctime>
#include <string>
#include <string_view>
#include <chrono>
#include <optional>

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
    return from(std::chrono::system_clock::now());    
  }
  
  static datetime from(system_time const& tp) noexcept {
    using date::floor;
    auto const dp = floor<date::days>(tp);
    year_month_day const ymd = dp;
    time_of_day const tod{floor<std::chrono::microseconds>(tp - dp)};
    return datetime{ymd, tod};
  }
    
  template<typename D> static datetime from(uint64_t units) noexcept {
    return from_system_time(eschaton::from<D>(units));
  }
  
  static datetime from(date::year y, date::month m, date::day d) noexcept {
    return datetime{y/m/d, time_of_day{}};
  }
  
  static datetime from(year_month_day const& ymd) noexcept {
    return datetime{ymd, time_of_day{}};
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


  template<typename S> friend
    S& operator << (S& stream, datetime const& dt) {
    stream << dt.ymd_.year() << '-';
    print_fixed_2(stream, unsigned(dt.ymd_.month()));
    stream << '-';
    print_fixed_2(stream, unsigned(dt.ymd_.day()));
    stream << ' ';
    print_fixed_2(stream, int(dt.tod_.hour()));
    stream << ':';
    print_fixed_2(stream, int(dt.tod_.minute()));
    stream << ':';
    print_fixed_2(stream, int(dt.tod_.second()));
    return stream;
  }


private:
  year_month_day ymd_;
  time_of_day tod_;


  datetime(year_month_day const& ymd, time_of_day const& tod) noexcept
      : ymd_(ymd), tod_(tod)
  { }


  template<class S, typename T>
  void print_fixed_2(S& stream, T value) {    
    if (value < 10)
      stream << '0' << ('0' + value);
    else
      stream << ('0' + value / 10) << ('0' + value % 10);
  }  
}; // datetime


inline std::optional<system_time> parse_time(std::string_view const& view) noexcept {

  std::optional<system_time> result;

  unsigned year, month, day, date_scanned;

  if (std::sscanf(view.data(), "%u/%u/%u%n", &year, &month, &day, &date_scanned) != 3 &&
    std::sscanf(view.data(), "%u-%u-%u%n", &year, &month, &day, &date_scanned) != 3)
    return result;

  if (date_scanned > view.size())
    return result;

  char const* after_date = view.data() + date_scanned;
  unsigned hour = 0, minute = 0, second = 0, time_scanned;

  if (date_scanned != view.size()) {

    if (std::sscanf(after_date, "%u:%u:%u%n", &hour, &minute, &second, &time_scanned) != 3
      || date_scanned + time_scanned != view.size())
      return result;

  }

  std::tm tm = { int(second), int(minute), int(hour),
                int(day), int(month - 1), int(year - 1900), 0, 0, 0 };
  std::time_t const tp =
#if _WIN32
    _mkgmtime(&tm);
#else
    timegm(&tm);
#endif

  if (tp == -1)
    return result;

  return result = std::chrono::system_clock::from_time_t(tp);
}


inline std::optional<system_time> parse_time(std::string const& s) {
  return parse_time(std::string_view{ s.data(), s.size() });
}


inline std::optional<system_duration> parse_duration(std::string_view const& view) noexcept {

  using namespace std::chrono;
  std::optional<system_duration> result;

  unsigned hour, minute, second, scanned;
  if (std::sscanf(view.data(), "%u:%u:%u%n", &hour, &minute, &second, &scanned) == 3 &&
    scanned == view.size()) {
    return result = seconds{ hour * 3600 + minute * 60 + second };
  }

  unsigned long long count; char unit[16];
  if (std::sscanf(view.data(), "%llu %13s%n", &count, &unit, &scanned) == 2 &&
    scanned == view.size()) {
    using std::strcmp;
    if (strcmp(unit, "microseconds") == 0 || strcmp(unit, "microsecond") == 0)
      return result = microseconds{ count };
    else if (strcmp(unit, "milliseconds") == 0 || strcmp(unit, "millisecond") == 0)
      return result = milliseconds{ count };
    else if (strcmp(unit, "seconds") == 0 || strcmp(unit, "second") == 0)
      return result = seconds{ count };
    else if (strcmp(unit, "minutes") == 0 || strcmp(unit, "minute") == 0)
      return result = minutes{ count };
    else if (strcmp(unit, "hours") == 0 || strcmp(unit, "hour") == 0)
      return result = hours{ count };
    else if (strcmp(unit, "days") == 0 || strcmp(unit, "day") == 0)
      return result = hours{ count * 24 };
    else if (strcmp(unit, "weeks") == 0 || strcmp(unit, "week") == 0)
      return result = hours{ count * 24 * 7 };
  }

  return result;
}


inline std::optional<system_duration> parse_duration(std::string const& s) noexcept {
  return parse_duration(std::string_view{ s.data(), s.size() });
}


inline std::optional<datetime> parse_datetime(std::string_view const& view) noexcept {
  std::optional<datetime> result;
  auto maybe_time = parse_time(view);
  if (!maybe_time)
    return result;
  return result = datetime::from(*maybe_time);
}


inline std::optional<datetime> parse_datetime(std::string const& s) noexcept {
  return parse_datetime(std::string_view{ s.data(), s.size() });
}


} // eschaton
