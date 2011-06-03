/***************************************************************************
 *   Copyright (C) 2005-2011 by the FIFE team                              *
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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>

//#define USE_QT_XML
#ifdef USE_QT_XML
#include <QDomDocument>
#else
#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"
#endif

#if defined(_OPENMP)
#	include <omp.h>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
	ui(new Ui::MainWindow), numAtlases(0)
{
    ui->setupUi(this);

	// signals
	connect(ui->addTexturesPushBtn, SIGNAL(pressed()),
		this, SLOT(addTexturesPressed()));
	connect(ui->addDirPushBtn, SIGNAL(pressed()),
		this, SLOT(addDirectoryPressed()));
	connect(ui->removeTexturesPushBtn, SIGNAL(pressed()),
		this, SLOT(removeTexturesPressed()));
	connect(ui->refreshPushBtn, SIGNAL(pressed()),
		this, SLOT(refreshPressed()));
	connect(ui->savePushBtn, SIGNAL(pressed()),
		this, SLOT(savePressed()));
	connect(ui->showFullPathCheckBox, SIGNAL(stateChanged(int)),
		this, SLOT(showFullPathChanged(int)));
	connect(ui->showSubimageCheckBox, SIGNAL(stateChanged(int)),
		this, SLOT(showSubImageChanged(int)));
	connect(ui->subimageNoSpinBox, SIGNAL(valueChanged(int)),
		this, SLOT(subimageNoChanged(int)));

	connect(ui->atlasPageSpinBox, SIGNAL(valueChanged(int)),
		ui->previewWidget, SLOT(atlasBookChanged(int)));
	connect(ui->alphaBlendingCheckBox, SIGNAL(stateChanged(int)),
		ui->previewWidget, SLOT(alphaBlendingChecked(int)));
	connect(ui->showOccupationCheckBox, SIGNAL(stateChanged(int)),
		ui->previewWidget, SLOT(showOccupationChecked(int)));
	connect(ui->subimageNoSpinBox, SIGNAL(valueChanged(int)),
		ui->previewWidget, SLOT(subimageNoChanged(int)));

	// default values
	ui->tabWidget->setCurrentIndex(0);
	ui->maxWidthLineEdit->setText("2048");
	ui->maxHeightLineEdit->setText("2048");
	ui->pOTAtlasCheckBox->setChecked(true);
	ui->pOTTexturesCheckBox->setChecked(false);
	ui->shrinkCheckBox->setChecked(true);
	ui->atlasPageSpinBox->setRange(0, 0);
	ui->showFullPathCheckBox->setChecked(true);

	ui->alphaBlendingCheckBox->setChecked(false);
	ui->showSubimageCheckBox->setChecked(false);
	ui->subimageNoSpinBox->setVisible(false);
	ui->subimageNoSpinBox->setRange(-1, 0);
	ui->subimageNoSpinBox->setValue(-1);
	ui->subimageNoLabel->setVisible(false);

	// validators
	QValidator* atlasSizeValidator = new QIntValidator(1, 16384, this);

	ui->maxWidthLineEdit->setValidator(atlasSizeValidator);
	ui->maxHeightLineEdit->setValidator(atlasSizeValidator);
}

MainWindow::~MainWindow()
{
	// Cleanup
	for(int i = 0; i < subImgs.size(); ++i)
		delete subImgs[i];

	subImgs.clear();
	regions.clear();
	imgAtlases.clear();

    delete ui;
}

void MainWindow::removeTexturesPressed()
{
	QList<QListWidgetItem*> selectedList =
			ui->fileListWidget->selectedItems();

	foreach(QListWidgetItem* item, selectedList)
	{
		// remove this item from texturePath as well
		for(int i = 0; i < texturePaths.count(); ++i)
		{
			if(ui->showFullPathCheckBox->isChecked())
			{
				if(texturePaths.at(i).first == item->text())
				{
					texturePaths.remove(i);
					break;
				}
			}
			else
			{
				if(texturePaths.at(i).second == item->text())
				{
					texturePaths.remove(i);
					break;
				}
			}
		}
		delete item;
	}
}

void MainWindow::addDirectoryPressed()
{
	QString dirPath = QFileDialog::getExistingDirectory(this,
		"Select your images directory", QDir::currentPath());

	if(dirPath.isEmpty())
		return;

	QDirIterator dirWalker(dirPath, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

	while(dirWalker.hasNext())
	{
		dirWalker.next();

		QString suffix = dirWalker.fileInfo().completeSuffix();
		if(suffix == "png" || suffix == "jpg" || suffix == "jpeg" || suffix == "tga" || suffix == "bmp")
		{
			QString filepath = dirWalker.filePath();

			texturePaths.push_back(QPair<QString, QString>(
					filepath, // fullpath
					QFileInfo(filepath).completeBaseName() + "." +QFileInfo(filepath).suffix())); // filename only

			if(ui->showFullPathCheckBox->isChecked())
			{
				ui->fileListWidget->addItem(filepath);
			}
			else
			{
				ui->fileListWidget->addItem(dirWalker.fileName());
			}
		}
	}
	ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::addTexturesPressed()
{
	QStringList filenames = QFileDialog::getOpenFileNames(this, "Add textures",
		".", "Texture Files (*.png *.bmp *.jpg *.jpeg *.tga)");

	// for now we allow duplicates on the texture list

	if(filenames.empty())
		return;

	foreach(QString filename, filenames)
	{
		texturePaths.push_back(QPair<QString, QString>(
				filename,
				QFileInfo(filename).completeBaseName() + "." +QFileInfo(filename).suffix()));
	}

	if(ui->showFullPathCheckBox->isChecked())
	{
		ui->fileListWidget->addItems(filenames);
	}
	else
	{
		typedef QPair<QString, QString> PairOfString;
		foreach(PairOfString pair, texturePaths)
		{
			ui->fileListWidget->addItem(pair.second);
		}
	}

	ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::showFullPathChanged(int state)
{
	for(int i = 0; i < ui->fileListWidget->count(); ++i)
	{
		if(state == Qt::Checked)
		{
			ui->fileListWidget->item(i)->setText(texturePaths.at(i).first);
		}
		else
		{
			ui->fileListWidget->item(i)->setText(texturePaths.at(i).second);
		}
	}
}

void MainWindow::showSubImageChanged(int state)
{
	static int oldIndex = 0;

	if(state == Qt::Checked)
	{
		ui->subimageNoSpinBox->setVisible(true);
		ui->subimageNoLabel->setVisible(true);

		ui->subimageNoSpinBox->setMinimum(0);
		ui->subimageNoSpinBox->setValue(oldIndex);
	}
	else
	{
		ui->subimageNoSpinBox->setVisible(false);
		ui->subimageNoLabel->setVisible(false);

		oldIndex = ui->subimageNoSpinBox->value();
		ui->subimageNoSpinBox->setMinimum(-1);
		ui->subimageNoSpinBox->setValue(-1);
	}
}

void MainWindow::subimageNoChanged(int index)
{
	if(subImgs.isEmpty())
		return;

	if(index < 0)
	{
		ui->statusBar->clearMessage();
		return;
	}

	ui->atlasPageSpinBox->setValue(regions[index].page);
	ui->statusBar->clearMessage();
	ui->statusBar->showMessage(QString::fromStdString(subImgs[index]->getImageName()));
}

bool ImageGreater(Image const* a, Image const* b)
{
	uint32_t aa = a->getWidth() * a->getHeight();
	uint32_t bb = b->getWidth() * b->getHeight();

	if(aa > bb)
		return true;

	else if(a->getHeight() > b->getHeight() && aa == bb)
		return true;

	else if(a->getWidth() > b->getWidth() && aa == bb)
		return true;

	else
		return false;
}


void MainWindow::refreshPressed()
{
	int atlasX = ui->maxWidthLineEdit->text().toInt();
	int atlasY = ui->maxHeightLineEdit->text().toInt();
	int numTextures = ui->fileListWidget->count();

	if(!numTextures)
	{
		QMessageBox::warning(this, "Warning", "There are no textures to be atlased.");
		return;
	}

	AtlasBook book(atlasX, atlasY, 4);

	// OPTIMIZE: dont release and load from the beginning the same images
	// only those who was changed

	// Cleanup
	for(int i = 0; i < subImgs.size(); ++i)
		delete subImgs[i];

	subImgs.clear();
	regions.clear();
	imgAtlases.clear();

	// New images
	subImgs.resize(numTextures);
	regions.resize(numTextures);
	numAtlases = 0;

	int i;

	// Load the image
	//#pragma omp parallel for private(i) default(shared)
	for(i = 0; i < numTextures; ++i)
	{
		subImgs[i] = new Image;
		subImgs[i]->loadImage(texturePaths[i].first.toStdString(), ui->pOTTexturesCheckBox->isChecked());
	}

	// sort images by size
	qSort(subImgs.begin(), subImgs.end(), ImageGreater);

	try
	{
		for(i = 0; i < numTextures; ++i)
		{
			// find suitable region for it
			regions[i] = *book.getBlock(
				subImgs[i]->getWidth(), subImgs[i]->getHeight());
		}
	}
	catch(std::string const& msg)
	{
		QMessageBox::critical(this, "Critical Error", QString::fromStdString(msg));
		return;
	}

	numAtlases = 1;
	for(i = 0; i < numTextures; ++i)
		numAtlases = qMax(static_cast<int>(regions[i].page + 1), numAtlases);

	if(ui->shrinkCheckBox->isChecked())
		book.shrink(ui->pOTAtlasCheckBox->isChecked());

	// Save the atlases
	imgAtlases.resize(numAtlases);
	for(i = 0; i < numAtlases; ++i)
		imgAtlases[i].setImage(book.pages[i].width, book.pages[i].height, 32);

	// For every subimage, copy its contents to corresponding atlas
	//#pragma omp parallel for private(i) default(shared)
	for(i = 0; i < numTextures; ++i)
	{
		imgAtlases[regions[i].page].updateSubimage(
			regions[i].left, regions[i].top, *subImgs[i]);
	}

	ui->previewWidget->refreshed(imgAtlases);
	ui->previewWidget->updateOccupation(regions, numAtlases);
	ui->atlasPageSpinBox->setRange(0, numAtlases-1);
	ui->subimageNoSpinBox->setMaximum(subImgs.size()-1);

	if(ui->showSubimageCheckBox->checkState() == Qt::Checked)
	{
		ui->subimageNoSpinBox->setMinimum(0);
		ui->subimageNoSpinBox->setValue(0);
	}
	else
	{
		ui->subimageNoSpinBox->setMinimum(-1);
		ui->subimageNoSpinBox->setValue(-1);
	}

	// find the best file path matching
	QString bestMatch = texturePaths[0].first;
	for(int i = 1; i < texturePaths.size(); ++i)
	{
		int q = 0;
		while(q < texturePaths[i].first.size() &&
			  q < bestMatch.size() &&
			  texturePaths[i].first.at(q) == bestMatch.at(q))
			++q;
		if(!q)
			break;
		bestMatch = bestMatch.left(q);
	}

	// we only need to match whole directories, not part of them (or even part of filenames)
	if(!bestMatch.endsWith('/') && !bestMatch.isEmpty())
	{
		bestMatch = bestMatch.left(bestMatch.lastIndexOf('/') + 1);
	}

	sharedPathNumChars = bestMatch.size();
	ui->prependPathLineEdit->setText(bestMatch);
}

void MainWindow::savePressed()
{
	if(!numAtlases || subImgs.empty())
	{
		QMessageBox::warning(this, "Warning", "There are no atlases to be saved.");
		return;
	}

	QString filenameBase = QFileDialog::getSaveFileName(this, "Save atlas",
		".", "Atlas Texture (*.png)");
	if(filenameBase.isNull())
		return;

	QVector<QString> atlasNames;
	if(numAtlases > 1)
	{
		QFileInfo pathInfo(filenameBase);
		QString absPath = pathInfo.absolutePath();
		QString filename = pathInfo.completeBaseName();
		QString ext = pathInfo.suffix();

		for(int i = 0; i < numAtlases; ++i)
		{
			QString name;
			QTextStream strm(&name);
			strm << absPath << "/" << filename << i << "." << ext;

			imgAtlases[i].saveImage(name.toStdString());
			atlasNames.push_back(filename + QString::number(i) + "." + ext);
		}
	}
	else
	{
		QFileInfo pathInfo(filenameBase);
		QString filename = pathInfo.completeBaseName();
		QString ext = pathInfo.suffix();

		atlasNames.push_back(filename + "." + ext);
		imgAtlases[0].saveImage(filenameBase.toStdString());
	}

	//
	// Atlas descriptor (.atlas)
	//

#ifdef USE_QT_XML
	for(int i = 0; i < numAtlases; ++i)
	{
		QDomDocument doc;
		QDomElement root = doc.createElement("atlas");
		doc.appendChild(root);

		root.setAttribute("source", atlasNames[i]);
		root.setAttribute("width", QString::number(imgAtlases[i].getWidth()));
		root.setAttribute("height", QString::number(imgAtlases[i].getHeight()));

		for(int j = 0; j < subImgs.size(); ++j)
		{
			if(regions[j].page != (uint32_t) i)
				continue;

			QDomElement elem = doc.createElement("image");

			QString imagePath = QString::fromStdString(subImgs[j]->getImageName());
			QFileInfo filePath(imagePath);
			QString dirPath = filePath.absoluteDir().path();
			QString source = ui->prependPathLineEdit->text() +
								dirPath.right(dirPath.size() - sharedPathNumChars) + "/" +
								filePath.completeBaseName() + "." + filePath.suffix();

			elem.setAttribute("source", source);
			elem.setAttribute("xoffset", QString::number(regions[j].left));
			elem.setAttribute("yoffset", QString::number(regions[j].top));
			elem.setAttribute("width", QString::number(regions[j].getWidth()));
			elem.setAttribute("height", QString::number(regions[j].getHeight()));

			root.appendChild(elem);
		}

		// save it
		QFileInfo pathInfo(filenameBase);

		// generate proper filename
		QString descFilename = pathInfo.absolutePath() + "/" + pathInfo.completeBaseName();
		if(numAtlases > 1)
			descFilename += QString::number(i);
		descFilename += ".atlas";

		// save xml doc
		QFile data(descFilename);
		if (data.open(QFile::WriteOnly | QFile::Truncate))
		{
			QTextStream out(&data);
			out << doc.toString();
		}
	}
#else
	for(int i = 0; i < numAtlases; ++i)
	{
		rapidxml::xml_document<> doc; // character type defaults to char
		rapidxml::xml_attribute<>* attr;

		rapidxml::xml_node<>* child = doc.allocate_node(rapidxml::node_element, "atlas");
		doc.append_node(child);

		char const* nameAttr = doc.allocate_string(
			atlasNames[i].toLocal8Bit().constData());
		char const* widthAttr = doc.allocate_string(
			QString::number(imgAtlases[i].getWidth()).toLocal8Bit().constData());
		char const* heightAttr = doc.allocate_string(
			QString::number(imgAtlases[i].getHeight()).toLocal8Bit().constData());

		attr = doc.allocate_attribute("name", nameAttr);
		child->append_attribute(attr);
		attr = doc.allocate_attribute("width", widthAttr);
		child->append_attribute(attr);
		attr = doc.allocate_attribute("height", heightAttr);
		child->append_attribute(attr);

		for(int j = 0; j < subImgs.size(); ++j)
		{
			if(regions[j].page != (uint32_t) i)
				continue;

			rapidxml::xml_node<>* newNode = doc.allocate_node(rapidxml::node_element, "image");
			child->append_node(newNode);

			QString imagePath = QString::fromStdString(subImgs[j]->getImageName());
			QFileInfo filePath(imagePath);
			QString dirPath = filePath.absoluteDir().path();
			QString source = ui->prependPathLineEdit->text() +
								dirPath.right(dirPath.size() - sharedPathNumChars) + "/" +
								filePath.completeBaseName() + "." + filePath.suffix();

			char const* sourceAttr = doc.allocate_string(
				source.toLocal8Bit().constData());
			char const* xoffsetAttr = doc.allocate_string(
				QString::number(regions[j].left).toLocal8Bit().constData());
			char const* yoffsetAttr = doc.allocate_string(
				QString::number(regions[j].top).toLocal8Bit().constData());
			char const* widthAttr = doc.allocate_string(
				QString::number(regions[j].getWidth()).toLocal8Bit().constData());
			char const* heightAttr = doc.allocate_string(
				QString::number(regions[j].getHeight()).toLocal8Bit().constData());

			attr = doc.allocate_attribute("source", sourceAttr);
			newNode->append_attribute(attr);
			attr = doc.allocate_attribute("xpos", xoffsetAttr);
			newNode->append_attribute(attr);
			attr = doc.allocate_attribute("ypos", yoffsetAttr);
			newNode->append_attribute(attr);
			attr = doc.allocate_attribute("width", widthAttr);
			newNode->append_attribute(attr);
			attr = doc.allocate_attribute("height", heightAttr);
			newNode->append_attribute(attr);
		}

		std::string s;
		rapidxml::print(std::back_inserter(s), doc, 0);

		// save it
		QFileInfo pathInfo(filenameBase);

		// generate proper filename
		QString descFilename = pathInfo.absolutePath() + "/" + pathInfo.completeBaseName();
		if(numAtlases > 1)
			descFilename += QString::number(i);
		descFilename += ".atlas";

		// save xml doc
		QFile data(descFilename);
		if (data.open(QFile::WriteOnly | QFile::Truncate))
		{
			QTextStream out(&data);
			out << QString::fromStdString(s);
		}
	}
#endif
}
