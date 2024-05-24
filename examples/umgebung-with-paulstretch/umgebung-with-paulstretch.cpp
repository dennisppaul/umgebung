#include <queue>

#include "Umgebung.h"

using namespace umgebung;

#include "KlangWellen.h"
#include "PaulStretch.h"
#include "AudioFileReader.h"

class UmgebungApp : public PApplet {
    PaulStretch     fStretcher{8, (int) (DEFAULT_AUDIO_SAMPLE_RATE * 0.25f), DEFAULT_AUDIO_SAMPLE_RATE};
    AudioFileReader fAudioFileReader;
    float*            fStretchedSampleBuffer;
    std::queue<float> fSampleFIFOBuffer;

    void settings() {
        size(1024, 768);
    }

    void setup() {
        fAudioFileReader.open("../audio-input.wav");
        fStretchedSampleBuffer = new float[fStretcher.get_output_buffer_size()];
    }

    void draw() {
        background(1);
        const int   mPadding  = 10;
        const float mProgress = (float) fAudioFileReader.current_position() / (float) fAudioFileReader.length();
        stroke(0);
        noFill();
        rect(mPadding, height * 0.5 - mPadding, width - mPadding * 2, mPadding * 2);
        fill(0);
        noStroke();
        rect(mPadding, height * 0.5 - mPadding, (width - mPadding * 2) * mProgress, mPadding * 2);
    }

    void refill_fifo() {
        int mNumRequiredSamples = fStretcher.get_required_samples();
        if (mNumRequiredSamples > 0) {
            float* mInputBuffer = new float[mNumRequiredSamples];
            int    mReadFrames  = fAudioFileReader.read(mNumRequiredSamples, mInputBuffer);
            if (mReadFrames == mNumRequiredSamples) {
                fStretcher.fill_input_buffer(mInputBuffer, mNumRequiredSamples);
            } else {
                for (int i = 0; i < mReadFrames; i++) {
                    fStretcher.get_input_buffer()[i] = mInputBuffer[i];
                }
                fAudioFileReader.rewind();
                int mMissingSamples = mNumRequiredSamples - mReadFrames;
                mReadFrames         = fAudioFileReader.read(mMissingSamples, mInputBuffer);
                if (mReadFrames == mMissingSamples) {
                    for (int i = 0; i < mMissingSamples; i++) {
                        fStretcher.get_input_buffer()[mReadFrames + i] = mInputBuffer[i];
                    }
                }
            }
            delete[] mInputBuffer;
        }

        fStretcher.process_segment(fStretchedSampleBuffer);
        for (int i = 0; i < fStretcher.get_output_buffer_size(); i++) {
            fSampleFIFOBuffer.push(fStretchedSampleBuffer[i]);
        }
    }

    void audioblock(float** input, float** output, int length) {
        for (int i = 0; i < length; i++) {
            if (fSampleFIFOBuffer.empty()) {
                refill_fifo();
            }
            const float mSample = fSampleFIFOBuffer.front();
            fSampleFIFOBuffer.pop();
            for (int j = 0; j < audio_output_channels; ++j) {
                output[j][i] = mSample;
            }
        }
    }


    void finish() {
        delete[] fStretchedSampleBuffer;
        fAudioFileReader.close();
    }
};

PApplet* umgebung::instance() {
    return new UmgebungApp();
}