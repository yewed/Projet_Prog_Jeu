#include "StationWidget.h"
#include <QResizeEvent>
#include <QDebug>
#include <QPainter>

StationWidget::StationWidget(QWidget *parent, idTile s, int x, int y) : QLabel(parent), Station(s, x, y)
{
	//setMaximumSize(TILESIZE*2, TILESIZE*2);
	//setMinimumSize(TILESIZE, TILESIZE);
	setPixmap(getPixmap(s));
//	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

	setAttribute(Qt::WA_DeleteOnClose);
	updatePixmap();
}

StationWidget::~StationWidget()
{

}

void StationWidget::mouseEnter(bool ok)
{
	QPixmap pix = *pixmap();
	QPainter p;
	p.begin(&pix);
	if (ok)
		p.fillRect(pix.rect(), QColor(0, 255, 0, 127));
	else
		p.fillRect(pix.rect(), QColor(255, 0, 0, 127));
	p.end();
	setPixmap(pix);
}

void StationWidget::mouseLeave()
{
	updatePixmap();
}

void StationWidget::updatePixmap()
{
	setPixmap(getPixmap(getType()).scaled(width(), height(), Qt::IgnoreAspectRatio));
}

void StationWidget::resizeEvent(QResizeEvent *e)
{
	//qDebug() << "label resize Station";
	int min;
	if (e->size().width() < e->size().height())
		min = e->size().width();
	else
		min = e->size().height();
	//setFixedHeight(min);
	//setFixedWidth(min);
	//setMaximumSize(min, min);
	//setMinimumSize(min, min);
	updatePixmap();
	//setPixmap(pixmap()->scaled(e->size(), Qt::KeepAspectRatio));
	QLabel::resizeEvent(e);
}
