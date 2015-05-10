#ifndef carbonwidgetsizeengine_h
#define carbonwidgetsizeengine_h
/*
* this file is part of the carbon gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or(at your option ) any later version.
*
* This library is distributed  in the hope that it will be useful,
* but  WITHOUT ANY WARRANTY; without even  the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License  along  with  this library;  if not,  write to  the Free
* Software Foundation, Inc., 51  Franklin St, Fifth Floor, Boston,
* MA 02110-1301, USA.
*/


#include "carbongenericengine.h"
#include "carbondatamap.h"
#include "carbonwidgetsizedata.h"

#include <gtk/gtk.h>

namespace Carbon
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable widgetsizees
    /*!
    ensures that the text entry and the button of editable widgetsizees
    gets hovered and focus flags at the same time
    */
    class WidgetSizeEngine: public GenericEngine<WidgetSizeData>
    {

        public:

        //! constructor
        WidgetSizeEngine( Animations* widget ):
            GenericEngine<WidgetSizeData>( widget )
            {}

        //! destructor
        virtual ~WidgetSizeEngine( void )
        {}

        //! update window XShape for given widget
        WidgetSizeData::ChangedFlags update( GtkWidget* widget )
        { return data().value( widget ).update(); }

        //! width
        int width( GtkWidget* widget )
        { return  data().value( widget ).width(); }

        //! height
        int height( GtkWidget* widget )
        { return data().value( widget ).height(); }

        //! alpha
        bool alpha( GtkWidget* widget )
        { return data().value( widget ).alpha(); }

    };

}

#endif
