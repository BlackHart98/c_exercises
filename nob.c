
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

    struct{
        const char *src_file;
        const char *build_file;
    } objects [] = {
        {.src_file = "code_gym/entity_component.c", .build_file = BUILD_FOLDER"entity_component"},
        {.src_file = "code_gym/game_inputs.c", .build_file = BUILD_FOLDER"game_inputs"},
        {.src_file = "code_gym/game_collision.c", .build_file = BUILD_FOLDER"game_collision"},
        {.src_file = "code_gym/game_textures.c", .build_file = BUILD_FOLDER"game_textures"},
        {.src_file = "code_gym/game_text.c", .build_file = BUILD_FOLDER"game_text"},
        {.src_file = "code_gym/game_audio.c", .build_file = BUILD_FOLDER"game_audio"},
        {.src_file = "code_gym/first_game_intro.c", .build_file = BUILD_FOLDER"first_game_intro"},
        {.src_file = "code_gym/creating_blocks.c", .build_file = BUILD_FOLDER"creating_blocks"},
    };
    for (int i = 0; i < NOB_ARRAY_LEN(objects); i++){
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
        nob_cc_inputs(&cmd, "code_gym/externals/raylib6/lib/libraylib.a");
        nob_cc_inputs(&cmd, objects[i].src_file);
        nob_cc_output(&cmd, objects[i].build_file);
        if (!nob_cmd_run(&cmd)) return 1;
    }


    nob_log(NOB_INFO, "Build successfull!");
    return 0;
}