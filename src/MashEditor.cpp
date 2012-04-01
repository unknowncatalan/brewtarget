/*
 * MashEditor.cpp is part of Brewtarget, and is Copyright Philip G. Lee
 * (rocketman768@gmail.com), 2009-2011.
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

#include "MashEditor.h"
#include <QWidget>
#include <QDebug>
#include "mash.h"
#include "brewtarget.h"
#include "unit.h"
#include "equipment.h"
#include "recipe.h"

MashEditor::MashEditor(QWidget* parent) : QDialog(parent), mashObs(0)
{
   setupUi(this);

   connect(pushButton_fromEquipment, SIGNAL(clicked()), this, SLOT(fromEquipment()) );
   connect(this, SIGNAL(accepted()), this, SLOT(saveAndClose()) );
   connect(this, SIGNAL(rejected()), this, SLOT(closeEditor()) );
}

void MashEditor::showEditor()
{
   showChanges();
   setVisible(true);
}

void MashEditor::closeEditor()
{
   setVisible(false);
}

void MashEditor::saveAndClose()
{
   if( mashObs == 0 )
      return;
   
   //mash->disableNotification(); // If we don't do this, the notification will propagate to a showChanges() and we'll lose any info we want saved.
   mashObs->setEquipAdjust( true ); // BeerXML won't like me, but it's just stupid not to adjust for the equipment when you're able.

   mashObs->setName( lineEdit_name->text() );
   mashObs->setGrainTemp_c(Brewtarget::tempQStringToSI(lineEdit_grainTemp->text()));
   mashObs->setSpargeTemp_c(Brewtarget::tempQStringToSI(lineEdit_spargeTemp->text()));
   mashObs->setPh(lineEdit_spargePh->text().toDouble());
   mashObs->setTunTemp_c(Brewtarget::tempQStringToSI(lineEdit_tunTemp->text()));
   mashObs->setTunWeight_kg(Brewtarget::weightQStringToSI(lineEdit_tunMass->text()));
   mashObs->setTunSpecificHeat_calGC(lineEdit_tunSpHeat->text().toDouble() );

   mashObs->setNotes( textEdit_notes->toPlainText() );
   
   //mash->reenableNotification();
   //mash->forceNotify();
}

void MashEditor::fromEquipment()
{
   if( mashObs == 0 )
      return;

   if ( equip == 0 )
      return;

   qDebug() << "resetting mass " << equip->tunWeight_kg();
   lineEdit_tunMass->setText(Brewtarget::displayAmount(equip->tunWeight_kg(), Units::kilograms));
   lineEdit_tunSpHeat->setText(Brewtarget::displayAmount(equip->tunSpecificHeat_calGC()));
}

void MashEditor::setMash(Mash* mash)
{
   if( mashObs )
      disconnect( mashObs, 0, this, 0 );
   
   mashObs = mash;
   if( mashObs )
   {
      connect( mashObs, SIGNAL(changed(QMetaProperty,QVariant)), this, SLOT(changed(QMetaProperty,QVariant)) );
      showChanges();
   }
}

void MashEditor::setEquipment(Equipment* e)
{
   if ( ! e )
      return;

   equip = e;
}

void MashEditor::changed(QMetaProperty prop, QVariant /*val*/)
{
   if( sender() == mashObs )
      showChanges(&prop);
}

void MashEditor::showChanges(QMetaProperty* prop)
{
   bool updateAll = false;
   QString propName;
   
   if( sender() != mashObs )
      return;
   
   if( mashObs == 0 )
   {
      clear();
      return;
   }

   if( prop == 0 )
      updateAll = true;
   else
   {
      propName = prop->name();
   }
   
   if( propName == "name" || updateAll ) {
      lineEdit_name->setText(mashObs->name());
      if( ! updateAll )
         return;
   }
   if( propName == "grainTemp_c" || updateAll ) {
      lineEdit_grainTemp->setText(Brewtarget::displayAmount(mashObs->grainTemp_c(), Units::celsius));
      if( ! updateAll )
         return;
   }
   if( propName == "spargeTemp_c" || updateAll ) {
      lineEdit_spargeTemp->setText(Brewtarget::displayAmount(mashObs->spargeTemp_c(), Units::celsius));
      if( ! updateAll )
         return;
   }
   if( propName == "ph" || updateAll ) {
      lineEdit_spargePh->setText(Brewtarget::displayAmount(mashObs->ph()));
      if( ! updateAll )
         return;
   }
   if( propName == "tunTemp_c" || updateAll ) {
      lineEdit_tunTemp->setText(Brewtarget::displayAmount(mashObs->tunTemp_c(), Units::celsius));
      if( ! updateAll )
         return;
   }
   /*
   if( propName == "tunMass_kg" || updateAll ) {
      lineEdit_tunMass->setText(Brewtarget::displayAmount(mashObs->tunMass_kg(), Units::kilograms));
      if( ! updateAll )
         return;
   }
   */
   if( propName == "tunSpecificHeat_calGC" || updateAll ) {
      lineEdit_tunSpHeat->setText(Brewtarget::displayAmount(mashObs->tunSpecificHeat_calGC()));
      if( ! updateAll )
         return;
   }
   if( propName == "notes" || updateAll ) {
      textEdit_notes->setPlainText(mashObs->notes());
      if( ! updateAll )
         return;
   }
}

void MashEditor::clear()
{
   lineEdit_name->setText("");
   lineEdit_grainTemp->setText("");
   lineEdit_spargeTemp->setText("");
   lineEdit_spargePh->setText("");
   lineEdit_tunTemp->setText("");
   lineEdit_tunMass->setText("");
   lineEdit_tunSpHeat->setText("");

   textEdit_notes->setPlainText("");
}
