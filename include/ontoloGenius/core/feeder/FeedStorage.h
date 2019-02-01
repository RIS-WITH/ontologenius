#ifndef FEEDSTORAGE
#define FEEDSTORAGE

#include <regex>
#include <mutex>
#include <string>
#include <queue>

enum action_t
{
  action_add,
  action_del
};

struct feed_t
{
  action_t action_;
  std::string from_;
  std::string prop_;
  std::string on_;
};

class FeedStorage
{
public:
  FeedStorage();

  void add(std::string& regex) noexcept;
  std::queue<feed_t> get() noexcept;

private:
  std::regex base_regex;
  std::regex simple_regex;

  std::mutex mutex_;

  bool queue_choice_;
  std::queue<feed_t> fifo_1;
  std::queue<feed_t> fifo_2;
};

#endif //FEEDSTORAGE
