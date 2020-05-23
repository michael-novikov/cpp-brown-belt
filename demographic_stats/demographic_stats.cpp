#include <iostream>
#include <vector>

using namespace std;

#ifdef Dev

enum class Gender {
  FEMALE,
  MALE
};

struct Person {
  int age;  // возраст
  Gender gender;  // пол
  bool is_employed;  // имеет ли работу
};

template <typename InputIt>
int ComputeMedianAge(InputIt range_begin, InputIt range_end) {
  if (range_begin == range_end) {
    return 0;
  }
  vector<typename InputIt::value_type> range_copy(range_begin, range_end);
  auto middle = begin(range_copy) + range_copy.size() / 2;
  nth_element(
      begin(range_copy), middle, end(range_copy),
      [](const Person& lhs, const Person& rhs) {
        return lhs.age < rhs.age;
      }
  );
  return middle->age;
}

#endif

void PrintStats(vector<Person> persons) {
  auto is_female = [](const Person& p) {return p.gender == Gender::FEMALE;};
  auto is_employed = [](const Person& p) {return p.is_employed;};

  cout << "Median age = " << ComputeMedianAge(begin(persons), end(persons)) << endl;

  auto end_females = partition(begin(persons), end(persons), is_female);
  cout << "Median age for females = " << ComputeMedianAge(begin(persons), end_females) << endl;
  cout << "Median age for males = " << ComputeMedianAge(end_females, end(persons)) << endl;

  auto end_females_employed = partition(begin(persons), end_females, is_employed);
  cout << "Median age for employed females = " << ComputeMedianAge(begin(persons), end_females_employed) << endl;
  cout << "Median age for unemployed females = " << ComputeMedianAge(end_females_employed, end_females) << endl;

  auto end_males_employed = partition(end_females, end(persons), is_employed);
  cout << "Median age for employed males = " << ComputeMedianAge(end_females, end_males_employed) << endl;
  cout << "Median age for unemployed males = " << ComputeMedianAge(end_males_employed, end(persons)) << endl;
}

int main() {
  vector<Person> persons = {
      {31, Gender::MALE, false},
      {40, Gender::FEMALE, true},
      {24, Gender::MALE, true},
      {20, Gender::FEMALE, true},
      {80, Gender::FEMALE, false},
      {78, Gender::MALE, false},
      {10, Gender::FEMALE, false},
      {55, Gender::MALE, true},
  };
  PrintStats(persons);
  return 0;
}

