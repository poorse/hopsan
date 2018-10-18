#include "MainWindow.h"
#include "FindWidget.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>

FindWidget::FindWidget(QWidget* parent) :
    QWidget(parent)
{
    QGridLayout *pLayout = new QGridLayout(this);

    QLabel *pFindLabel = new QLabel("Find:",this);
    mpFindLineEdit = new QLineEdit(this);
    QPushButton *pFindPreviousButton = new QPushButton("Find Previous",this);
    pFindPreviousButton->setShortcut(QKeySequence("Shift+F3"));
    QPushButton *pFindNextButton = new QPushButton("Find Next",this);
    pFindNextButton->setShortcut(QKeySequence("F3"));
    QToolButton *pCloseButton = new QToolButton(this);
    pCloseButton->setIcon(QIcon(":graphics/uiicons/Hopsan-Discard.png"));

    pLayout->addWidget(pFindLabel,0,0);
    pLayout->addWidget(mpFindLineEdit,0,1);
    pLayout->addWidget(pFindPreviousButton,0,2);
    pLayout->addWidget(pFindNextButton,0,3);
    pLayout->addWidget(pCloseButton,0,4);
    pLayout->setRowStretch(1,1);

    connect(pCloseButton, SIGNAL(clicked()),    this,   SLOT(hide()));
    connect(pFindPreviousButton, SIGNAL(clicked(bool)), this, SLOT(findPrevious()));
    connect(pFindNextButton, SIGNAL(clicked(bool)), this, SLOT(findNext()));
}

void FindWidget::setVisible(bool visible)
{
    mpFindLineEdit->setFocus();
    QWidget::setVisible(visible);
}

void FindWidget::findPrevious()
{
    emit findPrevious(mpFindLineEdit->text());
}

void FindWidget::findNext()
{
    emit findNext(mpFindLineEdit->text());
}
