#ifndef DMZ_QT_PLUGIN_CANVAS_MAP_DOT_H
#define DMZ_QT_PLUGIN_CANVAS_MAP_DOT_H

#include <dmzQtWidget.h>
#include <dmzRuntimeLog.h>
#include <dmzRuntimePlugin.h>
#include <QtGui/QWidget>


namespace qmapcontrol {

   class MapControl;
   class MapAdapter;
   class MapLayer;
};


namespace dmz {

   class QtModuleCanvas;

   class QtPluginCanvasMap :
         public QWidget,
         public Plugin,
         public QtWidget {
            
   Q_OBJECT

      public:
         QtPluginCanvasMap (const PluginInfo &Info, Config &local);
         ~QtPluginCanvasMap ();

         // Plugin Interface
         virtual void update_plugin_state (
            const PluginStateEnum State,
            const UInt32 Level);

         virtual void discover_plugin (
            const PluginDiscoverEnum Mode,
            const Plugin *PluginPtr);

         // QtWidget Interface
         virtual QWidget *get_qt_widget ();
         
      protected slots:
            void slot_center_changed (const QPointF &Pos);
            void slot_scale_changed (qreal);

      protected:
         virtual void resizeEvent (QResizeEvent* event);
         void _init (Config &local);

         Log _log;
         QLayout *_layout;
         QtModuleCanvas *_canvasModule;
         String _canvasModuleName;
         qmapcontrol::MapControl *_map;
         qmapcontrol::MapAdapter *_mapAdapter;
         qmapcontrol::MapLayer *_baseLayer;
         QWidget *_canvasWidget;
         
      private:
         QtPluginCanvasMap ();
         QtPluginCanvasMap (const QtPluginCanvasMap &);
         QtPluginCanvasMap &operator= (const QtPluginCanvasMap &);

   };
};

#endif // DMZ_QT_PLUGIN_CANVAS_MAP_DOT_H