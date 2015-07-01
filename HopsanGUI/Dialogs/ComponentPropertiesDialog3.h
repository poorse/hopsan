/*-----------------------------------------------------------------------------
 This source file is a part of Hopsan

 Copyright (c) 2009 to present year, Hopsan Group

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 For license details and information about the Hopsan Group see the files
 GPLv3 and HOPSANGROUP in the Hopsan source code root directory

 For author and contributor information see the AUTHORS file
-----------------------------------------------------------------------------*/

#ifndef COMPONENTPROPERTIESDIALOG3_H
#define COMPONENTPROPERTIESDIALOG3_H

#include <QTableWidget>
#include <QDialog>
#include <QGridLayout>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QTextEdit>
#include <QComboBox>
#include <QEvent>
#include <QCheckBox>
#include <QLabel>
#include "CoreAccess.h"
#include "UnitScale.h"

class ModelObject;
class ParameterSettingsLayout;
class SystemParametersWidget;


class TableWidgetTotalSize : public QTableWidget
{
public:
    TableWidgetTotalSize(QWidget *pParent=0);
    QSize sizeHint() const;
    void setMaxVisibleRows(const int maxRows);
private:
    int mMaxVisibleRows;
};

class VariableTableWidget :public TableWidgetTotalSize
{
    Q_OBJECT
public:
    enum VariameterTypEnumT {InputVaraiable, OutputVariable, OtherVariable, Constant}; //!< @todo maybe not only here
    enum ColumnEnumT {Name, Alias, Description, Unit, Value, Quantity, ShowPort, NumCols};
    VariableTableWidget(ModelObject *pModelObject, QWidget *pParent);
    bool setStartValues();
    //bool setCustomPlotScaleValues();
    bool setAliasNames();

protected:
    virtual bool focusNextPrevChild(bool next);

private:
    void createTableRow(const int row, const CoreVariameterDescription &rData, const VariameterTypEnumT variametertype);
    void createSeparatorRow(const int row, const QString &rName, const VariameterTypEnumT variametertype);
    void selectValueTextColor(const int row);
    bool cleanAndVerifyParameterValue(QString &rValue, const QString type);
    bool setAliasName(const int row);
    ModelObject *mpModelObject;
};

class ComponentPropertiesDialog3 : public QDialog
{
    Q_OBJECT

public:
    ComponentPropertiesDialog3(ModelObject *pModelObject, QWidget *pParent=0);

signals:
    void lockModelWidget(bool);

protected slots:
    void okPressed();
    void editPortPos();
    void copyToNewComponent();
    void recompile();
    virtual void reject();
    void openDescription();

protected:
    bool setAliasNames();
    bool setVariableValues();
    void setName();
    virtual void closeEvent(QCloseEvent* event);

private:
    QGridLayout* createNameAndTypeEdit();
    QDialogButtonBox* createButtonBox();
    QWidget* createHelpWidget();
    QWidget* createSourcodeBrowser(QString &rFilePath);
    void createEditStuff();

    ModelObject *mpModelObject;
    LibraryWidget *mpLibrary;
    SystemParametersWidget *mpSystemParametersWidget;
    QLineEdit *mpNameEdit;
    VariableTableWidget *mpVariableTableWidget;

    QSpinBox *mpInputPortsSpinBox;
    QSpinBox *mpOutputPortsSpinBox;
    QTextEdit *mpTextEdit;
    QPushButton *mpNewComponentButton;
    QPushButton *mpRecompileButton;
    QTextEdit *mpSourceCodeTextEdit;
    QComboBox *mpSolverComboBox;
};

// Help class declarations
//----------------------------------------------------------------------

//class PlotScaleSelectionWidget : public QWidget
//{
//    Q_OBJECT
//public:
//    PlotScaleSelectionWidget(const CoreVariameterDescription &rData, ModelObject *pModelObject, QWidget *pParent);
//    void registerCustomScale();
//    bool hasChanged() const;
//    QLineEdit *getPlotScaleEditPtr() const;

//private slots:
//    void createPlotScaleSelectionMenu();

//private:
//    QLineEdit *mpPlotScaleEdit;
//    ModelObject *mpModelObject;
//    QString mVariableTypeName, mVariablePortDataName, mOriginalUnit;
//};

class QuantitySelectionWidget : public QWidget
{
    Q_OBJECT
public:
    QuantitySelectionWidget(const CoreVariameterDescription &rData, ModelObject *pModelObject, QWidget *pParent);
    void registerCustomQuantity();
    bool hasChanged() const;
    //QLineEdit *getQuantityLabel() const;

private slots:
    void createQuantitySelectionMenu();

private:
    QLabel *mpQuantityLabel;
    ModelObject *mpModelObject;
    QString mVariableTypeName, mVariablePortDataName, mOriginalUnit, mQuantity;
};

//! @todo maybe should have eventfilters in a specific file
class MouseWheelEventEater : public QObject
{
    Q_OBJECT
public:
    MouseWheelEventEater(QWidget *pParent) : QObject(pParent)
    {
        // Nothing in particular
    }
protected:
    bool eventFilter(QObject *pObj, QEvent *pEvent)
    {
        if (pEvent->type() == QEvent::Wheel)
        {
            //QWheelEvent *pWheelEvent = static_cast<QKeyEvent *>(pEvent);
            return true;
        }
        else
        {
            // standard event processing
            return QObject::eventFilter(pObj, pEvent);
        }
    }
};

class UnitSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    UnitSelectionWidget(const QString &rQuantity, QWidget *pParent);
    void setUnitScaling(const UnitScale &rUs);
    QString getSelectedUnit() const;
    double getSelectedUnitScale() const;
    void getSelectedUnitScale(UnitScale &rUnitScale) const;
    bool isDefaultSelected() const;

public slots:
    void resetDefault();

signals:
    void unitChanged(const UnitScale &rUnitScale);

private slots:
    void selectionChanged(int idx);

private:
    QComboBox *mpUnitComboBox;
    QList<UnitScale> mUnitScales;
    QString mQuantity;
    QString mBaseUnit;
    int mBaseUnitIndex;
};

class ParameterValueSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    ParameterValueSelectionWidget(const CoreVariameterDescription &rData, VariableTableWidget::VariameterTypEnumT type, ModelObject *pModelObject, QWidget *pParent);
    void setValueText(const QString &rText);
    QString getValueText() const;
    const QString &getDataType() const;
    const QString &getName() const;
    UnitSelectionWidget *getUnitSelectionWidget();
    bool isValueDisabled() const;
    QLineEdit *getValueEditPtr() const;
    void setValueAndScale_nosignals(QString value, UnitScale &rCustomUS);

public slots:
    void refreshValueTextStyle();
    void rescaleByUnitScale(const UnitScale &rUnitScale);
    bool checkIfSysParEntered();

private slots:
    void setValue();
    void setConditionalValue(const int idx);
    void resetDefault();
    void createSysParameterSelectionMenu();
private:
    QString mVariableDataType, mVariablePortDataName, mVariablePortName;
    QLineEdit *mpValueEdit;
    QComboBox *mpConditionalValueComboBox;
    UnitSelectionWidget *mpUnitSelectionWidget;
    VariableTableWidget::VariameterTypEnumT mVariameterType;
    ModelObject *mpModelObject;
    UnitScale mCustomScale;
    UnitScale mDefaultUnitScale;
    void setDefaultValueTextStyle();
    void decideBackgroundColor(QString &rStyleString);
};

class HideShowPortWidget : public QWidget
{
    Q_OBJECT
public:
    HideShowPortWidget(const CoreVariameterDescription &rData, ModelObject *pModelObject, QWidget *pParent);
    QCheckBox *getCheckBoxPtr() const;
signals:
    void toggled(bool);
private slots:
    void hideShowPort(const bool doShow);
private:
    QCheckBox *mpCheckBox;
    QString mPortName;
    ModelObject *mpModelObject;
};

//----------------------------------------------------------------------

#endif // COMPONENTPROPERTIESDIALOG3_H
