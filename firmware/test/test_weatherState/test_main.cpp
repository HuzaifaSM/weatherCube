#include <unity.h>
#include "weatherState.h"

void setUp(void) {}
void tearDown(void) {}

static void assertState(WeatherState expected, int wmoCode) {
    TEST_ASSERT_EQUAL_INT(static_cast<int>(expected), static_cast<int>(stateFromWmoCode(wmoCode)));
}

void test_clearCodes(void) {
    assertState(WeatherState::clear, 0);
    assertState(WeatherState::clear, 1);
}

void test_cloudyCodes(void)
{
    assertState(WeatherState::cloudy, 2);
    assertState(WeatherState::cloudy, 3);
    assertState(WeatherState::cloudy, 45);
    assertState(WeatherState::cloudy, 48);
}

void test_rainCodes(void)
{
    const int codes[] = {51, 53, 55, 56, 57, 61, 63, 65, 66, 67, 80, 81, 82};
    for (int code : codes) {
        assertState(WeatherState::rain, code);
    }
}

void test_snowCodes(void)
{
    const int codes[] = {71, 73, 75, 77, 85, 86};
    for (int code : codes) {
        assertState(WeatherState::snow, code);
    }
}

void test_stormCodes(void)
{
    assertState(WeatherState::storm, 95);
    assertState(WeatherState::storm, 96);
    assertState(WeatherState::storm, 99);
}

void test_groupBoundaries(void)
{
    assertState(WeatherState::cloudy, 3);   // last cloud code before fog
    assertState(WeatherState::cloudy, 45);  // fog is cloudy, not clear
    assertState(WeatherState::rain, 82);    // last rain shower code
    assertState(WeatherState::snow, 85);    // first snow shower code
}

void test_unknownCodesFallBackToCloudy(void)
{
    const int codes[] = {-1, 4, 58, 62, 94, 100, 9999};
    for (int code : codes) {
        assertState(WeatherState::cloudy, code);
    }
}

void test_neverReturnsOffline(void)
{
    for (int code = -5; code <= 120; code++) {
        TEST_ASSERT_TRUE(stateFromWmoCode(code) != WeatherState::offline);
    }
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_clearCodes);
    RUN_TEST(test_cloudyCodes);
    RUN_TEST(test_rainCodes);
    RUN_TEST(test_snowCodes);
    RUN_TEST(test_stormCodes);
    RUN_TEST(test_groupBoundaries);
    RUN_TEST(test_unknownCodesFallBackToCloudy);
    RUN_TEST(test_neverReturnsOffline);
    return UNITY_END();
}