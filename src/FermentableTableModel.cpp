/*
 * FermentableTableModel.cpp is part of Brewtarget, and is Copyright Philip G. Lee
 * (rocketman768@gmail.com), 2009.
 *
 * Brewtarget is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Brewtarget is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QAbstractTableModel>
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QWidget>
#include <QModelIndex>
#include <QVariant>
#include <QCheckBox>
#include <QItemEditorFactory>
#include <QStyle>
#include <QRect>

#include "brewtarget.h"
#include <Qt>
#include <QSize>
#include <QComboBox>
#include <QLineEdit>

#include <vector>
#include <QString>
#include <vector>
#include <iostream>
#include "observable.h"
#include "fermentable.h"
#include "FermentableTableModel.h"
#include "unit.h"

//=====================CLASS FermentableTableModel==============================
FermentableTableModel::FermentableTableModel(FermentableTableWidget* parent)
: QAbstractTableModel(parent), MultipleObserver()
{
   fermObs.clear();
   parentTableWidget = parent;
   
   totalFermMass_kg = 0;
   displayPercentages = false;
}

void FermentableTableModel::addFermentable(Fermentable* ferm)
{
   std::vector<Fermentable*>::iterator iter;
   
   //Check to see if it's already in the list
   for( iter=fermObs.begin(); iter != fermObs.end(); iter++ )
      if( *iter == ferm )
         return;
   
   fermObs.push_back(ferm);
   addObserved(ferm);
   totalFermMass_kg += ferm->getAmount_kg();
   reset(); // Tell everybody that the table has changed.
   
   if(parentTableWidget)
   {
      parentTableWidget->resizeColumnsToContents();
      parentTableWidget->resizeRowsToContents();
   }
}

bool FermentableTableModel::removeFermentable(Fermentable* ferm)
{
   std::vector<Fermentable*>::iterator iter;
   
   for( iter=fermObs.begin(); iter != fermObs.end(); iter++ )
      if( *iter == ferm )
      {
         fermObs.erase(iter);
         removeObserved(ferm);
	 totalFermMass_kg -= ferm->getAmount_kg();
         reset(); // Tell everybody the table has changed.
         
         if(parentTableWidget)
         {
            parentTableWidget->resizeColumnsToContents();
            parentTableWidget->resizeRowsToContents();
         }
         
         return true;
      }
   
   return false;
}

void FermentableTableModel::removeAll()
{
   unsigned int i;

   for( i = 0; i < fermObs.size(); ++i )
      removeObserved(fermObs[i]);

   fermObs.clear();
   totalFermMass_kg = 0;
   reset();
}

void FermentableTableModel::updateTotalGrains()
{
   unsigned int i, size;
   
   totalFermMass_kg = 0;
   
   size = fermObs.size();
   for( i = 0; i < size; ++i )
      totalFermMass_kg += fermObs[i]->getAmount_kg();
}

void FermentableTableModel::setDisplayPercentages(bool var)
{
   displayPercentages = var;
}


void FermentableTableModel::notify(Observable* notifier, QVariant info)
{
   int i;
   
   // Find the notifier in the list
   for( i = 0; i < (int)fermObs.size(); ++i )
   {
      if( notifier == fermObs[i] )
      {
         /*
         emit dataChanged( QAbstractItemModel::createIndex(i, 0),
                           QAbstractItemModel::createIndex(i, FERMNUMCOLS));
          */
	 updateTotalGrains();
         reset();
         break;
      }
   }
}

int FermentableTableModel::rowCount(const QModelIndex& /*parent*/) const
{
   return fermObs.size();
}

int FermentableTableModel::columnCount(const QModelIndex& /*parent*/) const
{
   return FERMNUMCOLS;
}

QVariant FermentableTableModel::data( const QModelIndex& index, int role ) const
{
   Fermentable* row;
   
   // Ensure the row is ok.
   if( index.row() >= (int)fermObs.size() )
   {
      std::cerr << "Bad model index. row = " << index.row() << std::endl;
      return QVariant();
   }
   else
      row = fermObs[index.row()];
   
   switch( index.column() )
   {
      case FERMNAMECOL:
	 if( role == Qt::DisplayRole )
	    return QVariant(row->getName().c_str());
	 else
	    return QVariant();
      case FERMTYPECOL:
	 if( role == Qt::DisplayRole )
	    return QVariant(row->getType().c_str());
	 else
	    return QVariant();
      case FERMAMOUNTCOL:
	 if( role == Qt::DisplayRole )
	    return QVariant( Brewtarget::displayAmount(row->getAmount_kg(), Units::kilograms) );
	 else
	    return QVariant();
      case FERMISMASHEDCOL:
	 if( role == Qt::CheckStateRole )
	    return QVariant( row->getIsMashed() ? Qt::Checked : Qt::Unchecked);
	 else if( role == Qt::DisplayRole )
	    return row->getIsMashed() ? QString("Mashed") : QString ("Not mashed");
	 else
	    return QVariant();
      case FERMYIELDCOL:
	 if( role == Qt::DisplayRole )
	    return QVariant( Brewtarget::displayAmount(row->getYield_pct(), 0) );
	 else
	    return QVariant();
      case FERMCOLORCOL:
	 if( role == Qt::DisplayRole )
	    return QVariant( Brewtarget::displayAmount(row->getColor_srm(), 0) );
	 else
	    return QVariant();
      default :
         std::cerr << "Bad column: " << index.column() << std::endl;
         return QVariant();
   }
}

QVariant FermentableTableModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
   if( orientation == Qt::Horizontal && role == Qt::DisplayRole )
   {
      switch( section )
      {
         case FERMNAMECOL:
            return QVariant("Name");
         case FERMTYPECOL:
            return QVariant("Type");
         case FERMAMOUNTCOL:
            return QVariant("Amount");
	 case FERMISMASHEDCOL:
	    return QVariant("Mashed");
         case FERMYIELDCOL:
            return QVariant("Yield %");
         case FERMCOLORCOL:
            return QVariant("Color (SRM)");
         default:
            std::cerr << "Bad column: " << section << std::endl;
            return QVariant();
      }
   }
   else if( displayPercentages && orientation == Qt::Vertical && role == Qt::DisplayRole )
   {
      return QVariant( QString("%1\%").arg( (double)100.0 *fermObs[section]->getAmount_kg()/totalFermMass_kg, 0, 'f', 0 ) );
   }
   else
      return QVariant();
}

Qt::ItemFlags FermentableTableModel::flags(const QModelIndex& index ) const
{
   Qt::ItemFlags defaults = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
   
   if( index.column()  == FERMISMASHEDCOL )
      return (defaults | Qt::ItemIsUserCheckable);
   else
      return (defaults | Qt::ItemIsEditable);
}

bool FermentableTableModel::setData( const QModelIndex& index, const QVariant& value, int role )
{
   Fermentable* row;
   
   if( index.row() >= (int)fermObs.size() )
   {
      return false;
   }
   else
      row = fermObs[index.row()];
   
   switch( index.column() )
   {
      case FERMNAMECOL:
         if( value.canConvert(QVariant::String))
         {
            row->setName(value.toString().toStdString());
            return true;
         }
         else
            return false;
      case FERMTYPECOL:
         if( value.canConvert(QVariant::String) )
         {
            row->setType(value.toString().toStdString());
            return true;
         }
         else
            return false;
      case FERMAMOUNTCOL:
         if( value.canConvert(QVariant::String) )
         {
            row->setAmount_kg( Unit::qstringToSI(value.toString()) );
            return true;
         }
         else
            return false;
      case FERMISMASHEDCOL:
	 if( role == Qt::CheckStateRole && value.canConvert(QVariant::Int) )
	 {
	    row->setIsMashed( ((Qt::CheckState)value.toInt()) == Qt::Checked );
	    return true;
	 }
	 else
	    return false;
      case FERMYIELDCOL:
         if( value.canConvert(QVariant::String) )
         {
            row->setYield_pct( Unit::qstringToSI(value.toString()) );
            return true;
         }
         else
            return false;
      case FERMCOLORCOL:
         if( value.canConvert(QVariant::String) )
         {
            row->setColor_srm( Unit::qstringToSI(value.toString()) );
            return true;
         }
         else
            return false;
      default:
         std::cerr << "Bad column: " << index.column() << std::endl;
         return false;
   }
}

Fermentable* FermentableTableModel::getFermentable(unsigned int i)
{
   return fermObs[i];
}

//======================CLASS FermentableItemDelegate===========================

FermentableItemDelegate::FermentableItemDelegate(QObject* parent)
        : QStyledItemDelegate(parent)
{
   //connect( this, SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)), this, SLOT(destroyWidget(QWidget*, QAbstractItemDelegate::EndEditHint)) );
}

void FermentableItemDelegate::destroyWidget(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
{
   delete widget;
}


QWidget* FermentableItemDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
   if( index.column() == FERMTYPECOL )
   {
      QComboBox *box = new QComboBox(parent);

      box->addItem("Grain");
      box->addItem("Sugar");
      box->addItem("Extract");
      box->addItem("Dry Extract");
      box->addItem("Adjunct");
      box->setSizeAdjustPolicy(QComboBox::AdjustToContents);
      return box;
   }
   else if( index.column() == FERMISMASHEDCOL )
   {
      QCheckBox* box = new QCheckBox(parent);
      box->setFocusPolicy(Qt::StrongFocus);
      box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

      /*
      QWidget* displayWidget = (((FermentableTableModel*)(index.model()))->parentTableWidget)->indexWidget(index);
      if( displayWidget != 0 )
	 box->move(displayWidget->pos());
      ***Didn't work at all***/
      
      /*
      QRect rect = QStyle::alignedRect(Qt::LeftToRight, Qt::AlignLeft, box->sizeHint(), option.rect);
      std::cerr << "option.rect " << option.rect.x() << " " << option.rect.y() << " " << option.rect.width() << " " << option.rect.height() << std::endl;
      std::cerr << "rect " << rect.x() << " " << rect.y() << " " << rect.width() << " " << rect.height() << std::endl;
      box->move(rect.topRight());
      ***Didn't really do much either***/
      
      return box;

      //return QItemDelegate::createEditor(parent, option, index);
      //return itemEditorFactory()->createEditor(QVariant::Bool, parent);
   }
   else
      return new QLineEdit(parent);
}

void FermentableItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
   if( index.column() == FERMTYPECOL )
   {
      QComboBox* box = (QComboBox*)editor;
      QString text = index.model()->data(index, Qt::DisplayRole).toString();
      
      int index = box->findText(text);
      box->setCurrentIndex(index);
   }
   else if( index.column() == FERMISMASHEDCOL )
   {
      QCheckBox* checkBox = (QCheckBox*)editor;
      Qt::CheckState checkState = (Qt::CheckState)index.model()->data(index, Qt::CheckStateRole).toInt();
      
      checkBox->setCheckState( checkState );
   }
   else
   {
      QLineEdit* line = (QLineEdit*)editor;
      
      line->setText(index.model()->data(index, Qt::DisplayRole).toString());
   }
   
}

void FermentableItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
   if( index.column() == FERMTYPECOL )
   {
      QComboBox* box = (QComboBox*)editor;
      QString value = box->currentText();
      
      model->setData(index, value, Qt::EditRole);
   }
   else if( index.column() == FERMISMASHEDCOL )
   {
      QCheckBox* checkBox = (QCheckBox*)editor;
      bool checked = (checkBox->checkState() == Qt::Checked);
      
      model->setData(index, checked, Qt::EditRole);
   }
   else
   {
      QLineEdit* line = (QLineEdit*)editor;
      
      model->setData(index, line->text(), Qt::EditRole);
   }
}

/*
void FermentableItemDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
   if( index.column() == FERMISMASHEDCOL )
   {
      QItemDelegate::drawCheck(painter, option, option.rect, (Qt::CheckState)index.model()->data(index, Qt::CheckStateRole).toInt() );
   }
   else
   {
      QString str = index.model()->data(index, Qt::DisplayRole).toString();
      QItemDelegate::drawDisplay(painter, option, option.rect, str);
   }
}
*/

void FermentableItemDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   editor->setGeometry(option.rect);
}