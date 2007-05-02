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

#ifndef FPANEPLUGIN_H
#define FPANEPLUGIN_H

#include <QDesignerCustomWidgetInterface>

class FPanePlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

	public:
		FPanePlugin(QObject *parent = 0);

		bool isContainer() const;
		bool isInitialized() const;
		QIcon icon() const;
		QString codeTemplate() const;
		QString domXml() const;
		QString group() const;
		QString includeFile() const;
		QString name() const;
		QString toolTip() const;
		QString whatsThis() const;
		QWidget *createWidget(QWidget *parent);
		void initialize(QDesignerFormEditorInterface *core);

	private:
		bool initialized;
};

#endif // FPANEPLUGIN_H
