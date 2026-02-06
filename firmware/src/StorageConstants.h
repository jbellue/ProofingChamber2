#pragma once

namespace storage
{
    namespace keys
    {
        static constexpr char HOT_LOWER_LIMIT_KEY[] = "h_lower";
        static constexpr char HOT_UPPER_LIMIT_KEY[] = "h_upper";
        static constexpr char COLD_LOWER_LIMIT_KEY[] = "c_lower";
        static constexpr char COLD_UPPER_LIMIT_KEY[] = "c_upper";
        static constexpr char TIMEZONE_KEY[] = "timezone";
        static constexpr char TIMEZONE_INDEX_KEY[] = "tz_idx";
    }
    namespace defaults
    {
        static constexpr int HOT_LOWER_LIMIT_DEFAULT = 23;
        static constexpr int HOT_UPPER_LIMIT_DEFAULT = 32;
        static constexpr int COLD_LOWER_LIMIT_DEFAULT = 2;
        static constexpr int COLD_UPPER_LIMIT_DEFAULT = 7;
        static constexpr char TIMEZONE_DEFAULT[] = "CET-1CEST,M3.5.0,M10.5.0/3";
    }
}