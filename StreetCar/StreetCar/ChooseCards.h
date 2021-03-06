#ifndef CHOOSECARDS_H
#define CHOOSECARDS_H

#include <QWidget>
#include "../Shared/Goal.h"

namespace Ui {
class ChooseCards;
}

class ChooseCards : public QWidget
{
	Q_OBJECT

public:
	explicit ChooseCards(QWidget *parent = 0);
	~ChooseCards();
	QVector<Goal>* getGoal();
	void update();

private slots:
	void on_buttonCardLine1_clicked();
	void on_buttonCardLine2_clicked();
	void on_buttonCardLine3_clicked();
	void on_buttonCardLine4_clicked();
	void on_buttonCardLine5_clicked();
	void on_buttonCardLine6_clicked();
	void on_buttonCardStops1_clicked();
	void on_buttonCardStops2_clicked();
	void on_buttonCardStops3_clicked();
	void on_buttonCardStops4_clicked();
	void on_buttonCardStops5_clicked();
	void on_buttonCardStops6_clicked();
	void on_buttonValidate_clicked();

signals:
	void accepted();
	void validated();

private:
	Ui::ChooseCards *ui;
	QVector<Goal>* goal;
	QVector<QString>* cardsLineList;
	QVector<QString>* cardsStopList;
};

#endif // CHOOSECARDS_H
