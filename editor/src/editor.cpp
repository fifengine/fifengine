#include "editor.h"
#include <QApplication>

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	QWidget* form = new QWidget;
	Ui::Form ui;
	ui.setupUi(form);

	form->show();
	return app.exec();
}
