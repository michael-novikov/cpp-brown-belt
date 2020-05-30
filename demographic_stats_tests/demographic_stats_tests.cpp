#include <algorithm>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
#include <sstream>

#include "test_runner.h"

using namespace std;

enum class Gender {
  FEMALE,
  MALE
};

struct Person {
  int age;
  Gender gender;
  bool is_employed;
};

bool operator==(const Person& lhs, const Person& rhs) {
  return lhs.age == rhs.age
      && lhs.gender == rhs.gender
      && lhs.is_employed == rhs.is_employed;
}

ostream& operator<<(ostream& stream, const Person& person) {
  return stream << "Person(age=" << person.age
      << ", gender=" << static_cast<int>(person.gender)
      << ", is_employed=" << boolalpha << person.is_employed << ")";
}

struct AgeStats {
  int total;
  int females;
  int males;
  int employed_females;
  int unemployed_females;
  int employed_males;
  int unemployed_males;
};

template <typename InputIt>
int ComputeMedianAge(InputIt range_begin, InputIt range_end) {
  if (range_begin == range_end) {
    return 0;
  }
  vector<typename iterator_traits<InputIt>::value_type> range_copy(
      range_begin,
      range_end
  );
  auto middle = begin(range_copy) + range_copy.size() / 2;
  nth_element(
      begin(range_copy), middle, end(range_copy),
      [](const Person& lhs, const Person& rhs) {
        return lhs.age < rhs.age;
      }
  );
  return middle->age;
}

vector<Person> ReadPersons(istream& in_stream = cin) {
  int person_count;
  in_stream >> person_count;
  vector<Person> persons;
  persons.reserve(person_count);
  for (int i = 0; i < person_count; ++i) {
    int age, gender, is_employed;
    in_stream >> age >> gender >> is_employed;
    Person person{
        age,
        static_cast<Gender>(gender),
        is_employed == 1
    };
    persons.push_back(person);
  }
  return persons;
}

AgeStats ComputeStats(vector<Person> persons) {
  //                 persons
  //                |       |
  //          females        males
  //         |       |      |     |
  //      empl.  unempl. empl.   unempl.

  auto females_end = partition(
      begin(persons), end(persons),
      [](const Person& p) {
        return p.gender == Gender::FEMALE;
      }
  );
  auto employed_females_end = partition(
      begin(persons), females_end,
      [](const Person& p) {
        return p.is_employed;
      }
  );
  auto employed_males_end = partition(
      females_end, end(persons),
      [](const Person& p) {
        return p.is_employed;
      }
  );

  return {
       ComputeMedianAge(begin(persons), end(persons)),
       ComputeMedianAge(begin(persons), females_end),
       ComputeMedianAge(females_end, end(persons)),
       ComputeMedianAge(begin(persons),  employed_females_end),
       ComputeMedianAge(employed_females_end, females_end),
       ComputeMedianAge(females_end, employed_males_end),
       ComputeMedianAge(employed_males_end, end(persons))
  };
}

void PrintStats(const AgeStats& stats,
                ostream& out_stream = cout) {
  out_stream << "Median age = "
             << stats.total              << endl
             << "Median age for females = "
             << stats.females            << endl
             << "Median age for males = "
             << stats.males              << endl
             << "Median age for employed females = "
             << stats.employed_females   << endl
             << "Median age for unemployed females = "
             << stats.unemployed_females << endl
             << "Median age for employed males = "
             << stats.employed_males     << endl
             << "Median age for unemployed males = "
             << stats.unemployed_males   << endl;
}

void TestPersonEqual() {
  ASSERT_EQUAL((Person{18, Gender::FEMALE, true}), (Person{18, Gender::FEMALE, true}));

  ASSERT((Person{18, Gender::FEMALE, true} == Person{18, Gender::FEMALE, true}));
  ASSERT((Person{20, Gender::FEMALE, true} == Person{20, Gender::FEMALE, true}));
  ASSERT((Person{18, Gender::MALE, true} == Person{18, Gender::MALE, true}));
  ASSERT((Person{18, Gender::FEMALE, false} == Person{18, Gender::FEMALE, false}));

  ASSERT(!(Person{18, Gender::FEMALE, true} == Person{19, Gender::FEMALE, true}));
  ASSERT(!(Person{18, Gender::FEMALE, true} == Person{18, Gender::MALE, true}));
  ASSERT(!(Person{18, Gender::FEMALE, true} == Person{18, Gender::FEMALE, false}));
}

void TestPersonPrint() {
  {
    ostringstream actual;
    actual << Person{18, Gender::FEMALE, true};
    string expected = "Person(age=18, gender=0, is_employed=true)";
    ASSERT_EQUAL(actual.str(), expected);
  }
  {
    ostringstream actual;
    actual << Person{19, Gender::FEMALE, true};
    string expected = "Person(age=19, gender=0, is_employed=true)";
    ASSERT_EQUAL(actual.str(), expected);
  }
  {
    ostringstream actual;
    actual << Person{18, Gender::MALE, true};
    string expected = "Person(age=18, gender=1, is_employed=true)";
    ASSERT_EQUAL(actual.str(), expected);
  }
  {
    ostringstream actual;
    actual << Person{18, Gender::FEMALE, false};
    string expected = "Person(age=18, gender=0, is_employed=false)";
    ASSERT_EQUAL(actual.str(), expected);
  }
}

void TestComputeMedianAge() {
  { // Test empty
    vector<Person> empty;
    ASSERT_EQUAL(ComputeMedianAge(begin(empty), end(empty)), 0);
  }
  { // Test 1 element
    vector<Person> one_element = { Person{18, Gender::FEMALE, true} };
    ASSERT_EQUAL(ComputeMedianAge(begin(one_element), end(one_element)), 18);
  }
  { // Test even number of element
    vector<Person> even_number_of_elements = { Person{18, Gender::FEMALE, true}, Person{20, Gender::FEMALE, true} };
    ASSERT_EQUAL(ComputeMedianAge(begin(even_number_of_elements), end(even_number_of_elements)), 20);
  }
  { // Test even number of element not sorted
    vector<Person> even_not_sorted = { Person{20, Gender::FEMALE, true}, Person{18, Gender::FEMALE, true} };
    ASSERT_EQUAL(ComputeMedianAge(begin(even_not_sorted), end(even_not_sorted)), 20);
  }
  { // Test odd number of element
    vector<Person> odd_number_of_elements = { Person{18, Gender::FEMALE, true}, Person{20, Gender::FEMALE, true}, Person{22, Gender::FEMALE, true} };
    ASSERT_EQUAL(ComputeMedianAge(begin(odd_number_of_elements), end(odd_number_of_elements)), 20);
  }
  { // Test odd number of element
    vector<Person> odd_not_sorted = { Person{20, Gender::FEMALE, true}, Person{18, Gender::FEMALE, true}, Person{22, Gender::FEMALE, true} };
    ASSERT_EQUAL(ComputeMedianAge(begin(odd_not_sorted), end(odd_not_sorted)), 20);
  }
}

void TestReadPersons() {
  { // Test empty
    istringstream empty {"0"};
    ASSERT_EQUAL(ReadPersons(empty), vector<Person>{});
  }
  { // Test 1 element
    istringstream one_element {"1 18 0 1"};
    ASSERT_EQUAL(ReadPersons(one_element), (vector<Person>{ {18, Gender::FEMALE, true} }));
  }
  { // Test 2 element
    istringstream two_element {"2 18 0 1 20 1 0"};
    ASSERT_EQUAL(ReadPersons(two_element), (vector<Person>{ {18, Gender::FEMALE, true}, {20, Gender::MALE, false} }));
  }
}

void TestComputeStats() {
  auto age_stats_equal = [](const AgeStats& lhs, const AgeStats& rhs) -> bool {
    return lhs.total == rhs.total
      && lhs.females == rhs.females
      && lhs.males == rhs.males
      && lhs.employed_females == rhs.employed_females
      && lhs.unemployed_females == rhs.unemployed_females
      && lhs.employed_males == rhs.employed_males
      && lhs.unemployed_males == rhs.unemployed_males;
  };

  { // Test empty
    ASSERT(age_stats_equal(
        ComputeStats(vector<Person>{}),
        AgeStats{ 0, 0, 0, 0, 0, 0, 0 }
    ));
  }
  { // Test one employed female
    ASSERT(age_stats_equal(
        ComputeStats(vector<Person>{ {18, Gender::FEMALE, true} }),
        AgeStats{ 18, 18, 0, 18, 0, 0, 0 }
    ));
  }
  { // Test one unemployed female
    ASSERT(age_stats_equal(
        ComputeStats(vector<Person>{ {18, Gender::FEMALE, false} }),
        AgeStats{ 18, 18, 0, 0, 18, 0, 0 }
    ));
  }
  { // Test one employed male
    ASSERT(age_stats_equal(
        ComputeStats(vector<Person>{ {20, Gender::MALE, true} }),
        AgeStats{ 20, 0, 20, 0, 0, 20, 0 }
    ));
  }
  { // Test one unemployed male
    ASSERT(age_stats_equal(
        ComputeStats(vector<Person>{ {20, Gender::MALE, false} }),
        AgeStats{ 20, 0, 20, 0, 0, 0, 20 }
    ));
  }
  { // Test 1 + 1 females
    ASSERT(age_stats_equal(
        ComputeStats(vector<Person>{ {20, Gender::FEMALE, true}, {22, Gender::FEMALE, false} }),
        AgeStats{ 22, 22, 0, 20, 22, 0, 0 }
    ));
  }
  { // Test 1 + 1 males
    ASSERT(age_stats_equal(
        ComputeStats(vector<Person>{ {20, Gender::MALE, true}, {22, Gender::MALE, false} }),
        AgeStats{ 22, 0, 22, 0, 0, 20, 22 }
    ));
  }
  { // Test 2 + 2 males and females
    ASSERT(age_stats_equal(
        ComputeStats(vector<Person>{ {19, Gender::FEMALE, true}, {18, Gender::FEMALE, false}, {21, Gender::MALE, true}, {20, Gender::MALE, false} }),
        AgeStats{ 20, 19, 21, 19, 18, 21, 20 }
    ));
  }
}

void TestPrintStats() {
  { // Test empty
    string expected = R"(Median age = 0
Median age for females = 0
Median age for males = 0
Median age for employed females = 0
Median age for unemployed females = 0
Median age for employed males = 0
Median age for unemployed males = 0
)";

    AgeStats empty { 0, 0, 0, 0, 0, 0, 0 };
    ostringstream stats_str;
    PrintStats(empty, stats_str);

    ASSERT_EQUAL(stats_str.str(), expected);
  }
  { // Test prints all values
    string expected = R"(Median age = 1
Median age for females = 2
Median age for males = 3
Median age for employed females = 4
Median age for unemployed females = 5
Median age for employed males = 6
Median age for unemployed males = 7
)";

    AgeStats empty { 1, 2, 3, 4, 5, 6, 7 };
    ostringstream stats_str;
    PrintStats(empty, stats_str);

    ASSERT_EQUAL(stats_str.str(), expected);
  }
  { // Test 1 employed females
    string expected = R"(Median age = 18
Median age for females = 18
Median age for males = 0
Median age for employed females = 18
Median age for unemployed females = 0
Median age for employed males = 0
Median age for unemployed males = 0
)";

    AgeStats employed_females { 18, 18, 0, 18, 0, 0, 0 };
    ostringstream stats_str;
    PrintStats(employed_females, stats_str);

    ASSERT_EQUAL(stats_str.str(), expected);
  }
  { // Test 1 unemployed female
    string expected = R"(Median age = 18
Median age for females = 18
Median age for males = 0
Median age for employed females = 0
Median age for unemployed females = 18
Median age for employed males = 0
Median age for unemployed males = 0
)";

    AgeStats unemployed_females { 18, 18, 0, 0, 18, 0, 0 };
    ostringstream stats_str;
    PrintStats(unemployed_females, stats_str);

    ASSERT_EQUAL(stats_str.str(), expected);
  }
  { // Test 1 employed male
    string expected = R"(Median age = 18
Median age for females = 0
Median age for males = 18
Median age for employed females = 0
Median age for unemployed females = 0
Median age for employed males = 18
Median age for unemployed males = 0
)";

    AgeStats employed_females { 18, 0, 18, 0, 0, 18, 0 };
    ostringstream stats_str;
    PrintStats(employed_females, stats_str);

    ASSERT_EQUAL(stats_str.str(), expected);
  }
  { // Test 1 unemployed male
    string expected = R"(Median age = 18
Median age for females = 0
Median age for males = 18
Median age for employed females = 0
Median age for unemployed females = 0
Median age for employed males = 0
Median age for unemployed males = 18
)";

    AgeStats employed_females { 18, 0, 18, 0, 0, 0, 18 };
    ostringstream stats_str;
    PrintStats(employed_females, stats_str);

    ASSERT_EQUAL(stats_str.str(), expected);
  }
}

int main() {
  TestRunner tr;

  RUN_TEST(tr, TestPersonEqual);
  RUN_TEST(tr, TestPersonPrint);
  RUN_TEST(tr, TestComputeMedianAge);
  RUN_TEST(tr, TestReadPersons);
  RUN_TEST(tr, TestComputeStats);
  RUN_TEST(tr, TestPrintStats);

  return 0;
}

