/****************************************************************************
**
** Copyright (C) 2005-2006 Trolltech AS. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "fpane.h"
#include "fpaneplugin.h"

#include <QtPlugin>

FPanePlugin::FPanePlugin(QObject *parent) : QObject(parent) {
    initialized = false;
}

void FPanePlugin::initialize(QDesignerFormEditorInterface * /* core */) {
    initialized = true;
}

bool FPanePlugin::isInitialized() const {
    return initialized;
}

QWidget *FPanePlugin::createWidget(QWidget *parent) {
	FIFEDIT::FPane *fpane = new FIFEDIT::FPane(parent);
	fpane->setInDesigner( true ); // Disable some buggy behaviour
	// TODO: Enable all features when in form preview mode, and disable some when in design mode
	return fpane;
}

QString FPanePlugin::name() const {
    return "FIFEDIT::FPane";
}

QString FPanePlugin::group() const {
    return "FIFEdit widgets";
}

QIcon FPanePlugin::icon() const {
    return QIcon();
}

QString FPanePlugin::toolTip() const {
    return "FPane";
}

QString FPanePlugin::whatsThis() const {
    return "FPane";
}

bool FPanePlugin::isContainer() const {
    return true;
}

QString FPanePlugin::domXml() const {
    return	"<widget class=\"FIFEDIT::FPane\" name=\"fpane\">\n"
			"	<property name=\"geometry\">\n"
			"		<rect>\n"
			"			<x>0</x>\n"
			"			<y>0</y>\n"
			"			<width>100</width>\n"
			"			<height>100</height>\n"
			"		</rect>\n"
			"	</property>\n"
			"</widget>\n";
}

QString FPanePlugin::includeFile() const {
    return "fpane.h";
}

QString FPanePlugin::codeTemplate() const {
    return "";
}

Q_EXPORT_PLUGIN2(customwidgetplugin, FPanePlugin)
