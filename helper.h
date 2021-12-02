#ifndef HELPER_H
#define HELPER_H

#include <QColor>
#include <QPoint>

struct windowSize {
    int width;
    int height;
    windowSize(int w, int h) : width(w), height(h) {};
};

struct Player {
    int height;
    int length;
    int score;
    int step;
    QPoint pos;
    QColor color;
    Player (int h, int l, int s, int st, QPoint p, QColor c) :
        height(h), length(l), score(s), step(st), pos(p), color(c) {}
};

struct Ball {
    double step;
    double radius;
    QPointF pos;
    QColor color;
    Ball (double s, double r, QPointF p, QColor c) :
        step(s), radius(r), pos(p), color(c) {}
};

#endif // HELPER_H
