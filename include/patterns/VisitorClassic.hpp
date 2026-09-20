#pragma once

class Circle;
class Square;

class Visitor {
public:
  virtual void visit(Circle *circle) = 0;
  virtual void visit(Square *square) = 0;
};

class Shape {
  public:
    virtual ~Shape() = default;
    virtual void accept(Visitor& visitor) = 0;
};

class Circle : public Shape {
  public:
    double radius;

    Circle(double radius) : radius(radius) {};

    virtual void accept(Visitor &visitor) override {
      // Resolves to visit(Circle&) at compile time,
      // so the Circle knows its Visitor's method!
      visitor.visit(this);
    }
};

class Square : public Shape {
  public:
    double side;

    Square(double side) : side(side) {};

    virtual void accept(Visitor &visitor) override {
      // Resolves to visit(Square&) at compile time,
      // so the Square knows its Visitor's method!
      visitor.visit(this);
    }
};

class AreaCalculator : public Visitor {
  public:
    virtual void visit(Circle *circle) override {
      double area = std::numbers::pi * circle->radius * circle->radius;
      std::println("Circle area: {}", area);
    }

    virtual void visit(Square *square) override {
      double area = square->side * square->side;
      std::println("Square area: {}", area);
    }
};

void runVisitorClassic() {
  std::vector<std::unique_ptr<Shape>> shapes;
  shapes.push_back(std::make_unique<Circle>(2));
  shapes.push_back(std::make_unique<Square>(3));

  AreaCalculator calc;

  for (auto &shape : shapes) {
    shape->accept(calc);
  }
}