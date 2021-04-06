#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define TXT_SIZ(str) (str), 1, strlen(str)

int main(int argc, char* argv[]) {
    // assert wav is 16 bits
    short buf[1024];
    FILE* f = fopen(argv[1], "r");
    printf("#include <stdint.h>\n");
    printf("#include \"audio_sample.h\"\n");
    printf("const uint16_t AUDIO_SAMPLE[] = {\n");
    while(1) {
        printf("\n");
        ssize_t r = fread(buf, 1, sizeof(buf), f);
        if (r == -1 || r == 0)
            break;
        for(int i=0; i< r /sizeof(short); i++) {
            if (i!= 0 && i%16 == 0)
                printf("\n");
            printf("0x%04hx, ", buf[i]);
        }
    }
    printf("};\n");
    printf("WAVE_FormatTypeDef* wav_get_audio_sample() {");
    printf("    return (WAVE_FormatTypeDef*)AUDIO_SAMPLE;");
    printf("}\n");
    fclose(f);
    return 0;
}
