#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <math.h>

int main(void) {

	const unsigned int sampleRate = 44100;
	const unsigned int numChannels = 2;
	const unsigned int bitsPerSample = 16;
	unsigned int bytesPerSampleSlice = bitsPerSample / 8 * numChannels;
	unsigned int averageBytesPerSec = bytesPerSampleSlice * sampleRate;	
	
	const long unsigned int numSamples = 441000;
	unsigned int dataChunkSize = numSamples * bytesPerSampleSlice;
	unsigned int totalFileSize = dataChunkSize + 30;
	
	unsigned int amplitude = (int)(pow(2, bitsPerSample) / 2) - 1;
	int pitchL = 880;
	int pitchR = 440;

	int audioTempL;
	int audioTempR;
	char audioData[dataChunkSize];

	cout << "Creating new wave data... " << endl;
	for(unsigned int i = 0; i < dataChunkSize; ++i) {

		// get appropriate integer values for each sample, left then right
		// then change integer into little-endian format and store in audioData array
		audioTempL = (int)(amplitude * ( sin ( i * M_PI * pitchL / sampleRate / 2)) );
		audioData[i] = (char)(audioTempL);
		++i;
		audioData[i] = (char)(audioTempL >>  8);
		++i;

		audioTempR = (int)(amplitude * ( sin ( i * M_PI * pitchR / sampleRate) / 2) );
		audioData[i] = (char)(audioTempR);
		++i;
		audioData[i] = (char)(audioTempR >>  8);
		
	} // end for
	cout << "Wave data complete." << endl;	
	
	ofstream wav_file("test.wav", ios::out | ios::binary);
	
	char fileType[] = {'R', 'I', 'F', 'F'};

	char fileSize[] = {    (totalFileSize),
				(totalFileSize >>  8),
				(totalFileSize >> 16),	
				(totalFileSize >> 24)};

	char riffType[] = {'W', 'A', 'V', 'E'};
	char fmtChunk[] = {'f', 'm', 't', ' '};
	char fmtSize[]  = {0x10, 0x00, 0x00, 0x00};
	char compCode[] = {0x01, 0x00};

	char numChannelsWord[] = {      (numChannels),
					(numChannels >>  8) };

	char sampleRateWord[] = {       (sampleRate),
					(sampleRate >>  8),
					(sampleRate >> 16),	
					(sampleRate >> 24)};
	
	char averageBytesPerSecWord[]= {(averageBytesPerSec),
					(averageBytesPerSec >>  8),
					(averageBytesPerSec >> 16),	
					(averageBytesPerSec >> 24)};
	
	char bytesPerSampleSliceWord[] = {      (bytesPerSampleSlice),
						(bytesPerSampleSlice >>  8)};
	
	char bitsPerSampleWord[] = {    (bitsPerSample),
					(bitsPerSample >>  8)};

	char dataChunk[] = {'d', 'a', 't', 'a'};

	char dataChunkSizeWord[]= {     (dataChunkSize),
					(dataChunkSize >>  8),
					(dataChunkSize >> 16),	
					(dataChunkSize >> 24)};


	wav_file.write(fileType, 4);
	wav_file.write(fileSize, 4);
	wav_file.write(riffType, 4);
	wav_file.write(fmtChunk, 4);
	wav_file.write(fmtSize,  4);
	wav_file.write(compCode, 2);
	wav_file.write(numChannelsWord, 2);
	wav_file.write(sampleRateWord,  4);
	wav_file.write(averageBytesPerSecWord, 4);
	wav_file.write(bytesPerSampleSliceWord, 2);
	wav_file.write(bitsPerSampleWord, 2);	
	wav_file.write(dataChunk, 4);
	wav_file.write(dataChunkSizeWord, 4);
	wav_file.write(audioData, dataChunkSize);
	wav_file.close();


} // end main()
