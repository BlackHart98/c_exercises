#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


void
get_data_as_hex_text_fn(const unsigned int *data, int dataSize, char *output_buf);

char text[128] = { 0 }; // text box static buffer

int
main(void)
{
    const int screen_width = 800;
    const int screen_height = 450;
    InitWindow(screen_width, screen_height, "raylib [core] example - compute hash");
    {
        char text_input[96] = "The quick brown fox jumps over the lazy dog.";
        bool text_box_edit_mode = false;
        bool btn_compute_hashes = false;

        // Data hash values
        unsigned int hash_CRC32 = 0;
        unsigned int *hash_MD5 = NULL;
        unsigned int *hash_SHA1 = NULL;
        unsigned int *hash_SHA256 = NULL;

        // Base64 encoded data
        char *base64_text = NULL;
        int base64_text_size = 0;

        char *output_buffer = text;
        SetTargetFPS(60);
        while (!WindowShouldClose()) {
            if (btn_compute_hashes) {
                int text_input_len = (int)strlen(text_input);
                base64_text = EncodeDataBase64((unsigned char*)text_input, text_input_len, &base64_text_size);
                hash_CRC32 = ComputeCRC32((unsigned char *)text_input, text_input_len);
                hash_MD5 = ComputeMD5((unsigned char*)text_input, text_input_len);
                hash_SHA1 = ComputeSHA1((unsigned char*)text_input, text_input_len);
                hash_SHA256 = ComputeSHA256((unsigned char*)text_input, text_input_len);
            }

            BeginDrawing();
            {
                ClearBackground(RAYWHITE);
                GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
                GuiSetStyle(DEFAULT, TEXT_SPACING, 2);
                GuiLabel((Rectangle){ 40, 26, 720, 32 }, "INPUT DATA (TEXT):");
                GuiSetStyle(DEFAULT, TEXT_SPACING, 1);
                GuiSetStyle(DEFAULT, TEXT_SIZE, 10);

                if (GuiTextBox((Rectangle){ 40, 64, 720, 32 }, text_input, 95, text_box_edit_mode)) text_box_edit_mode = !text_box_edit_mode;

                btn_compute_hashes = GuiButton((Rectangle){ 40, 64 + 40, 720, 32 }, "COMPUTE INPUT DATA HASHES");

                GuiSetStyle(DEFAULT, TEXT_SIZE, 20);
                GuiSetStyle(DEFAULT, TEXT_SPACING, 2);
                GuiLabel((Rectangle){ 40, 160, 720, 32 }, "INPUT DATA HASH VALUES:");
                GuiSetStyle(DEFAULT, TEXT_SPACING, 1);
                GuiSetStyle(DEFAULT, TEXT_SIZE, 10);

                GuiSetStyle(TEXTBOX, TEXT_READONLY, 1);
                GuiLabel((Rectangle){ 40, 200, 120, 32 }, "CRC32 [32 bit]:");
                get_data_as_hex_text_fn(&hash_CRC32, 1, output_buffer);
                GuiTextBox((Rectangle){ 40 + 120, 200, 720 - 120, 32 }, output_buffer, 120, false);
                GuiLabel((Rectangle){ 40, 200 + 36, 120, 32 }, "MD5 [128 bit]:");
                get_data_as_hex_text_fn(hash_MD5, 4, output_buffer);
                GuiTextBox((Rectangle){ 40 + 120, 200 + 36, 720 - 120, 32 }, output_buffer, 120, false);
                GuiLabel((Rectangle){ 40, 200 + 36*2, 120, 32 }, "SHA1 [160 bit]:");
                get_data_as_hex_text_fn(hash_SHA1, 5, output_buffer);
                GuiTextBox((Rectangle){ 40 + 120, 200 + 36*2, 720 - 120, 32 }, output_buffer, 120, false);
                GuiLabel((Rectangle){ 40, 200 + 36*3, 120, 32 }, "SHA256 [256 bit]:");
                get_data_as_hex_text_fn(hash_SHA256, 8, output_buffer);
                GuiTextBox((Rectangle){ 40 + 120, 200 + 36*3, 720 - 120, 32 }, output_buffer, 120, false);

                GuiSetState(STATE_FOCUSED);
                GuiLabel((Rectangle){ 40, 200 + 36*5 - 30, 320, 32 }, "BONUS - BAS64 ENCODED STRING:");
                GuiSetState(STATE_NORMAL);
                GuiLabel((Rectangle){ 40, 200 + 36*5, 120, 32 }, "BASE64 ENCODING:");
                GuiTextBox((Rectangle){ 40 + 120, 200 + 36*5, 720 - 120, 32 }, base64_text, 120, false);
                GuiSetStyle(TEXTBOX, TEXT_READONLY, 0);
            }
            EndDrawing();
        }
        MemFree(base64_text); 
    }
    CloseWindow();
    return 0;
}

void
get_data_as_hex_text_fn(const unsigned int *data, int dataSize, char *output_buf)
{
    memset(output_buf, 0, 128);
    if ((data != NULL) && (dataSize > 0) && (dataSize < ((128/8) - 1))){
        for (int i = 0; i < dataSize; i++) 
            TextCopy(output_buf + i*8, TextFormat("%08X", data[i]));
    }
    else TextCopy(output_buf, "00000000");

}