/*
 * Hydrogen
 * Copyright(c) 2002-2008 by Alex >Comix< Cominu [comix@users.sourceforge.net]
 *
 * http://www.hydrogen-music.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <QtGui>

#include "../HydrogenApp.h"
#include "SoundLibraryPropertiesDialog.h"
#include "../InstrumentRack.h"
#include "SoundLibraryPanel.h"
#include <hydrogen/hydrogen.h>

namespace H2Core
{

//globals
Drumkit *drumkitinfo = NULL ;
Drumkit *predrumkit = NULL;
QString oldName;

const char* SoundLibraryPropertiesDialog::__class_name = "SoundLibraryPropertiesDialog";

SoundLibraryPropertiesDialog::SoundLibraryPropertiesDialog( QWidget* pParent, Drumkit *drumkitInfo, Drumkit *preDrumKit )
 : QDialog( pParent )
 , Object( __class_name )
{
	setupUi( this );
	INFOLOG( "INIT" );
	setWindowTitle( trUtf8( "SoundLibrary Properties" ) );
	setFixedSize( width(), height() );
	predrumkit = preDrumKit;

	//display the current drumkit infos into the qlineedit
	if ( drumkitInfo != NULL ){
		drumkitinfo = drumkitInfo;
		nameTxt->setText( QString( drumkitInfo->get_name() ) );
		oldName = drumkitInfo->get_name();
		authorTxt->setText( QString( drumkitInfo->get_author() ) );
		infoTxt->append( QString( drumkitInfo->get_info() ) );
		licenseTxt->setText( QString( drumkitInfo->get_license() ) );
	}

}




SoundLibraryPropertiesDialog::~SoundLibraryPropertiesDialog()
{
	INFOLOG( "DESTROY" );

}



void SoundLibraryPropertiesDialog::on_saveBtn_clicked()
{

	bool reload = false;

	if ( saveChanges_checkBox->isChecked() ){
		//test if the drumkit is loaded
		if ( Hydrogen::get_instance()->getCurrentDrumkitname() != drumkitinfo->get_name() ){
			QMessageBox::information( this, "Hydrogen", trUtf8 ( "This is not possible, you can only save changes inside instruments to the current loaded sound library"));
			saveChanges_checkBox->setChecked( false );
			return;
		}
		reload = true;
	}

	//load the selected drumkit to save it correct.... later the old drumkit will be reloaded
	if ( drumkitinfo != NULL && ( !saveChanges_checkBox->isChecked() ) ){
		if ( Hydrogen::get_instance()->getCurrentDrumkitname() != drumkitinfo->get_name() ){
			Hydrogen::get_instance()->loadDrumkit( drumkitinfo );
			Hydrogen::get_instance()->getSong()->__is_modified = true;
		}
	}

	//check the drumkit name. if the name is a new one, one qmessagebox with question "are you sure" will displayed.
	if ( nameTxt->text() != oldName  ){
		int res = QMessageBox::information( this, "Hydrogen", tr( "Warning! Changing the drumkit name will result in creating a new drumkit with this name.\nAre you sure?"), tr("&Ok"), tr("&Cancel"), 0, 1 );
		if ( res == 1 ) {
			return;
		}
		else
		{
			reload = true;
		}
	}

	//save the drumkit
	if( !H2Core::Drumkit::save( nameTxt->text(), authorTxt->text(), infoTxt->toHtml(), licenseTxt->text(), H2Core::Hydrogen::get_instance()->getSong()->get_instrument_list(), H2Core::Hydrogen::get_instance()->getSong()->get_components(), true ) ) {
        QMessageBox::information( this, "Hydrogen", trUtf8 ( "Saving of this drumkit failed."));
    }

	//check the name and set the drumkitinfo to current drumkit
	if ( drumkitinfo != NULL && !nameTxt->text().isEmpty() ){
		drumkitinfo->set_name( nameTxt->text() );
		drumkitinfo->set_author( authorTxt->text() );
		drumkitinfo->set_info( infoTxt->toHtml() );
		drumkitinfo->set_license( licenseTxt->text() );
	}


	//check pre loaded drumkit name  and reload the old drumkit
	if ( predrumkit != NULL ){
		if ( predrumkit->get_name() !=  Hydrogen::get_instance()->getCurrentDrumkitname() ){
			Hydrogen::get_instance()->loadDrumkit( predrumkit );
			Hydrogen::get_instance()->getSong()->__is_modified = true;
		}
	}

	//reload if necessary
	if ( reload == true ){
		HydrogenApp::get_instance()->getInstrumentRack()->getSoundLibraryPanel()->test_expandedItems();
		HydrogenApp::get_instance()->getInstrumentRack()->getSoundLibraryPanel()->updateDrumkitList();
	}

	accept();

}

}
