#include "Controller.h"
#include "ui_GameWindow.h"
#include "stdlib.h"
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <fstream>
#include <QInputDialog>
#include <QStringListModel>


using namespace std;

Controller::Controller(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Controller)
{
    ui->setupUi(this);

    srand(time(NULL));

    delay = 1000;
    timer.setInterval(delay);
    timer.setSingleShot(true);

    imageGaufre = new QPixmap("../image/gaufre.png");
    imageGaufreSelect = new QPixmap("../image/gaufreSelect.png");
    imageEat = new QPixmap("../image/gaufreEat.png");
    imageEatHaut = new QPixmap("../image/gaufreEatHaut.png");
    imageEatCote = new QPixmap("../image/gaufreEatCote.png");
    imageEatCoin = new QPixmap("../image/gaufreEatCoin.png");
    imagePoison = new QPixmap("../image/gaufrePoison.png");


    configWindow = new ConfigGameWindow(this);
    loadWindow = new LoadWindow(this);
    scene = new QGraphicsScene();
    loadWindow = new LoadWindow();

    game.gameMode = PvC;
    game.diff1 = Medium;
    initBoard(5, 4);

    scene->setSceneRect(0, 0, 200, 200);
    ui->graphicsView->setScene(scene);

    ui->diffLabel2->setText(difficultyToStr(game.diff1));
    ui->diffLabel1->setText(difficultyToStr(game.diff2));

    connect(configWindow, SIGNAL(accepted()), this, SLOT(slotConfig()));
    connect(ui->newButton, SIGNAL (clicked()), this, SLOT(newGame()));
    connect(ui->redoButton, SIGNAL (clicked()), this, SLOT(redo()));
    connect(ui->undoButton, SIGNAL (clicked()), this, SLOT(undo()));
    connect(ui->configureAction, SIGNAL(triggered()), this, SLOT(configure()));
    connect(ui->saveAction, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->loadAction, SIGNAL(triggered()), this, SLOT(load()));
    connect(ui->exitAction, SIGNAL(triggered()), this, SLOT(close()));
    connect(ui->undoAction, SIGNAL(triggered()), this, SLOT(undo()));
    connect(ui->redoAction, SIGNAL(triggered()), this, SLOT(redo()));
    connect(loadWindow, SIGNAL(loadGame(Game)), this, SLOT(slotLoadGame(Game)));
    connect(&timer, SIGNAL(timeout()), this, SLOT(iaPlay()));

    won = false;
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

void Controller::gaufreHoverEnter(Point p)
{
    if ((game.gameMode == PvC && !game.turn) || game.gameMode == PvP)
        if (p.x != 0 || p.y != 0)
            for (int i = p.x; i < game.height; i ++)
                for (int j = p.y; j < game.gameBoard[i]; j ++)
                    imageBoard[i][j]->setImage(imageGaufreSelect);
}

void Controller::gaufreHoverLeave(Point p)
{
    if ((game.gameMode == PvC && !game.turn) || game.gameMode == PvP)
        if (p.x != 0 || p.y != 0)
            for (int i = p.x; i < game.height; i ++)
                for (int j = p.y; j < game.gameBoard[i]; j ++)
                     imageBoard[i][j]->setImage(imageGaufre);

}

void Controller::gaufrePressed(Point p)
{
    if ((game.gameMode == PvC && !game.turn) || game.gameMode == PvP)
        if (p.x != 0 || p.y != 0)
            hasPlayed(p);
}

void Controller::slotConfig()
{
    cout << "config" << endl;
    game.gameMode = configWindow->getMode();
    if (game.gameMode == PvC || game.gameMode == CvC)
        game.diff1 = configWindow->getDiff1();

    if (game.gameMode == CvC)
        game.diff2 = configWindow->getDiff2();

    ui->diffLabel1->setText(difficultyToStr1());
    ui->diffLabel2->setText(difficultyToStr2());

    cout << "game mode : " << game.gameMode << endl;
    cout << "game diff1 : " << game.diff1 << endl;
    cout << "game diff2 : " << game.diff2 << endl;
    initBoard(configWindow->getWidth(), configWindow->getHeight());
}

void Controller::newGame()
{
    initBoard(game.width, game.height);
}

void Controller::undo()
{
    if (timer.isActive())
        timer.stop();
    listBoardRedo.push_back(game.gameBoard);

    game.gameBoard = listBoardUndo.back();
    listBoardUndo.pop_back();

    displayBoard();

    ui->redoButton->setEnabled(true);
    ui->redoAction->setEnabled(true);
    if (listBoardUndo.empty()){
        ui->undoButton->setEnabled(false);
        ui->undoAction->setEnabled(false);
    }
    if (won)
    {
        game.turn = !game.turn;
        won = false;
    }
    changePlayer();
}

void Controller::displayBoard(){
    for (int i = 0; i < game.gameBoard.size(); i++){
        for (int j = 0; j < game.width; j++){
            if (i == 0 && j == 0) //poison image
                imageBoard[i][j]->setImage(imagePoison);
            else if (game.gameBoard[i] == game.width)
                imageBoard[i][j]->setImage(imageGaufre);
            else if (i == 0)
                if (j == game.gameBoard[i])
                    imageBoard[i][j]->setImage(imageEatCote);
                else if (j < game.gameBoard[i])
                    imageBoard[i][j]->setImage(imageGaufre);
                else
                    imageBoard[i][j]->setImage(imageEat);
            else if (game.gameBoard[i] == 0)
                if (game.gameBoard[i-1] > 0 && j < game.gameBoard[i-1])
                    imageBoard[i][j]->setImage(imageEatHaut);
                else
                    imageBoard[i][j]->setImage(imageEat);
            else if (i > 0 && game.gameBoard[i-1] == game.gameBoard[i])
                if (j == game.gameBoard[i])
                    imageBoard[i][j]->setImage(imageEatCote);
                else if (j < game.gameBoard[i])
                    imageBoard[i][j]->setImage(imageGaufre);
                else
                    imageBoard[i][j]->setImage(imageEat);
            else if (i > 0 && game.gameBoard[i-1] > game.gameBoard[i])
                if (j == game.gameBoard[i])
                    imageBoard[i][j]->setImage(imageEatCoin);
                else if (j < game.gameBoard[i])
                    imageBoard[i][j]->setImage(imageGaufre);
                else if (j > game.gameBoard[i] && j < game.gameBoard[i-1])
                    imageBoard[i][j]->setImage(imageEatHaut);
                else
                    imageBoard[i][j]->setImage(imageEat);
            else
                imageBoard[i][j]->setImage(imageEat);
        }
    }
}

void Controller::redo()
{
    if (timer.isActive())
        timer.stop();
    listBoardUndo.push_back(game.gameBoard);

    game.gameBoard = listBoardRedo.back();
    listBoardRedo.pop_back();

    displayBoard();


    if (listBoardRedo.empty()){
        ui->redoButton->setEnabled(false);
        ui->redoAction->setEnabled(false);
    }
    ui->undoButton->setEnabled(true);
    ui->undoAction->setEnabled(true);
    changePlayer();
}

void Controller::slotLoadGame(Game g)
{
    won = false;
    loadWindow->close();
    game = g;
    initImageBoard();
    game.turn = !game.turn;
    changePlayer();
    listBoardUndo.clear();
    listBoardRedo.clear();
    ui->redoButton->setEnabled(false);
    ui->redoAction->setEnabled(false);
    ui->undoButton->setEnabled(false);
    ui->undoAction->setEnabled(false);
    displayBoard();
}

void Controller::initBoard(int w, int h){
    game.width = w;
    game.height = h;

    won = false;

    listBoardUndo.clear();
    listBoardRedo.clear();
    game.gameBoard.clear();

    for (int i = 0; i < h; i++) {
        game.gameBoard.push_back(w);
    }

    ui->undoButton->setEnabled(false);
    ui->undoAction->setEnabled(false);
    ui->redoButton->setEnabled(false);
    ui->redoAction->setEnabled(false);

    initImageBoard();

    listBoardUndo.push_back(game.gameBoard);

    game.turn = rand() % 2;
    changePlayer();
}

void Controller::changePlayer() {
    game.turn = !game.turn;
    if ((game.gameMode == PvC && game.turn) || game.gameMode == CvC)
       timer.start();


    if(!game.turn){
         ui->playerLabel1->setText("<font size= '16' color='blue'> <b>" + playerToStr1() +"</b></font>");
         ui->playerLabel2->setText(playerToStr2());
    }else{
        ui->playerLabel1->setText(playerToStr1());
        ui->playerLabel2->setText("<font size= '16' color='blue'> <b>" + playerToStr2() +"</b></font>");
    }

}


void Controller::hasPlayed(Point p) {
    cout << "A player play in (" << p.x << " , " << p.y << ")" << endl;

    if (p.x >= (int) game.gameBoard.size() || p.y >= game.gameBoard[p.x]){
        cout << "It's not possible to play here" << endl;
        return;
    }

    listBoardUndo.push_back(game.gameBoard);


    for (int i = p.x; i < game.height; i++) {
        for (int j = p.y; j < game.width; j ++){
            if (i == 0 && j == p.y)
                imageBoard[i][j]->setImage(imageEatCote);
            else if (j == 0 && i == p.x)
                imageBoard[i][j]->setImage(imageEatHaut);
            else if (i == p.x && j == p.y)
                imageBoard[i][j]->setImage(imageEatCoin);
            else if (i == 0)
                imageBoard[i][j]->setImage(imageEat);
            else if (j == 0)
                imageBoard[i][j]->setImage(imageEat);
            else if (i == p.x && game.gameBoard[i-1]>j)
                imageBoard[i][j]->setImage(imageEatHaut);
            else if (j == p.y && game.gameBoard[i]>=p.y)
                imageBoard[i][j]->setImage(imageEatCote);
            else
                imageBoard[i][j]->setImage(imageEat);
        }

        if (game.gameBoard[i] > p.y)
            game.gameBoard[i] = p.y;
    }

    listBoardRedo.clear();

    ui->undoButton->setEnabled(true);
    ui->undoAction->setEnabled(true);
    ui->redoButton->setEnabled(false);
    ui->redoAction->setEnabled(false);

    isWon();
    if (!won)
        changePlayer();
    else{
        if(game.turn)
            QMessageBox::information(this, "Gagnant", playerToStr2() + " a gagné!");
        else
            QMessageBox::information(this, "Gagnant", playerToStr1() + " a gagné!");
    }
}

QString Controller::playerToStr1()
{
    switch (game.gameMode) {
    case PvP:
        return tr("Joueur 1");
        break;
    case PvC:
        return tr("Joueur");
        break;
    default:
        return tr("Ordinateur 1");
        break;
    }
}

QString Controller::playerToStr2()
{
    switch(game.gameMode){
    case PvP:
        return tr("Joueur 2");
        break;
    case PvC:
        return tr("Ordinateur");
        break;
    default:
        return tr("Ordinateur 2");
        break;
    }
}

QString Controller::difficultyToStr1(){
    switch(game.gameMode){
    case PvP:
        return "";
        break;
    case PvC:
        return "";
        break;
    default:
        return difficultyToStr(game.diff1);
        break;
    }
}

QString Controller::difficultyToStr2(){
    switch(game.gameMode){
    case PvP:
        return "";
        break;
    case PvC:
        return difficultyToStr(game.diff1);
        break;
    default:
        return difficultyToStr(game.diff2);
        break;
    }
}

QString Controller::difficultyToStr(difficulty diff){
    switch(diff){
    case Easy:
        return tr("Aléatoire");
        break;
    case Medium:
        return tr("Coup gagnant / perdant");
        break;
    case Hard:
        return tr("Minimax");
        break;
    default:
        return "";
        break;
    }
}

void Controller::iaPlay(){

    if (game.gameMode == PvC || game.gameMode == CvC && !game.turn) {
        cout << "IA play in difficulty " << game.diff1 << endl;
        hasPlayed(play(game.gameBoard, game.diff1));
    }
    else {
        cout << "IA play in difficulty " << game.diff2 << endl;
        hasPlayed(play(game.gameBoard, game.diff2));
    }
}

void Controller::isWon(){
    if (game.gameBoard[0]==1 && game.gameBoard[1] == 0) {
        won = true;
    }
    else
        won = false;
}

void Controller::initImageBoard()
{
    for (unsigned int i = 0; i < imageBoard.size(); i ++)
    {
        for (unsigned int j = 0; j < imageBoard[i].size(); j ++)
        {
            scene->removeItem(imageBoard[i][j]);
            delete imageBoard[i][j];
        }
        imageBoard[i].clear();
    }

    imageBoard.clear();

    for (int i = 0; i < game.height; i++) {
        imageBoard.push_back(vector<GaufreItem*>());

        for (int j = 0; j < game.width; j ++) {
            GaufreItem *item = new GaufreItem((Point){i, j});
            imageBoard[i].push_back(item);
            item->setImage(imageGaufre);
            item->setPos(j*(imageGaufre->width()-1), i*(imageGaufre->height()-1));
            connect(item, SIGNAL(hoverEnter(Point)), this, SLOT(gaufreHoverEnter(Point)));
            connect(item, SIGNAL(hoverLeave(Point)), this, SLOT(gaufreHoverLeave(Point)));
            connect(item, SIGNAL(pressed(Point)), this, SLOT(gaufrePressed(Point)));
            scene->addItem(item);
        }
    }
    imageBoard[0][0]->setImage(imagePoison);
}

void Controller::save(){
    vector<Game> listGame;
    Game g;
    bool ajout = true;
    QString nameGame = QInputDialog::getText(this, tr("Sauvegarde de la partie"), tr("Nom de la partie : "));
    if (!nameGame.isEmpty()){

        ifstream fileIn(".save.txt", ios::in);
        if (fileIn){
            while (fileIn >> g)
                listGame.push_back(g);

            for (unsigned int i = 0; i < listGame.size(); i++)
                if (listGame[i].name.compare(nameGame.toStdString()) == 0){
                    QMessageBox::critical(this, tr("Erreur"), tr("Nom de la partie déjà existant !"));
                    ajout = false;
                }

            fileIn.close();
        }

        if (ajout)
        {
            ofstream fileOut(".save.txt", ios::out | ios::app);
            game.name = nameGame.toStdString();
            fileOut << game;
            fileOut.close();
        }
    }
}

void Controller::load(){
    vector<Game> listGame;
    Game g;
    ifstream fileIn(".save.txt", ios::in);
    if (fileIn){
        while (fileIn >> g)
        {
            listGame.push_back(g);
        }

        if (listGame.empty())
            QMessageBox::information(this, tr("Pas de sauvegarde"), tr("Aucune partie n'a été sauvegardé"));
        loadWindow->setList(listGame);
        loadWindow->show();
    }
    else
        QMessageBox::information(this, tr("Pas de sauvegarde"), tr("Aucune partie n'a été sauvegardé"));
}

