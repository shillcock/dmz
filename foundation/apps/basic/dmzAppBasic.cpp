#include <dmzCommandLine.h>
#include <dmzApplication.h>
#include <dmzRuntimeLogObserverBasic.h>
#include <dmzSystem.h>

using namespace dmz;

int
main (int argc, char *argv[]) {

   CommandLine cl (argc, argv);

   Application app ("dmzAppBasic", "dmz");

   LogObserverBasic obs (app.get_context ());

   obs.set_level (string_to_log_level (get_env ("DMZ_APP_LOG_LEVEL")));

   app.load_session ();
   app.process_command_line (cl);
   app.load_plugins ();
   app.start ();
   while (app.sync ()) {;}
   app.stop ();
   app.unload_plugins ();
   app.save_session ();

   return app.is_error () ? -1 : 0;
}