#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include <QFrame>
#include <QLabel>
#include <QPixmap>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QGridLayout>
#include "TileLabel.h"
#include "UtilsGui.h"
#include "StationWidget.h"
#include "HandWidget.h"
#include "../Shared/Board.h"


class BoardView : public QFrame, public Board
{
	public:
		BoardView();
		~BoardView();

		void initEmpty();
		void resizeEvent(QResizeEvent *e);
		void change(Tile *sBoard, Tile *sHand);
		void put(TileLabel *sBoard, TileLabel *sHand);
		void put(TileLabel *t);

	private:

		QGridLayout *layout;

	protected:
		void dragEnterEvent(QDragEnterEvent *e);
		void dragMoveEvent(QDragMoveEvent *e);
		void dropEvent(QDropEvent *e);
		void mousePressEvent(QMouseEvent *e);

		void setSquare(Square *s);
		void changeSquare(Square *s);
};

#endif // BOARDVIEW_H
