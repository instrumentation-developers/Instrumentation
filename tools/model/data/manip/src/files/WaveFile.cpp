#include "WaveFile.h"

WaveFile::WaveFile():WaveFile(
  std::vector<char>{},
  2,
  44100,
  2,
  WaveEncoding::PCM) {}

WaveFile::WaveFile(
  std::vector<char> data,
  unsigned int num_channels,
  unsigned int sample_rate,
  unsigned int bytes_per_sample,
  unsigned int encoding_type) {
    set_data(data, num_channels, sample_rate, bytes_per_sample, encoding_type);
}

WaveFile::WaveFile(
  std::vector<std::vector<char>> data,
  unsigned int sample_rate,
  unsigned int bytes_per_sample,
  unsigned int encoding_type) {
    set_data(data, sample_rate, bytes_per_sample, encoding_type);
}

void WaveFile::read(std::string file_name) {
  /*erase data*/
  erase_data();

  unsigned char buffer4[4];
  unsigned char buffer2[2];

  FILE *file_ptr = fopen(file_name.c_str(), "rb");

  if (file_ptr == NULL) {
    std::cout << "ERROR: Unable to open file \"" << file_name << "\".\n";
    return;
  }

  /*read header information*/
  fread(header.riff_header, sizeof(header.riff_header), 1, file_ptr);

  fread(buffer4, sizeof(buffer4), 1, file_ptr);
  btoi(buffer4, 4, header.bytes_in_file);

  fread(header.wave_string, sizeof(header.wave_string), 1, file_ptr);

  fread(header.fmt_chunk_header, sizeof(header.fmt_chunk_header), 1, file_ptr);

  fread(buffer4, sizeof(buffer4), 1, file_ptr);
  btoi(buffer4, 4, header.bytes_in_fmt);

  fread(buffer2, sizeof(buffer2), 1, file_ptr);
  btoi(buffer2, 2, header.encoding_type);

  fread(buffer2, sizeof(buffer2), 1, file_ptr);
  btoi(buffer2, 2, header.num_channels);

  fread(buffer4, sizeof(buffer4), 1, file_ptr);
  btoi(buffer4, 4, header.sample_rate);

  fread(buffer4, sizeof(buffer4), 1, file_ptr);
  btoi(buffer4, 4, header.byte_rate);

  fread(buffer2, sizeof(buffer2), 1, file_ptr);
  btoi(buffer2, 2, header.block_align);

  fread(buffer2, sizeof(buffer2), 1, file_ptr);
  btoi(buffer2, 2, header.bits_per_sample);

  fread(header.data_chunk_header, sizeof( header.data_chunk_header), 1, file_ptr);

  fread(buffer4, sizeof(buffer4), 1, file_ptr);
  btoi(buffer4, 4, header.bytes_in_data);

  /*initialize properties*/
  properties.bytes_per_sample = header.bits_per_sample / BITS_PER_BYTE;
  properties.num_samples = header.bytes_in_data / (properties.bytes_per_sample * header.num_channels);
  properties.duration = properties.num_samples / header.sample_rate;

  /*read data*/
  std::vector<std::vector<char>> channel_vectors(header.num_channels, std::vector<char>{});
  this -> data = channel_vectors;

  if (header.encoding_type == WaveEncoding::PCM) {
    char data_buffer[properties.bytes_per_sample];

    while (!feof(file_ptr)) {
      for (unsigned channel = 0; channel < header.num_channels; channel++) {
        fread(data_buffer, sizeof(data_buffer), 1, file_ptr);
        if (!ferror(file_ptr)) {
            for (unsigned i = 0; i < properties.bytes_per_sample; i++) {
              data[channel].push_back(data_buffer[i]);
            }
        } else {
          break;
        }
      }
    }
  }
  fclose(file_ptr);
}

void WaveFile::write(std::string file_name) {
  unsigned char buffer4[4];
  unsigned char buffer2[2];

  FILE *file_ptr = fopen(file_name.c_str(), "wb");

  if (file_ptr == NULL) {
    std::cout << "ERROR: Unable to open file \"" << file_name << "\".\n";
    return;
  }

  /*write header information*/
  fwrite(header.riff_header, sizeof(header.riff_header), 1, file_ptr);

  itob(header.bytes_in_file, buffer4, 4);
  fwrite(buffer4, sizeof(buffer4), 1, file_ptr);

  fwrite(header.wave_string, sizeof(header.wave_string), 1, file_ptr);

  fwrite(header.fmt_chunk_header, sizeof(header.fmt_chunk_header), 1, file_ptr);

  itob(header.bytes_in_fmt, buffer4, 4);
  fwrite(buffer4, sizeof(buffer4), 1, file_ptr);

  itob(header.encoding_type, buffer2, 2);
  fwrite(buffer2, sizeof(buffer2), 1, file_ptr);

  itob(header.num_channels, buffer2, 2);
  fwrite(buffer2, sizeof(buffer2), 1, file_ptr);

  itob(header.sample_rate, buffer4, 4);
  fwrite(buffer4, sizeof(buffer4), 1, file_ptr);

  itob(header.byte_rate, buffer4, 4);
  fwrite(buffer4, sizeof(buffer4), 1, file_ptr);

  itob(header.block_align, buffer2, 2);
  fwrite(buffer2, sizeof(buffer2), 1, file_ptr);

  itob(header.bits_per_sample, buffer2, 2);
  fwrite(buffer2, sizeof(buffer2), 1, file_ptr);

  fwrite(header.data_chunk_header, sizeof( header.data_chunk_header), 1, file_ptr);

  itob(header.bytes_in_data, buffer4, 4);
  fwrite(buffer4, sizeof(buffer4), 1, file_ptr);

  /*write data*/
  if (header.encoding_type == WaveEncoding::PCM) {
    char data_buffer[properties.bytes_per_sample];

    for (unsigned int sample = 0; sample < properties.num_samples; sample++) {
      for (unsigned channel = 0; channel < header.num_channels; channel++) {
        for (unsigned int byte_index = 0; byte_index < properties.bytes_per_sample; byte_index++) {
          data_buffer[byte_index] = data[channel][properties.bytes_per_sample * sample + byte_index];
        }
        fwrite(data_buffer, sizeof(data_buffer), 1, file_ptr);
      }
    }
  }
  fclose(file_ptr);
}

void WaveFile::split(int duration, std::vector<WaveFile*> & clips) {
  unsigned int count = properties.duration / duration;
  unsigned int num_samples = properties.num_samples / count;

  unsigned int sample = 0;
  for (unsigned int clip = 0; clip < count; clip++) {

    std::vector<std::vector<char>> clip_data(header.num_channels, std::vector<char>{});
    for (; sample < (clip + 1) * num_samples; sample++) {
      for (unsigned int channel = 0; channel < header.num_channels; channel++) {
        for (unsigned int byte_index = 0; byte_index < properties.bytes_per_sample; byte_index++) {
          clip_data[channel].push_back(data[channel][properties.bytes_per_sample * sample + byte_index]);
        }
      }
    }
    clips.push_back(new WaveFile(clip_data, header.sample_rate, properties.bytes_per_sample, header.encoding_type));
  }
}

void WaveFile::erase_data() {
    data.clear();
    header.bytes_in_file = WaveHeader::DEFAULT_BYTES_IN_FILE;
    header.bytes_in_data = 0;

    header.encoding_type = WaveHeader::DEFAULT_ENCODING_TYPE;
    header.num_channels = WaveHeader::DEFAULT_NUM_CHANNELS;
    header.sample_rate = WaveHeader::DEFAULT_SAMPLE_RATE;
    header.byte_rate = WaveHeader::DEFAULT_BYTE_RATE;

    header.block_align = header.byte_rate / header.sample_rate;

    header.bits_per_sample =
    (header.block_align / header.num_channels) * BITS_PER_BYTE;

    std::vector<std::vector<char>> channel_vectors(header.num_channels, std::vector<char>{});
    this -> data = channel_vectors;
}

void WaveFile::to_mono() {
  unsigned int num_samples = header.bytes_in_data / (properties.bytes_per_sample * header.num_channels);

  std::vector<char> sampled_data;

  for (unsigned int sample = 0; sample < num_samples; sample++) {
    for (unsigned int byte_index = 0; byte_index < properties.bytes_per_sample; byte_index++) {
      int new_sample = 0;

      for (int channel = 0; channel < header.num_channels; channel++) {
          new_sample = data[channel][properties.bytes_per_sample * sample + byte_index];
      }
      //new_sample /= header.num_channels;
      sampled_data.push_back((char)new_sample);
    }
  }

  set_data(std::vector<std::vector<char>>{sampled_data}, header.sample_rate, properties.bytes_per_sample, header.encoding_type);
}

void WaveFile::resample(int sample_rate) {
  if (sample_rate == header.sample_rate) {
    return;
  }

  unsigned int sample_step = (unsigned int) (header.sample_rate / sample_rate);

  std::vector<std::vector<char>> sampled_data(header.num_channels, std::vector<char>{});

  for (unsigned int sample = 0; sample < properties.num_samples; sample += sample_step) {
    for (unsigned int channel = 0; channel < header.num_channels; channel++) {
      for (unsigned int byte_index = 0; byte_index < properties.bytes_per_sample; byte_index++) {
          sampled_data[channel].push_back(data[channel][properties.bytes_per_sample * sample + byte_index]);
      }
    }
  }

  set_data(sampled_data, sample_rate, properties.bytes_per_sample, header.encoding_type);
}

/*accessors*/
std::vector<std::vector<char>> WaveFile::get_data() {
  return this -> data;
}
std::vector<char> WaveFile::get_combined_data() {

  std::vector<char> combined_data;
  for (unsigned int sample = 0; sample < properties.num_samples; sample++) {
    for (unsigned int channel = 0; channel < header.num_channels; channel++) {
      for (unsigned int byte_index = 0; byte_index < properties.bytes_per_sample; byte_index++) {
        combined_data.push_back(data[channel][properties.bytes_per_sample * sample + byte_index]);
      }
    }
  }

  return combined_data;
}

unsigned int WaveFile::get_bytes_in_file() {
  return header.bytes_in_file;
}
unsigned int WaveFile::get_encoding_type() {
  return header.encoding_type;
}
unsigned int WaveFile::get_num_channels() {
  return header.num_channels;
}
unsigned int WaveFile::get_sample_rate() {
  return header.sample_rate;
}

/*mutators*/
void WaveFile::set_data(
  std::vector<char> data,
  unsigned int num_channels,
  unsigned int sample_rate,
  unsigned int bytes_per_sample,
  unsigned int encoding_type) {
    /*modify all invalid values to defaults*/

    if (num_channels <= 0) {
      num_channels = WaveHeader::DEFAULT_NUM_CHANNELS;
    }

    if (sample_rate <= 0) {
      sample_rate = WaveHeader::DEFAULT_SAMPLE_RATE;
    }

    if (bytes_per_sample <= 0) {
      bytes_per_sample =
      WaveHeader::DEFAULT_BYTE_RATE / (WaveHeader::DEFAULT_SAMPLE_RATE * WaveHeader::DEFAULT_NUM_CHANNELS);
    }

    if (encoding_type == UNDEFINED) {
      encoding_type = WaveEncoding::PCM;
    }

    /*initialize header information*/

    /*data chunk*/
    header.bytes_in_data = data.size();

    /*format chunk*/
    header.bits_per_sample = bytes_per_sample * BITS_PER_BYTE;
    header.block_align = num_channels * bytes_per_sample;
    header.byte_rate = num_channels * sample_rate * bytes_per_sample;
    header.sample_rate = sample_rate;
    header.encoding_type = encoding_type;
    header.num_channels = num_channels;

    header.bytes_in_file = WaveHeader::DEFAULT_BYTES_IN_FILE + header.bytes_in_data;

    /*initialize properties*/
    properties.bytes_per_sample = bytes_per_sample;
    properties.num_samples = header.bytes_in_data / (properties.bytes_per_sample * header.num_channels);
    properties.duration = properties.num_samples / header.sample_rate;

    /*parse data*/
    std::vector<std::vector<char>> channel_vectors(header.num_channels, std::vector<char>{});
    this -> data = channel_vectors;

    for (unsigned int sample = 0; sample < properties.num_samples; sample++) {
      for (unsigned int channel = 0; channel < header.num_channels; channel++) {
        for (unsigned int byte_index = 0; byte_index < properties.bytes_per_sample; byte_index++) {
          this -> data[channel].push_back(data[header.block_align * sample + properties.bytes_per_sample * channel + byte_index]);
        }
      }
    }
}

void WaveFile::set_data(
  std::vector<std::vector<char>> data,
  unsigned int sample_rate,
  unsigned int bytes_per_sample,
  unsigned int encoding_type) {

    /*modify all invalid values to defaults*/

    unsigned int num_channels = data.size();

    if (num_channels <= 0) {
      num_channels = WaveHeader::DEFAULT_NUM_CHANNELS;
      header.bytes_in_data = 0;
    } else {
      header.bytes_in_data = data[0].size() * num_channels;
    }

    if (sample_rate <= 0) {
      sample_rate = WaveHeader::DEFAULT_SAMPLE_RATE;
    }

    if (bytes_per_sample <= 0) {
      bytes_per_sample =
      WaveHeader::DEFAULT_BYTE_RATE / (WaveHeader::DEFAULT_SAMPLE_RATE * WaveHeader::DEFAULT_NUM_CHANNELS);
    }

    if (encoding_type == UNDEFINED) {
      encoding_type = PCM;
    }

    /*initialize header information*/

    /*format chunk*/
    header.bits_per_sample = bytes_per_sample * BITS_PER_BYTE;
    header.block_align = num_channels * bytes_per_sample;
    header.byte_rate = num_channels * sample_rate * bytes_per_sample;
    header.sample_rate = sample_rate;
    header.encoding_type = encoding_type;
    header.num_channels = num_channels;

    header.bytes_in_file = WaveHeader::DEFAULT_BYTES_IN_FILE + header.bytes_in_data;

    /*initialize properties*/
    properties.bytes_per_sample = bytes_per_sample;
    properties.num_samples = header.bytes_in_data / (properties.bytes_per_sample * header.num_channels);
    properties.duration = properties.num_samples / header.sample_rate;

    /*parse data*/
    std::vector<std::vector<char>> channel_vectors(header.num_channels, std::vector<char>{});
    this -> data = channel_vectors;

    for (unsigned int sample = 0; sample < properties.num_samples; sample++) {
      for (unsigned int channel = 0; channel < header.num_channels; channel++) {
        for (unsigned int byte_index = 0; byte_index < properties.bytes_per_sample; byte_index++) {
          this -> data[channel].push_back(data[channel][properties.bytes_per_sample * sample + byte_index]);
        }
      }
    }
}

/*helper functions*/
WaveEncoding get_encoding_from_string(std::string encoding_string) {
  if (encoding_string == "PCM") {
    return PCM;
  }

  if (encoding_string == "A_LAW") {
    return A_LAW;
  }

  if (encoding_string == "MU_LAW") {
    return MU_LAW;
  }

  return UNDEFINED;
}
