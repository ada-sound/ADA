/*
A simple sound library for CSE 20211 by Douglas Thain (dthain@nd.edu).
This work is made available under the Creative Commons Attribution license.
https://creativecommons.org/licenses/by/4.0/

For course assignments, you should not change this file.
For complete documentation, see:
http://www.nd.edu/~dthain/courses/cse20211/fall2013/wavfile
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>

struct wavfile_header {
    char riff_tag[4];
    int riff_length;
    char wave_tag[4];
    char fmt_tag[4];
    int fmt_length;
    short audio_format;
    short num_channels;
    int sample_rate;
    int byte_rate;
    short block_align;
    short bits_per_sample;
    char data_tag[4];
    int data_length;
};

FILE* wavfile_open(const char *filename, int sample_rate, int bits_per_sample, int channels) {
    struct wavfile_header header;

    strncpy(header.riff_tag, "RIFF", 4);
    strncpy(header.wave_tag, "WAVE", 4);
    strncpy(header.fmt_tag, "fmt ", 4);
    strncpy(header.data_tag, "data", 4);

    header.riff_length = 0;
    header.fmt_length = 16;
    header.audio_format = 1;
    header.num_channels = channels;
    header.sample_rate = sample_rate;
    header.byte_rate = sample_rate * (bits_per_sample / 8);
    header.block_align = bits_per_sample / 8;
    header.bits_per_sample = bits_per_sample;
    header.data_length = 0;

    FILE* file = fopen(filename, "wb+");
    if (file == NULL)
        return NULL;

    fwrite(&header, sizeof(header), 1, file);

    fflush(file);

    return file;
}

void wavfile_write(FILE *file, short data[], int length) {
    fwrite(data, sizeof(short), length, file);
}

void wavfile_close(FILE *file) {
    int file_length = ftell(file);

    int data_length = file_length - sizeof(struct wavfile_header);
    fseek(file, sizeof(struct wavfile_header) - sizeof(int), SEEK_SET);
    fwrite(&data_length, sizeof(data_length), 1, file);

    int riff_length = file_length - 8;
    fseek(file, 4, SEEK_SET);
    fwrite(&riff_length, sizeof(riff_length), 1, file);

    fclose(file);
}

const int BITS = 16;
const int WAVFILE_SAMPLES_PER_SECOND = 48000;
const int CHANNELS = 1;
const int NUM_SAMPLES = (WAVFILE_SAMPLES_PER_SECOND * CHANNELS);

int main() {
    short waveform[NUM_SAMPLES];
    double frequency = 440.0;
    int volume = 32767;
    int length = NUM_SAMPLES;

    int i;
    for (i = 0; i < length; i++) {
        double t = (double)i / WAVFILE_SAMPLES_PER_SECOND;
        waveform[i] = volume * sin(frequency * t * 2 * M_PI);
    }

    char filename[256];
    sprintf(filename, "%.0fHz-%dk-%db-%s.wav", frequency, WAVFILE_SAMPLES_PER_SECOND/1000, BITS, CHANNELS == 1 ? "mono":"stereo");
    FILE *f = wavfile_open(filename, WAVFILE_SAMPLES_PER_SECOND, BITS, CHANNELS);
    if (f == NULL) {
        fprintf(stderr, "couldn't open sound.wav for writing: %s\n", strerror(errno));
        return 1;
    }

    wavfile_write(f, waveform, length);
    wavfile_close(f);

    return 0;
}
