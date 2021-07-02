#define _USE_MATH_DEFINES

#include <stdio.h>
#include <Windows.h>
#include <math.h>

#pragma comment(lib, "winmm.lib")

struct WAVHEADER
{
	int chunk_id;
	int chunk_size;
	int format;
	int sub1_id;
	int sub1_size;
	short audioFormat;
	short numChannels;
	int sampleRate;
	int byteRate;
	short blockAlign;
	short bitsPerSample;
	int sub2_id;
	int sub2_size;
};

void* MakeData(int size, int samplerate = 44100)
{
	void* data = malloc(size * 16 / 8 + sizeof(WAVHEADER));

	WAVHEADER* wav = (WAVHEADER*)data;
	wav->chunk_id = 0x46464952; // 'RIFF'
	wav->format = 0x45564157; // 'WAVE'
	wav->sub1_id = 0x20746d66; // 'fmt '
	wav->sub1_size = 16;
	wav->audioFormat = 1;
	wav->numChannels = 1; // Mono 1 Stereo 2
	wav->sampleRate = samplerate; //22050 44100
	wav->bitsPerSample = 16;
	wav->blockAlign = wav->numChannels * wav->bitsPerSample / 8;
	wav->byteRate = wav->sampleRate * wav->blockAlign;
	wav->sub2_id = 0x61746164; // 'data'
	wav->sub2_size = wav->blockAlign * size;
	wav->chunk_size = wav->sub2_size + sizeof(WAVHEADER) - 8;
	return data;
}

inline double sinewave(int i, int j, int samplerate = 44100)
{
	return sin(i * M_PI / samplerate * 360 * j / 180);
}

#define Octave 12

inline int freq(int n)
{
	return 440 * pow(2, (n - 9.0) / 12);
}

#define KEYCOUNT 17
#define SAMPLE 44100

void testpiano(void* data, char* note)
{
	for (int i = 0; i < SAMPLE / 2; i++)
	{
		double val = 0;
		for (int j = 0; j < KEYCOUNT; j++)
		{
			if (note[j] == 1)
				val += sinewave(i, freq(j), SAMPLE) * 7500;
		}
		*((short*)((char*)data + sizeof(WAVHEADER)) + i) = val;
	}
	PlaySound((LPCWSTR)data, NULL, SND_ASYNC | SND_MEMORY);
}

int main()
{
	void* data = MakeData(SAMPLE / 2, SAMPLE);
	char notedata[KEYCOUNT] = { 0, };
	char notekey[KEYCOUNT] = {'Q','2','W','3','E','R','5','T','6','Y','7','U','I','9','O','0','P'};
	bool changed = false;
	while (true)
	{
		for (int i = 0; i < KEYCOUNT; i++)
		{
			if (!!GetAsyncKeyState(notekey[i]) != notedata[i])
			{
				changed = true;
				notedata[i] = !notedata[i];
				printf("%c ", notekey[i]);
			}
		}
		if (changed)
			testpiano(data, notedata);
		changed = false;
	}
}

//샌즈 아시는구나!
