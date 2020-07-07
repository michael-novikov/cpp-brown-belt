#include "test_runner.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <utility>
#include <map>
#include <optional>
#include <unordered_set>
#include <unordered_map>
#include <functional>

using namespace std;

struct HttpRequest {
  string method, path, body;
  map<string, string> get_params;
};

pair<string, string> SplitBy(const string& what, const string& by) {
  size_t pos = what.find(by);
  if (by.size() < what.size() && pos < what.size() - by.size()) {
    return {what.substr(0, pos), what.substr(pos + by.size())};
  } else {
    return {what, {}};
  }
}

template<typename T>
T FromString(const string& s) {
  T x;
  istringstream is(s);
  is >> x;
  return x;
}

pair<size_t, string> ParseIdAndContent(const string& body) {
  auto [id_string, content] = SplitBy(body, " ");
  return {FromString<size_t>(id_string), content};
}

struct LastCommentInfo {
  size_t user_id, consecutive_count;
};

struct HttpHeader {
  string name, value;
};

ostream& operator<<(ostream& output, const HttpHeader& h) {
  return output << h.name << ": " << h.value;
}

bool operator==(const HttpHeader& lhs, const HttpHeader& rhs) {
  return lhs.name == rhs.name && lhs.value == rhs.value;
}

enum class HttpCode {
  Ok = 200,
  NotFound = 404,
  Found = 302,
};

ostream& operator << (ostream& output, const HttpCode& code) {
  switch(code) {
    case HttpCode::Ok: output << "OK"; break;
    case HttpCode::NotFound: output << "Not found"; break;
    case HttpCode::Found: output << "Found"; break;
    default: output.setstate(ios_base::failbit);
  }
  return output;
}

class HttpResponse {
public:
  explicit HttpResponse(HttpCode code) {
    SetCode(code);
  }

  HttpResponse& AddHeader(string name, string value) {
    headers.push_back({name, value});
    return *this;
  }

  HttpResponse& SetContent(string a_content) {
    content_length = a_content.size();
    content = move(a_content);
    return *this;
  }

  HttpResponse& SetCode(HttpCode a_code) {
    code = a_code;
    return *this;
  }

  friend ostream& operator << (ostream& output, const HttpResponse& resp) {
    static const string version = "HTTP/1.1";
    static const string content_length_header = "Content-Length";

    output
      << version << ' ' << static_cast<size_t>(resp.code) << ' ' << resp.code << '\n';

    for (const auto& header : resp.headers) {
      if (header.name != content_length_header) {
        output << header.name << ": " << header.value << '\n';
      }
    }

    if (resp.GetContent().size() > 0) {
      output << content_length_header << ": " << resp.content_length << '\n';
    }

    output << '\n';
    output << resp.content;

    return output;
  }

  HttpCode GetCode() const {
    return code;
  }

  const vector<HttpHeader>& GetHeaders() const {
    return headers;
  }

  const string& GetContent() const {
    return content;
  }

private:
  HttpCode code;
  vector<HttpHeader> headers;
  size_t content_length;
  string content;
};

class CommentServer {
public:
  HttpResponse ServeRequest(const HttpRequest& req) {
    const auto it = handlers.find({req.method, req.path});
    const auto& handler = it == end(handlers) ? NotFound : it->second;
    return handler(*this, req);
  }

private:
  vector<vector<string>> comments_;
  std::optional<LastCommentInfo> last_comment;
  unordered_set<size_t> banned_users;

  using Handler = function<HttpResponse(CommentServer&, const HttpRequest&)>;

  static HttpResponse PostAddUser(CommentServer& server, const HttpRequest& req);
  static HttpResponse PostAddComment(CommentServer& server, const HttpRequest& req);
  static HttpResponse PostCheckCaptcha(CommentServer& server, const HttpRequest& req);

  static HttpResponse GetUserComments(CommentServer& server, const HttpRequest& req);
  static HttpResponse GetCaptcha(CommentServer& server, const HttpRequest& req);

  static HttpResponse NotFound(CommentServer& server, const HttpRequest& req);

  const map<pair<string, string>, Handler> handlers = {
    {{"POST", "/add_user"}, CommentServer::PostAddUser},
    {{"POST", "/add_comment"}, CommentServer::PostAddComment},
    {{"POST", "/checkcaptcha"}, CommentServer::PostCheckCaptcha},

    {{"GET", "/user_comments"}, CommentServer::GetUserComments},
    {{"GET", "/captcha"}, CommentServer::GetCaptcha},
  };
};

HttpResponse CommentServer::PostAddUser(CommentServer& server, const HttpRequest& req) {
    server.comments_.emplace_back();
    auto content = to_string(server.comments_.size() - 1);

    return HttpResponse(HttpCode::Ok)
      .SetContent(content);
}

HttpResponse CommentServer::PostAddComment(CommentServer& server, const HttpRequest& req) {
  auto [user_id, comment] = ParseIdAndContent(req.body);

  if (!server.last_comment || server.last_comment->user_id != user_id) {
    server.last_comment = LastCommentInfo {user_id, 1};
  } else if (++server.last_comment->consecutive_count > 3) {
    server.banned_users.insert(user_id);
  }

  if (server.banned_users.count(user_id) == 0) {
    server.comments_[user_id].push_back(string(comment));
    return HttpResponse(HttpCode::Ok);
  } else {
    return HttpResponse(HttpCode::Found)
      .AddHeader("Location", "/captcha");
  }
}

HttpResponse CommentServer::PostCheckCaptcha(CommentServer& server, const HttpRequest& req) {
  if (auto [id, response] = ParseIdAndContent(req.body); response == "42") {
    server.banned_users.erase(id);
    if (server.last_comment && server.last_comment->user_id == id) {
      server.last_comment.reset();
    }
    return HttpResponse(HttpCode::Ok);
  }

  return HttpResponse(HttpCode::Found)
    .AddHeader("Location", "/captcha");
}

HttpResponse CommentServer::GetUserComments(CommentServer& server, const HttpRequest& req) {
  auto user_id = FromString<size_t>(req.get_params.at("user_id"));
  string response;
  for (const string& c : server.comments_[user_id]) {
    response += c + '\n';
  }

  return HttpResponse(HttpCode::Ok)
    .SetContent(response);
}

HttpResponse CommentServer::GetCaptcha(CommentServer& server, const HttpRequest& req) {
  return HttpResponse(HttpCode::Ok)
    .SetContent("What's the answer for The Ultimate Question of Life, the Universe, and Everything?");
}

HttpResponse CommentServer::NotFound(CommentServer& server, const HttpRequest& req) {
  return HttpResponse(HttpCode::NotFound);
}

struct ParsedResponse {
  int code;
  vector<HttpHeader> headers;
  string content;
};

istream& operator >>(istream& input, ParsedResponse& r) {
  string line;
  getline(input, line);

  {
    istringstream code_input(line);
    string dummy;
    code_input >> dummy >> r.code;
  }

  size_t content_length = 0;

  r.headers.clear();
  while (getline(input, line) && !line.empty()) {
    if (auto [name, value] = SplitBy(line, ": "); name == "Content-Length") {
      istringstream length_input(value);
      length_input >> content_length;
    } else {
      r.headers.push_back( {std::move(name), std::move(value)});
    }
  }

  r.content.resize(content_length);
  input.read(r.content.data(), r.content.size());
  return input;
}

void Test(CommentServer& srv, const HttpRequest& request, const ParsedResponse& expected) {
  stringstream ss;
  HttpResponse resp = srv.ServeRequest(request);

  ASSERT_EQUAL(static_cast<int>(resp.GetCode()), expected.code);
  ASSERT_EQUAL(resp.GetHeaders(), expected.headers);
  ASSERT_EQUAL(resp.GetContent(), expected.content);
}

template <typename CommentServer>
void TestServer() {
  CommentServer cs;

  const ParsedResponse ok{200};
  const ParsedResponse redirect_to_captcha{302, {{"Location", "/captcha"}}, {}};
  const ParsedResponse not_found{404};

  Test(cs, {"POST", "/add_user"}, {200, {}, "0"});
  Test(cs, {"POST", "/add_user"}, {200, {}, "1"});
  Test(cs, {"POST", "/add_comment", "0 Hello"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Hi"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Buy my goods"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Enlarge"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Buy my goods"}, redirect_to_captcha);
  Test(cs, {"POST", "/add_comment", "0 What are you selling?"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Buy my goods"}, redirect_to_captcha);
  Test(
    cs,
    {"GET", "/user_comments", "", {{"user_id", "0"}}},
    {200, {}, "Hello\nWhat are you selling?\n"}
  );
  Test(
    cs,
    {"GET", "/user_comments", "", {{"user_id", "1"}}},
    {200, {}, "Hi\nBuy my goods\nEnlarge\n"}
  );
  Test(
    cs,
    {"GET", "/captcha"},
    {200, {}, {"What's the answer for The Ultimate Question of Life, the Universe, and Everything?"}}
  );
  Test(cs, {"POST", "/checkcaptcha", "1 24"}, redirect_to_captcha);
  Test(cs, {"POST", "/checkcaptcha", "1 42"}, ok);
  Test(cs, {"POST", "/add_comment", "1 Sorry! No spam any more"}, ok);
  Test(
    cs,
    {"GET", "/user_comments", "", {{"user_id", "1"}}},
    {200, {}, "Hi\nBuy my goods\nEnlarge\nSorry! No spam any more\n"}
  );

  Test(cs, {"GET", "/user_commntes"}, not_found);
  Test(cs, {"POST", "/add_uesr"}, not_found);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestServer<CommentServer>);
}
