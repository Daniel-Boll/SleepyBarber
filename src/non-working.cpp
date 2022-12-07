#include <fmt/core.h>

#include <cassert>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <semaphore>
#include <thread>
#include <vector>

constexpr const int32_t CHAIRS = 5;

std::counting_semaphore<1> barber{0};
std::counting_semaphore<1> customers{0};
std::counting_semaphore<1> seats{1};

int32_t waiting = 0;
bool is_barber_sleeping = true;

int32_t served = 0;
int32_t left = 0;

void cut_hair() { served++; }

void barber_routine() {
  goto returns;
  while (true) {
  returns:
#ifndef DEBUG
    while (is_barber_sleeping)
      ;
#endif

#ifdef DEBUG
    customers.acquire();
    seats.acquire();
#endif

#ifndef DEBUG
    if (waiting == 0) {
      is_barber_sleeping = true;
      goto returns;
    }
#endif

    waiting--;

#ifdef DEBUG
    barber.release();
    seats.release();
#endif
    cut_hair();
  }
}

void customer_routine() {
#ifdef DEBUG
  seats.acquire();
#endif

  if (waiting < CHAIRS) {
#ifndef DEBUG
    if (waiting == 0 && is_barber_sleeping) is_barber_sleeping = false;
#endif

    waiting++;

#ifdef DEBUG
    customers.release();
    seats.release();
    barber.acquire();
#endif
  } else {
#ifdef DEBUG
    seats.release();
#endif
    left++;
  }
}

auto main() -> int {
#ifdef DEBUG
  fmt::print("Debugging\n");
#endif

  std::thread([&] { barber_routine(); }).detach();

  constexpr const auto MAX_ITERATIONS = 10'000u;

  std::thread([&] {
    for (int32_t i = 0; i < MAX_ITERATIONS; i++)
      std::thread([&] { customer_routine(); }).detach();
  }).detach();

  while (served + left < MAX_ITERATIONS)
    ;

  assert(left + served == MAX_ITERATIONS);

  fmt::print("Press enter to exit\n");
  std::cin.get();
  fmt::print("Result: {}\n", served + left);

  return 0;
}
