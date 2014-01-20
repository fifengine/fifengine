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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "glwidget.h"
#include "objectcreator.h"
#include "atlas.h"

#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_print.hpp"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
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
	connect(ui->fileListWidget, SIGNAL(itemSelectionChanged()),
		this, SLOT(textureSelectionChanged()));

	connect(ui->addObjectPushBtn, SIGNAL(pressed()),
		this, SLOT(addObjectPressed()));
	connect(ui->removeObjectsPushBtn, SIGNAL(pressed()),
		this, SLOT(removeObjectsPressed()));
	connect(ui->objectListWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
		this, SLOT(objectDoubleClicked(QListWidgetItem*)));

	connect(ui->disassemblePushBtn, SIGNAL(pressed()),
		this, SLOT(disassemblePressed()));

	connect(ui->alphaBlendingCheckBox, SIGNAL(stateChanged(int)),
		ui->previewWidget, SLOT(alphaBlendingChecked(int)));
	connect(ui->showOccupationCheckBox, SIGNAL(stateChanged(int)),
		ui->previewWidget, SLOT(showOccupationChecked(int)));

	// default values
	ui->tabWidget->setCurrentIndex(0);
	ui->maxWidthLineEdit->setText("2048");
	ui->maxHeightLineEdit->setText("2048");
	ui->pOTAtlasCheckBox->setChecked(true);
	ui->shrinkCheckBox->setChecked(true);
	ui->showFullPathCheckBox->setChecked(true);

	ui->alphaBlendingCheckBox->setChecked(false);
	ui->tabWidget->setCurrentIndex(0);
	//ui->objectEditorTabs->setEnabled(false);

	// validators
	QValidator* atlasSizeValidator = new QIntValidator(1, 16384, this);

	ui->maxWidthLineEdit->setValidator(atlasSizeValidator);
	ui->maxHeightLineEdit->setValidator(atlasSizeValidator);

	fileExtAllowed.push_back("png");
	fileExtAllowed.push_back("jpg");
	fileExtAllowed.push_back("jpeg");
	fileExtAllowed.push_back("tga");
	fileExtAllowed.push_back("bmp");

	filter = "Texture Files (";
	foreach(QString ext, fileExtAllowed)
		filter += "*." + ext + " ";
	filter.chop(1);
	filter += ")";
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::addTexturesPressed()
{
	QStringList filenames = QFileDialog::getOpenFileNames(this, "Add textures",
		lastLoadDir, filter);

	if(filenames.empty())
		return;

	QDir dir(filenames.back());
	lastLoadDir = dir.absolutePath();

	foreach(QString filename, filenames)
	{

		QFileInfo q(filename);
		QString filename1 = q.absoluteFilePath();
		QString filename2 = q.completeBaseName() + "." + q.suffix();
		texturePaths.push_back(QPair<QString, QString>(filename1, filename2));

		if(ui->showFullPathCheckBox->isChecked())
		{
			ui->fileListWidget->addItem(filename1);
		}
		else
		{
			ui->fileListWidget->addItem(filename2);
		}
	}

	ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::addDirectoryPressed()
{
	QString dirPath = QFileDialog::getExistingDirectory(this,
		"Select your images directory", lastLoadDir);

	if(dirPath.isEmpty())
		return;

	QDir dir(dirPath);
	dir.cdUp();
	lastLoadDir = dir.absolutePath();

	QDirIterator dirWalker(dirPath, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

	while(dirWalker.hasNext())
	{
		dirWalker.next();

		QString suffix = dirWalker.fileInfo().completeSuffix();
		bool suffixok = false;
		foreach(QString ext, fileExtAllowed)
		{
			if(suffix == ext)
			{
				suffixok = true;
				break;
			}
		}

		if(suffixok)
		{
			QString filepath = dirWalker.filePath(); // fullpath
			QFileInfo fileInfo(filepath);
			QString basename = fileInfo.completeBaseName() + "." + fileInfo.suffix();// filename only

			texturePaths.push_back(QPair<QString, QString>(
					filepath, basename));

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

void MainWindow::removeTexturesPressed()
{
	QList<QListWidgetItem*> selectedList =
			ui->fileListWidget->selectedItems();

	foreach(QListWidgetItem* item, selectedList)
	{
		// remove this item from texturePath as well
		for(int i = texturePaths.count() - 1; i >= 0; --i)
		{
			if(ui->showFullPathCheckBox->isChecked())
			{
				if(texturePaths[i].first == item->text())
				{
					texturePaths.remove(i);
					break;
				}
			}
			else
			{
				if(texturePaths[i].second == item->text())
				{
					texturePaths.remove(i);
					break;
				}
			}
		}
		delete item;
	}
}

void MainWindow::showFullPathChanged(int state)
{
	for(int i = 0; i < ui->fileListWidget->count(); ++i)
	{
		if(state == Qt::Checked)
		{
			ui->fileListWidget->item(i)->setText(texturePaths[i].first);
		}
		else
		{
			ui->fileListWidget->item(i)->setText(texturePaths[i].second);
		}
	}
}

void MainWindow::textureSelectionChanged()
{
	QList<QListWidgetItem*> selectedList =
			ui->fileListWidget->selectedItems();
	QVector<int> indices;

	if(selectedList.isEmpty())
	{
		if(!subImgs.isEmpty())
		{
			ui->previewWidget->highlightedChanged(indices);
		}
		else return;
	}

	foreach(QListWidgetItem* item, selectedList)
	{
		int row = ui->fileListWidget->row(item);
		int id = -1;

		for(int i = 0; i < subImgs.size(); ++i)
		{
			if(subImgs[i].name == texturePaths[row].second)
			{
				id = i;
				break;
			}
		}
		if(id != -1)
			indices.push_back(id);
	}

	ui->previewWidget->highlightedChanged(indices);
}

void MainWindow::refreshPressed()
{
	int numTextures = ui->fileListWidget->count();

	if(!numTextures)
	{
		QMessageBox::critical(this, "Atlas Creator", "There are no textures to be atlased.");
		return;
	}

	// find the best file path matching
	if(numTextures > 1)
	{
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
	}
	else
	{
		sharedPathNumChars = texturePaths[0].first.lastIndexOf('/') + 1;
	}

	// find possible root directories
	QString highestRootDir = texturePaths[0].first.left(sharedPathNumChars);
	QDir dirTrav(highestRootDir);

	ui->lwRootDir->clear();
	do
	{
		ui->lwRootDir->addItem(dirTrav.absolutePath());
	} while(dirTrav.cdUp());
	ui->lwRootDir->setCurrentItem(ui->lwRootDir->item(0));

	// OPTIMIZE: dont release and load from the beginning the same images
	// only those who was changed
	int atlasX = ui->maxWidthLineEdit->text().toInt();
	int atlasY = ui->maxHeightLineEdit->text().toInt();
	AtlasBook book(atlasX, atlasY, 4);

	// Cleanup
	subImgs.clear();
	regions.clear();

	// New images
	regions.resize(numTextures);

	// Load the image
	for(int i = 0; i < numTextures; ++i)
	{
		QFileInfo imagePath(texturePaths[i].first);
		QString dirPath = imagePath.absoluteDir().path() + "/";
		QString append = (dirPath.size() == sharedPathNumChars) ?
					"" : dirPath.right(dirPath.size() - sharedPathNumChars) ;
		QString source = append + imagePath.completeBaseName() + "." + imagePath.suffix();
		texturePaths[i].second = source;

		QImage img(texturePaths[i].first);
		if(img.format() != QImage::Format_ARGB32)
			img = img.convertToFormat(QImage::Format_ARGB32);

		QNamedImage nimg(img);
		nimg.name = source;
		subImgs.push_back(nimg);
	}

	// sort images by size
	qSort(subImgs.begin(), subImgs.end(), ImageGreater);

	try
	{
		int i = 0;
		foreach(QNamedImage img, subImgs)
		{
			regions[i] = *book.getBlock(
				img.width(), img.height());

			// temporary hack - we now allowed only one atlas to be created
			if(regions[i].page > 0)
			{
				QMessageBox::critical(this, "Atlas Creator",
					"Too much images to be packed.\n"
					"Remove some of them or enlarge atlas maximum allowed size and try again.");
				return;
			}

			++i;
		}
	}
	catch(std::string const& msg)
	{
		QMessageBox::critical(this, "Atlas Creator", QString::fromStdString(msg));
		return;
	}

	if(ui->shrinkCheckBox->isChecked())
		book.shrink(ui->pOTAtlasCheckBox->isChecked());

	// Generate empty atlas
	imgAtlas = QImage(book.pages[0].width, book.pages[0].height, QImage::Format_ARGB32);
	imgAtlas.fill(qRgba(0, 0, 0, 0));

	// For every subimage, copy its contents to corresponding atlas
	int i = 0;
	foreach(QImage img, subImgs)
	{
		updateSubimage(imgAtlas, regions[i].left, regions[i].top, img);
		++i;
	}

	textureSelectionChanged();
	ui->previewWidget->refreshed(imgAtlas);
	ui->previewWidget->updateOccupation(regions);
	ui->objectEditorTabs->setEnabled(true);
	ui->statusBar->showMessage(QString("Atlas size: %1x%2")
	   .arg(imgAtlas.width())
	   .arg(imgAtlas.height()));
}

void MainWindow::savePressed()
{
	if(subImgs.empty())
	{
		QMessageBox::critical(this, "Atlas Creator", "Atlas hasn't been generated yet.");
		return;
	}

	QString filenameBase = QFileDialog::getSaveFileName(this, "Save atlas",
		lastSaveDir, "Atlas Texture (*.png)");
	if(filenameBase.isNull())
		return;

	QDir dir(filenameBase);
	lastSaveDir = dir.absolutePath();

	QFileInfo pathInfo(filenameBase);
	QString filename = pathInfo.completeBaseName();
	QString ext = pathInfo.suffix();
	QString atlasName = filename + "." + ext;

	bool res = imgAtlas.save(filenameBase, "PNG");
	if(!res)
	{
		QMessageBox::critical(this, "Atlas Creator", "Something went wrong with saving atlas image");
		return;
	}

	QString append;
	if(ui->lwRootDir->currentRow() != 0)
	{
		QString root = ui->lwRootDir->item(0)->text();
		QString chosenRoot = ui->lwRootDir->currentItem()->text();

		if(ui->lwRootDir->currentRow() ==  ui->lwRootDir->count() - 1)
		{
			append = root;
		}
		else
		{
			int len = root.length() - chosenRoot.length();
			append = root.right(len - 1) + '/';
		}
	}

	//
	// Atlas descriptor (.xml)
	//

	rapidxml::xml_document<> doc; // character type defaults to char
	rapidxml::xml_attribute<>* attr;

	rapidxml::xml_node<>* child = doc.allocate_node(rapidxml::node_pi, "fife", "type=\"atlas\"");
	doc.append_node(child);
	child = doc.allocate_node(rapidxml::node_element, "atlas");
	doc.append_node(child);

	char const* nameAttr = doc.allocate_string(
		atlasName.toLocal8Bit().constData());
	char const* widthAttr = doc.allocate_string(
		QString::number(imgAtlas.width()).toLocal8Bit().constData());
	char const* heightAttr = doc.allocate_string(
		QString::number(imgAtlas.height()).toLocal8Bit().constData());

	QString ns = atlasName;
	ns.chop(4);
	const char* nsAttr = doc.allocate_string(
				ui->namespaceLineEdit->text().isEmpty() ?
					ns.toLocal8Bit().constData() : ui->namespaceLineEdit->text().toLocal8Bit().constData());

	attr = doc.allocate_attribute("name", nameAttr);
	child->append_attribute(attr);
	attr = doc.allocate_attribute("namespace", nsAttr);
	child->append_attribute(attr);
	attr = doc.allocate_attribute("width", widthAttr);
	child->append_attribute(attr);
	attr = doc.allocate_attribute("height", heightAttr);
	child->append_attribute(attr);

	int j = 0;
	foreach(QNamedImage img, subImgs)
	{
		rapidxml::xml_node<>* newNode = doc.allocate_node(rapidxml::node_element, "image");
		child->append_node(newNode);

		char const* sourceAttr = doc.allocate_string(
			(append + img.name).toLocal8Bit().constData());
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

		++j;
	}

	// Now save all of the objects
	foreach(FifeObject obj, fifeObjects)
	{
		child = doc.allocate_node(rapidxml::node_element, "object");
		doc.append_node(child);

		// if object namespace isnt declared use one provided from atlas
		QString nsobj = obj.ns.isEmpty() ? QString(nsAttr) : obj.ns;

		char const* idAttr = doc.allocate_string(
			obj.objId.toLocal8Bit().constData());
		char const* nsAttr = doc.allocate_string(
			nsobj.toLocal8Bit().constData());
		char const* blockingAttr = doc.allocate_string(
			QString::number(static_cast<int>(obj.isBlocking)).toLocal8Bit().constData());
		char const* staticAttr = doc.allocate_string(
			QString::number(static_cast<int>(obj.isStatic)).toLocal8Bit().constData());

		attr = doc.allocate_attribute("id", idAttr);
		child->append_attribute(attr);
		attr = doc.allocate_attribute("namespace", nsAttr);
		child->append_attribute(attr);
		attr = doc.allocate_attribute("blocking", blockingAttr);
		child->append_attribute(attr);
		attr = doc.allocate_attribute("static", staticAttr);
		child->append_attribute(attr);

		if(!obj.pather.isEmpty())
		{
			attr = doc.allocate_attribute("pather", doc.allocate_string(
				obj.pather.toLocal8Bit().constData()));
			child->append_attribute(attr);
		}

		foreach(FifeObject::ImageData img, obj.images)
		{
			rapidxml::xml_node<>* newNode = doc.allocate_node(rapidxml::node_element, "image");
			child->append_node(newNode);

			char const* sourceAttr = doc.allocate_string(
				(append + img.source).toLocal8Bit().constData());
			char const* directionAttr = doc.allocate_string(
				QString::number(img.direction).toLocal8Bit().constData());

			attr = doc.allocate_attribute("source", sourceAttr);
			newNode->append_attribute(attr);
			attr = doc.allocate_attribute("direction", directionAttr);
			newNode->append_attribute(attr);

			if(img.xoffset != 0)
			{
				attr = doc.allocate_attribute("x_offset", doc.allocate_string(
					QString::number(img.xoffset).toLocal8Bit().constData()));
				newNode->append_attribute(attr);
			}
			if(img.yoffset != 0)
			{
				attr = doc.allocate_attribute("y_offset", doc.allocate_string(
					QString::number(img.yoffset).toLocal8Bit().constData()));
				newNode->append_attribute(attr);
			}
		}
	}

	std::string s;
	rapidxml::print(std::back_inserter(s), doc, 0);

	// save it - generate proper filename
	QString descFilename = pathInfo.absolutePath() + "/" + pathInfo.completeBaseName();
	descFilename += ".xml";

	// save xml doc
	QFile data(descFilename);
	if (data.open(QFile::WriteOnly | QFile::Truncate))
	{
		QTextStream out(&data);
		out << QString::fromStdString(s);
	}
}

void MainWindow::addObjectPressed()
{
	FifeObject newobj;
	newobj.objId = "!NEWONE!";
	// at this point we dont know what would be final namespace for atlas
	// this is known during saving .xml file
	newobj.ns = ui->namespaceLineEdit->text().isEmpty() ? "" : ui->namespaceLineEdit->text();
	ObjectCreator dialog(newobj, this);
	dialog.exec();

	if(dialog.result() == QDialog::Accepted)
	{
		QString ns = newobj.ns.isEmpty() ? "" : (newobj.ns + ":");
		QString key = ns + newobj.objId;
		fifeObjects.insert(key, newobj);

		// add this item to displayed list
		ui->objectListWidget->addItem(key);
	}
}

void MainWindow::objectDoubleClicked(QListWidgetItem* item)
{
	FifeObject obj = fifeObjects[item->text()];
	ObjectCreator dialog(obj, this);
	dialog.exec();

	if(dialog.result() == QDialog::Accepted)
	{
		QString ns = obj.ns.isEmpty() ? "" : (obj.ns + ":");
		QString key = ns + obj.objId;
		fifeObjects.remove(item->text());
		fifeObjects.insert(key, obj);

		// update selected item
		item->setText(key);
	}
}

void MainWindow::removeObjectsPressed()
{
	QList<QListWidgetItem*> selectedList =
			ui->objectListWidget->selectedItems();

	if(selectedList.isEmpty())
		return;

	int res = QMessageBox::question(this, "Atlas Creator",
		"Are you sure to remove " + QString::number(selectedList.count()) + " object(s)",
		QMessageBox::Yes, QMessageBox::No);

	if(res == QMessageBox::Yes)
	{
		foreach(QListWidgetItem* item, selectedList)
		{
			fifeObjects.remove(item->text());
			delete item;
		}
	}
}

void MainWindow::updateSubimage(QImage& dest, int xoffset, int yoffset, const QImage& src)
{
	Q_ASSERT(src.constBits());
	Q_ASSERT(dest.bits());
	Q_ASSERT(src.depth() == dest.depth());
	Q_ASSERT(xoffset + src.width() <= dest.width());
	Q_ASSERT(yoffset + src.height() <= dest.height());

	xoffset = xoffset * dest.depth() / 8;

	for(int y = 0; y < src.height(); ++y)
	{
		const uint8_t* pSrc = src.scanLine(y);
		uint8_t* pDst = dest.scanLine(yoffset + y);
		memcpy(pDst + xoffset, pSrc, src.bytesPerLine());
	}
}

QString getAttrib(rapidxml::xml_node<>* node, QString const& attribName,
	QString const& defValue = "")
{
	static rapidxml::xml_attribute<>* attr;
	attr = node->first_attribute(attribName.toLocal8Bit().constData());
	if(!attr)
		return defValue;

	return QString::fromLatin1(attr->value());
}

int getAttribInt(rapidxml::xml_node<>* node, QString const& attribName,
	int defValue = -1)
{
	static rapidxml::xml_attribute<>* attr;
	attr = node->first_attribute(attribName.toLocal8Bit().constData());
	if(!attr)
		return defValue;

	return QString::fromLatin1(attr->value()).toInt();
}

void MainWindow::disassemblePressed()
{
	QString filename = QFileDialog::getOpenFileName(this,
		"Disassemble atlas", ".", "Atlas files (*.xml)");

	if(filename.isNull())
		return;
	QFileInfo fileInfo(filename);
	QString appendPath = fileInfo.path() + '/';

	QFile data(filename);
	if (!data.open(QFile::ReadOnly))
	{
		QMessageBox::critical(this, "Atlas Creator",
			"Couldn't read selected file.");
		return;
	}

	QTextStream in(&data);
	QString inputXml = in.readAll();

	QByteArray xmlCopy(inputXml.toLocal8Bit().constData(), inputXml.size());
	xmlCopy.push_back('\0');

	// parse file
	rapidxml::xml_document<> doc;
	try
	{
		doc.parse<rapidxml::parse_pi_nodes>(xmlCopy.data());
	}
	catch(rapidxml::parse_error& error)
	{
		QMessageBox::critical(this, "Atlas Creator", QString::fromLatin1(error.what()));
		return;
	}

	rapidxml::xml_node<>* root = doc.first_node();
	if(qstrcmp(root->value(), "type=\"atlas\""))
	{
		QMessageBox::critical(this, "Atlas Creator",
			"Selected file doesn't seem to be proper FIFE atlas file.");
		return;
	}

	root = root->next_sibling();
	QString ns = getAttrib(root, "namespace");
	if(ns.isEmpty())
	{
		QMessageBox::critical(this, "Atlas Creator",
			"Selected file doesn't seem to be proper FIFE atlas file.");
		return;
	}
	ns = appendPath + ns;

	// make new directory if necessary for subimages
	if(!QDir(ns).exists())
		QDir().mkdir(ns);

	QString atlasFile = getAttrib(root, "name");
	if(atlasFile.isEmpty())
	{
		QMessageBox::critical(this, "Atlas Creator",
			"Selected file doesn't seem to be proper FIFE atlas file.");
		return;
	}
	atlasFile = appendPath + atlasFile;

	QImage atlas;
	if(!atlas.load(atlasFile))
	{
		QMessageBox::critical(this, "Atlas Creator",
			"Couldn't load atlas image file.");
		return;
	}

	for(rapidxml::xml_node<>* node = root->first_node();
		node != 0; node = node->next_sibling())
	{
		QString dest = getAttrib(node, "source");
		if(dest.isEmpty())
		{
			QMessageBox::critical(this, "Atlas Creator",
				"One of image doesn't have source attrib.");
			return;
		}
		dest = ns + "/" + dest;

		int xpos = getAttribInt(node, "xpos");
		int ypos = getAttribInt(node, "ypos");
		int width = getAttribInt(node, "width");
		int height = getAttribInt(node, "height");

		if(xpos < 0 || ypos < 0 || width < 0 || height < 0)
		{
			QMessageBox::critical(this, "Atlas Creator",
				"One of image doesn't have valid rectangle defined.");
			return;
		}

		QDir().mkpath(QFileInfo(dest).path());
		QImage subImage = atlas.copy(xpos, ypos, width, height);
		subImage.save(dest, "PNG");
	}
	QMessageBox::information(this, "Atlas Creator", "Disassembling done.");
}
