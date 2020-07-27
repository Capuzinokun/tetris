/* Class: Coordinates
 * ----------
 * TIE-02201/TIE-02207 SPRING 2020
 * ----------
 * Class that has the structure for tetromino's blocks.
 * */
#ifndef COORDINATES_HH
#define COORDINATES_HH
#include <QBrush>

/**
 * @brief Stores the blocks coordinates, color and shape.
 */
struct Blocks
{
    int x;
    int y;
    QBrush color;
    int shape;
};

// Created for the use of find operator at mainwindow.hh
inline bool operator== (const Blocks& lhs, const Blocks& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

#endif // COORDINATES_HH
