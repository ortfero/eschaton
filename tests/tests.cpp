#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <ctime>
#include <thread>
#include <eschaton/eschaton.hpp>



SCENARIO("Constructed datetime") {


    GIVEN("Constructed by default") {
      
        eschaton::datetime target;
        
        THEN("Should be invalid") {
          REQUIRE(!target);
        }
    }


    GIVEN("Constructed from current time") {
      
        using namespace std;
        auto const target = eschaton::datetime::utc();
        
        time_t const tp = time(nullptr);
        tm const* utc = gmtime(&tp);
        
        THEN("Should have current year") {
          REQUIRE(target.year() == utc->tm_year + 1900);
        }
        THEN("Should have current month") {
          REQUIRE(target.month() == unsigned(utc->tm_mon + 1));
        }
        THEN("Should have current day") {
          REQUIRE(target.day() == unsigned(utc->tm_mday));
        }
        THEN("Should have current hour") {
          REQUIRE(target.hours() == utc->tm_hour);
        }
        THEN("Should have current minute") {
          REQUIRE(target.minutes() == utc->tm_min);
        }
        THEN("Should have current second") {
          REQUIRE(target.seconds() == utc->tm_sec);
        }
        THEN("Should have current weekday") {
          REQUIRE(target.weekday().c_encoding() == unsigned(utc->tm_wday));
        }
    }
}



SCENARIO("Current time accuracy") {
  
  
  GIVEN("Current time in seconds") {
    
    using namespace std::chrono;
    auto const millis = eschaton::current<milliseconds>();
    auto const secs = eschaton::current<seconds>();
    
    THEN("Should be thousand times less then milliseconds") {
      REQUIRE(millis / 1000 == secs);
    }
  }
  
  
  GIVEN("Current time in milliseconds") {
    
    using namespace std::chrono;
    auto const micros = eschaton::current<microseconds>();
    auto const millis = eschaton::current<milliseconds>();
    
    THEN("Should be around thousand times less then microseconds") {
      REQUIRE(millis - micros / 1000 < 2);
    }
  }
  
}



SCENARIO("Elapsed time accuracy") {
  
  
  GIVEN("Elapsed time in milliseconds") {
    
    using namespace std::chrono;
    auto const micros = eschaton::current<microseconds>();
    auto const millis = eschaton::current<milliseconds>();
    
    std::this_thread::sleep_for(milliseconds{10});
    
    auto const elapsed_millis = eschaton::elapsed<milliseconds>(millis);
    auto const elapsed_micros = eschaton::elapsed<microseconds>(micros);

    std::cout << elapsed_millis << std::endl;
    std::cout << elapsed_micros << std::endl;
    
    THEN("Should be around thousand times less then microseconds") {
      REQUIRE(elapsed_millis - (elapsed_micros / 1000) < 2);
    }
  }
  
}
