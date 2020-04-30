#include "geo2d.h"
#include "game_object.h"

#include "test_runner.h"

#include <vector>
#include <memory>

#define COLLIDE_WITH(THIS_TYPE, OTHER_TYPE) \
bool THIS_TYPE::CollideWith(const OTHER_TYPE& that) const { \
  return geo2d::Collide(Geometry(), that.Geometry()); \
}

#define COLLIDE_WITH_ALL(TYPE) \
COLLIDE_WITH(TYPE, Unit); \
COLLIDE_WITH(TYPE, Building); \
COLLIDE_WITH(TYPE, Tower); \
COLLIDE_WITH(TYPE, Fence);

#define COLLIDE_WITH_ALL_DECLARATION() \
virtual bool CollideWith(const Unit& that) const override; \
virtual bool CollideWith(const Building& that) const override; \
virtual bool CollideWith(const Tower& that) const override; \
virtual bool CollideWith(const Fence& that) const override;

using namespace std;

template <typename T>
class Collider : public GameObject {
  virtual bool Collide(const GameObject& that) const final {
    return that.CollideWith(static_cast<const T&>(*this));
  }
};

class Unit: public Collider<Unit> {
public:
  explicit Unit(geo2d::Point position) : obj_geometry(position) {}

  COLLIDE_WITH_ALL_DECLARATION();

  geo2d::Point Geometry() const {
    return obj_geometry;
  }

private:
  geo2d::Point obj_geometry;
};

class Building: public Collider<Building> {
public:
  explicit Building(geo2d::Rectangle geometry) : obj_geometry(geometry) {}

  COLLIDE_WITH_ALL_DECLARATION();

  geo2d::Rectangle Geometry() const {
    return obj_geometry;
  }
private:
  geo2d::Rectangle obj_geometry;
};

class Tower: public Collider<Tower> {
public:
  explicit Tower(geo2d::Circle geometry) : obj_geometry(geometry) {}

  COLLIDE_WITH_ALL_DECLARATION();

  geo2d::Circle Geometry() const {
    return obj_geometry;
  }
private:
  geo2d::Circle obj_geometry;
};

class Fence: public Collider<Fence> {
public:
  explicit Fence(geo2d::Segment geometry) : obj_geometry(geometry) {}

  COLLIDE_WITH_ALL_DECLARATION();

  geo2d::Segment Geometry() const {
    return obj_geometry;
  }
private:
  geo2d::Segment obj_geometry;
};

COLLIDE_WITH_ALL(Unit);
COLLIDE_WITH_ALL(Building);
COLLIDE_WITH_ALL(Tower);
COLLIDE_WITH_ALL(Fence);

bool Collide(const GameObject& first, const GameObject& second) {
  return first.Collide(second);
}

void TestAddingNewObjectOnMap() {
  // Юнит-тест моделирует ситуацию, когда на игровой карте уже есть какие-то объекты,
  // и мы хотим добавить на неё новый, например, построить новое здание или башню.
  // Мы можем его добавить, только если он не пересекается ни с одним из существующих.
  using namespace geo2d;

  const vector<shared_ptr<GameObject>> game_map = {
    make_shared<Unit>(Point{3, 3}),
    make_shared<Unit>(Point{5, 5}),
    make_shared<Unit>(Point{3, 7}),
    make_shared<Fence>(Segment{{7, 3}, {9, 8}}),
    make_shared<Tower>(Circle{Point{9, 4}, 1}),
    make_shared<Tower>(Circle{Point{10, 7}, 1}),
    make_shared<Building>(Rectangle{{11, 4}, {14, 6}})
  };

  for (size_t i = 0; i < game_map.size(); ++i) {
    Assert(
      Collide(*game_map[i], *game_map[i]),
      "An object doesn't collide with itself: " + to_string(i)
    );

    for (size_t j = 0; j < i; ++j) {
      Assert(
        !Collide(*game_map[i], *game_map[j]),
        "Unexpected collision found " + to_string(i) + ' ' + to_string(j)
      );
    }
  }

  auto new_warehouse = make_shared<Building>(Rectangle{{4, 3}, {9, 6}});
  ASSERT(!Collide(*new_warehouse, *game_map[0]));
  ASSERT( Collide(*new_warehouse, *game_map[1]));
  ASSERT(!Collide(*new_warehouse, *game_map[2]));
  ASSERT( Collide(*new_warehouse, *game_map[3]));
  ASSERT( Collide(*new_warehouse, *game_map[4]));
  ASSERT(!Collide(*new_warehouse, *game_map[5]));
  ASSERT(!Collide(*new_warehouse, *game_map[6]));

  auto new_defense_tower = make_shared<Tower>(Circle{{8, 2}, 2});
  ASSERT(!Collide(*new_defense_tower, *game_map[0]));
  ASSERT(!Collide(*new_defense_tower, *game_map[1]));
  ASSERT(!Collide(*new_defense_tower, *game_map[2]));
  ASSERT( Collide(*new_defense_tower, *game_map[3]));
  ASSERT( Collide(*new_defense_tower, *game_map[4]));
  ASSERT(!Collide(*new_defense_tower, *game_map[5]));
  ASSERT(!Collide(*new_defense_tower, *game_map[6]));
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestAddingNewObjectOnMap);
  return 0;
}
