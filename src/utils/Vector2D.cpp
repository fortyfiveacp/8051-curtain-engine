#include "Vector2D.h"

#include <cmath>

bool Vector2D::operator==(const Vector2D& vector) const {
    return vector.x == this->x && vector.y == this->y;
}

bool Vector2D::operator!=(const Vector2D& vector) const {
    return vector.x != this->x || vector.y != this->y;
}

Vector2D Vector2D::operator-() const {
    return Vector2D (-x, -y);
}

// Member operator function so we can use the Vector2D that we want to change on the left hand side
Vector2D& Vector2D::operator+=(const Vector2D& vector) {
    this->x += vector.x;
    this->y += vector.y;
    return *this;
}

Vector2D& Vector2D::operator-=(const Vector2D& vector) {
    this->x -= vector.x;
    this->y -= vector.y;
    return *this;
}

Vector2D& Vector2D::operator*=(float scalar) {
    this->x *= scalar;
    this->y *= scalar;
    return *this;
}

Vector2D &Vector2D::operator/=(float scalar) {
    this->x /= scalar;
    this->y /= scalar;
    return *this;
}

Vector2D& Vector2D::operator*=(const Vector2D& vector) {
    this->x *= vector.x;
    this->y *= vector.y;
    return *this;
}

Vector2D& Vector2D::operator/=(const Vector2D& vector) {
    this->x /= vector.x;
    this->y /= vector.y;
    return *this;
}

Vector2D Vector2D::operator+(const Vector2D& vector) const {
    return Vector2D(this->x + vector.x, this->y + vector.y);
}

Vector2D Vector2D::operator-(const Vector2D& vector) const {
    return Vector2D(this->x - vector.x, this->y - vector.y);
}

// Member operator function so we can use the Vector2D on the left hand side
Vector2D Vector2D::operator*(float scalar) const {
    return Vector2D (x * scalar, y * scalar);
}

// Non-member operator function so we make use of putting the Vector2D on the right hand side
Vector2D operator*(float scalar, Vector2D& vector) {
    return Vector2D (vector.x * scalar, vector.y * scalar);
}

Vector2D Vector2D::operator/(float scalar) const {
    return Vector2D (x / scalar, y / scalar);
}

Vector2D& Vector2D::normalize() {
    // Pythagorean theorem
    float length = std::sqrt(x * x + y * y);

    // If the length was 4.4
    // x = x / 4.4
    // y = y / 4.4
    if (length > 0) {
        this->x /= length;
        this->y /= length;
    }

    return *this;
}