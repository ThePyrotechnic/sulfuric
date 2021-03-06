#ifndef CATCH_CONFIG_MAIN
  #define CATCH_CONFIG_MAIN
#endif

#include <PluginSynthesiser.h>
#include <catch2/catch.hpp>

TEST_CASE("one is equal to one", "[dummy]")
{
  REQUIRE(1 == 1);
}

// https://github.com/McMartin/FRUT/issues/490#issuecomment-663544272
SulfuricAudioProcessor testPlugin;

TEST_CASE("Plugin instance name", "[name]")
{
  CHECK_THAT(testPlugin.getName().toStdString(),
             Catch::Matchers::Equals("sulfuric"));
}
