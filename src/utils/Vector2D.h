#pragma once

class Vector2D {
public:
    float x = 0.0f;
    float y = 0.0f;

    Vector2D() {
        x = 0.0f;
        y = 0.0f;
    }

    Vector2D(float x, float y) : x(x), y(y) {}

    bool operator==(const Vector2D& vector) const;

    bool operator!=(const Vector2D& vector) const;

    Vector2D operator-() const;

    // Return a reference, so it returns itself
    // Vector that we want to return is on the left
    // Another vector is on the right
    // Member operator function
    Vector2D& operator+=(const Vector2D& vector);

    Vector2D& operator-=(const Vector2D& vector);

    Vector2D& operator*=(float scalar);

    Vector2D& operator/=(float scalar);

    Vector2D& operator*=(const Vector2D& vector);

    Vector2D& operator/=(const Vector2D& vector);

    Vector2D operator+(const Vector2D& vector) const;

    Vector2D operator-(const Vector2D& vector) const;

    // Member operator function
    // Vector2D on the left, float on the right
    Vector2D operator*(float scalar) const;

    // Vector2D on the right, float on the left
    // friend makes it a non-member function
    friend Vector2D operator*(float scalar, Vector2D& vector);

    Vector2D operator/(float scalar) const;

    Vector2D& normalize();

    float length() const;
};