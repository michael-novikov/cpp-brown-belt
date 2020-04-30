#include "ini.h"

#include <iostream>
#include <exception>
#include <string_view>
#include <string>

using namespace std;

namespace Ini {

Section& Document::AddSection(string name) {
  return sections[move(name)];
}

const Section& Document::GetSection(const string& name) const {
  auto it = sections.find(name);

  if (it == end(sections)) {
    throw out_of_range("name not found");
  }

  return it->second;
}

size_t Document::SectionCount() const {
  return sections.size();
}

pair<string_view, string_view> Split(string_view line, char by) {
  size_t pos = line.find(by);
  string_view left = line.substr(0, pos);

  if (pos < line.size() && pos + 1 < line.size()) {
    return {left, line.substr(pos + 1)};
  } else {
    return {left, string_view()};
  }
}

string_view Unquote(string_view value, char front_quote, char back_quote) {
  if (!value.empty() && value.front() == front_quote) {
    value.remove_prefix(1);
  }
  if (!value.empty() && value.back() == back_quote) {
    value.remove_suffix(1);
  }
  return value;
}

string_view GetSectionName(string_view value) {
  return Unquote(value, '[', ']');
}

Document Load(istream& input) {
  Document doc;

  Section* section;
  for (string line; getline(input, line); ) {
    if (line.empty()) {
      continue;
    }
    else if (line.front() == '[') {
      section = &doc.AddSection(string{GetSectionName(line)});
    }
    else {
      section->insert(Split(line, '='));
    }
  }

  return doc;
}

}

