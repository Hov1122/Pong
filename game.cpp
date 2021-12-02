#include "game.h"
#include "ui_game.h"
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include <ctime>
#include <cmath>
#include <chrono>
#include <thread>
#include <QLabel>
#include <QFont>

Game::Game(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Game)  
    , player1(15, 0, 0, 20, QPoint(0, 0), Qt::black)
    , player2(15, 0, 0, 20, QPoint(0, 0), Qt::black)
    , ball(0, 0, QPoint(0, 0), Qt::red)
    , gameDifficalty(0.4)   
    , timer(new QTimer) 
    , wSize(this->size().width(), this->size().height())   
    , singlePlayer(false)
    , gameStarted(false)
    , gamePaused(false)
{

    wSize.width = this->size().width();
    ui->setupUi(this);
    qApp->installEventFilter(this); // to track keyPresses

    timer->setInterval(10);
    connect(timer, &QTimer::timeout, this, &Game::processKeys);

    ui->continueGameBtn->setHidden(true);

    result = new QLabel();

    //result->setText(QString::number(player1.score) + " - " + QString::number(player2.score));
    statusBar()->addWidget(result);
    statusBar()->setFont(QFont("Times", 14));

}

Game::~Game()
{
    delete ui;
}

void Game::processKeys()
{
    if (gameStarted) {
        moveBall();

        if (keys[Qt::Key_Left])
            if (player1.pos.x() - player1.step - player1.length > 1) {
                player1.pos.setX(player1.pos.x() - player1.step);
                update();
            }

        if (keys[Qt::Key_Right])
            if (wSize.width - (player1.pos.x() + player1.step + player1.length) > 1) {
                player1.pos.setX(player1.pos.x() + player1.step);
                update();
            }

        if (singlePlayer) {
            player2.pos.setX(player2.pos.x() + getAutoX());
            if (player2.pos.x() < player2.length) player2.pos.setX(player2.length);
            else if (player2.pos.x() > wSize.width - player2.length) player2.pos.setX(wSize.width - player2.length);
        }
        else {
            if (keys[Qt::Key_A])
                if (player2.pos.x() - player2.step - player2.length > 1) {
                    player2.pos.setX(player2.pos.x() - player2.step);
                    update();
                }

            if (keys[Qt::Key_D])
                if (wSize.width - (player2.pos.x() + player2.step + player2.length) > 1) {
                    player2.pos.setX(player2.pos.x() + player2.step);
                    update();
                }
        }
    }

}

void Game::moveBall()
{
    double tmp;
    if (modf(angle, &tmp) == 0 && int(tmp) % 90 == 0 && int(tmp) % 180 != 0) angle += 1;

    double y = 0, x = 0;
    bool hittedPlayer1 = false, hittedPlayer2 = false;

    if (ball.pos.y() + ball.radius >= wSize.height - player1.height) {
        if (abs(player1.pos.x() - ball.pos.x()) > player1.length && ball.pos.y() + ball.radius >= wSize.height)
        {
            qDebug() << "2 won";
            //if (singlePlayer && gameDifficalty > 0.2) gameDifficalty -= 0.05;
            ball.color = QColor(rand()%256, rand()%256, rand()%256);;
            if (ball.color == Qt::white) ball.color = Qt::red;
            player2.score += 1;
            result->setText(QString::number(player1.score) + " - " + QString::number(player2.score));
            timer->stop();
            setToDefaults();
            std::chrono::milliseconds timespan(1000);
            std::this_thread::sleep_for(timespan);
            return;
        }

        if (abs(player1.pos.x() - ball.pos.x()) < player1.length) {
            angle = 180 - angle;
            hittedPlayer1 = true;
        }
        else if (abs(player1.pos.x() - (ball.pos.x() + ball.radius)) < player1.length) {
            if (qTan(qDegreesToRadians(angle)) < 0) {
                angle = 180 - angle;
            }
            else ball.step *= -1;
            hittedPlayer1 = true;
        }
        else if (abs(player1.pos.x() - (ball.pos.x() - ball.radius)) < player1.length) {
            if (qTan(qDegreesToRadians(angle)) > 0) {
                angle = 180 - angle; // change the angle a little when it hits the edge
            }
            else ball.step *= -1;
            hittedPlayer1 = true;
        }
    }

    else if (ball.pos.y() - ball.radius <= player2.height) {
        if (abs(player2.pos.x() - ball.pos.x()) > player2.length && ball.pos.y() - ball.radius <= 0)
        {
            qDebug() << "1 won";
            ball.color = QColor(rand()%256, rand()%256, rand()%256);
            if (singlePlayer) player2.color = QColor(rand()%256, rand()%256, rand()%256);
            if (ball.color == Qt::white) ball.color = Qt::red;
            if (player2.color == Qt::white) ball.color = Qt::black;
            player1.score += 1;
            result->setText(QString::number(player1.score) + " - " + QString::number(player2.score));
            timer->stop();
            if (singlePlayer && gameDifficalty <= 0.95) gameDifficalty += 0.05;

            if (gameDifficalty >=  0.7) {
                player1.color = QColor(Qt::darkGreen);
            }
            //qDebug() << gameDifficalty;
            setToDefaults();
            std::chrono::milliseconds timespan(1000);
            std::this_thread::sleep_for(timespan);
            return;
        }

        if (abs(player2.pos.x() - ball.pos.x()) < player2.length) {
            angle = 180 - angle;
            hittedPlayer2 = true;
        }
        else if (abs(player2.pos.x() - (ball.pos.x() + ball.radius)) < player2.length) {
            if (qTan(qDegreesToRadians(angle)) > 0) {
                angle = 180 - angle; // change the angle a little when it hits the edge
            }
            else ball.step *= -1;
            hittedPlayer2 = true;
        }
        else if (abs(player2.pos.x() - (ball.pos.x() - ball.radius)) < player2.length) {
            if (qTan(qDegreesToRadians(angle)) < 0) {
                angle = 180 - angle; // change the angle a little when it hits the edge
            }
            else ball.step *= -1;
            hittedPlayer2 = true;
        }
    }

    else if (ball.pos.x() + ball.radius >= wSize.width) {
        angle = 180 - angle;
        ball.step *= -1;
    }

    else if (ball.pos.x() - ball.radius <= 0) {
        ball.step *= -1;
        angle = 180 - angle;
    }

    x = ball.pos.x() + ball.step;

    if (hittedPlayer1) y = wSize.height - player1.height - ball.radius - 1;
    else if (hittedPlayer2) y = player2.height + ball.radius + 1; // +1 to avoid loopa
    else y = qTan(qDegreesToRadians(angle)) * (x - ball.pos.x()) + ball.pos.y();

    if (x < ball.radius) x = ball.radius;
    else if (x > wSize.width - ball.radius) x = wSize.width - ball.radius;
    ball.pos.setX(x);
    if (y + ball.radius >= wSize.height) y = wSize.height - ball.radius;
    else if (y - ball.radius <= 0) y = ball.radius;
    ball.pos.setY(y);
    update();

}

void Game::setToDefaults()
{
    player1.pos.setX(wSize.width / 2);
    player1.pos.setY(wSize.height);

    player2.pos.setX(wSize.width / 2);
    player2.pos.setY(0);

    ball.pos = QPointF(wSize.width / 2, wSize.height / 2);

    /// move ball at random angle from 60 to 120 or -60 to -120 at different directions
    srand(time(0));
    QList<int> signs = {1, -1};
    int s = signs.at(rand() % signs.size());
    int min1 = 40;
    int max1 = 70;
    int angle1 = (rand()%(max1-min1+1)+min1);
    int min2 = 110;
    int max2 = 130;
    int angle2 = (rand()%(max2-min2+1)+min2);
    QList<int> angles = {angle1, angle2};
    angle = angles.at(rand() % angles.size());

    //double tmp;
    //if (modf(angle, &tmp) == 0 && int(tmp) % 90 == 0 && int(tmp) % 180 != 0) angle += 10;
    ball.step = 3.25 / abs(qTan(qDegreesToRadians(angle))); // set ball.step relative to moving angle
    ball.step *= s;
    timer->start();

    result->setText(QString::number(player1.score) + " - " + QString::number(player2.score));

    ui->singlePlayerBtn->setDisabled(true);
    ui->singlePlayerBtn->setHidden(true);
    ui->multiPlayerBtn->setDisabled(true);
    ui->multiPlayerBtn->setHidden(true);
    ui->continueGameBtn->setDisabled(true);
    ui->continueGameBtn->setHidden(true);
    ui->gameTitleLbl->setHidden(true);

}

double Game::getAutoX()
{
    if (ball.pos.y() >= wSize.height * gameDifficalty || ball.pos.y() < wSize.height * 0.25) return 0; // to make pc weaker

    double x = -ball.pos.y() / qTan(qDegreesToRadians(angle)) + ball.pos.x();
    if (abs(x - player2.pos.x()) <= player2.length) return 0;
    else if (player2.pos.x() - player2.length > x) return -player2.step/2;
    else return player2.step/2;
}

void Game::paintEvent(QPaintEvent *pe)
{
    Q_UNUSED(pe);
    if (gameStarted) {
        QPainter painter(this);

        painter.setPen(QPen(player1.color, player1.height, Qt::SolidLine, Qt::FlatCap));
        painter.drawLine(player1.pos.x() - player1.length, player1.pos.y(), player1.pos.x() + player1.length, player1.pos.y());
        painter.setPen(QPen(player2.color, player2.height, Qt::SolidLine, Qt::FlatCap));
        painter.drawLine(player2.pos.x() - player2.length, player2.pos.y(), player2.pos.x() + player2.length, player2.pos.y());

        painter.setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::FlatCap));
        painter.setBrush(QBrush(ball.color, Qt::SolidPattern));
        painter.drawEllipse(ball.pos, ball.radius, ball.radius);
    }
}

void Game::resizeEvent(QResizeEvent* event)
{
   Q_UNUSED(event);
   double scale = (double)this->size().width() / (double)wSize.width;

   //int error = wSize.width / 2 - player1.pos.x()>0?this->size().width() * 2/100:-this->size().width() *2/100;
   int error = 0.001 * (wSize.width / 2 - player1.pos.x());

   player1.pos.setX(this->size().width() / 2 - scale * (wSize.width / 2 - player1.pos.x()) + error);

   if (player1.pos.x() - player1.length < 1) player1.pos.setX(player1.length + 1);
   if (player1.pos.x() + player1.length > this->size().width() - 1) player1.pos.setX(this->size().width() - player1.length - 1);

   player2.pos.setX(this->size().width() / 2 - scale * (wSize.width / 2 - player2.pos.x()) + error);

   if (player2.pos.x() - player2.length < 1) player2.pos.setX(player2.length + 1);
   if (player2.pos.x() + player2.length > this->size().width() - 1) player2.pos.setX(this->size().width() - player2.length - 1);

   wSize.width = this->size().width();
   wSize.height = this->size().height();
   player1.pos.setY(wSize.height);
   player2.pos.setY(0);
   player1.length = (wSize.width / 2) * 15 / 100;
   player1.step = player1.length * 0.3;
   player2.length = (wSize.width / 2) * 15 / 100;
   player2.step = player2.length * 0.3;
   ball.pos = QPointF(wSize.width / 2, wSize.height / 2);
   ball.radius = std::min(wSize.width, wSize.height) * 0.05;
}

bool Game::eventFilter(QObject *obj, QEvent *event)
{

   if (event->type() == QEvent::KeyPress && obj == this)
   {
       QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

       if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return)
           ui->singlePlayerBtn->animateClick();

       if (keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right || keyEvent->key() == Qt::Key_A || keyEvent->key() == Qt::Key_D)
           keys[keyEvent->key()] = true;

       if(keyEvent->key() == Qt::Key_Escape && !ui->singlePlayerBtn->isEnabled() && !gamePaused) {
           ui->singlePlayerBtn->setDisabled(false);
           ui->singlePlayerBtn->setHidden(false);
           ui->multiPlayerBtn->setDisabled(false);
           ui->multiPlayerBtn->setHidden(false);
           ui->continueGameBtn->setDisabled(false);
           ui->continueGameBtn->setHidden(false);
           ui->gameTitleLbl->setHidden(false);
           gameStarted = false;
           gamePaused = true;
           timer->stop();
       }
       else if(keyEvent->key() == Qt::Key_Escape && gamePaused) {
           ui->continueGameBtn->animateClick();
       }
   }
   else if (event->type() == QEvent::KeyRelease) {
       QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
       if (keyEvent->key() == Qt::Key_Left || keyEvent->key() == Qt::Key_Right || keyEvent->key() == Qt::Key_A || keyEvent->key() == Qt::Key_D)
           keys[keyEvent->key()] = false;
   }
    // pass the event on to the parent class
    return QMainWindow::eventFilter(obj, event);
}

void Game::on_singlePlayerBtn_clicked()
{
    singlePlayer = true;
    gameStarted = true;
    gamePaused = false;
    gameDifficalty = 0.4;
    player1.color = QColor(Qt::black);
    player2.color = QColor(Qt::black);
    player1.score = 0;
    player2.score = 0;
    std::chrono::milliseconds timespan(500);
    std::this_thread::sleep_for(timespan);
    setToDefaults();
}

void Game::on_multiPlayerBtn_clicked()
{
    singlePlayer = false;
    gameStarted = true;
    gamePaused = false;
    player1.color = QColor(Qt::black);
    player2.color = QColor(Qt::black);
    player1.score = 0;
    player2.score = 0;
    std::chrono::milliseconds timespan(500);
    std::this_thread::sleep_for(timespan);
    setToDefaults();
}

void Game::on_continueGameBtn_clicked()
{
   gameStarted = true;
   gamePaused = false;
   ui->singlePlayerBtn->setDisabled(true);
   ui->singlePlayerBtn->setHidden(true);
   ui->multiPlayerBtn->setDisabled(true);
   ui->multiPlayerBtn->setHidden(true);
   ui->continueGameBtn->setDisabled(true);
   ui->continueGameBtn->setHidden(true);
   ui->gameTitleLbl->setHidden(true);
   std::chrono::milliseconds timespan(500);
   std::this_thread::sleep_for(timespan);
   timer->start();
}

