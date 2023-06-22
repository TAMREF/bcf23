#include "capper.hpp"
#include "catch2/catch_all.hpp"

TEST_CASE("verify capper", "[capper]") {
    NoCapOperationCapper nc;
    REQUIRE( nc.incr() );

    NormalOperationCapper c(5);
    REQUIRE( c.incr(2) ); // 0 <= 5, counter <- 2
    REQUIRE( c.incr(3) ); // 2 <= 5, counter <- 5
    REQUIRE( c.incr(4) ); // 5 <= 5, counter <- 9
    REQUIRE_FALSE( c.incr() );
}