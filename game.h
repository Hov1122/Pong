#ifndef GAME_H
#define GAME_H

#include <QMainWindow>
#include "helper.h"
#include <QMap>

class QLabel;

QT_BEGIN_NAMESPACE
namespace Ui { class Game; }
QT_END_NAMESPACE

class Game : public QMainWindow
{
    Q_OBJECT

public:
    Game(QWidget *parent = nullptr);
    ~Game();

private slots:
    void on_singlePlayerBtn_clicked();
    void on_multiPlayerBtn_clicked();
    void on_continueGameBtn_clicked();

private:
    Ui::Game *ui;
    QLabel* result;
    Player player1;
    Player player2;
    Ball ball;
    double gameDifficalty;
    double angle;
    QMap<int, bool> keys;
    QTimer* timer;
    windowSize wSize;
    bool singlePlayer;
    bool gameStarted;
    bool gamePaused;

    void processKeys();
    void moveBall();
    void setToDefaults();
    double getAutoX();
    virtual bool eventFilter(QObject*, QEvent*);
    virtual void paintEvent(QPaintEvent*);
    virtual void resizeEvent(QResizeEvent*);
};
#endif // GAME_H
