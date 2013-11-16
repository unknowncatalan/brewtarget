/*
 * btTreeItem.cpp is part of Brewtarget and was written by Mik
 * Firestone (mikfire@gmail.com).  Copyright is granted to Philip G. Lee
 * (rocketman768@gmail.com), 2009-2013.
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

#include <QString>
#include <QModelIndex>
#include <QVariant>
#include <Qt>
#include <QDateTime>
#include <QString>
#include <QObject>
#include <QVector>

#include "btTreeItem.h"
#include "brewnote.h"
#include "brewtarget.h"
#include "equipment.h"
#include "fermentable.h"
#include "hop.h"
#include "recipe.h"
#include "misc.h"
#include "yeast.h"
#include "style.h"
#include "btFolder.h"

bool operator==(btTreeItem& lhs, btTreeItem& rhs)
{
   // Things of different types are not equal
   if ( lhs._type != rhs._type )
      return false;

   return lhs.data(lhs._type,0) == rhs.data(rhs._type,0);
}

btTreeItem::btTreeItem(int _type, btTreeItem *parent)
   : parentItem(parent), thing(0)
{
   setType(_type);
}

btTreeItem::~btTreeItem()
{
   qDeleteAll(childItems);
}

btTreeItem* btTreeItem::child(int number)
{
   if ( number < childItems.count() )
      return childItems.value(number);

   return 0;
}

btTreeItem* btTreeItem::parent()
{
   return parentItem;
}

int btTreeItem::type()
{
    return _type;
}

int btTreeItem::childCount() const
{
   return childItems.count();
}

int btTreeItem::columnCount(int _type) const
{
    switch(_type)
    {
        case RECIPE:
            return RECIPENUMCOLS;
        case EQUIPMENT:
            return EQUIPMENTNUMCOLS;
        case FERMENTABLE:
            return FERMENTABLENUMCOLS;
        case HOP:
            return HOPNUMCOLS;
        case MISC:
            return MISCNUMCOLS;
        case YEAST:
            return YEASTNUMCOLS;
        case STYLE:
            return STYLENUMCOLS;
        case BREWNOTE:
            return BREWNUMCOLS;
        case FOLDER:
            return FOLDERNUMCOLS;
        default:
         Brewtarget::log(Brewtarget::WARNING, QString("btTreeItem::columnCount Bad column: %1").arg(_type));
            return 0;
    }
            
}

QVariant btTreeItem::data(int _type, int column)
{

   switch(_type)
   {
      case RECIPE:
         return dataRecipe(column);
      case EQUIPMENT:
         return dataEquipment(column);
      case FERMENTABLE:
         return dataFermentable(column);
      case HOP:
         return dataHop(column);
      case MISC:
         return dataMisc(column);
      case YEAST:
         return dataYeast(column);
      case STYLE:
         return dataStyle(column);
      case BREWNOTE:
         return dataBrewNote(column);
      case FOLDER:
         return dataFolder(column);
      default:
         Brewtarget::log(Brewtarget::WARNING, QString("btTreeItem::data Bad column: %1").arg(column));
         return QVariant();
    }
}

int btTreeItem::childNumber() const
{
   if (parentItem)
      return parentItem->childItems.indexOf(const_cast<btTreeItem*>(this));
   return 0;
}

void btTreeItem::setData(int t, QObject* d)
{
   thing = d;
   _type  = t;
}

QVariant btTreeItem::getData(int column)
{
   return data(type(),column);
}

bool btTreeItem::insertChildren(int position, int count, int _type)
{
   int i;
   if ( position < 0  || position > childItems.size())
      return false;

   for(i=0; i < count; ++i)
   {
      btTreeItem *newItem = new btTreeItem(_type,this);
      childItems.insert(position+i,newItem);
   }

   return true;
}

bool btTreeItem::removeChildren(int position, int count)
{
   if ( position < 0 || position + count > childItems.count() )
      return false;

   for (int row = 0; row < count; ++row)
      //delete childItems.takeAt(position);
      // FIXME: memory leak here. With delete, it's a concurrency/memory
      // access error, due to the fact that these pointers are floating around.
      childItems.takeAt(position);

   return true;
}

QVariant btTreeItem::dataRecipe( int column ) 
{
   Recipe* recipe = qobject_cast<Recipe*>(thing);
   switch(column)
   {
        case RECIPENAMECOL:
         if (! thing)
            return QVariant(QObject::tr("Recipes"));
        else
            return QVariant(recipe->name());
         break;
        case RECIPEBREWDATECOL:
         if ( recipe )
            return QVariant(recipe->date());
         break;
        case RECIPESTYLECOL:
         if ( recipe && recipe->style() )
            return QVariant(recipe->style()->name());
         break;
      default :
         Brewtarget::log(Brewtarget::WARNING, QString("btTreeItem::dataRecipe Bad column: %1").arg(column));
   }
   return QVariant();
}

QVariant btTreeItem::dataEquipment(int column) 
{
   Equipment* kit = qobject_cast<Equipment*>(thing);
   switch(column)
   {
        case EQUIPMENTNAMECOL:
         if ( ! kit )
            return QVariant(QObject::tr("Equipment"));
         else
            return QVariant(kit->name());
        case EQUIPMENTBOILTIMECOL:
         if ( kit )
            return QVariant(kit->boilTime_min());
         break;
      default :
         Brewtarget::log(Brewtarget::WARNING, QString("btTreeItem::dataEquipment Bad column: %1").arg(column));
   }
   return QVariant();
}

QVariant btTreeItem::dataFermentable(int column)
{
    Fermentable* ferm = qobject_cast<Fermentable*>(thing);
   switch(column)
   {
        case FERMENTABLENAMECOL:
         if ( ferm )
            return QVariant(ferm->name());
         else
            return QVariant(QObject::tr("Fermentables"));
        case FERMENTABLETYPECOL:
         if ( ferm )
            return QVariant(ferm->typeStringTr());
         break;
        case FERMENTABLECOLORCOL:
         if ( ferm )
            return QVariant(ferm->color_srm());
         break;
      default :
         Brewtarget::log(Brewtarget::WARNING, QString("btTreeItem::dataFermentable Bad column: %1").arg(column));
   }
   return QVariant();
}

QVariant btTreeItem::dataHop(int column)
{
    Hop* hop = qobject_cast<Hop*>(thing);
   switch(column)
   {
      case HOPNAMECOL:
         if ( ! hop )
            return QVariant(QObject::tr("Hops"));
         else
            return QVariant(hop->name());
      case HOPFORMCOL:
         if ( hop )
            return QVariant(hop->formStringTr());
         break;
      case HOPUSECOL:
         if ( hop )
            return QVariant(hop->useStringTr());
         break;
      default :
         Brewtarget::log(Brewtarget::WARNING, QString("btTreeItem::dataHop Bad column: %1").arg(column));
   }
   return QVariant();
}

QVariant btTreeItem::dataMisc(int column)
{
    Misc* misc = qobject_cast<Misc*>(thing);
   switch(column)
   {
      case MISCNAMECOL:
         if ( ! misc )
            return QVariant(QObject::tr("Miscellaneous"));
         else
            return QVariant(misc->name());
      case MISCTYPECOL:
         if ( misc )
            return QVariant(misc->typeStringTr());
         break;
      case MISCUSECOL:
         if ( misc )
            return QVariant(misc->useStringTr());
         break;
      default :
         Brewtarget::log(Brewtarget::WARNING, QString("btTreeItem::dataMisc Bad column: %1").arg(column));
   }
   return QVariant();
}

QVariant btTreeItem::dataYeast(int column)
{
   Yeast* yeast = qobject_cast<Yeast*>(thing);
   switch(column)
   {
      case YEASTNAMECOL:
         if ( ! yeast )
            return QVariant(QObject::tr("Yeast"));
         else
            return QVariant(yeast->name());
      case YEASTTYPECOL:
         if ( yeast )
            return QVariant(yeast->typeStringTr());
         break;
      case YEASTFORMCOL:
         if ( yeast )
            return QVariant(yeast->formStringTr());
         break;
      default :
         Brewtarget::log(Brewtarget::WARNING, QString("btTreeItem::dataYeast Bad column: %1").arg(column));
   }
   return QVariant();
}

QVariant btTreeItem::dataBrewNote(int column)
{
   if ( ! thing )
      return QVariant();

   BrewNote* bNote = qobject_cast<BrewNote*>(thing);

   return bNote->brewDate_short();
}

QVariant btTreeItem::dataStyle(int column)
{
   Style* style = qobject_cast<Style*>(thing);

   if ( ! style && column == STYLENAMECOL )
   {
      return QVariant(QObject::tr("Style"));
   }
   else if ( style )
   {
      switch(column)
      {
         case STYLENAMECOL:
               return QVariant(style->name());
         case STYLECATEGORYCOL:
            return QVariant(style->category());
         case STYLENUMBERCOL:
               return QVariant(style->categoryNumber());
         case STYLELETTERCOL:
               return QVariant(style->styleLetter());
         case STYLEGUIDECOL:
               return QVariant(style->styleGuide());
         default :
            Brewtarget::log(Brewtarget::WARNING, QString("btTreeItem::dataStyle Bad column: %1").arg(column));
      }
   }
   return QVariant();
}

QVariant btTreeItem::dataFolder(int column)
{
   btFolder* folder = qobject_cast<btFolder*>(thing);


   if ( ! folder && column == FOLDERNAMECOL )
      return QVariant(QObject::tr("Folder"));

   if ( ! folder ) {
      return QVariant(QObject::tr("Folder"));
   }
   else 
   {
      switch(column)
      {
         case FOLDERNAMECOL:
            return QVariant( folder->name() );
         case FOLDERPATHCOL:
            return QVariant( folder->path() );
         case FOLDERFULLCOL:
            return QVariant( folder->fullPath() );
         default :
            Brewtarget::log(Brewtarget::WARNING, QString("btTreeItem::dataFolder Bad column: %1").arg(column));
      }
   }
   return QVariant();
}

void btTreeItem::setType(int t)
{
    _type = t;
}

Recipe* btTreeItem::getRecipe()
{
    if ( _type == RECIPE && thing )
        return qobject_cast<Recipe*>(thing);

    return 0;
}

Equipment* btTreeItem::getEquipment()
{
    if ( _type == EQUIPMENT )
       return qobject_cast<Equipment*>(thing);
    return 0;
}

Fermentable* btTreeItem::getFermentable()
{
    if ( _type == FERMENTABLE )
       return qobject_cast<Fermentable*>(thing);
    return 0;
}

Hop* btTreeItem::getHop()
{
    if ( _type == HOP ) 
       return qobject_cast<Hop*>(thing);
    return 0;
}

Misc* btTreeItem::getMisc()
{
    if ( _type == MISC ) 
       return qobject_cast<Misc*>(thing);
    return 0;
}

Yeast* btTreeItem::getYeast()
{
    if ( _type == YEAST ) 
       return qobject_cast<Yeast*>(thing);
    return 0;
}

BrewNote* btTreeItem::getBrewNote()
{
    if ( _type == BREWNOTE && thing ) 
       return qobject_cast<BrewNote*>(thing);

    return 0;
}

Style* btTreeItem::getStyle()
{
    if ( _type == STYLE && thing ) 
       return qobject_cast<Style*>(thing);

    return 0;
}

btFolder* btTreeItem::getFolder()
{
    if ( _type == FOLDER && thing ) 
       return qobject_cast<btFolder*>(thing);

    return 0;
}

BeerXMLElement* btTreeItem::getThing()
{
    if ( thing )
        return qobject_cast<BeerXMLElement*>(thing);

    return 0;
}

