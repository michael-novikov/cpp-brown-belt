#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
  string from;
  string to;
  string body;
};


class Worker {
public:
  virtual ~Worker() = default;
  virtual void Process(unique_ptr<Email> email) = 0;
  virtual void Run() {
    // should be implemented for the first worker in pipeline
    throw logic_error("Unimplemented");
  }

protected:
  void PassOn(unique_ptr<Email> email) const;

public:
  void SetNext(unique_ptr<Worker> next);

private:
  unique_ptr<Worker> next_;
};

void Worker::PassOn(unique_ptr<Email> email) const {
  if (next_) {
    next_->Process(move(email));
  }
}

void Worker::SetNext(unique_ptr<Worker> next) {
  next_ = move(next);
}

class Reader : public Worker {
public:
  explicit Reader(istream& in);
  virtual void Process(unique_ptr<Email> email) override;
  void Run() override;
private:
  istream& in_;
};

unique_ptr<Email> ReadEmail(istream& in) {
  string from, to, body;
  bool read = getline(in, from)
    && getline(in, to)
    && getline(in, body);
  return read ? make_unique<Email>(Email{from, to, body}) : nullptr;
}

Reader::Reader(istream& in) : in_(in) {}

void Reader::Process(unique_ptr<Email> email) {
  PassOn(move(email));
}

void Reader::Run() {
  bool keep_read{true};
  while (keep_read) {
    auto email = ReadEmail(in_);
    if (email) {
      Process(move(email));
    }
    else {
      keep_read = false;
    }
  }
}

class Filter : public Worker {
public:
  using Function = function<bool(const Email&)>;

  explicit Filter(Function f) : filter_func(f) {}
  virtual void Process(unique_ptr<Email> email) override;
private:
  Function filter_func;
};

void Filter::Process(unique_ptr<Email> email) {
  if (filter_func(*email)) {
    PassOn(move(email));
  }
}

class Copier : public Worker {
public:
  explicit Copier(string to) : to_(move(to)) {}
  virtual void Process(unique_ptr<Email> email) override;
private:
  string to_;
};

void Copier::Process(unique_ptr<Email> email) {
  unique_ptr<Email> copy;
  if (to_ != email->to) {
    copy = make_unique<Email>(Email{email->from, to_, email->body});
  }

  PassOn(move(email));

  if (copy) {
    PassOn(move(copy));
  }
}

class Sender : public Worker {
public:
  explicit Sender(ostream& out) : out_(out) {}
  virtual void Process(unique_ptr<Email> email) override;
private:
  ostream& out_;
};

void Sender::Process(unique_ptr<Email> email) {
  out_
    << email->from << endl
    << email->to << endl
    << email->body << endl;

  PassOn(move(email));
}

class PipelineBuilder {
public:
  explicit PipelineBuilder(istream& in)
  {
    pipeline.push_back(make_unique<Reader>(in));
  }

  PipelineBuilder& FilterBy(Filter::Function filter) {
    pipeline.push_back(make_unique<Filter>(filter));
    return *this;
  }

  PipelineBuilder& CopyTo(string recipient) {
    pipeline.push_back(make_unique<Copier>(recipient));
    return *this;
  }

  PipelineBuilder& Send(ostream& out) {
    pipeline.push_back(make_unique<Sender>(out));
    return *this;
  }

  unique_ptr<Worker> Build() {
    if (pipeline.size() > 1) {
      for (int cur = pipeline.size() - 2; cur >= 0; --cur) {
        pipeline[cur]->SetNext(move(pipeline[cur + 1]));
      }
    }
    return move(pipeline.front());
  }
private:
  vector<unique_ptr<Worker>> pipeline;
};


void TestSanity() {
  string input = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "ralph@example.com\n"
    "erich@example.com\n"
    "I do not make mistakes of that kind\n"
  );
  istringstream inStream(input);
  ostringstream outStream;

  PipelineBuilder builder(inStream);
  builder.FilterBy([](const Email& email) {
    return email.from == "erich@example.com";
  });
  builder.CopyTo("richard@example.com");
  builder.Send(outStream);
  auto pipeline = builder.Build();

  pipeline->Run();

  string expectedOutput = (
    "erich@example.com\n"
    "richard@example.com\n"
    "Hello there\n"

    "erich@example.com\n"
    "ralph@example.com\n"
    "Are you sure you pressed the right button?\n"

    "erich@example.com\n"
    "richard@example.com\n"
    "Are you sure you pressed the right button?\n"
  );

  ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSanity);
  return 0;
}
