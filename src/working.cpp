#include <fmt/color.h>
#include <fmt/core.h>

#include <chrono>
#include <cstdint>
#include <future>
#include <iostream>
#include <semaphore>
#include <thread>

constexpr int32_t SEATS = 3;

// Sleepy barber problem
std::counting_semaphore<1> barber{1};
std::counting_semaphore seats{SEATS};

void cut_hair() {
#ifdef DEBUG
  std::this_thread::sleep_for(std::chrono::seconds(3));
  fmt::print(fmt::emphasis::bold | fg(fmt::color::green), "Hair cut\n");
#endif
}

void new_customer() {
// A new customer has arrived
#ifdef DEBUG
  fmt::print(fmt::emphasis::bold | fg(fmt::color::blue), "New customer\n");
#endif

  // 1. If the barber is sleeping, wake him up
  if (barber.try_acquire()) {
#ifdef DEBUG
    fmt::print(fmt::emphasis::bold | fg(fmt::color::deep_sky_blue),
               "Barber is awake 🥱\n");
#endif
    std::async(std::launch::async, cut_hair).wait();
    return barber.release();
  }

  // 2. If the barber is busy, but there is a seat available, sit down
  if (seats.try_acquire()) {
#ifdef DEBUG
    fmt::print(fmt::emphasis::bold | fg(fmt::color::yellow),
               "Barber is busy, but there IS a seat available\n");
#endif
    barber.acquire();
    seats.release();
    std::async(std::launch::async, cut_hair).wait();
    return barber.release();
  }

  // 3. If the barber is busy and there are no seats available, leave
#ifdef DEBUG
  fmt::print(fmt::emphasis::bold | fg(fmt::color::red),
             "Barber is busy and there are NO seats available\n");
#endif
}

int main(int argc, char **argv) {
#ifdef DEBUG
  constexpr const auto MAX_ITERATIONS = 1'000'000;
#else
  constexpr const auto MAX_ITERATIONS = 10;
#endif

  int32_t idx = 0;
  while (idx < MAX_ITERATIONS) {
#ifdef DEBUG
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
#endif

    if (std::rand() % 2 == 0) {
      std::thread(new_customer).detach();
      ++idx;
    }
  }
}
