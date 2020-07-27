/* Class: MainWindow
 * ----------
 * TIE-02201/TIE-02207 SPRING 2020
 * ----------
 * Class that represents the UI and it's functions.
 * */
#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include "coordinates.hh"
#include <QMainWindow>
#include <QGraphicsScene>
#include <random>
#include <QTimer>
#include <QGraphicsRectItem>
#include <vector>
#include <algorithm>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QTimer* timer;
    QTimer* speed_timer;

private slots:

    /**
     * @brief Updates the game which is connected to a QTimer.
     */
    void update();

    /**
     * @brief Second timer update.
     */
    void timer_update();

    /**
     * @brief Start button - which initializes the game.
     */
    void on_startButton_clicked();

    /**
     * @brief Pauses the game when pressed
     */
    void on_pauseButton_clicked();

    /**
     * @brief Stops the game, resulting in "Game Over".
     */
    void on_stopButton_clicked();

    // Close button implemented into the build

private:
    Ui::MainWindow *ui;

    // Initializes scenes for the play, next, and hold area.
    QGraphicsScene* scene_;
    QGraphicsScene* next_piece_scene_;
    QGraphicsScene* hold_piece_scene_;

    // Constants describing scene coordinates
    // Copied from moving circle example and modified a bit
    const int BORDER_UP = 0;
    const int BORDER_DOWN = 480; // 260; (in moving circle)
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = 240; // 680; (in moving circle)

    // Size of a tetromino component
    const int SQUARE_SIDE = 20;
    // Number of horizontal cells (places for tetromino components)
    const int COLUMNS = BORDER_RIGHT / SQUARE_SIDE;
    // Number of vertical cells (places for tetromino components)
    const int ROWS = BORDER_DOWN / SQUARE_SIDE;

    // Constants for different tetrominos and the number of them
    enum Tetromino_kind {HORIZONTAL,
                         LEFT_CORNER,
                         RIGHT_CORNER,
                         SQUARE,
                         STEP_UP_RIGHT,
                         PYRAMID,
                         STEP_UP_LEFT,
                         NUMBER_OF_TETROMINOS};

    // For randomly selecting the next dropping tetromino.
    std::default_random_engine randomEng;
    std::uniform_int_distribution<int> distr;

    // Initializes variables.
    bool playing = false;
    bool paused = false;
    bool hold_cooldown = false;

    int seconds = 0;
    int speed = 350;
    int score = 0;
    int level = 1;
    int direction = 0;
    int total_cleared_lines = 0;

    // Creates empty tetrominos for storing.
    std::vector<Blocks> current_piece;
    std::vector<Blocks> next_piece;
    std::vector<Blocks> hold_piece;
    std::vector<Blocks> fallen;

    /**
     * @brief Resets variables.
     */
    void reset_variables();

    /**
     * @brief Takes in a tetromino vector which the funtion resets.
     * @param tetromino
     * @param shape (randomizes the tetromino if not given shape)
     */
    void new_drop(std::vector<Blocks>& tetromino, int i = 7);

    /**
     * @brief Renders the tetrominos.
     */
    void render();

    /**
     * @brief Renders the next tetromino display.
     */
    void render_next();

    /**
     * @brief Renders the held tetromino display.
     */
    void render_hold();

    /**
     * @brief Checks for block collision in given direction
     * @param direction
     * @return true - if collision happened
     */
    bool block_collision(int direction);

    /**
     * @brief Puts the falling tetromino into
     * the vector which stores fallen ones.
     */
    void solidify();

    /**
     * @brief Rotates the tetromino and also tests for collision.
     * @param rotation - direction
     */
    void rotate(int rotation);

    /**
     * @brief Switches the falling tetromino with the held one.
     */
    void switch_to_hold();

    /**
     * @brief Checks if full lines exist or a block has reached the top.
     * Also removes the lines and updates player score.
     */
    void game_state();

    /**
     * @brief Tracks the player's input
     * @param event
     */
    void keyPressEvent(QKeyEvent* event);

};

#endif // MAINWINDOW_HH
