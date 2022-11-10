#include <chrono>
#include <cstdint>
#include <fmt/color.h>
#include <fmt/core.h>
#include <future>
#include <iostream>
#include <semaphore>
#include <thread>

constexpr int32_t SEATS = 3;

// Sleepy barber problem
std::counting_semaphore<1> barber{1};
std::counting_semaphore seats{SEATS};

// TODO: I think we need to create a mutex to protect the semaphore access

// TODO: Check if semaphore is fair

void cut_hair() {
  std::this_thread::sleep_for(std::chrono::seconds(3));
  fmt::print(fmt::emphasis::bold | fg(fmt::color::green), "Hair cut\n");
}

void new_customer() {
  // A new customer has arrived
  fmt::print(fmt::emphasis::bold | fg(fmt::color::blue), "New customer\n");

  // 1. If the barber is sleeping, wake him up
  if (barber.try_acquire()) {
    fmt::print(fmt::emphasis::bold | fg(fmt::color::deep_sky_blue),
               "Barber is awake 🥱\n");
    std::async(std::launch::async, cut_hair).wait();
    return barber.release();
  }

  // 2. If the barber is busy, but there is a seat available, sit down
  if (seats.try_acquire()) {
    fmt::print(fmt::emphasis::bold | fg(fmt::color::yellow),
               "Barber is busy, but there IS a seat available\n");
    barber.acquire();
    seats.release();
    std::async(std::launch::async, cut_hair).wait();
    return barber.release();
  }

  // 3. If the barber is busy and there are no seats available, leave
  fmt::print(fmt::emphasis::bold | fg(fmt::color::red),
             "Barber is busy and there are NO seats available\n");
}

int main(int argc, char **argv) {
  std::thread([&] {
    while (true) {
      // TODO: it might be a better idea to only print the statements in
      // `new_customer()` if a --debug flag is passed and instead print the
      // current state of the barber shop
      std::this_thread::sleep_for(std::chrono::milliseconds(700));

      if (std::rand() % 2 == 0) // Create new_customer in a new thread
        std::thread(new_customer).detach();
    }
  }).detach();

  // TODO: perhaps add a way to create new customers manually

  // Wait for user input to finish
  std::cin.get();
}
