#pragma once
#include<string>
#include<chrono>
#include<vector>

static uint64_t getCurTime(){// 获取当前系统启动以来的毫秒数
#ifndef WIN32//Linux
    struct timespec now;// tv_sec (s) tv_nsec (ns-纳秒)
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (now.tv_sec * 1000 + now.tv_nsec / 1000000);
#else
    long long now = std::chrono::steady_clock::now().time_since_epoch().count();
    return now;
#endif
}

static std::string getCurFormatTime(const char *format = "%Y-%m-%d %H:%M:%S"){//以年月日时分秒的格式返回时间

    time_t t = time(nullptr);
    char tc[64];
    strftime(tc, sizeof(tc), format, localtime(&t));
    std::string tcs = tc;
    return tcs;
}
