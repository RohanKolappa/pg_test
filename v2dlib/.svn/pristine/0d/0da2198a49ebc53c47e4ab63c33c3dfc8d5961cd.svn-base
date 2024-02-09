/*
 * geometry.hh
 *
 *  Created on: Nov 30, 2010
 *      Author: rkale
 */

#ifndef GEOMETRY_HH_
#define GEOMETRY_HH_

namespace V2DLib
{

class Point {
public:
    Point() :
        x(0), y(0) {}
    Point(int a_x, int a_y) :
        x(a_x), y(a_y) {}

    bool operator==(const Point &other) {
        return IsEqualTo(&other);
    }
    bool operator!=(const Point &other) {
        return !IsEqualTo(&other);
    }

    inline void Clear() {
        x = y = 0;
    }
    inline void SetPoint(int a_x, int a_y) {
        x = a_x; y = a_y;
    }
    inline void Move(int deltaX, int deltaY) {
        x += deltaX; y += deltaY;
    }
    inline bool IsEqualTo(const Point *other) const {
        return (x == other->x && y == other->y);
    }

    int x;
    int y;
};

class Rect {
public:
    Rect(void) :
        left(0), top(0), right(0), bottom(0) {}
    Rect(const Rect *rect) {
        SetRect(rect);
    }
    Rect(const Rect &rect) {
        SetRect(&rect);
    }
    Rect(int lt, int tp, int rt, int bm) {
        SetRect(lt, tp, rt, bm);
    }
    Rect(int width, int height) {
        SetRect(0, 0, width, height);
    }

    ~Rect(void) {}

    int left;
    int top;
    int right;
    int bottom;

    inline void SetRect(int lt, int tp, int rt, int bm) {
        left = lt;
        top = tp;
        right = rt;
        bottom = bm;
    }

    inline void SetRect(const Rect *rect) {
        left = rect->left;
        top = rect->top;
        right = rect->right;
        bottom = rect->bottom;
    }

    inline Rect & operator=(const Rect &rect) {
        SetRect(&rect);
        return *this;
    }

    inline void Move(int offsetX, int offsetY) {
        left += offsetX;
        right += offsetX;
        top += offsetY;
        bottom += offsetY;
    }

    inline void SetLocation(int destX, int destY) {
        int offsetX = destX - left;
        int offsetY = destY - top;
        left = destX;
        right += offsetX;
        top = destY;
        bottom += offsetY;
    }

    inline bool isEqualTo(const Rect *rect) const {
        return (rect->left == left && rect->top == top && rect->right == right
                && rect->bottom == bottom);
    }

    inline void SetWidth(int value) {
        right = left + value;
    }
    inline void SetHeight(int value) {
        bottom = top + value;
    }

    inline int GetWidth() const {
        return right - left;
    }
    inline int GetHeight() const {
        return bottom - top;
    }

    inline bool IsEmpty() const {
        return GetWidth() <= 0 || GetHeight() <= 0;
    }

    inline int Area() const {
        return IsEmpty() ? 0 : GetWidth() * GetHeight();
    }

    inline void Clear() {
        left = top = right = bottom = 0;
    }

    Rect Intersection(const Rect *other) const
    {
        Rect result;
        result.SetRect(
                (left > other->left) ? left : other->left,
                (top > other->top) ? top : other->top,
                (right < other->right) ? right : other->right,
                (bottom < other->bottom) ? bottom : other->bottom);
        return result;
    }
};

class Dimension {
public:
    Dimension(void) :
        width(0), height(0) { }
    Dimension(const int w, const int h){
        SetDim(w, h);
    }
    Dimension(const Rect *r) {
        width = r->GetWidth();
        height = r->GetHeight();
    }
    Dimension(const Dimension &other) :
        width(other.width), height(other.height) {}

    const Dimension &operator=(const Dimension &other) {
        width = other.width;
        height = other.height;
        return *this;
    }
    bool operator==(const Dimension &other) {
        return IsEqualTo(&other);
    }
    bool operator!=(const Dimension &other) {
        return !IsEqualTo(&other);
    }

    ~Dimension(void) { }

    Rect GetRect() const {
        Rect r(width, height);
        return r;
    }

    inline void SetDim(const int w, const int h) {
        width = w;
        height = h;
    }

    inline void SetDim(const Rect *rect) {
        width = rect->GetWidth();
        height = rect->GetHeight();
    }

    inline bool IsEmpty() const {
        return width <= 0 || height <= 0;
    }

    inline bool IsEqualTo(const Dimension *other) const {
        return (width == other->width) && (height == other->height);
    }

    inline int Area() const {
        return IsEmpty() ? 0 : width * height;
    }

    inline void Clear() {
        width = height = 0;
    }

    inline Point Clip(const Point & point) const {
        return Point(
                point.x > width ? width : point.x,
                point.y > height ? height : point.y);
    }

    inline Dimension Intersection(const Dimension *other) const {
        return Dimension(
                width < other->width ? width : other->width,
                height < other->height ? height : other->height);
    }

    int width;
    int height;
};


}
#endif /* GEOMETRY_HH_ */
