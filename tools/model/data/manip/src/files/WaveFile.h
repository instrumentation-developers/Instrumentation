#ifndef WAVEFILE_H
#define WAVEFILE_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <bitset>
#include <cmath>

#include "../util/Utils.h"
#include "../Constants.h"
#include "../files/File.h"

enum WaveEncoding {PCM = 1, A_LAW = 6, MU_LAW = 7, UNDEFINED = -1};
WaveEncoding get_encoding_from_string(std::string encoding_string);

struct WaveHeader {
    //RIFF file format header.
    unsigned char riff_header[4] = {'R', 'I', 'F', 'F'};              //4 bytes.

    //the length of the file (in bytes).
    const static unsigned int DEFAULT_BYTES_IN_FILE = 28;
    unsigned int bytes_in_file = DEFAULT_BYTES_IN_FILE;               //4 bytes.

    //WAVE file format header.
    unsigned char wave_string[4] = {'W', 'A', 'V', 'E'};              //4 bytes.

    /*format chunk*/

    //the format chunk header.
    unsigned char fmt_chunk_header[4] = {'f', 'm', 't', ' '};         //4 bytes.

    //length of the format data.
    const static unsigned int DEFAULT_BYTES_IN_FMT = 16;
    unsigned int bytes_in_fmt = DEFAULT_BYTES_IN_FMT;                 //4 bytes.

    //the encoding type.
    const static unsigned int DEFAULT_ENCODING_TYPE = PCM;
    unsigned int encoding_type = DEFAULT_ENCODING_TYPE;               //2 bytes.

    //the number of channels.
    const static unsigned int DEFAULT_NUM_CHANNELS = 2;
    unsigned int num_channels = DEFAULT_NUM_CHANNELS;                 //2 bytes.

    //the sampling rate (in samples per second).
    const static unsigned int DEFAULT_SAMPLE_RATE = 44100;
    unsigned int sample_rate = DEFAULT_SAMPLE_RATE;                   //4 bytes.

    //the byte rate (num_channels * sample_rate * bytes_per_sample)
    const static unsigned int DEFAULT_BYTE_RATE = 176400;
    unsigned int byte_rate = DEFAULT_BYTE_RATE;                       //4 bytes.

    //the number of bytes per block (num_channels * bytes_per_sample).
    unsigned int block_align = byte_rate / sample_rate;               //2 bytes.

    //the number of bits per sample.
    unsigned int bits_per_sample =
    (block_align / num_channels) * BITS_PER_BYTE;                     //2 bytes.

    /*data chunk*/

    //the data chunk header.
    unsigned char data_chunk_header[4] = {'d', 'a', 't', 'a'};        //4 bytes.

    //the length of the data.
    unsigned int bytes_in_data = 0;                                   //4 bytes.
};

struct WaveProperties {
  //the number of bytes per sample.
  unsigned int bytes_per_sample = 0;

  //the number of samples.
  unsigned int num_samples = 0;

  //the duration.
  unsigned int duration = 0;
};

class WaveFile: public File {
public:
  WaveFile();

  WaveFile(
    std::vector<char> data,
    unsigned int bytes_per_sample,
    unsigned int encoding_type,
    unsigned int num_channels,
    unsigned int sample_rate);

  WaveFile(
    std::vector<std::vector<char>> data,
    unsigned int bytes_per_sample,
    unsigned int encoding_type,
    unsigned int sample_rate);

  /*
  Reads data for this WaveFile object from the specified file.
      <file_name>   - the file to read from.
  */
  void read(std::string file_name);

  /*
  Writes this WaveFile object into the specified file.
      <file_name>   - the name of the file to write to.
  */
  void write(std::string file_name);

  /*
  Splits this WaveFile object into multiple WaveFile objects of the specified duration.
      <duration>    - the duration of each clip.
      <clips>       - the vector of clips.
  */
  void split(int duration, std::vector<WaveFile*> & clips);

  /*
  Resamples the data of this WaveFile object at the specified sample rate.
      <sample_rate>   - the rate at which to sample (in Hz.)
  */
  void resample(int sample_rate);

  /*
  Averages all channels in this WaveFile object into 1 channel.
  */
  void to_mono();

  /*
  Erases all data in this WaveFile.
  */
  void erase_data();

  /*accessors*/
  std::vector<std::vector<char>> get_data();
  std::vector<char> get_combined_data();
  unsigned int get_bytes_in_file();
  unsigned int get_encoding_type();
  unsigned int get_num_channels();
  unsigned int get_sample_rate();

private:
  //header.
  struct WaveHeader header;

  //properties.
  struct WaveProperties properties;

  //data.
  std::vector<std::vector<char>> data;

  /*mutators*/
  void set_data(
    std::vector<std::vector<char>> data,
    unsigned int sample_rate,
    unsigned int bytes_per_sample,
    unsigned int encoding_type);

    void set_data(
      std::vector<char> data,
      unsigned int num_channels,
      unsigned int sample_rate,
      unsigned int bytes_per_sample,
      unsigned int encoding_type);

};

#endif
