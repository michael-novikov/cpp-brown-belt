#include "test_runner.h"

#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

using namespace std;

struct Record {
  string id;
  string title;
  string user;
  int timestamp;
  int karma;

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
  map<string, Record> data;
  multimap<int, Record*> ranged_by_timestamp;
  multimap<int, Record*> ranged_by_karma;
  multimap<string, Record*> ranged_by_user;

  template<typename Key, typename Callback>
  void RangeBy(const multimap<Key, Record*>& ranged_by_key,
      const Key& low, const Key& high, Callback callback) const;

  template <typename Key>
  void RemoveFromRanged(multimap<Key, Record*>& ranged_by_key,
      Key key, Record* value);
};

bool Database::Put(const Record& record) {
  auto insert_info = data.try_emplace(record.id, record);
  auto it = insert_info.first;
  bool inserted = insert_info.second;

  if (inserted) {
    auto address = std::addressof(it->second);
    ranged_by_timestamp.emplace(record.timestamp, address);
    ranged_by_karma.emplace(record.karma, address);
    ranged_by_user.emplace(record.user, address);
  }

  return inserted;
}

const Record* Database::GetById(const string& id) const {
  auto it = data.find(id);
  return it == end(data) ? nullptr : std::addressof(it->second);
}

template <typename Key>
void Database::RemoveFromRanged(multimap<Key, Record*>& ranged_by_key,
    Key key, Record* value) {
  auto value_eq = [value] (const pair<Key, Record*>& elem) {
    return elem.second == value;
  };

  auto eq_range = ranged_by_key.equal_range(key);
  ranged_by_key.erase(
    find_if(eq_range.first, eq_range.second, value_eq)
  );
}

bool Database::Erase(const string& id) {
  auto it = data.find(id);
  bool has_elem = it != end(data);

  if (has_elem) {
    auto& record = it->second;
    auto address = std::addressof(record);
    RemoveFromRanged(ranged_by_timestamp, record.timestamp, address);
    RemoveFromRanged(ranged_by_karma, record.karma, address);
    RemoveFromRanged(ranged_by_user, record.user, address);
  }

  data.erase(it);
  return has_elem;
}

template<typename Key, typename Callback>
void Database::RangeBy(const multimap<Key, Record*>& ranged_by_key,
    const Key& low, const Key& high, Callback callback) const {
  auto lb = ranged_by_key.lower_bound(low);
  auto ub = ranged_by_key.upper_bound(high);
  for (auto it = lb; it != ub && callback(*it->second); ++it) {}
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
