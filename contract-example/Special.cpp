#include "Special.h"

short int wavREC[900000];
short int wavLIS[900000];
void  PlayRecord(unsigned char* data, int seconds);

short int* RecordMic(int seconds) // just record like 4 3s
{
    memset(wavREC, 0, 44100 * 2 * 2);
    // Fill the WAVEFORMATEX struct to indicate the format of our recorded audio
    //   For this example we'll use "CD quality", ie:  44100 Hz, stereo, 16-bit
    WAVEFORMATEX wfx = {};
    wfx.wFormatTag = WAVE_FORMAT_PCM;       // PCM is standard
    wfx.nChannels = 2;                      // 2 channels = stereo sound
    wfx.nSamplesPerSec = 44100;             // Samplerate.  44100 Hz
    wfx.wBitsPerSample = 16;                // 16 bit samples
    // These others are computations:
    wfx.nBlockAlign = wfx.wBitsPerSample * wfx.nChannels / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;


    // Open our 'waveIn' recording device
    HWAVEIN wi;
    waveInOpen(&wi,            // fill our 'wi' handle
        WAVE_MAPPER,    // use default device (easiest)
        &wfx,           // tell it our format
        NULL, NULL,      // we don't need a callback for this example
        CALLBACK_NULL | WAVE_FORMAT_DIRECT   // tell it we do not need a callback
    );
    WAVEHDR      WaveInHdr;

  //  short int* wavData =(short int* )  malloc(wfx.nSamplesPerSec * seconds);
    // Set up and prepare header for input
    WaveInHdr.lpData = (LPSTR)wavREC;
    WaveInHdr.dwBufferLength = (wfx.nSamplesPerSec * seconds) * 2;
    WaveInHdr.dwBytesRecorded = 0;
    WaveInHdr.dwUser = 0L;
    WaveInHdr.dwFlags = 0L;
    WaveInHdr.dwLoops = 0L;
    waveInPrepareHeader(wi, &WaveInHdr, sizeof(WAVEHDR));

    // Insert a wave input buffer
    waveInAddBuffer(wi, &WaveInHdr, sizeof(WAVEHDR));


    // Commence sampling input
    waveInStart(wi);


    std::cout << "recording..." << std::endl;

    Sleep(seconds * 1000);

    waveInClose(wi);

    // All done!
    //PlayRecord((unsigned char *)wavData, 5 );
    return wavREC;

}

void RecordMicB(int seconds) 
{
    WAVEFORMATEX wfx = {};
    wfx.wFormatTag = WAVE_FORMAT_PCM;       // PCM is standard
    wfx.nChannels = 2;                      // 2 channels = stereo sound
    wfx.nSamplesPerSec = 44100;             // Samplerate.  44100 Hz
    wfx.wBitsPerSample = 16;                // 16 bit samples
    // These others are computations:
    wfx.nBlockAlign = wfx.wBitsPerSample * wfx.nChannels / 8;
    wfx.nAvgBytesPerSec = wfx.nBlockAlign * wfx.nSamplesPerSec;

    // Open our 'waveIn' recording device
    HWAVEIN wi;
    waveInOpen(&wi,            // fill our 'wi' handle
        WAVE_MAPPER,    // use default device (easiest)
        &wfx,           // tell it our format
        NULL, NULL,      // we don't need a callback for this example
        CALLBACK_NULL | WAVE_FORMAT_DIRECT   // tell it we do not need a callback
    );

    // At this point, we have our device, now we need to give it buffers (with headers) that it can
    //  put the recorded audio somewhere
    char buffers[2][44100 * 2 * 2 / 2];    // 2 buffers, each half of a second long
    WAVEHDR headers[2] = { {},{} };           // initialize them to zeros
    for (int i = 0; i < 2; ++i)
    {
        headers[i].lpData = buffers[i];             // give it a pointer to our buffer
        headers[i].dwBufferLength = 44100 * 2 * 2 / 2;      // tell it the size of that buffer in bytes
        // the other parts of the header we don't really care about for this example, and can be left at zero

        // Prepare each header
        waveInPrepareHeader(wi, &headers[i], sizeof(headers[i]));

        // And add it to the queue
        //  Once we start recording, queued buffers will get filled with audio data
        waveInAddBuffer(wi, &headers[i], sizeof(headers[i]));
    }


    // start recording!
    waveInStart(wi);
    // Now that we are recording, keep polling our buffers to see if they have been filled.
    //   If they have been, dump their contents to the file and re-add them to the queue so they
    //   can get filled again, and again, and again

    short* data = (short*)malloc(44100*seconds);

    int targettime = GetTimeStamp() + seconds;

    while (GetTimeStamp() < targettime) 
    {
        for (auto& h : headers)      // check each header
        {
            if (h.dwFlags & WHDR_DONE)           // is this header done?
            {
                // if yes, dump it to our file
                //outfile.write(h.lpData, h.dwBufferLength);
                memcpy(data, h.lpData, h.dwBufferLength);
                // then re-add it to the queue
                h.dwFlags = 0;          // clear the 'done' flag
                h.dwBytesRecorded = 0;  // tell it no bytes have been recorded

                // re-add it  (I don't know why you need to prepare it again though...)
                waveInPrepareHeader(wi, &h, sizeof(h));
                waveInAddBuffer(wi, &h, sizeof(h));
            }
        }
    }


    // Once the user hits escape, stop recording, and clean up
    waveInStop(wi);

    for (auto& h : headers)
    {
        waveInUnprepareHeader(wi, &h, sizeof(h));
    }

    waveInClose(wi);


}

void PlayRecord(unsigned char * data, int seconds)
{
    memcpy(wavLIS, data, 44100 * 2 * seconds);
    const int NUMPTS = 44100 * seconds;   // 3 seconds
    int sampleRate = 44100;
    // 'short int' is a 16-bit type; I request 16-bit samples below
                                // for 8-bit capture, you'd    use 'unsigned char' or 'BYTE' 8-bit types

    HWAVEIN  hWaveIn;

    WAVEFORMATEX pFormat;
    pFormat.wFormatTag = WAVE_FORMAT_PCM;       // PCM is standard
    pFormat.nChannels = 2;                      // 2 channels = stereo sound
    pFormat.nSamplesPerSec = 44100;             // Samplerate.  44100 Hz
    pFormat.wBitsPerSample = 16;                // 16 bit samples
    // These others are computations:
    pFormat.nBlockAlign = pFormat.wBitsPerSample * pFormat.nChannels / 8;
    pFormat.nAvgBytesPerSec = pFormat.nBlockAlign * pFormat.nSamplesPerSec;


    // Specify recording parameters

    waveInOpen(&hWaveIn, WAVE_MAPPER, &pFormat, 0L, 0L, WAVE_FORMAT_DIRECT);

    WAVEHDR      WaveInHdr;
    // Set up and prepare header for input
    WaveInHdr.lpData = (LPSTR)wavLIS;
    WaveInHdr.dwBufferLength = NUMPTS * 2;
    WaveInHdr.dwBytesRecorded = 0;
    WaveInHdr.dwUser = 0L;
    WaveInHdr.dwFlags = 0L;
    WaveInHdr.dwLoops = 0L;
    waveInPrepareHeader(hWaveIn, &WaveInHdr, sizeof(WAVEHDR));

    HWAVEOUT hWaveOut;
    std::cout << "playing..." << std::endl;
    waveOutOpen(&hWaveOut, WAVE_MAPPER, &pFormat, 0, 0, WAVE_FORMAT_DIRECT);
    waveOutWrite(hWaveOut, &WaveInHdr, sizeof(WaveInHdr)); // Playing the data
    Sleep(seconds * 1000); //Sleep for as long as there was recorded
    waveInClose(hWaveIn);
    waveOutClose(hWaveOut);
}

