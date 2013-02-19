/***************************************************************************
 *   Copyright (C) 2005-2013 by the FIFE team                              *
 *   http://www.fifengine.net                                              *
 *   This file is part of FIFE.                                            *
 *                                                                         *
 *   FIFE is free software; you can redistribute it and/or                 *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/

#include "objectcreator.h"
#include "ui_objectcreator.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <QInputDialog>

static int prevDirIndex = -1;

ObjectCreator::ObjectCreator(FifeObject& fobject, QWidget *parent) :
    QDialog(parent),
	ui(new Ui::ObjectCreator), obj(fobject)
{
    ui->setupUi(this);

	// signals
	connect(ui->addDirectionPushBtn, SIGNAL(pressed()),
		this, SLOT(addDirectionPressed()));
	connect(ui->removeDirectionPushBtn, SIGNAL(pressed()),
		this, SLOT(removeDirectionPressed()));
	connect(ui->saveObjectPushBtn, SIGNAL(pressed()),
		this, SLOT(savePressed()));
	connect(ui->cancelObjectPushBtn, SIGNAL(pressed()),
		this, SLOT(cancelPressed()));

	QValidator* imageOffsetsValidator = new QIntValidator(-65535, 65535, this);
	ui->xLineEdit->setValidator(imageOffsetsValidator);
	ui->yLineEdit->setValidator(imageOffsetsValidator);

	// fill list of images
	const MainWindow::Images& imgs = static_cast<MainWindow*>(parent)->images();
	foreach(QNamedImage img, imgs)
	{
		ui->previewComboBox->addItem(img.name);
	}

	// its here so filling the list wont trigger this event
	connect(ui->previewComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(previewImageIndexChanged(int)));

	// if obj is the object to edit, fill necessary widgets
	if(obj.objId != "!NEWONE!")
	{
		ui->objectIDLineEdit->setText(obj.objId);
		ui->blockingCheckBox->setChecked(obj.isBlocking);
		ui->staticCheckBox->setChecked(obj.isStatic);
		ui->patherLineEdit->setText(obj.pather);
	}
	ui->objNamespaceLineEdit->setText(obj.ns);

	if(!obj.images.isEmpty())
	{
		foreach(FifeObject::ImageData data, obj.images)
		{
			ui->directionComboBox->addItem(QString::number(data.direction));
		}

		prevDirIndex = 0;
		ui->xLineEdit->setText(QString::number(obj.images[0].xoffset));
		ui->yLineEdit->setText(QString::number(obj.images[0].yoffset));

		int previewIndex = ui->previewComboBox->findText(obj.images[0].source);
		ui->previewComboBox->setCurrentIndex(previewIndex);
		ui->directionComboBox->setCurrentIndex(0);

		ui->imagesBox->setVisible(true);
	}
	else
	{
		ui->imagesBox->setVisible(false);
		prevDirIndex = -1;
	}

	// same as with previewImageIndexChanged()
	connect(ui->directionComboBox, SIGNAL(currentIndexChanged(int)),
		this, SLOT(directionIndexChanged(int)));

	// now we can trigger events
	if(!obj.images.isEmpty())
	{
		// BUG: triggering implicit signals here (via setCurrentIndex)
		// doesn't generate proper event for index == 0 (for greater index it works)
		directionIndexChanged(0);
		previewImageIndexChanged(0);	
	}
}

ObjectCreator::~ObjectCreator()
{
    delete ui;
}

void ObjectCreator::addDirectionPressed()
{
	bool ok;
	int i = QInputDialog::getInt(this, "Add Direction",
		"Direction: ", 0, 0, 360, 1, &ok);

	if (ok)
	{
		int index = 0;
		foreach(FifeObject::ImageData data, obj.images)
		{
			if(i == data.direction)
			{
				QMessageBox::critical(this, "Atlas Creator",
					"It seems you already added image for given direction");
				return;
			}
			if(i > data.direction)
			{
				index++;
			}
		}

		FifeObject::ImageData data;
		data.direction = i;
		// we set this to previous one
		data.source = ui->previewComboBox->currentText();
		data.xoffset = ui->xLineEdit->text().toInt();
		data.yoffset = ui->yLineEdit->text().toInt();

		obj.images.insert(index, data);
		ui->directionComboBox->insertItem(index, QString::number(i));
		ui->directionComboBox->setCurrentIndex(index);

		ui->imagesBox->setVisible(true);
	}
}

void ObjectCreator::removeDirectionPressed()
{
	if(ui->directionComboBox->count() == 0)
		return;

	int index = ui->directionComboBox->currentIndex();

	ui->directionComboBox->removeItem(index);
	obj.images.remove(index);

	if(ui->directionComboBox->count() == 0)
		ui->imagesBox->setVisible(false);

	// trigger a signal
	ui->directionComboBox->setCurrentIndex(
		ui->directionComboBox->currentIndex());
}

void ObjectCreator::directionIndexChanged(int index)
{
	if(index == -1)
	{
		prevDirIndex = -1;
		return;
	}

	// save what we had before change happened
	if(prevDirIndex != -1)
	{
		obj.images[prevDirIndex].direction =
				ui->directionComboBox->itemText(prevDirIndex).toInt();
		obj.images[prevDirIndex].source =
			ui->previewComboBox->currentText();
		obj.images[prevDirIndex].xoffset =
				ui->xLineEdit->text().toInt();
		obj.images[prevDirIndex].yoffset =
			ui->yLineEdit->text().toInt();
	}

	// set new values for widgets
	ui->xLineEdit->setText(QString::number(obj.images[index].xoffset));
	ui->yLineEdit->setText(QString::number(obj.images[index].yoffset));

	int previewIndex = ui->previewComboBox->findText(obj.images[index].source);
	ui->previewComboBox->setCurrentIndex(previewIndex);

	if(previewIndex == 0)
		previewImageIndexChanged(0);

	prevDirIndex = index;
}

void ObjectCreator::previewImageIndexChanged(int index)
{
	if(index == -1)
		return;

	MainWindow* parent = static_cast<MainWindow*>(parentWidget());
	const MainWindow::Images& imgs = parent->images();

	// scale only bigger ones
	const QNamedImage& img = imgs[index];

	int width = ui->previewLabel->width();
	int height = ui->previewLabel->height();

	if(img.width() > width || img.height() > height)
	{
		ui->previewLabel->setPixmap(QPixmap::fromImage(
			img.scaled(width, height, Qt::KeepAspectRatio)));
	}
	else
	{
		ui->previewLabel->setPixmap(QPixmap::fromImage(img));
	}
}

void ObjectCreator::savePressed()
{
	// sanity check
	if(ui->objectIDLineEdit->text().isEmpty())
	{
		QMessageBox::critical(this, "Atlas Creator", "Given object ID is empty");
		return;
	}

	obj.objId = ui->objectIDLineEdit->text();
	obj.ns = ui->objNamespaceLineEdit->text();
	obj.isBlocking = ui->blockingCheckBox->isChecked();
	obj.isStatic = ui->staticCheckBox->isChecked();
	obj.pather = ui->patherLineEdit->text();

	// we must also save current direction as it is saved only when change happens
	if(!obj.images.isEmpty())
	{
		int index = ui->directionComboBox->currentIndex();
		obj.images[index].direction =
			ui->directionComboBox->itemText(index).toInt();
		obj.images[index].source =
			ui->previewComboBox->currentText();
		obj.images[index].xoffset =
			ui->xLineEdit->text().toInt();
		obj.images[index].yoffset =
			ui->yLineEdit->text().toInt();
	}

	done(QDialog::Accepted);
}

void ObjectCreator::cancelPressed()
{
	// ask the user if he's sure only when object id isn't empty
	if(!ui->objectIDLineEdit->text().isEmpty())
	{
		int res = QMessageBox::question(this, "Atlas Creator", "Are you sure to discard any changes for this object",
			QMessageBox::Yes, QMessageBox::No);
		if(res == QMessageBox::Yes)
		{
			done(QDialog::Rejected);
		}
	}
	else
	{
		done(QDialog::Rejected);
	}
}
