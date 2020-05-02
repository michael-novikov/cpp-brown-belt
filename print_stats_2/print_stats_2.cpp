#include "test_runner.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>
#include <string>
#include <numeric>
#include <sstream>
#include <map>
#include <functional>
#include <memory>

using namespace std;

template <typename Iterator>
class IteratorRange {
public:
  IteratorRange(Iterator begin, Iterator end)
    : first(begin)
    , last(end)
  {
  }

  Iterator begin() const {
    return first;
  }

  Iterator end() const {
    return last;
  }

private:
  Iterator first, last;
};

template <typename Collection>
auto Head(Collection& v, size_t top) {
  return IteratorRange{v.begin(), next(v.begin(), min(top, v.size()))};
}

struct Person {
  string name;
  int age, income;
  bool is_male;
};

class PeopleDB {
public:
  void ReadPeople(istream& input) {
    int count;
    input >> count;

    people.resize(count);
    for (Person& p : people) {
      char gender;
      input >> p.name >> p.age >> p.income >> gender;
      p.is_male = gender == 'M';
    }

    SetPopularNames();
    ComputeSortedByAge();
    ComputeTotalIncome();
  }

  int GetPeopleForMaturityAge(int adult_age) const {
    return std::distance(
      lower_bound(
        begin(sorted_by_age), end(sorted_by_age), 
        adult_age,
        [](const Person& lhs, int age) {
          return lhs.age < age;
        }),
      end(sorted_by_age)
    );
  }

  int GetTotalIncome(int top_n) const {
    if (top_n < 1) {
      return 0;
    }

    if (top_n > cummulative_incomes.size()) {
      top_n = cummulative_incomes.size();
    }

    return cummulative_incomes[top_n - 1];
  }

optional<string> GetMostPopularName(bool is_male) const {
  return is_male ? popular_name_m : popular_name_f;
}

private:
  vector<Person> people;
  vector<Person> sorted_by_age;
  vector<int> cummulative_incomes;

  optional<string> popular_name_m;
  optional<string> popular_name_f;

  static bool LessName(const Person& lhs, const Person& rhs) {
    return lhs.age < rhs.age;
  }

  void ComputeSortedByAge() {
    sorted_by_age.resize(people.size());
    copy(begin(people), end(people), begin(sorted_by_age));
    sort(begin(sorted_by_age), end(sorted_by_age), LessName);
  }

  void ComputeTotalIncome() {
    vector<int> incomes(people.size());
    transform(begin(people), end(people), begin(incomes),
       [](const Person& p) { return p.income; });
    sort(begin(incomes), end(incomes), greater<int>());

    cummulative_incomes.resize(incomes.size());
    partial_sum(begin(incomes), end(incomes), begin(cummulative_incomes));
  }

  template<typename Iter>
  optional<string> FindPopularName(IteratorRange<Iter> range) {
    if (begin(range) == end(range)) {
      return nullopt;
    }

    sort(
      begin(range), end(range),
      [](const Person& lhs, const Person& rhs) { return lhs.name < rhs.name; }
    );

    string* most_popular_name = &(range.begin()->name);
    int count = 1;
    for (auto i = begin(range); i != end(range); ) {
      auto same_name_end = find_if_not(i, end(range), [i](const Person& p) {
        return p.name == i->name;
      });
      auto cur_name_count = std::distance(i, same_name_end);
      if (cur_name_count > count) {
        count = cur_name_count;
        most_popular_name = &(i->name);
      }
      i = same_name_end;
    }
    return *most_popular_name;
  }

  void SetPopularNames() {
    auto people_copy = people;
    auto border = partition(
      begin(people_copy), end(people_copy),
      [](const Person& p) { return p.is_male; }
    );

    IteratorRange males{begin(people_copy), border};
    IteratorRange females{border, end(people_copy)};

    popular_name_m = FindPopularName(males);
    popular_name_f = FindPopularName(females);
  }
};

void ProcessCommands(const PeopleDB& db, istream& in, ostream& out) {
  for (string command; in >> command; ) {
    if (command == "AGE") {
      int adult_age;
      in >> adult_age;

      out << "There are "
        << db.GetPeopleForMaturityAge(adult_age)
        << " adult people for maturity age " << adult_age << '\n';
    } else if (command == "WEALTHY") {
      int count;
      in >> count;

      out << "Top-" << count << " people have total income "
        << db.GetTotalIncome(count) << '\n';
    } else if (command == "POPULAR_NAME") {
      char gender;
      in >> gender;
      bool is_male = (gender == 'M');

      if (auto popular_name = db.GetMostPopularName(is_male); popular_name) {
        out << "Most popular name among people of gender " << gender << " is "
             << *popular_name << '\n';
      } else {
        out << "No people of gender " << gender << '\n';
      }
    }
  }
}

void Test1() {
  istringstream input {
R"(11
Ivan 25 1000 M
Olga 30 623 W
Sergey 24 825 M
Maria 42 1254 W
Mikhail 15 215 M
Oleg 18 230 M
Denis 53 8965 M
Maxim 37 9050 M
Ivan 47 19050 M
Ivan 17 50 M
Olga 23 550 W
AGE 18
AGE 25
WEALTHY 5
POPULAR_NAME M
POPULAR_NAME W
)"
  };

  PeopleDB db;
  db.ReadPeople(input);

  ostringstream output;
  ProcessCommands(db, input, output);

  const string expected {
R"(There are 9 adult people for maturity age 18
There are 6 adult people for maturity age 25
Top-5 people have total income 39319
Most popular name among people of gender M is Ivan
Most popular name among people of gender W is Olga
)"
  };

  ASSERT_EQUAL(expected, output.str());
}

void RunTests() {
  TestRunner tr;
  RUN_TEST(tr, Test1);
}

int main() {
  RunTests();

  PeopleDB db;
  db.ReadPeople(cin);
  ProcessCommands(db, cin, cout);
}
