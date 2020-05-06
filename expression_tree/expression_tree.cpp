#include "Common.h"
#include "test_runner.h"

#include <cctype>
#include <sstream>
#include <functional>

using namespace std;

class ValueExpression : public Expression {
public:
  explicit ValueExpression(int value) : value_(value) {}

  virtual int Evaluate() const override {
    return value_;
  }

  virtual std::string ToString() const override {
    ostringstream out;
    out << std::to_string(value_);
    return out.str();
  }

protected:
  int value_;
};

template <typename BinaryOperation, char OpSymbol = 'o'>
class BinaryExpression : public Expression {
public:
  explicit BinaryExpression(ExpressionPtr left, ExpressionPtr right)
    : left_(move(left))
    , right_(move(right))
  {
  }

  virtual int Evaluate() const override {
    return op(left_->Evaluate(), right_->Evaluate());
  }

  virtual std::string ToString() const override {
    ostringstream out;
    out << '(' << left_->ToString() << ')' << op_symbol << '(' << right_->ToString() << ')';
    return out.str();
  }
protected:
  static const char op_symbol = OpSymbol;
  BinaryOperation op;
  ExpressionPtr left_;
  ExpressionPtr right_;
};

using SumExpression = BinaryExpression<std::plus<int>, '+'>;
using MultExpression = BinaryExpression<std::multiplies<int>, '*'>;

ExpressionPtr Value(int value) {
  return make_unique<ValueExpression>(value);
}

ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
  return make_unique<SumExpression>(move(left), move(right));
}

ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
  return make_unique<MultExpression>(move(left), move(right));
}

string Print(const Expression* e) {
  if (!e) {
    return "Null expression provided";
  }
  stringstream output;
  output << e->ToString() << " = " << e->Evaluate();
  return output.str();
}

void Test() {
  ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
  ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14");

  ExpressionPtr e2 = Sum(move(e1), Value(5));
  ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19");

  ASSERT_EQUAL(Print(e1.get()), "Null expression provided");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, Test);
  return 0;
}
