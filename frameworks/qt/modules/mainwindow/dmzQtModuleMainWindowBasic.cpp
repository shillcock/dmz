#include <dmzInputEventMasks.h>
#include <dmzInputModule.h>
#include "dmzQtModuleMainWindowBasic.h"
#include <dmzQtUtil.h>
#include <dmzRuntimeConfig.h>
#include <dmzRuntimeConfigRead.h>
#include <dmzRuntimePluginFactoryLinkSymbol.h>
#include <dmzRuntimePluginInfo.h>
#include <dmzRuntimeSession.h>
#include <QtGui/QtGui>


dmz::QtModuleMainWindowBasic::QtModuleMainWindowBasic (
      const PluginInfo &Info,
      Config &local) :
      QMainWindow (0),
      QtModuleMainWindow (Info),
      Plugin (Info),
      InputObserverUtil (Info, local),
      _exit (get_plugin_runtime_context ()),
      _log (Info),
      _fileExitAction (0),
      _fileMenu (0),
      _showUnifiedTitleAndToolBar (False),
      _centralWidgetTable (),
      _dockWidgetTable (),
      _toolBarTable (),
      _channelTable (),
      _channel (0) {

   setObjectName (get_plugin_name ().get_buffer ());
//   setAttribute (Qt::WA_AlwaysShowToolTips);
   
   _ui.setupUi (this);
   
   _create_actions ();
   _create_menus ();
   _create_tool_bars ();
   _create_status_bar ();
   
   QWidget *widget (new QWidget (this));
   _ui.stackedWidget->addWidget (widget);

   _init (local);
   
   setCorner (Qt::BottomRightCorner, Qt::RightDockWidgetArea);
}


dmz::QtModuleMainWindowBasic::~QtModuleMainWindowBasic () {

   _centralWidgetTable.clear ();
   _dockWidgetTable.empty ();
   _toolBarTable.empty ();
   _channelTable.empty ();
}




// Plugin Interface
void
dmz::QtModuleMainWindowBasic::update_plugin_state (
      const PluginStateEnum State,
      const UInt32 Level) {

   if (State == PluginStateStart) {

      _load_session ();
      setUnifiedTitleAndToolBarOnMac (_showUnifiedTitleAndToolBar);
      show ();
   }
   else if (State == PluginStateStop) {

      _save_session ();
      hide ();
   }
}


void
dmz::QtModuleMainWindowBasic::update_channel_state (
      const UInt32 Channel,
      const Boolean State) { 

   Int32 *count (_channelTable.lookup (Channel));
   
   if (!count) {
      
      count = new Int32 (0);
      if (!_channelTable.store (Channel, count)) { delete count; count = 0; }
   }

   if (count) {
      
      if (State) { (*count)++; }
      else if (*count) { (*count)--; }

      _channel = Channel;
      _update ();
   }
}


// QtModuleMainWindow Interface
QString
dmz::QtModuleMainWindowBasic::get_window_name () { return _windowName; }


QWidget *
dmz::QtModuleMainWindowBasic::get_widget () { return this; }


QMenuBar *
dmz::QtModuleMainWindowBasic::get_menu_bar () { return menuBar (); }


QStatusBar *
dmz::QtModuleMainWindowBasic::get_status_bar () { return statusBar (); }


dmz::Boolean
dmz::QtModuleMainWindowBasic::add_tool_bar (QToolBar *toolBar) {
   
   Boolean retVal (False);
   
   if (toolBar) {

      addToolBar (toolBar);
      retVal = True;
   }
   
   return retVal;
}


dmz::Boolean
dmz::QtModuleMainWindowBasic::add_tool_bar (const Handle Channel, QToolBar *toolBar) {
   
   Boolean retVal (False);
   
   if (toolBar) {
      
      ToolBarList *list (_toolBarTable.lookup (Channel));

      if (!list) {

         list = new ToolBarList ();
         _toolBarTable.store (Channel, list);
      }

      if (list) {

         ToolBarStruct *tbs (new ToolBarStruct ());
         tbs->visible = True;
         tbs->toolBar = toolBar;
         tbs->area = Qt::TopToolBarArea;
         
         list->append (tbs);
         
         Int32 *channel (_channelTable.lookup (Channel));
         
         if (channel && *channel) {
            
            addToolBar (toolBar);
         }

         retVal = True;
      }
   }
   
   return retVal;
}


dmz::Boolean
dmz::QtModuleMainWindowBasic::remove_tool_bar (QToolBar *toolBar) {
   
   Boolean retVal (False);
   
   if (toolBar) {
      
      removeToolBar (toolBar);
      retVal = True;
   }
   
   return retVal;
}


dmz::Boolean
dmz::QtModuleMainWindowBasic::remove_tool_bar (const Handle Channel, QToolBar *toolBar) {
   
   Boolean retVal (False);
   
   if (toolBar) {
      
      ToolBarList *list (_toolBarTable.lookup (Channel));

      if (list) {

         ToolBarStruct *tbs (0);
         
         foreach (ToolBarStruct *ctbs, *list) {
            
            if (ctbs->toolBar == toolBar) {
         
               tbs = ctbs;
               break;
            }
         }

         Int32 index (list->indexOf (tbs));

         if (index != -1) {

            list->takeAt (index);
            removeToolBar (tbs->toolBar);
            delete tbs; tbs = 0;
            retVal = True;
         }

         if (!list->count ()) {

            _toolBarTable.remove (Channel);
            delete list;
            list = 0;
         }
      }
   }
   
   return retVal;
}


void
dmz::QtModuleMainWindowBasic::add_tool_bar_break (const Qt::ToolBarArea Area) {
  
   addToolBarBreak (Area); 
}


dmz::Boolean
dmz::QtModuleMainWindowBasic::add_dock_widget (
      const Qt::DockWidgetArea Area,
      QDockWidget *dock) {

   Boolean retVal (False);

   if (dock) {

      addDockWidget (Area, dock);
      retVal = True;
   }
   
   return retVal;
}


dmz::Boolean
dmz::QtModuleMainWindowBasic::add_dock_widget (
      const Handle Channel,
      const Qt::DockWidgetArea Area,
      QDockWidget *dock) {

   Boolean retVal (False);
   
   if (dock) {
      
      DockWidgetList *list (_dockWidgetTable.lookup (Channel));

      if (!list) {
      
         list = new DockWidgetList ();
         _dockWidgetTable.store (Channel, list);
      }
   
      if (list) {

         DockWidgetStruct *dws (new DockWidgetStruct ());
         dws->visible = True;
         dws->dockWidget = dock;
         dws->area = Area;
         list->append (dws);

         addDockWidget (Area, dock);
         
         Int32 *channel (_channelTable.lookup (Channel));
         
         if (channel && !(*channel)) {
         
            removeDockWidget (dock);
         }
      
         retVal = True;
      }
   }
   
   return retVal;
}


dmz::Boolean
dmz::QtModuleMainWindowBasic::remove_dock_widget (QDockWidget *dock) {
   
   Boolean retVal (False);
   
   if (dock) {

      removeDockWidget (dock);
      retVal = True;
   }
   
   return retVal;
}


dmz::Boolean
dmz::QtModuleMainWindowBasic::remove_dock_widget (const Handle Channel, QDockWidget *dock) {
   
   Boolean retVal (False);
   
   if (dock) {
      
      DockWidgetList *list (_dockWidgetTable.lookup (Channel));

      if (list) {
         
         DockWidgetStruct *dws (0);
         
         foreach (DockWidgetStruct *cdws, *list) {
            
            if (cdws->dockWidget == dock) {
         
               dws = cdws;
               break;
            }
         }

         Int32 index (list->indexOf (dws));

         if (index != -1) {

            list->takeAt (index);
            removeDockWidget (dws->dockWidget);
            delete dws; dws = 0;
            retVal = True;
         }

         if (!list->count ()) {

            _dockWidgetTable.remove (Channel);
            delete list;
            list = 0;
         }
      }
   }
   
   return retVal;
}


dmz::Boolean
dmz::QtModuleMainWindowBasic::add_central_widget (const Handle Channel, QWidget *widget) {
   
   Boolean retVal (False);
   
   if (widget) {
      
      if (!_centralWidgetTable.lookup (Channel)) {

         _centralWidgetTable.store (Channel, widget);
         _ui.stackedWidget->addWidget (widget);

         if (Channel == _channel) {

            _ui.stackedWidget->setCurrentWidget (widget);
         }
      }
   }
   
   return retVal;
}


dmz::Boolean
dmz::QtModuleMainWindowBasic::remove_central_widget (const Handle Channel) {
   
   Boolean retVal (False);

   QWidget *widget (_centralWidgetTable.remove (Channel));
   
   if (widget) {
   
      _ui.stackedWidget->removeWidget (widget);
      retVal = True;
   }
   
   return retVal;
}


void
dmz::QtModuleMainWindowBasic::closeEvent (QCloseEvent *event) {
   
   _exit.request_exit (dmz::ExitStatusNormal, get_plugin_name () + " Closed");
}


void
dmz::QtModuleMainWindowBasic::_create_actions () {

   _fileExitAction = new QAction (tr ("E&xit"), this);
   _fileExitAction->setShortcut (tr ("Ctrl+Q"));
   _fileExitAction->setStatusTip (tr ("Exit the application"));
   connect (_fileExitAction, SIGNAL (triggered ()), this, SLOT (close ()));
}


void
dmz::QtModuleMainWindowBasic::_create_menus () {

   _fileMenu = menuBar ()->addMenu (tr ("&File"));
   _fileMenu->addAction (_fileExitAction);
}


void
dmz::QtModuleMainWindowBasic::_create_tool_bars () {

}


void
dmz::QtModuleMainWindowBasic::_create_status_bar () {

   
   statusBar()->showMessage (tr ("Ready"), 2000);
}


void
dmz::QtModuleMainWindowBasic::_update () {
   
   _ui.stackedWidget->setCurrentIndex (0);
   
   HashTableHandleIterator it;

   ToolBarList *tbList (_toolBarTable.get_first (it));

   while (tbList) {

      foreach (ToolBarStruct *tbs, *tbList) {
         
         Int32 *channel (_channelTable.lookup (it.get_hash_key ()));
         
         if (channel && *channel) {
            
            addToolBar (tbs->area, tbs->toolBar);
            //if (tbs->visible) { tbs->toolBar->setVisible (True); }
            tbs->toolBar->setVisible (True);
         }
         else {

            //tbs->visible = tbs->toolBar->isVisible ();
            //tbs->area = toolBarArea (tbs->toolBar);
            removeToolBar (tbs->toolBar);
         }
      }
      
      tbList = _toolBarTable.get_next (it);
   }
   
   DockWidgetList *dwList (_dockWidgetTable.get_first (it));
   
   while (dwList) {
      
      foreach (DockWidgetStruct *dws, *dwList) {
         
         Int32 *channel (_channelTable.lookup (it.get_hash_key ()));
         
         if (channel && *channel) {

            addDockWidget (dws->area, dws->dockWidget);
            //if (dws->visible) { dws->dockWidget->setVisible (True); }
            dws->dockWidget->setVisible (True);
         }
         else {
            
            //dws->visible = dws->dockWidget->isVisible ();
            //dws->area = dockWidgetArea (dws->dockWidget);
            removeDockWidget (dws->dockWidget);
         }
      }
      
      dwList = _dockWidgetTable.get_next (it);
   }
   
   QWidget *widget (_centralWidgetTable.lookup (_channel));
   if (widget) { _ui.stackedWidget->setCurrentWidget (widget); }
}


void
dmz::QtModuleMainWindowBasic::_save_session () {

   String data;

   Config session (get_plugin_name ());

   session.add_config (qbytearray_to_config ("geometry", saveGeometry ()));
   session.add_config (qbytearray_to_config ("state", saveState ()));

   set_session_config (get_plugin_runtime_context (), session);
}


void
dmz::QtModuleMainWindowBasic::_load_session () {

   Config session (get_session_config (get_plugin_name (), get_plugin_runtime_context ()));

   QByteArray geometry (config_to_qbytearray ("geometry", session, saveGeometry ()));
   restoreGeometry (geometry);

   QByteArray stateData (config_to_qbytearray ("state", session, saveState ()));
   restoreState (stateData);
}


void
dmz::QtModuleMainWindowBasic::_init (Config &local) {

   _windowName =
      config_to_string ("window.title", local, "DMZ Application").get_buffer ();
   
   if (!_windowName.isEmpty ()) { setWindowTitle (_windowName); }

   String iconFile (config_to_string ("window.icon", local, ":/images/windowIcon.png"));

   if (iconFile) {
      
      QFileInfo fi (iconFile.get_buffer ());

      if (fi.exists ()) {

         setWindowIcon (QIcon (fi.absoluteFilePath ()));
         _log.info << "Window Icon: " << iconFile << endl;
      }
   }
   
   init_input_channels (local, InputEventChannelStateMask, &_log);
   
   _showUnifiedTitleAndToolBar = config_to_boolean (
      "showUnifiedTitleAndToolBar.value",
      local, _showUnifiedTitleAndToolBar);
   
   set_qwidget_stylesheet ("stylesheet", local, this);
}


extern "C" {

DMZ_PLUGIN_FACTORY_LINK_SYMBOL dmz::Plugin *
create_dmzQtModuleMainWindowBasic (
      const dmz::PluginInfo &Info,
      dmz::Config &local,
      dmz::Config &global) {

   return new dmz::QtModuleMainWindowBasic (Info, local);
}

};