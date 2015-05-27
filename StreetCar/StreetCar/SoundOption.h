#ifndef SOUNDOPTION_H
#define SOUNDOPTION_H

#include <QWidget>

namespace Ui {
class SoundOption;
}

class SoundOption : public QWidget
{
	Q_OBJECT

public:
	explicit SoundOption(QWidget *parent = 0);
	~SoundOption();

private slots:
	void on_buttonAccept_clicked();
	void on_buttonCancel_clicked();


signals:
	void accepted();
	void rejected();

private:
	Ui::SoundOption *ui;
};

#endif // SOUNDOPTIONWINDOW_H