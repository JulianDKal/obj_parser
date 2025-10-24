#pragma once
#include <chrono>
#include <iostream>

class Timer {
public:
    //Timer gets started when constructor is called
    Timer() {
        m_startPoint = std::chrono::high_resolution_clock::now();
    }
    //Call this for printing elapsed time to the console
    void Stop() {
        auto endPoint = std::chrono::high_resolution_clock::now();
        auto start =
            std::chrono::time_point_cast<std::chrono::microseconds>(m_startPoint).time_since_epoch().count();
        auto end =
            std::chrono::time_point_cast<std::chrono::microseconds>(endPoint).time_since_epoch().count();
        auto duration = end - start;
        std::cout << "time elapsed: " << duration << " us, " << duration * 0.001 << " ms" << std::endl;
    }

    //Call this to return the elapsed time in microseconds and store it in a variable
    long stopReturn(){
        auto endPoint = std::chrono::high_resolution_clock::now();
        auto start =
            std::chrono::time_point_cast<std::chrono::microseconds>(m_startPoint).time_since_epoch().count();
        auto end =
            std::chrono::time_point_cast<std::chrono::microseconds>(endPoint).time_since_epoch().count();
        auto duration = end - start;
        return duration;
    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startPoint;
};

