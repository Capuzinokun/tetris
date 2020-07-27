#include "mainwindow.hh"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // We need a graphics scene in which to draw rectangles.
    scene_ = new QGraphicsScene(this);
    next_piece_scene_ = new QGraphicsScene(this);
    hold_piece_scene_ = new QGraphicsScene(this);

    // The graphicsView object is placed on the window
    // at the following coordinates.
    int left_margin = 100; // x coordinate
    int top_margin = 150; // y coordinate

    // The width of the graphicsView is BORDER_RIGHT added by 2,
    // since the borders take one pixel on each side
    // (1 on the left, and 1 on the right).
    // Similarly, the height of the graphicsView is BORDER_DOWN added by 2.
    ui->graphicsView->setGeometry(left_margin, top_margin,
                                  BORDER_RIGHT + 2, BORDER_DOWN + 2);

    ui->graphicsView->setScene(scene_);
    ui->nextBlockGraphicsView->setScene(next_piece_scene_);
    ui->holdBlockGraphicsView->setScene(hold_piece_scene_);

    // The width of the scene_ is BORDER_RIGHT decreased by 1 and
    // the height of it is BORDER_DOWN decreased by 1, because
    // each square of a tetromino is considered to be inside the sceneRect,
    // if its upper left corner is inside the sceneRect.
    scene_->setSceneRect(0, 0, BORDER_RIGHT - 1, BORDER_DOWN - 1);

    // Setting random engine ready for the first real call.
    int seed = time(0); // You can change seed value for testing purposes
    randomEng.seed(seed);
    distr = std::uniform_int_distribution<int>(0, NUMBER_OF_TETROMINOS - 1);
    distr(randomEng); // Wiping out the first random number (which is almost always 0)
    // After the above settings, you can use randomEng to draw the next falling
    // tetromino by calling: distr(randomEng) in a suitable method.

    // Setting up timers for the time display and speed control.
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_update()));

    speed_timer = new QTimer(this);
    connect(speed_timer, SIGNAL(timeout()), this, SLOT(update()));

    // If new session, hide "Game Over" text.
    ui->game_overLabel->hide();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_clicked()
{
    // Checks if game is paused
    if (paused == true) {
        timer->start(1000);
        speed_timer->start(speed);
        playing = true;
    }

    // Starts a new session
    if (playing == false) {

        // Resets values.
        reset_variables();

        // Start rendering
        render_next();
        render_hold();
        QString level_text = "Level: " + QString::number(level);
        ui->levelLabel->setText(level_text);
        ui->game_overLabel->hide();

        timer->start(1000);
        speed_timer->start(speed);
        playing = true;
    }
}

void MainWindow::on_pauseButton_clicked()
{
    if (playing == true) {
        timer->stop();
        speed_timer->stop();
        playing = false;
        paused = true;
    }
}

void MainWindow::on_stopButton_clicked()
{
    playing = false;
    paused = false;
    hold_cooldown = false;
    timer->stop();
    speed_timer->stop();
    ui->game_overLabel->show();
}

void MainWindow::reset_variables()
{
    seconds = 0;
    score = 0;
    speed = 350;
    level = 1;

    current_piece.clear();
    next_piece.clear();
    hold_piece.clear();
    fallen.clear();
    scene_->clear();
    next_piece_scene_->clear();
    hold_piece_scene_->clear();
}

void MainWindow::new_drop(std::vector<Blocks>& tetromino, int shape)
{
    // Randomizes the shape
    if (shape == 7) {
        shape = distr(randomEng);
    }

    if (shape == HORIZONTAL) {
        QBrush color = QBrush(Qt::cyan);
        tetromino = {{5, 0, color, 0}, {6, 0, color, 0}, {7, 0, color, 0}, {8, 0, color, 0}};
    }

    if (shape == LEFT_CORNER) {
        QBrush color = QBrush(Qt::blue);
        tetromino = {{5, 1, color, 1}, {7, 1, color, 1}, {6, 1, color, 1}, {7, 0, color, 1}};
    }

    if (shape == RIGHT_CORNER) {
        QBrush color = QBrush(QColor(255, 0, 0, 127));
        tetromino = {{5, 1, color, 2}, {7, 1, color, 2}, {6, 1, color, 2}, {5, 0, color, 2}};
    }

    if (shape == SQUARE) {
        QBrush color = QBrush(Qt::yellow);
        tetromino = {{6, 0, color, 3}, {7, 0, color, 3}, {6, 1, color, 3}, {7, 1, color, 3}};
    }

    if (shape == STEP_UP_RIGHT) {
        QBrush color = QBrush(Qt::green);
        tetromino = {{5, 1, color, 4}, {6, 0, color, 4}, {6, 1, color, 4}, {7, 0, color, 4}};
    }

    if (shape == PYRAMID) {
        QBrush color = QBrush(Qt::magenta);
        tetromino = {{5, 1, color, 5}, {7, 1, color, 5}, {6, 1, color, 5}, {6, 0, color, 5}};
    }

    if (shape == STEP_UP_LEFT) {
        QBrush color = QBrush(Qt::red);
        tetromino = {{5, 0, color, 6}, {6, 0, color, 6}, {6, 1, color, 6}, {7, 1, color, 6}};
    }

    render();
}

void MainWindow::update()
{
    game_state();

    // Checks if a tetromino is falling
    if (current_piece.empty()) {

        if (next_piece.empty()) {
            new_drop(next_piece);
        }

        current_piece = next_piece;
        new_drop(next_piece);

        render_next();
    }

    // If no block collision is happening,
    // drops the tetromino y-level by one.
    if (!block_collision(0)) {
        for (Blocks& c: current_piece) {

            c.y += 1;
        }
    }

    render();
}

void MainWindow::timer_update()
{
    seconds += 1;
}

void MainWindow::render()
{
    scene_->clear();

    // Renders the fallen tetrominos
    for (Blocks f: fallen) {

        QGraphicsRectItem* item = scene_->addRect(f.x*SQUARE_SIDE, f.y*SQUARE_SIDE, SQUARE_SIDE, SQUARE_SIDE);

        item->setBrush(f.color);
    }

    // Renders the falling tetromino
    for (Blocks c: current_piece) {

        QGraphicsRectItem* item = scene_->addRect(c.x*SQUARE_SIDE, c.y*SQUARE_SIDE, SQUARE_SIDE, SQUARE_SIDE);
        
        item->setBrush(c.color);

    }

    // Renders the new seconds and scores
    ui->timerLcdNumber->display(seconds);
    ui->scoreLcdNumber->display(score);
}

void MainWindow::render_next()
{
    next_piece_scene_->clear();

    for (Blocks n: next_piece) {

        QGraphicsRectItem* item = next_piece_scene_->addRect(n.x*SQUARE_SIDE, n.y*SQUARE_SIDE, SQUARE_SIDE, SQUARE_SIDE);

        item->setBrush(n.color);

    }
}

void MainWindow::render_hold()
{
    hold_piece_scene_->clear();

    for (Blocks h: hold_piece) {

        QGraphicsRectItem* item = hold_piece_scene_->addRect(h.x*SQUARE_SIDE, h.y*SQUARE_SIDE, SQUARE_SIDE, SQUARE_SIDE);

        item->setBrush(h.color);
    }
}

bool MainWindow::block_collision(int direction)
{
    // Direction: 0 - Down, 1 - Left, 2 - Right, 3 - -90-degrees, 4 - +90-degrees

    for (Blocks c: current_piece) {

        if (direction == 0 && c.y >= ROWS-1) {
            solidify();
            return true;
        }

        if (direction == 1 && c.x == 0) {
            return true;
        }

        if (direction == 2 && c.x >= COLUMNS-1) {
            return true;
        }

        for (Blocks f : fallen) {

            if (c.y == f.y-1 && c.x == f.x) {
                solidify();
                return true;
            }

            if (direction == 1) {
                if (c.x == f.x+1 && c.y == f.y) {
                    return true;
                }
            }

            if (direction == 2) {
                if (c.x == f.x-1 && c.y == f.y) {
                    return true;
                }
            }
        }
    }

    return false;
}

void MainWindow::solidify()
{
    for (Blocks c : current_piece) {

        fallen.push_back(c);

    }

    hold_cooldown = false;

    // Clears the tetromino which was falling
    current_piece.clear();
}

void MainWindow::rotate(int rotation)
{
    if (current_piece.empty()) {
        return;
    }

    if (current_piece.at(0).color == QBrush {Qt::yellow}) {
        return;
    }

    Blocks holder1 = current_piece.at(0);
    Blocks holder2 = current_piece.at(1);
    Blocks holder3 = current_piece.at(2);
    Blocks holder4 = current_piece.at(3);

    bool collision_detected = false;

    if (rotation == 90) {

        for (int i = 0; i < 4; i++) {

            if (i != 2) {
                int holder_x = current_piece.at(i).x;
                int holder_y = current_piece.at(i).y;
                current_piece.at(i).x = -(holder_y - current_piece.at(2).y) + current_piece.at(2).x;
                current_piece.at(i).y = (holder_x - current_piece.at(2).x) + current_piece.at(2).y;

                if (current_piece.at(i).y >= ROWS || current_piece.at(i).x < 0 || current_piece.at(i).x >= COLUMNS) {
                    collision_detected = true;
                    break;
                }

                for (Blocks f : fallen) {

                    if (current_piece.at(i).x == f.x && current_piece.at(i).y == f.y) {

                        collision_detected = true;
                        break;
                    }
                }
            }
        }

        if (collision_detected) {
            current_piece.at(0) = holder1;
            current_piece.at(1) = holder2;
            current_piece.at(2) = holder3;
            current_piece.at(3) = holder4;
        }

    }

    if (rotation == -90) {

        for (int rounds = 0; rounds < 3; rounds++) {

            for (int i = 0; i < 4; i++) {

                if (i != 2) {
                    int holder_x = current_piece.at(i).x;
                    int holder_y = current_piece.at(i).y;
                    current_piece.at(i).x = -(holder_y - current_piece.at(2).y) + current_piece.at(2).x;
                    current_piece.at(i).y = (holder_x - current_piece.at(2).x) + current_piece.at(2).y;

                    if (current_piece.at(i).y >= ROWS || current_piece.at(i).x < 0 || current_piece.at(i).x >= COLUMNS) {
                        collision_detected = true;
                        break;
                    }

                    for (Blocks f : fallen) {

                        if (current_piece.at(i).x == f.x && current_piece.at(i).y == f.y) {

                            collision_detected = true;
                            break;
                        }
                    }
                }
            }
        }

        if (collision_detected) {
            current_piece.at(0) = holder1;
            current_piece.at(1) = holder2;
            current_piece.at(2) = holder3;
            current_piece.at(3) = holder4;
        }
    }
}

void MainWindow::switch_to_hold()
{
    if (!hold_cooldown) {

        int current_shape = current_piece.at(0).shape;

        if (!hold_piece.empty()) {

            new_drop(current_piece, hold_piece.at(0).shape);
            new_drop(hold_piece, current_shape);
        }

        // If no held tetromino.
        else {

            new_drop(hold_piece, current_shape);
            current_piece.clear();
        }

        hold_cooldown = true;

        render_hold();
    }
    return;
}

void MainWindow::game_state()
{
    if (fallen.empty()) {
        return;
    }

    for (Blocks f : fallen) {

        if (f.y == 0) {

            on_stopButton_clicked();

        }
    }

    // Check full lines.
    std::vector<std::vector<Blocks>> grid_line;
    int cleared_lines = 0;
    for (int i = 0; i <= 23; ++i) {
        grid_line.push_back({});
    }

    for (Blocks block : fallen) {
        grid_line.at(block.y).push_back(block);
    }

    for (int y = 0; y < ROWS; ++y) {

        if (grid_line.at(y).size() == 12) {

            for (Blocks to_be_removed : grid_line.at(y)) {

                fallen.erase(std::find(fallen.begin(), fallen.end(), to_be_removed));

            }

            cleared_lines++;
            total_cleared_lines++;

            if (total_cleared_lines % 10 == 0 && speed > 50) {
                level++;
                QString level_text = "Level: " + QString::number(level);
                ui->levelLabel->setText(level_text);
                speed = speed - 50;
                speed_timer->stop();
                speed_timer->start(speed);
            }

            // Drops the blocks which were above the removed line/lines.
            for (int y2 = y-1; y2 > 0; --y2) {

                for (Blocks& block : fallen) {

                    if (block.y == y2) {

                        block.y++;
                    }

                }

            }

        }
    }

    // Adds score
    if (cleared_lines == 4) {
        score = score + 800;
    }

    else if (cleared_lines == 3) {
        score = score + 500;
    }

    else if (cleared_lines == 2) {
        score = score + 300;
    }

    else if (cleared_lines == 1) {
        score = score + 100;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (playing == true && !current_piece.empty()) {
        if (event->key() == Qt::Key_A) {
            if (!block_collision(1)) {
                for (Blocks& c: current_piece) {
                    c.x -= 1;
                }
            }
        }

        if (event->key() == Qt::Key_D) {
            if (!block_collision(2)) {
                for (Blocks& c: current_piece) {
                    c.x += 1;
                }
            }
        }

        if (event->key() == Qt::Key_S) {
            if (!block_collision(0)) {
                for (Blocks& c: current_piece) {
                    c.y += 1;
                }
            }
        }

        if (event->key() == Qt::Key_Q) {

            rotate(-90);

        }

        if (event->key() == Qt::Key_E) {

            rotate(90);

        }

        if (event->key() == Qt::Key_W) {

            switch_to_hold();

        }

        render();
    }
}
