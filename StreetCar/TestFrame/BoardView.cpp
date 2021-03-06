#include "BoardView.h"
#include <QDebug>
#include <QMimeData>
#include <QByteArray>
#include <QDataStream>
#include <QDrag>
#include <QPainter>
#include <cstdlib>
#include <time.h>
#include <sstream>
#include "CardWidget.h"

using namespace std;

BoardView::BoardView()
	: QFrame(), Board()
{
	//setMinimumSize(TILESIZE * getSize() + TILESIZE, TILESIZE * getSize() + TILESIZE);

	setAcceptDrops(true);
	srand (time(NULL));
/*
	for (int i = 0; i < getSize(); i ++) {

		for (int j = 0; j < getSize(); j ++)
		{
			if (get(i, j)->isEmpty())
				get(i, j)->setType((idTile)((rand() % 15) -1));
			qDebug() << "create " << i << " " << j << " " << get(i, j)->getType();
			TileLabel *t = new TileLabel(this, i, j, get(i, j)->getType());
			t->move(i*TILESIZE, j*TILESIZE);
		}
	}
	*/
	layout = new QGridLayout(this);
	layout->setSpacing(1);
	initEmpty();

}

BoardView::~BoardView()
{

}

void BoardView::initEmpty()
{
	setSize(14, 12);
	//layout->rowCount(getSize());
	//layout->columnCount(getSize());

//	setMinimumSize(TILESIZE * getSize(), TILESIZE * getSize());
	setMinimumSize(TILESIZE * getSize() + TILESIZE, TILESIZE * getSize() + TILESIZE);
	//setMinimumSize(100, 100);
	for(int i = 0; i < getSize()-1; i++){
		changeSquare(new TileLabel(this, Wall, 0, i));
		changeSquare(new TileLabel(this, Wall, getSize()-1, i+1));
		changeSquare(new TileLabel(this, Wall, i+1, 0));
		changeSquare(new TileLabel(this, Wall, i, getSize()-1));
	}

	for (int i = 1; i < getSize()-1; i ++) {
		for (int j = 1; j < getSize()-1; j ++)
			changeSquare(new TileLabel(this, Empty, i, j));
	}


	changeSquare(new TileLabel(this, Terminus4_1,2,0));
	changeSquare(new TileLabel(this, Terminus4_2,3,0));
	changeSquare(new TileLabel(this, Terminus5_1,6,0));
	changeSquare(new TileLabel(this, Terminus5_2,7,0));
	changeSquare(new TileLabel(this, Terminus6_1,10,0));
	changeSquare(new TileLabel(this, Terminus6_2,11,0));

	changeSquare(new TileLabel(this, Terminus3_2,0,2));
	changeSquare(new TileLabel(this, Terminus3_1,0,3));
	changeSquare(new TileLabel(this, Terminus2_2,0,6));
	changeSquare(new TileLabel(this, Terminus2_1,0,7));
	changeSquare(new TileLabel(this, Terminus1_2,0,10));
	changeSquare(new TileLabel(this, Terminus1_1,0,11));

	changeSquare(new TileLabel(this, Terminus6_4,2,13));
	changeSquare(new TileLabel(this, Terminus6_3,3,13));
	changeSquare(new TileLabel(this, Terminus4_4,6,13));
	changeSquare(new TileLabel(this, Terminus4_3,7,13));
	changeSquare(new TileLabel(this, Terminus5_4,10,13));
	changeSquare(new TileLabel(this, Terminus5_3,11,13));

	changeSquare(new TileLabel(this, Terminus2_3,13,2));
	changeSquare(new TileLabel(this, Terminus2_4,13,3));
	changeSquare(new TileLabel(this, Terminus1_3,13,6));
	changeSquare(new TileLabel(this, Terminus1_4,13,7));
	changeSquare(new TileLabel(this, Terminus3_3,13,10));
	changeSquare(new TileLabel(this, Terminus3_4,13,11));

	stations[0] = new StationWidget(this, StationA, 1, 5);
	stations[1] = new StationWidget(this, StationB, 4, 2);
	stations[2] = new StationWidget(this, StationC, 8, 1);
	stations[3] = new StationWidget(this, StationD, 11, 4);
	stations[4] = new StationWidget(this, StationE, 12, 8);
	stations[5] = new StationWidget(this, StationF, 9, 11);
	stations[6] = new StationWidget(this, StationG, 5, 12);
	stations[7] = new StationWidget(this, StationH, 2, 9);
	stations[8] = new StationWidget(this, StationI, 4, 7);
	stations[9] = new StationWidget(this, StationK, 6, 4);
	stations[10] = new StationWidget(this, StationL, 9, 6);
	stations[11] = new StationWidget(this, StationM, 7, 9);

	for (int i = 0; i < nbrStation; i ++)
		changeSquare(stations[i]);

}

void BoardView::resizeEvent(QResizeEvent *e)
{
	qDebug() << "board resize" << e->size();
	int min;
	if (e->size().width() < e->size().height())
		min = e->size().width();
	else
		min = e->size().height();
	setFixedHeight(min);
	setFixedWidth(min);
	//setMaximumSize(min, min);
	//setMinimumSize(min, min);
	//resizeEvent(e);
}

void BoardView::change(TileLabel *sBoard, TileLabel *sHand)
{
	layout->removeWidget(sBoard);
	Board::change((Tile*)sBoard, (Tile*)sHand);
	layout->addWidget(sBoard, sBoard->getCoordinates().y, sBoard->getCoordinates().x);

	//setSquare(*sBoard);
}

void BoardView::put(TileLabel *sBoard, TileLabel *sHand)
{
	layout->removeWidget(sBoard);
	Board::put((Tile*)sBoard, (Tile*)sHand);
	layout->addWidget(sBoard, sBoard->getCoordinates().y, sBoard->getCoordinates().x);
}

void BoardView::put(TileLabel *t)
{
	put((TileLabel*)get(t->getCoordinates()), t);
}

void BoardView::set(Tile *t)
{
	Board::set(t->getCoordinates().x, t->getCoordinates().y, t);
	((TileLabel*)Board::get(t->getCoordinates()))->updatePixmap();
}

void BoardView::update()
{
	for (int i = 0; i < getSize(); i ++)
		for (int j = 0; j < getSize(); j ++){
			Square* s = get(i, j);
			if (s->isStation())
				((StationWidget*)s)->updatePixmap();
			else
				((TileLabel*)s)->updatePixmap();
		}
	QFrame::update();
}

void BoardView::dragEnterEvent(QDragEnterEvent *e)
{
	qDebug() << "drag enter board";
    lastCoordo = (Point){-1, -1};
	if (e->mimeData()->hasFormat("application/x-dnditemdata")) {
		if (e->source() == this) {
			qDebug() << "source";


			TileLabel *child = static_cast<TileLabel *>(childAt(e->pos()));
			if (!child)
					return;
			e->setDropAction(Qt::MoveAction);
			e->accept();
			lastCoordo = child->getCoordinates();
		} else {
			qDebug() << "no source";
			/*TileLabel *child = static_cast<TileLabel *>(childAt(e->pos()));
			if (!child)
					return;
			e->setDropAction(Qt::MoveAction);
			e->accept();
			lastCoordo = child->getCoordinates();*/
			e->acceptProposedAction();
		}
	} else {
		e->ignore();
	}
}

void BoardView::dragMoveEvent(QDragMoveEvent *e)
{
	//qDebug() << "drag move board";
	TileLabel *child = static_cast<TileLabel *>(childAt(e->pos()));
	if (!child)
		return;

	if (e->mimeData()->hasFormat("application/x-dnditemdata")) {
		if (lastCoordo != child->getCoordinates()) {
			qDebug() << "drag move board";
			if (lastCoordo.x != -1) {
				Square* s = get(lastCoordo);
				if (s->isStation())
					((StationWidget*)s)->mouseLeave();
				else
					((TileLabel*)s)->mouseLeave();
			}

			lastCoordo = child->getCoordinates();
			if (e->source() == this) {
				qDebug() << "source";
				if (child->getCoordinates().x < 3)
					e->ignore();
				else {
					e->setDropAction(Qt::MoveAction);
					e->accept();
				}
			} else {
				qDebug() << "no source";

				QByteArray itemData = e->mimeData()->data("application/x-dnditemdata");
				QDataStream dataStream(&itemData, QIODevice::ReadOnly);

				CardWidget h;
				dataStream >> h;


				if ((child->isEmpty() && putPossible(child->getCoordinates(), &h)) ||
						(!child->isEmpty() && changePossible(child, &h))) {
					child->mouseEnter(true);
					e->acceptProposedAction();
				}
				else {
					child->mouseEnter(false);
					e->ignore();
				}
			}
		}
	} else {
		e->ignore();
	}

}

void BoardView::dropEvent(QDropEvent *e)
{
	qDebug() << "Drop board";
	TileLabel *child = static_cast<TileLabel *>(childAt(e->pos()));
	if (!child)
		return;

	if (e->mimeData()->hasFormat("application/x-dnditemdata")) {
		QByteArray itemData = e->mimeData()->data("application/x-dnditemdata");
		QDataStream dataStream(&itemData, QIODevice::ReadOnly);

		if (lastCoordo.x != -1) {
			Square* s = get(lastCoordo);
			if (s->isStation())
				((StationWidget*)s)->mouseLeave();
			else
				((TileLabel*)s)->mouseLeave();
		}

		int idx;
		TileLabel *card = new TileLabel();
		dataStream >> idx >> *card;
		card->setCoordinates(child->getCoordinates());


		if (child->isEmpty() && putPossible(child->getCoordinates(), card)) {

			put(child, card);

			child->updatePixmap();
			e->setDropAction(Qt::MoveAction);
			emit tileDrop(idx);
		}
		else if (!child->isEmpty() && changePossible(child, card)) {
			change(child, card);

			child->updatePixmap();
			e->setDropAction(Qt::MoveAction);
			emit tileChange(idx, (Tile)(*card));
		}
		else
			e->ignore();

	} else {
		e->ignore();
	}

}

void BoardView::mousePressEvent(QMouseEvent *e)
{
	TileLabel *child = static_cast<TileLabel *>(childAt(e->pos()));
	if (!child)
		return;
}

void BoardView::dragLeaveEvent(QDragLeaveEvent *e)
{
	cout << "Leave " << endl;
	if (lastCoordo.x != -1) {
		Square* s = get(lastCoordo);
		if (s->isStation())
			((StationWidget*)s)->mouseLeave();
		else
			((TileLabel*)s)->mouseLeave();
        lastCoordo = (Point){-1, -1};
	}
}

void BoardView::setSquare(Square *s)
{
	//qDebug() << "set";
	Board::setSquare(s);
	//((TileLabel*)s)->move(s->getCoordinates().x*TILESIZE, s->getCoordinates().y*TILESIZE);
	layout->addWidget((TileLabel*)s, s->getCoordinates().y, s->getCoordinates().x);
}

void BoardView::changeSquare(Square *s)
{
	//qDebug() << "change";
	Board::changeSquare(s);
	//((TileLabel*)s)->move(s->getCoordinates().x*TILESIZE, s->getCoordinates().y*TILESIZE);
	//layout->removeWidget((TileLabel*)get(s->getCoordinates()));
	layout->addWidget((TileLabel*)s, s->getCoordinates().y, s->getCoordinates().x);
}
