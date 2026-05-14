
#define NOB_IMPLEMENTATION
#define NOB_FETCH_IMPLEMENTATION
#define NOB_WARN_DEPRECATED
#include "nob.h"

#define BUILD_FOLDER "build/"

int 
main(int argc, char *argv[])
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd cmd = {0};
    Nob_String_View path_sv = nob_sv_from_cstr(argv[2]);

    nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cc_add_include(&cmd, "code_gym/externals/raylib6/include");

#if defined(__APPLE__) || defined(__MACH__)
    nob_cc_add_framework(&cmd, "CoreVideo");
    nob_cc_add_framework(&cmd, "IOKit");
    nob_cc_add_framework(&cmd, "Cocoa");
    nob_cc_add_framework(&cmd, "GLUT");
    nob_cc_add_framework(&cmd, "OpenGL");
#endif
    nob_cc_output(&cmd, BUILD_FOLDER"entity_component");
    nob_cc_inputs(&cmd, "code_gym/externals/raylib6/lib/libraylib.a");
    nob_cc_inputs(&cmd, "code_gym/entity_component.c");
    if (!nob_cmd_run(&cmd)) return 1;


    nob_log(NOB_INFO, "Build successfull!");
    return 0;
}