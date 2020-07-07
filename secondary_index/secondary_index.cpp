#include "test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>
#include <algorithm>

using namespace std;

struct Record {
  string id;
  string title;
  string user;
  int timestamp;
  int karma;

  struct DatabaseIterators {
    multimap<int, Record*>::iterator it_by_timestamp;
    multimap<int, Record*>::iterator it_by_karma;
    multimap<string, Record*>::iterator it_by_user;
  } db_iterators;

  bool operator==(const Record& other) const;
};

bool Record::operator==(const Record& other) const {
  return id == other.id
      && title == other.title
      && user == other.user
      && timestamp == other.timestamp
      && karma == other.karma;
}

class Database {
public:
  bool Put(const Record& record);
  const Record* GetById(const string& id) const;
  bool Erase(const string& id);

  template <typename Callback>
  void RangeByTimestamp(int low, int high, Callback callback) const;

  template <typename Callback>
  void RangeByKarma(int low, int high, Callback callback) const;

  template <typename Callback>
  void AllByUser(const string& user, Callback callback) const;

private:
  unordered_map<string, Record> data;
  multimap<int, Record*> ranged_by_timestamp;
  multimap<int, Record*> ranged_by_karma;
  multimap<string, Record*> ranged_by_user;

  template<typename Key, typename Callback>
  void RangeBy(const multimap<Key, Record*>& ranged_by_key,
      const Key& low, const Key& high, Callback callback) const;
};

bool Database::Put(const Record& record) {
  auto [it, inserted] = data.try_emplace(record.id, record);

  if (inserted) {
    auto address = std::addressof(it->second);
    it->second.db_iterators = {
      ranged_by_timestamp.emplace(record.timestamp, address),
      ranged_by_karma.emplace(record.karma, address),
      ranged_by_user.emplace(record.user, address),
    };
  }

  return inserted;
}

const Record* Database::GetById(const string& id) const {
  auto it = data.find(id);
  return it == end(data) ? nullptr : std::addressof(it->second);
}

bool Database::Erase(const string& id) {
  auto it = data.find(id);

  if (it == end(data)) {
    return false;
  }

  ranged_by_timestamp.erase(it->second.db_iterators.it_by_timestamp);
  ranged_by_karma.erase(it->second.db_iterators.it_by_karma);
  ranged_by_user.erase(it->second.db_iterators.it_by_user);
  data.erase(it);

  return true;
}

template<typename Key, typename Callback>
void Database::RangeBy(const multimap<Key, Record*>& ranged_by_key,
    const Key& low, const Key& high, Callback callback) const {
  if (low > high) {
    return;
  }

  auto lb = ranged_by_key.lower_bound(low);
  auto ub = ranged_by_key.upper_bound(high);
  for (auto it = lb; it != ub && callback(*it->second); ++it);
}

template <typename Callback>
void Database::RangeByTimestamp(int low, int high, Callback callback) const {
  RangeBy(ranged_by_timestamp, low, high, callback);
}

template <typename Callback>
void Database::RangeByKarma(int low, int high, Callback callback) const {
  RangeBy(ranged_by_karma, low, high, callback);
}

template <typename Callback>
void Database::AllByUser(const string& user, Callback callback) const {
  RangeBy(ranged_by_user, user, user, callback);
}

void TestRangeBoundaries() {
  const int good_karma = 1000;
  const int bad_karma = -10;

  Database db;
  db.Put({"id1", "Hello there", "master", 1536107260, good_karma});
  db.Put({"id2", "O>>-<", "general2", 1536107260, bad_karma});

  int count = 0;
  db.RangeByKarma(bad_karma, good_karma, [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestSameUser() {
  Database db;
  db.Put({"id1", "Don't sell", "master", 1536107260, 1000});
  db.Put({"id2", "Rethink life", "master", 1536107260, 2000});

  int count = 0;
  db.AllByUser("master", [&count](const Record&) {
    ++count;
    return true;
  });

  ASSERT_EQUAL(2, count);
}

void TestReplacement() {
  const string final_body = "Feeling sad";

  Database db;
  db.Put({"id", "Have a hand", "not-master", 1536107260, 10});
  db.Erase("id");
  db.Put({"id", final_body, "not-master", 1536107260, -10});

  auto record = db.GetById("id");
  ASSERT(record != nullptr);
  ASSERT_EQUAL(final_body, record->title);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestRangeBoundaries);
  RUN_TEST(tr, TestSameUser);
  RUN_TEST(tr, TestReplacement);
  return 0;
}
