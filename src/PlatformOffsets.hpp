#include <cstdint>
#include <Geode/platform/platform.hpp>

struct GJMoreGamesLayer;

// TODO UPDATE FOR 2.208 oh fuck me
// Base URL: https://www.boomlings.com/database/getGJLevelLists.php
// Secondary URL: aHR0cDovL3d3dy5ib29tbGluZ3MuY29tL2RhdGFiYXNlL2dldEdKTGV2ZWxzMjEucGhw
#ifdef GEODE_IS_ANDROID
    constexpr bool IS_SUPPORTED_PLATFORM = true;
    inline bool evaluateAmazon() {
        return !((GJMoreGamesLayer *volatile)nullptr)->getMoreGamesList()->count();
    }
    #ifdef GEODE_IS_ANDROID64
        constexpr std::uintptr_t BASE_URL_OFFSET_AMAZON      = 0xea27f8; // These amazon ones are the ones from prev. commits; may be inaccurate
        constexpr std::uintptr_t BASE_URL_OFFSET             = 0xecce60;
        constexpr std::uintptr_t SECONDARY_URL_OFFSET_AMAZON = 0xea15b8;
        constexpr std::uintptr_t SECONDARY_URL_OFFSET        = 0xecbcc8;
    #elif defined(GEODE_IS_ANDROID32)
        constexpr std::uintptr_t BASE_URL_OFFSET_AMAZON      = 0x952cce;
        constexpr std::uintptr_t BASE_URL_OFFSET             = 0x96bfcb;
        constexpr std::uintptr_t SECONDARY_URL_OFFSET_AMAZON = 0x861c41;
        constexpr std::uintptr_t SECONDARY_URL_OFFSET        = 0x96afcb;
    #endif
#else
    inline bool evaluateAmazon() { return false; }
#endif
#ifdef GEODE_IS_WINDOWS
    constexpr bool IS_SUPPORTED_PLATFORM = true;
    constexpr std::uintptr_t BASE_URL_OFFSET_AMAZON      = 0x0;
    constexpr std::uintptr_t BASE_URL_OFFSET             = 0x558d78;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET_AMAZON = 0x0;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET        = 0x558dc0;
#elif defined(GEODE_IS_ARM_MAC)
    constexpr bool IS_SUPPORTED_PLATFORM = true;
    constexpr std::uintptr_t BASE_URL_OFFSET_AMAZON      = 0x0;
    constexpr std::uintptr_t BASE_URL_OFFSET             = 0x77d8d8;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET_AMAZON = 0x0;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET        = 0x77d989;
#elif defined(GEODE_IS_INTEL_MAC)
    constexpr bool IS_SUPPORTED_PLATFORM = true;
    constexpr std::uintptr_t BASE_URL_OFFSET_AMAZON      = 0x0;
    constexpr std::uintptr_t BASE_URL_OFFSET             = 0x868fcf;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET_AMAZON = 0x0;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET        = 0x869080;
#elif defined(GEODE_IS_IOS)
    constexpr bool IS_SUPPORTED_PLATFORM = true;
    constexpr std::uintptr_t BASE_URL_OFFSET_AMAZON      = 0x0;
    constexpr std::uintptr_t BASE_URL_OFFSET             = 0x6b8e9f;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET_AMAZON = 0x0;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET        = 0x6b8f50;
#elif !defined(GEODE_IS_ANDROID)
    constexpr bool IS_SUPPORTED_PLATFORM = false;
    constexpr std::uintptr_t BASE_URL_OFFSET_AMAZON      = 0x0;
    constexpr std::uintptr_t BASE_URL_OFFSET             = 0x0;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET_AMAZON = 0x0;
    constexpr std::uintptr_t SECONDARY_URL_OFFSET        = 0x0;
#endif
