#include "Common.h"

#include <utility>

using namespace std;

class Shape : public IShape {
public:
  virtual void SetPosition(Point position) override {
    position_ = position;
  }

  virtual Point GetPosition() const override {
    return position_;
  }

  virtual void SetSize(Size size) override {
    size_ = size;
  }

  virtual Size GetSize() const override {
    return size_;
  }

  virtual void SetTexture(std::shared_ptr<ITexture> ptr) override {
    texture_ = move(ptr);
  }

  virtual ITexture* GetTexture() const override {
    return texture_.get();
  }

  virtual void Draw(Image& image) const override {
    pair<Point, Point> border{
      position_,
      { position_.x + size_.width, position_.y + size_.height }
    };

    auto texture_size = texture_ ? texture_->GetSize() : Size{ 0, 0 };
    pair<Point, Point> texture_border{
      position_,
      { position_.x + texture_size.width, position_.y + texture_size.height }
    };

    for (int y = border.first.y; y < border.second.y && y < image.size(); ++y) {
      for (int x = border.first.x; x < border.second.x && x < image[y].size(); ++x) {
        if (Contains({x, y})) {
          auto color = y < texture_border.second.y && x < texture_border.second.x
            ? texture_->GetImage()[y - texture_border.first.y][x - texture_border.first.x]
            : '.';
          image[y][x] = color;
        }
      }
    }
  }
protected:
  Point position_;
  Size size_;
  shared_ptr<ITexture> texture_;

  virtual bool Contains (Point p) const = 0;

  explicit Shape() = default;
  explicit Shape(Point position, Size size, shared_ptr<ITexture> texture)
    : position_(position)
    , size_(size)
    , texture_(move(texture))
  {
  }
};

class Rectangle : public Shape {
public:
  explicit Rectangle() = default;
  explicit Rectangle(Point position, Size size, shared_ptr<ITexture> texture)
    : Shape(position, size, texture)
  {
  }

  virtual std::unique_ptr<IShape> Clone() const override {
   return make_unique<Rectangle>(position_, size_, texture_);
  }

private:
  virtual bool Contains (Point p) const override {
    return p.y >= position_.y && p.y < position_.y + size_.height
      && p.x >= position_.x && p.x < position_.x + size_.width;
  };
};

class Ellipse : public Shape {
public:
  explicit Ellipse() = default;
  explicit Ellipse(Point position, Size size, shared_ptr<ITexture> texture)
    : Shape(position, size, texture)
  {
  }

  virtual std::unique_ptr<IShape> Clone() const override {
   return make_unique<Ellipse>(position_, size_, texture_);
  }

private:
  virtual bool Contains (Point p) const override {
    Point tmp{p.x - position_.x, p.y - position_.y};
    return IsPointInEllipse(tmp, size_);
  };
};

unique_ptr<IShape> MakeShape(ShapeType shape_type) {
  switch (shape_type) {
    case ShapeType::Rectangle:
      return make_unique<Rectangle>();
    case ShapeType::Ellipse:
      return make_unique<Ellipse>();
    default:
      return nullptr;
  }
}
