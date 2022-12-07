#include <fmt/color.h>
#include <fmt/core.h>

#include <future>
#include <thread>

constexpr const int32_t SEATS = 3;

// Controllers
bool barber_sleeping = true;
bool is_seat_available = true;
int32_t customers_waiting = 0;

void cut_hair() {
#ifdef DEBUG
  std::this_thread::sleep_for(std::chrono::seconds(3));
  fmt::print(fmt::emphasis::bold | fg(fmt::color::green), "Hair cut\n");
#endif
  customers_waiting--;
  is_seat_available = true;
  if (customers_waiting == 0) {
    barber_sleeping = true;
#ifdef DEBUG
    fmt::print(fmt::emphasis::bold, "Barber is sleeping 😴\n");
#endif
  }
}

void new_customer() {
#ifdef DEBUG
  fmt::print(fmt::emphasis::bold | fg(fmt::color::blue), "New customer\n");
#endif

  if (barber_sleeping && is_seat_available) {
    barber_sleeping = false;
    is_seat_available = false;
#ifdef DEBUG
    fmt::print("Barber is awake 🥱\n");
#endif
    return std::async(std::launch::async, cut_hair).wait();
  }

  if (customers_waiting < SEATS) {
#ifdef DEBUG
    fmt::print(fmt::emphasis::bold | fg(fmt::color::yellow),
               "Barber is busy, but there IS a seat available\n");
#endif
    customers_waiting++;

    // while (!is_seat_available)
    //   ;
    while (!is_seat_available) std::this_thread::yield();

    is_seat_available = false;
    return std::async(std::launch::async, cut_hair).wait();
  }

#ifdef DEBUG
  fmt::print(fmt::emphasis::bold | fg(fmt::color::red),
             "Barber is busy and there are NO seats available\n");
#endif
}

int main(int argc, char *argv[]) {
#ifdef DEBUG
  constexpr const auto MAX_ITERATIONS = 10;
#else
  constexpr const auto MAX_ITERATIONS = 100'000;
#endif

  int32_t idx = 0;
  while (idx < MAX_ITERATIONS) {
#ifdef DEBUG
    std::this_thread::sleep_for(std::chrono::milliseconds(600));
#endif
    if (std::rand() % 2 == 0) {
      std::thread(new_customer).detach();
      ++idx;
    }
  }
}
