#ifndef carbonstyle_h
#define carbonstyle_h
/*
* this file is part of the carbon gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

#include "carbonanimations.h"
#include "carbonanimationdata.h"
#include "carbonanimationmodes.h"
#include "carbonargbhelper.h"
#include "carboncairocontext.h"
#include "carbongeometry.h"
#include "carbongtkcellinfo.h"
#include "carbongtkgap.h"
#include "carbonloghandler.h"
#include "carbonmetrics.h"
#include "carbonqtsettings.h"
#include "carbonshadowhelper.h"
#include "carbonstylehelper.h"
#include "carbonstyleoptions.h"
#include "carbontaboptions.h"
#include "carbontileset.h"
#include "carbonwidgetlookup.h"
#include "carbonwidgetexplorer.h"
#include "carbonwindecooptions.h"
#include "carbonwindecobutton.h"
#include "carbonwindowmanager.h"

#include <gdk/gdk.h>

#ifdef GDK_WINDOWING_X11
#include <X11/Xdefs.h>
#endif

namespace Carbon
{

    //! handle all plainting
    class Style
    {

        public:

        //! singleton
        static Style& instance( void );

        //! destructor
        virtual ~Style( void )
        {
            if( _instance == this )
            { _instance = 0L; }
        }

        //! initialize
        bool initialize( unsigned int flags = QtSettings::All );

        //! settings
        const QtSettings& settings( void ) const
        { return _settings; }

        //! settings
        QtSettings& settings( void )
        { return _settings; }

        //! helper
        StyleHelper& helper( void )
        { return _helper; }

        //! helper
        const StyleHelper& helper( void ) const
        { return _helper; }

        //! animations
        const Animations& animations( void ) const
        { return _animations; }

        //! animations
        Animations& animations( void )
        { return _animations; }

        //! argb helper
        ArgbHelper& argbHelper( void )
        { return _argbHelper; }

        //! shadow helper
        ShadowHelper& shadowHelper( void )
        { return _shadowHelper; }

        //! window manager
        WidgetExplorer& widgetExplorer( void )
        { return _widgetExplorer; }

        //! window manager
        WindowManager& windowManager( void )
        { return _windowManager; }

        //! widget lookup
        WidgetLookup& widgetLookup( void )
        { return _widgetLookup; }

        //! return tabCloseButton for given set of options
        Cairo::Surface tabCloseButton( const StyleOptions& );

        //! background surface
        bool hasBackgroundSurface( void ) const;

        //! update widget mask
        void adjustMask( GtkWidget*, int width, int height, bool alpha );

        //! update kwin blur region
        void setWindowBlur( GtkWidget*, bool enable );

        //!@name primitives
        //@{

        //! fill given rectangle with flat color
        void fill( cairo_t* context, gint x, gint y, gint w, gint h, Palette::Role role ) const
        { fill( context, x, y, w, h, _settings.palette().color( Palette::Active, role ) ); }

        //! fill given rectangle with flat color
        void fill( cairo_t* context, gint x, gint y, gint w, gint h, Palette::Group group, Palette::Role role ) const
        { fill( context, x, y, w, h, _settings.palette().color( group, role ) ); }

        //! fill given rectangle with flat color
        void fill( cairo_t*, gint, gint, gint, gint, const ColorUtils::Rgba& color ) const;

        //! outline given rectangle with flat color
        void outline( cairo_t*, gint, gint, gint, gint, const ColorUtils::Rgba& color = ColorUtils::Rgba( 1, 0, 0 ) ) const;

        //! separators
        void drawSeparator( GtkWidget*, cairo_t*, gint, gint, gint, gint, const StyleOptions& );
        void drawSeparator( cairo_t* context, gint x, gint y, gint w, gint h, const StyleOptions& options )
        { drawSeparator( 0L, context, x, y, w, h, options ); }

        //! window background
        /*! returns true if window gradient could be rendered */
        bool renderWindowBackground( cairo_t*, GdkWindow*, GtkWidget*, gint, gint, gint, gint, const StyleOptions& = StyleOptions(), bool isMaximized=false );
        bool renderWindowBackground( cairo_t* c, gint x, gint y, gint w, gint h, bool maximized )
        { return renderWindowBackground( c, 0, 0, x, y, w, h, StyleOptions(), maximized );}

        bool renderWindowBackground( GdkWindow* window, GtkWidget* widget, gint x, gint y, gint w, gint h, const StyleOptions& o = StyleOptions() )
        { return renderWindowBackground( 0L, window, widget, x, y, w, h, o ); }

        bool renderWindowBackground( cairo_t* context, GdkWindow* window, gint x, gint y, gint w, gint h, const StyleOptions& o = StyleOptions() )
        { return renderWindowBackground( context, window, 0L, x, y, w, h, o ); }

        // render background gradient
        bool renderBackgroundGradient( cairo_t*, GdkWindow*, GtkWidget*, gint, gint, gint, gint, const StyleOptions& = StyleOptions(), bool isMaximized=false );

        // render background pixmap
        bool renderBackgroundPixmap( cairo_t*, GdkWindow*, GtkWidget*, gint, gint, gint, gint, bool isMaximized=false );

        //! titlebar background
        bool renderTitleBarBackground( cairo_t*, GtkWidget*, gint, gint, gint, gint );

        //! groupbox background
        bool renderGroupBoxBackground( cairo_t*, GtkWidget*, gint, gint, gint, gint, const StyleOptions&, TileSet::Tiles = TileSet::Center );

        //! menu background
        bool renderMenuBackground( cairo_t*, gint, gint, gint, gint, const StyleOptions& ) const;

        //! tooltip background
        void renderTooltipBackground( cairo_t*, gint, gint, gint, gint, const StyleOptions& ) const;

        //! tree view header
        void renderHeaderBackground( cairo_t*, GdkWindow*, GtkWidget*, gint, gint, gint, gint );
        void renderHeaderBackground( cairo_t* context, GdkWindow* window, gint x, gint y, gint w, gint h )
        { renderHeaderBackground( context, window, 0L, x, y, w, y ); }

        //! tree view header
        void renderHeaderLines( cairo_t*, gint, gint, gint, gint ) const;

        //! tree view lines
        void renderTreeLines( cairo_t*, gint, gint, gint, gint, const Gtk::CellInfoFlags&, const StyleOptions& ) const;

        //!@name editors hole
        //@{
        void renderHoleBackground( cairo_t*, GdkWindow*, GtkWidget*, gint, gint, gint, gint, const StyleOptions&, TileSet::Tiles = TileSet::Ring, gint = Entry_SideMargin );

        void renderHoleBackground( cairo_t* context, GdkWindow* window, GtkWidget* widget, gint x, gint y, gint w, gint h, TileSet::Tiles tiles = TileSet::Ring, gint margin = Entry_SideMargin )
        { renderHoleBackground( context, window, widget, x, y, w, h, StyleOptions(), tiles, margin ); }

        void renderHoleBackground( cairo_t* context, GdkWindow* window, gint x, gint y, gint w, gint h, TileSet::Tiles tiles = TileSet::Ring, gint margin = Entry_SideMargin )
        { renderHoleBackground( context, window, 0L, x, y, w, h, StyleOptions(), tiles, margin ); }

        //@}

        //! splitters
        void renderSplitter( cairo_t*, gint, gint, gint, gint, const StyleOptions&, const AnimationData& = AnimationData() ) const;

        //!@name progressbar
        //@{
        void renderProgressBarHole( cairo_t*, gint, gint, gint, gint, const StyleOptions& );
        void renderProgressBarHandle( cairo_t*, gint, gint, gint, gint, const StyleOptions& );
        //@}

        //!@name scrollbar
        //@{
        void renderScrollBarHole( cairo_t*, gint, gint, gint, gint, const StyleOptions& );
        void renderScrollBarHandle( cairo_t*, gint, gint, gint, gint, const StyleOptions&, const AnimationData& = AnimationData() );
        //@}

        //! toolbar handle
        void renderToolBarHandle( cairo_t*, gint, gint, gint, gint, const StyleOptions& ) const;

        //! frame
        void drawFloatFrame( cairo_t*, gint, gint, gint, gint, const StyleOptions&, Palette::Role = Palette::Window ) const;

        //!@name button slab
        //@{
        void renderButtonSlab( GtkWidget*, cairo_t*, gint, gint, gint, gint, const StyleOptions&, const AnimationData&, TileSet::Tiles = TileSet::Ring );

        void renderButtonSlab( cairo_t* context, gint x, gint y, gint w, gint h, const StyleOptions& options, const AnimationData& data, TileSet::Tiles tiles = TileSet::Ring )
        { renderButtonSlab( 0L, context, x, y, w, h, options, data, tiles ); }

        void renderButtonSlab( GtkWidget* widget, cairo_t* context, gint x, gint y, gint w, gint h, const StyleOptions& options, TileSet::Tiles tiles = TileSet::Ring )
        { renderButtonSlab( widget, context, x, y, w, h, options, AnimationData(), tiles ); }

        void renderButtonSlab( cairo_t* context, gint x, gint y, gint w, gint h, const StyleOptions& options, TileSet::Tiles tiles = TileSet::Ring )
        { renderButtonSlab( 0L, context, x, y, w, h, options, AnimationData(), tiles ); }
        //@}

        //! checkbox
        /*! shadow type is used to decide whether check is on/off or tristate */
        void renderCheckBox( GtkWidget*, cairo_t*, gint, gint, gint, gint, GtkShadowType, const StyleOptions&, const AnimationData& = AnimationData() );
        void renderCheckBox( cairo_t* context, gint x, gint y, gint w, gint h, GtkShadowType shadow, const StyleOptions& options, const AnimationData& data = AnimationData() )
        { renderCheckBox( 0L, context, x, y, w, h, shadow, options, data ); }

        //! radio button
        void renderRadioButton( GtkWidget*, cairo_t*, gint, gint, gint, gint, GtkShadowType, const StyleOptions&, const AnimationData& = AnimationData() );
        void renderRadioButton( cairo_t* context, gint x, gint y, gint w, gint h, GtkShadowType shadow, const StyleOptions& options, const AnimationData& data = AnimationData() )
        { renderRadioButton( 0L, context, x, y, w, h, shadow, options, data ); }

        //!@name generic slab
        //@{

        void renderSlab( cairo_t*, gint, gint, gint, gint, const Gtk::Gap&, const StyleOptions&, const AnimationData& = AnimationData() );
        void renderSlab( cairo_t* context, int x, gint y, gint w, gint h, const StyleOptions& options, const AnimationData& animationData = AnimationData() )
        { renderSlab( context, x, y, w, h, Gtk::Gap(), options, animationData ); }

        //@}

        //! info bar
        void renderInfoBar( GtkWidget*, cairo_t*, gint, gint, gint, gint, const ColorUtils::Rgba& );
        void renderInfoBar( cairo_t* context, gint x, gint y, gint w, gint h, const ColorUtils::Rgba& color )
        { renderInfoBar( 0L, context, x, y, w, h, color ); }

        //!@name hole
        //@{

        void renderHole( cairo_t*, gint, gint, gint, gint, const Gtk::Gap&, const StyleOptions&, const AnimationData& = AnimationData(), TileSet::Tiles = TileSet::Ring );

        void renderHole( cairo_t* context, gint x, gint y, gint w, gint h, const StyleOptions& o, TileSet::Tiles tiles = TileSet::Ring )
        { renderHole( context, x, y, w, h, Gtk::Gap(), o, AnimationData(), tiles ); }

        void renderHole( cairo_t* context, gint x, gint y, gint w, gint h, const StyleOptions& o, const AnimationData& animationData, TileSet::Tiles tiles = TileSet::Ring )
        { renderHole( context, x, y, w, h, Gtk::Gap(), o, animationData, tiles ); }

        //@}

        //!@name dock frame
        //@{

        void renderDockFrame( GtkWidget*, cairo_t*, gint, gint, gint, gint, const Gtk::Gap&, const StyleOptions& );
        void renderDockFrame( cairo_t* context, gint x, gint y, gint w, gint h, const Gtk::Gap& gap, const StyleOptions& options )
        { renderDockFrame( 0L, context, x, y, w, h, gap, options ); }

        void renderDockFrame( cairo_t* context, gint x, gint y, gint w, gint h, const StyleOptions& options )
        { renderDockFrame( 0L, context, x, y, w, h, Gtk::Gap(), options ); }

        //@}

        //! groupbox frame
        void renderGroupBoxFrame( cairo_t*, GtkWidget*, gint, gint, gint, gint, const StyleOptions&  );

        //! menu item
        void renderMenuItemRect( cairo_t*, GdkWindow*, GtkWidget*, gint, gint, gint, gint, const StyleOptions&, const AnimationData& = AnimationData() );

        //! selection
        void renderSelection( cairo_t*, gint, gint, gint, gint, TileSet::Tiles tiles, const StyleOptions& );

        //! arrow
        void renderArrow( cairo_t*, GtkArrowType, gint, gint, gint, gint, QtSettings::ArrowSize, const StyleOptions&, const AnimationData&, Palette::Role ) const;
        void renderArrow( cairo_t* context, GtkArrowType type, gint x, gint y, gint w, gint h, QtSettings::ArrowSize size = QtSettings::ArrowNormal, const StyleOptions& options = Contrast, Palette::Role role = Palette::ButtonText ) const
        { renderArrow( context, type, x, y, w, h, size, options, AnimationData(), role ); }

        //! slider groove
        void renderSliderGroove( cairo_t*, gint, gint, gint, gint, const StyleOptions&, TileSet::Tiles = TileSet::Full );

        //! slider handle
        void renderSliderHandle( cairo_t*, gint, gint, gint, gint, const StyleOptions&, const AnimationData& = AnimationData() );

        //! size grip
        void renderSizeGrip( cairo_t*, GdkWindowEdge, gint, gint, gint, gint ) const;

        //! tab
        void renderTab(
            cairo_t*,
            gint, gint, gint, gint,
            GtkPositionType,
            const StyleOptions&,
            const TabOptions&,
            const AnimationData& = AnimationData() );

        //! tabbar base
        void renderTabBarBase(
            cairo_t*,
            gint, gint, gint, gint,
            GtkPositionType, Gtk::Gap,
            const StyleOptions&,
            const TabOptions& );

        //! tabwidget frame
        void renderTabBarFrame( cairo_t*, gint, gint, gint, gint, const Gtk::Gap&, const StyleOptions& );

        //! tree 'standard' expanders (that is: +!-)
        void renderTreeExpander( cairo_t*, gint, gint, gint, gint, GtkExpanderStyle, const StyleOptions&, const AnimationData&, Palette::Role ) const;
        void renderTreeExpander( cairo_t* context, gint x, gint y, gint w, gint h, GtkExpanderStyle style, const StyleOptions& options, Palette::Role role ) const
        { renderTreeExpander( context, x, y, w, h, style, options, AnimationData(), role ); }

        //@}

        //! draw window decorations
        void drawWindowDecoration( cairo_t*, WinDeco::Options, gint, gint, gint, gint, const gchar**, gint, gint);

        //! draw window shadow
        void drawWindowShadow( cairo_t* context, WinDeco::Options wopt, gint x, gint y, gint w, gint h );

        //! render XShape window decoration mask
        void drawWindecoShapeMask( cairo_t* context, WinDeco::Options wopt, gint x, gint y, gint w, gint h );

        //! draw windeco button
        void drawWindecoButton(cairo_t*,WinDeco::ButtonType,WinDeco::ButtonStatus,WinDeco::Options, gint,gint,gint,gint);

        // adjust scrollbar hole, depending on orientation and buttons settings
        void adjustScrollBarHole( gdouble& x, gdouble& y, gdouble& w, gdouble& h, const StyleOptions& ) const;

        //! sanitize size
        void sanitizeSize( GdkWindow* window, gint& width, gint& height ) const;

        // get tiles for given tab orientation
        TileSet::Tiles tabTiles( GtkPositionType position ) const
        {

            TileSet::Tiles out( TileSet::Ring );
            switch( position )
            {
                case GTK_POS_BOTTOM: out &= ~TileSet::Bottom; break;
                case GTK_POS_TOP: out &= ~TileSet::Top; break;
                case GTK_POS_LEFT: out &= ~TileSet::Left; break;
                case GTK_POS_RIGHT: out &= ~TileSet::Right; break;
                default: break;
            }

            return out;
        }

        protected:

        //! constructor
        Style( void );

        //! get color matching role from either style option or default palette
        const ColorUtils::Rgba& color( Palette::Role role, const StyleOptions& option ) const
        { return color( Palette::Active, role, option ); }

        //! get color matching group and role from either style option or default palette
        const ColorUtils::Rgba& color( Palette::Group group, Palette::Role role, const StyleOptions& option ) const
        {
            Palette::ColorSet::const_iterator iter( option._customColors.find( role ) );
            return iter == option._customColors.end() ? _settings.palette().color( group, role ) : iter->second;
        }

        //! set background surface
        void setBackgroundSurface( const std::string& );

        //@name internal rendering
        //@{

        //! tab
        void renderActiveTab(
            cairo_t*,
            gint, gint, gint, gint,
            GtkPositionType,
            const StyleOptions&,
            const TabOptions& );

        //! tab
        void renderInactiveTab_Plain(
            cairo_t*,
            gint, gint, gint, gint,
            GtkPositionType,
            const StyleOptions&,
            const TabOptions&,
            const AnimationData& );

        //! tab
        void renderInactiveTab_Single(
            cairo_t*,
            gint, gint, gint, gint,
            GtkPositionType,
            const StyleOptions&,
            const TabOptions&,
            const AnimationData& );

        //! slab glowing color
        ColorUtils::Rgba slabShadowColor( const StyleOptions&, const AnimationData& = AnimationData() ) const;

        //! hole glowing color
        ColorUtils::Rgba holeShadowColor( const StyleOptions&, const AnimationData& = AnimationData() ) const;

        //! groupbox
        void renderGroupBox( cairo_t*, const ColorUtils::Rgba&, gint, gint, gint, gint, const StyleOptions& = StyleOptions() );

        //! slab
        void renderSlab( cairo_t*, gint, gint, gint, gint, const ColorUtils::Rgba&, const StyleOptions&, const AnimationData& = AnimationData(), TileSet::Tiles tiles = TileSet::Ring );

        //! progressbar hole (groove)
        /*! also used for progress bars */
        void renderScrollBarHole( cairo_t*, gint, gint, gint, gint, const ColorUtils::Rgba&, bool vertical, TileSet::Tiles tiles = TileSet::Full );

        //! add hole mask to context
        void renderHoleMask( cairo_t*, gint, gint, gint, gint, TileSet::Tiles, gint );

        //! returns point position for generic arrows
        Polygon genericArrow( GtkArrowType, QtSettings::ArrowSize = QtSettings::ArrowNormal ) const;

        //@}

        //!@name window decoration rendering
        //@{

        //! draw resize handles for window decoration
        void renderWindowDots(cairo_t*, gint x, gint y, gint w, gint h, const ColorUtils::Rgba&, WinDeco::Options);

        //! internal windeco renderer
        void renderWindowDecoration( cairo_t*, WinDeco::Options, gint, gint, gint, gint, const gchar**, gint, gint, bool = true);

        //@}

        //!@name utilities
        //@{

        // center rect
        void centerRect( GdkRectangle*, GdkRectangle* ) const;

        // generate map from gap
        void generateGapMask( cairo_t*, gint, gint, gint, gint, const Gtk::Gap& ) const;

        //@}

        //! monitored files is changed
        static void fileChanged( GFileMonitor*, GFile*, GFile*, GFileMonitorEvent, gpointer );

        //! used to store slab characteristics
        class SlabRect
        {
            public:

            //! constructor
            explicit SlabRect(void):
                _x(0),
                _y(0),
                _w(-1),
                _h(-1),
                _tiles( TileSet::Ring )
            {}

            //! constructor
            explicit SlabRect( int x, int y, int w, int h, const TileSet::Tiles& tiles, const StyleOptions& options = StyleOptions() ):
                _x(x),
                _y(y),
                _w(w),
                _h(h),
                _tiles( TileSet::Tiles( tiles ) ),
                _options( options )
            {}

            int _x;
            int _y;
            int _w;
            int _h;
            TileSet::Tiles _tiles;
            StyleOptions _options;

            typedef std::vector<SlabRect> List;

        };

        private:

        //! log handler
        LogHandler _logHandler;

        //! Qt settings
        QtSettings _settings;

        //! helper
        StyleHelper _helper;

        //! animations
        Animations _animations;

        //! argb helper
        ArgbHelper _argbHelper;

        //! shadow helper
        ShadowHelper _shadowHelper;

        //! widget explorer
        WidgetExplorer _widgetExplorer;

        //! window manager
        WindowManager _windowManager;

        //! widget lookup
        WidgetLookup _widgetLookup;

        //! background surface
        Cairo::Surface _backgroundSurface;

        //! Tab close buttons
        class TabCloseButtons
        {
            public:

            //! constructor
            TabCloseButtons( void ):
                normal(0L),
                active(0L),
                inactive(0L),
                prelight(0L)
            {}

            //! destructor
            virtual ~TabCloseButtons( void )
            {}

            Cairo::Surface normal;
            Cairo::Surface active;
            Cairo::Surface inactive;
            Cairo::Surface prelight;
        };

        //! Tab close buttons
        TabCloseButtons _tabCloseButtons;

        #ifdef GDK_WINDOWING_X11
        //! Atom to show kwin what regions of translucent windows should be blurred
        Atom _blurAtom;
        #endif

        //! singleton
        static Style* _instance;

    };

}

#endif
