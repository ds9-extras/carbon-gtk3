#ifndef carbonthemingengine_h
#define carbonthemingengine_h
/*
* this file is part of the carbon gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or( at your option ) any later version.
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

#include <gtk/gtk.h>

// carbon theming engine struct
struct CarbonThemingEngine
{ GtkThemingEngine parent; };

// carbon theming engine class struct
struct CarbonThemingEngineClass
{  GtkThemingEngineClass parent; };

namespace Carbon
{
    class ThemingEngine
    {

        public:

        //! type registration
        static void registerType( GTypeModule* );

        //! version type registration
        /*!
        it is used to let an external program retrieve
        the carbon-gtk version that it uses, if any
        */
        static void registerVersionType( void );

        //! registered type
        static GType type( void );

        //! parent class
        inline static GtkThemingEngineClass* parentClass( void )
        { return _parentClass; }

        protected:

        //! instance initialization
        static void instanceInit( CarbonThemingEngine* );

        //! class initialization
        static void classInit( CarbonThemingEngineClass* );

        private:

        //! parent class
        static GtkThemingEngineClass* _parentClass;

        //! registered type indo
        static GTypeInfo _typeInfo;

        //! registered type
        static GType _type;

    };
}

#endif
