#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <ctime>

#include <eschaton/eschaton.hpp>



SCENARIO("datetime constructed") {

    GIVEN("Constructed by default") {
        eschaton::datetime target;
        THEN("Should be invalid") { REQUIRE(!target); }
    }

    GIVEN("Constructed from current time") {
        using namespace std;
        auto const target = eschaton::datetime::utc();
        time_t const tp = time(nullptr);
        tm const* utc = gmtime(&tp);
        THEN("Should have current year") { REQUIRE(target.year() == utc->tm_year + 1900); }
        THEN("Should have current month") { REQUIRE(target.month() == unsigned(utc->tm_mon + 1)); }
        THEN("Should have current day") { REQUIRE(target.day() == unsigned(utc->tm_mday)); }
        THEN("Should have current hour") { REQUIRE(target.hours() == utc->tm_hour); }
        THEN("Should have current minute") { REQUIRE(target.minutes() == utc->tm_min); }
        THEN("Should have current second") { REQUIRE(target.seconds() == utc->tm_sec); }
        THEN("Should have current weekday") { REQUIRE(target.weekday().c_encoding() == unsigned(utc->tm_wday)); }
    }
}
