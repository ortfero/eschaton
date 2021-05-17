#define _CRT_SECURE_NO_WARNINGS

#include <ctime>
#include <thread>

#include <eschaton/eschaton.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"


TEST_CASE("datetime constructed by default") {
  eschaton::datetime target;
  REQUIRE(!target);
}


TEST_CASE("datetime constructed from current time") {
  using namespace std;
  auto const target = eschaton::datetime::utc();

  time_t const tp = time(nullptr);
  tm const* utc = gmtime(&tp);

  REQUIRE(target.year() == utc->tm_year + 1900);
  REQUIRE(target.month() == unsigned(utc->tm_mon + 1));
  REQUIRE(target.day() == unsigned(utc->tm_mday));
  REQUIRE(target.hours() == utc->tm_hour);
  REQUIRE(target.minutes() == utc->tm_min);
  REQUIRE(target.seconds() == utc->tm_sec);
  REQUIRE(target.weekday().c_encoding() == unsigned(utc->tm_wday));
}


TEST_CASE("current time in seconds vs millis") {
  using namespace std::chrono;
  auto const millis = eschaton::current<milliseconds>();
  auto const secs = eschaton::current<seconds>();
  REQUIRE(millis / 1000 == secs);
}


TEST_CASE("current time in milliseconds vs micros") {
  using namespace std::chrono;
  auto const micros = eschaton::current<microseconds>();
  auto const millis = eschaton::current<milliseconds>();
  REQUIRE(millis - micros / 1000 < 2);
}


TEST_CASE("elapsed time in millis vs micros") {

  using namespace std::chrono;
  auto const micros = eschaton::current<microseconds>();
  auto const millis = eschaton::current<milliseconds>();

  std::this_thread::sleep_for(milliseconds{10});

  auto const elapsed_millis = eschaton::elapsed<milliseconds>(millis);
  auto const elapsed_micros = eschaton::elapsed<microseconds>(micros);

  REQUIRE(elapsed_millis - (elapsed_micros / 1000) < 2);
}
