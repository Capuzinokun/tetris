/* Tetris (TIE-02201)
 *
 * Desc: This program allows the user play the
 * popular game "Tetris". In which the tetrominos
 * fall and go solid if they touch bottom or another block.
 * Player must connect tetrominos in a row to clear
 * said line.
 *
 * In this tetris the player can keep track
 * of his time, score, level and next tetromino. Game also features
 * a hold function for storing tetromino of choice.
 *
 * Program Author:
 * Name: Petrus Jussila
 */

#include "mainwindow.hh"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
