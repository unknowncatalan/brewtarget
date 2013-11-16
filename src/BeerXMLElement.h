/*
 * BeerXMLElement.h is part of Brewtarget, and is Copyright Philip G. Lee
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

#ifndef _BEERXMLELEMENT_H
#define _BEERXMLELEMENT_H

#include <QDomText>
#include <QDomNode>
#include <QDomDocument>
#include <QString>
#include <QObject>
#include <QMetaProperty>
#include <QVariant>
#include <QDateTime>
#include "brewtarget.h"
// For uintptr_t.
#if HAVE_STDINT_H
#   include <stdint.h>
#else
#   include "pstdint.h"
#endif

// Make uintptr_t available in QVariant.
Q_DECLARE_METATYPE( uintptr_t )

class BeerXMLElement;

/*!
 * \class BeerXMLElement
 * \author Philip G. Lee
 *
 * \brief The base class for our database items.
 */
class BeerXMLElement : public QObject
{
   Q_OBJECT
   Q_CLASSINFO("version","1")
   
   friend class Database;
   friend class SetterCommand;
public:
   BeerXMLElement();
   BeerXMLElement( BeerXMLElement const& other );

   // Everything that inherits from BeerXML has delete, display and a folder
   Q_PROPERTY( bool deleted READ deleted WRITE setDeleted )
   Q_PROPERTY( bool display READ display WRITE setDisplay )
   Q_PROPERTY( QString folder READ folder WRITE setFolder )

   Q_PROPERTY( int key READ key )
   Q_PROPERTY( Brewtarget::DBTable table READ table )
   
   //! Convenience method to determine if we are deleted or displayed
   bool deleted() const;
   bool display() const;
   //! Access to the folder attribute.
   QString folder() const;

   //! And ways to set those flags
   void setDeleted(bool var);
   void setDisplay(bool var);
   //! and a way to set the folder
   void setFolder(QString fName);

   //! \returns our key in the table we are stored in.
   int key() const;
   //! \returns the table we are stored in.
   Brewtarget::DBTable table() const;
   //! \returns the BeerXML version of this element.
   int version() const;
   //! Convenience method to get a meta property by name.
   QMetaProperty metaProperty(const char* name) const;
   //! Convenience method to get a meta property by name.
   QMetaProperty metaProperty(QString const& name) const;
   
   // Some static helpers to convert to/from text.
   static double getDouble( const QDomText& textNode );
   static bool getBool( const QDomText& textNode );
   static int getInt( const QDomText& textNode );
   static QString getString( QDomText const& textNode );
   static QDateTime getDateTime( QDomText const& textNode );
   static QDate getDate( QDomText const& textNode );
   //! Convert the string to a QDateTime according to Qt::ISODate.
   static QDateTime getDateTime(QString const& str = "");
   static QDate getDate(QString const& str = "");
   static QString text(bool val);
   static QString text(double val);
   static QString text(int val);
   //! Convert the date to string in Qt::ISODate format for storage NOT display.
   static QString text(QDate const& val);
   
   //! Use this to pass pointers around in QVariants.
   static inline QVariant qVariantFromPtr( BeerXMLElement* ptr )
   {
      uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
      return QVariant::fromValue<uintptr_t>(addr);
   }
   
   static inline BeerXMLElement* extractPtr( QVariant ptrVal )
   {
      uintptr_t addr = ptrVal.value<uintptr_t>();
      return reinterpret_cast<BeerXMLElement*>(addr);
   }

   bool isValid();
   void invalidate();
   
signals:
   /*!
    * Passes the meta property that has changed about this object.
    * NOTE: when subclassing, be \em extra careful not to create a method with
    * the same signature. Otherwise, everything will silently break.
    */
   void changed(QMetaProperty, QVariant value = QVariant());
   
protected:
   
   //! The key of this ingredient in its table.
   int _key;
   //! The table where this ingredient is stored.
   Brewtarget::DBTable _table;

   /*!
    * \param prop_name - A meta-property name
    * \param col_name - The appropriate column in the table.
    * Should do the following:
    * 1) Set the appropriate value in the appropriate table row.
    * 2) Call the NOTIFY method associated with \b prop_name if \b notify == true.
    */
   void set( const char* prop_name, const char* col_name, QVariant const& value, bool notify = true );
   
   /*!
    * \param col_name - The database column of the attribute we want to get.
    * Returns the value of the attribute specified by key/table/col_name.
    */
   QVariant get( const char* col_name ) const;
   
private:
   /*!
    * \param valid - Indicates if the beerXML element was valid. There is a problem with importing invalid
    * XML. I'm hoping this helps fix it
    */
  bool valid;
};


#endif   /* _BEERXMLELEMENT_H */

