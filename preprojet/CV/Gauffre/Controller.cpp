#include "Controller.h"
#include "ui_GameWindow.h"
#include "stdlib.h"
#include <QGraphicsPixmapItem>

using namespace std;

Controller::Controller(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Controller)
{
    ui->setupUi(this);
    srand(time(NULL));

    delay = 3000;

    configWindow = new ConfigGameWindow(this);
    QGraphicsScene *scene = new QGraphicsScene();

    initBoard(5, 4);

    ui->graphicsView->setScene(scene);

    ImageGaufre *imageGaufre = new ImageGaufre(0, 0);
    scene->addItem(imageGaufre);

    connect(ui->newButton, SIGNAL (clicked()), this, SLOT (configure()));
}

Controller::~Controller()
{
    delete configWindow;
    delete ui;
}

void Controller::configure()
{
    configWindow->show();
}

void Controller::initBoard(int w, int h){
    this->width = w;
    this->height = h;
    this->gameBoard.clear();
    for (int i = 0; i < h; i++) {
        gameBoard.push_back(w);
    }
    this->turn = rand() % 2;
    this->displayBoard();
    changePlayer();


}

void Controller::changePlayer(){
    turn = !turn;
    if ((gameMode == PvC && turn) || gameMode == CvC)
        QTimer::singleShot(delay, this, SLOT(iaPlay()));

}

void Controller::hasPlayed(Point p){
    for (int i = 0; i < height; i++) {
        if (i >= p.x)
            if (gameBoard[i] > p.y)
                this->gameBoard[i] = p.y;
    }
    changePlayer();
}

void Controller::displayBoard()
{

}

void Controller::iaPlay(){
    hasPlayed(play(this->gameBoard, gameDifficulty));
}
/*
isWon(){

}
*/
