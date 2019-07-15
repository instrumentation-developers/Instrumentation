g++ -c ../src/util/Utils.cpp
g++ -c ../src/util/Command.cpp
g++ -c ../src/files/File.cpp
g++ -c ../src/files/WaveFile.cpp
g++ -c ../src/files/BinaryFile.cpp
g++ -c ../src/Wav2Bin.cpp
g++ -c ../src/Bin2Wav.cpp
g++ -o Wav2Bin.exe Utils.o Command.o File.o WaveFile.o BinaryFile.o Wav2Bin.o
g++ -o Bin2Wav.exe Utils.o Command.o File.o WaveFile.o BinaryFile.o Bin2Wav.o
