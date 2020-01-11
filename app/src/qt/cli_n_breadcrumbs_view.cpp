/*
 cli_n_breadcrumbs_view.cpp     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "cli_n_breadcrumbs_view.h"

namespace m8r {

CliView::CliView(CliAndBreadcrumbsView* cliAndBreadcrumps, QWidget* parent)
    : QLineEdit(parent)
{
    this->cliAndBreadcrumps = cliAndBreadcrumps;
    QPalette *palette = new QPalette();
    palette->setColor(QPalette::Text, LookAndFeels::getInstance().getCliTextColor());
    setPalette(*palette);
}

void CliView::keyPressEvent(QKeyEvent* event)
{
    switch(event->key()) {
    case Qt::Key_Escape:
        cliAndBreadcrumps->showBreadcrumb();
        break;
    default:
        break;
    }

    QLineEdit::keyPressEvent(event);
}

/*
 * CliAndBreadcrumbsView
 */

const QString CliAndBreadcrumbsView::CMD_FTS = ".fts ";
const QString CliAndBreadcrumbsView::CMD_FIND_OUTLINE_BY_NAME = ".find outline by name ";
const QString CliAndBreadcrumbsView::CMD_LIST_OUTLINES = ".list outlines";

// TODO migrate all commands to constants
const QStringList CliAndBreadcrumbsView::DEFAULT_CMDS = QStringList()
        /*
        << CMD_HELP
        << CMD_EXIT
        // home tools
        //<< "home"
        //<< "navigator"
        // outlines
        << "list outlines"
        << CMD_FIND_OUTLINE_BY_NAME
        << "show outline" // "abc"
        << "edit outline" // "abc"
        // << "[outline "abc"] list associations
        // notes
        << "list notes"
        << "find note by name " // "def"
        << "show note " // "abc"."def"
        // << "[outline "abc"]  show note "efg"
        // << "edit note "abc"."efg"
        // << "[outline "abc"] list notes
        // << "[note "abc"."efg"] list associations

        << "fts " // "expr"
        // << "[outline "abc"] fts "expr"
        // << "[note "abc"."efg"] fts "expr"

        << "forget 25%";

        // TODO new outline
        // TODO new note
        */
        << CMD_FIND_OUTLINE_BY_NAME
        << CMD_FTS
        << CMD_LIST_OUTLINES
        ;


CliAndBreadcrumbsView::CliAndBreadcrumbsView(QWidget* parent, bool zenMode)
    : QWidget(parent),
      zenMode{zenMode}
{
    setFixedHeight(this->fontMetrics().height()*1.5);

    QHBoxLayout* layout = new QHBoxLayout(this);
    // ensure that wont be extra space around member widgets
    layout->setContentsMargins(QMargins(0, 0, 0, 0));
    setLayout(layout);

    breadcrumbsLabel = new QLabel(this);
    breadcrumbsLabel->setText("$");
    if(zenMode) {
        breadcrumbsLabel->hide();
    }
    layout->addWidget(breadcrumbsLabel);

    cli = new CliView(this, parent);
    cliCompleter = new QCompleter(DEFAULT_CMDS, parent);
    cliCompleter->setCaseSensitivity(Qt::CaseSensitivity::CaseInsensitive);
    cliCompleter->setCompletionMode(QCompleter::PopupCompletion);
    cli->setCompleter(cliCompleter);
    layout->addWidget(cli);

    showBreadcrumb();
}

void CliAndBreadcrumbsView::updateCompleterModel(const QStringList* list)
{
    QStringListModel* completerModel=(QStringListModel*)cliCompleter->model();
    if(completerModel==nullptr) {
        completerModel = new QStringListModel();
    }

    QStringList completerItems{};
    completerItems.append(cliCompleterHistoryList);
    if(list==nullptr) {
        completerItems.append(DEFAULT_CMDS);
    } else {
        completerItems.append(*list);
    }

    completerModel->setStringList(completerItems);
}

void CliAndBreadcrumbsView::forceFtsHistoryCompletion()
{
    updateCompleterModel();

    // ensure completion is shown despite there is NO filtering character
    cliCompleter->complete();
}

QString CliAndBreadcrumbsView::getFirstCompletion() const
{
    if(cliCompleter->completionCount()) {
        return cliCompleter->currentCompletion();
    } else {
        return QString::null;
    }
}

void CliAndBreadcrumbsView::setBreadcrumbPath(const QString& path)
{
    if(zenMode) {
        breadcrumbsLabel->setText("");
    } else {
        breadcrumbsLabel->setText(path);
    }
}

void CliAndBreadcrumbsView::setCommand(const char* command)
{
    cli->setText(command);
}

const QString CliAndBreadcrumbsView::getCommand() const
{
    return cli->text();
}

void CliAndBreadcrumbsView::show()
{
    if(!zenMode) {
        breadcrumbsLabel->show();
    }
    cli->show();
    cliCompleter->complete();
}

void CliAndBreadcrumbsView::hide()
{
    if(!zenMode) {
        breadcrumbsLabel->hide();
        cli->hide();
    }
}

void CliAndBreadcrumbsView::showBreadcrumb()
{
    if(!zenMode) {
        breadcrumbsLabel->show();
        cli->hide();
    }
}

void CliAndBreadcrumbsView::showCli(bool selectAll)
{
    show();
    cli->setFocus();
    if(selectAll) {
        cli->selectAll();
    }

    cliCompleter->complete();
}

} // m8r namespace
