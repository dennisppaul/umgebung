#include <queue>

#include "Umgebung.h"

using namespace umgebung;

#define KLANG_SAMPLES_PER_AUDIO_BLOCK DEFAULT_FRAMES_PER_BUFFER
#define KLANG_SAMPLING_RATE DEFAULT_AUDIO_SAMPLE_RATE

#include "KlangWellen.h"
#include "PaulStretch.h"

#define DR_WAV_IMPLEMENTATION
#include "dr_wav.h"

class UmgebungApp : public PApplet {
    PaulStretch       fStretcher{8, (int) (DEFAULT_AUDIO_SAMPLE_RATE * 0.25f), DEFAULT_AUDIO_SAMPLE_RATE};
    drwav             fWAVFile;
    float*            fStretchedSampleBuffer;
    std::queue<float> fSampleFIFOBuffer;

    void settings() {
        size(1024, 768);
    }

    void setup() {
        if (!drwav_init_file(&fWAVFile, "../audio-input.wav", NULL)) {
            std::cerr << "Error opening WAV file." << std::endl;
        }

        fStretchedSampleBuffer = new float[fStretcher.get_output_buffer_size()];
    }

    void draw() {
        background(1);
        const int   mPadding  = 10;
        const float mProgress = (float) fWAVFile.readCursorInPCMFrames / (float) fWAVFile.totalPCMFrameCount;
        stroke(0);
        noFill();
        rect(mPadding, height * 0.5 - mPadding, width - mPadding * 2, mPadding * 2);
        fill(0);
        noStroke();
        rect(mPadding, height * 0.5 - mPadding, (width - mPadding * 2) * mProgress, mPadding * 2);
    }

    void finish() {
        delete[] fStretchedSampleBuffer;
        drwav_uninit(&fWAVFile);
    }

    void refill_fifo() {
        int mNumRequiredSamples = fStretcher.get_required_samples();
        if (mNumRequiredSamples > 0) {
            float*       mInputBuffer = new float[mNumRequiredSamples];
            drwav_uint64 mReadFrames  = drwav_read_pcm_frames_f32(&fWAVFile, mNumRequiredSamples, mInputBuffer);
            if (mReadFrames == mNumRequiredSamples) {
                fStretcher.fill_input_buffer(mInputBuffer, mNumRequiredSamples);
            } else {
                for (int i = 0; i < mReadFrames; i++) {
                    fStretcher.get_input_buffer()[i] = mInputBuffer[i];
                }
                drwav_seek_to_first_pcm_frame(&fWAVFile);
                int mMissingSamples = mNumRequiredSamples - mReadFrames;
                mReadFrames         = drwav_read_pcm_frames_f32(&fWAVFile, mMissingSamples, mInputBuffer);
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

    //        float*       mFrames     = new float[length];
    //        drwav_uint64 mReadFrames = drwav_read_pcm_frames_f32(&fWAVFile, length, mFrames);
    //        if (mReadFrames == length) {
    //            for (int i = 0; i < length; i++) {
    //                float mSample = mFrames[i];
    //                //            float mSample = fWavetable.process();
    //                //            mSample       = fADSR.process(mSample);
    //                //            mSample       = fReverb.process(mSample);
    //                for (int j = 0; j < audio_output_channels; ++j) {
    //                    output[j][i] = mSample;
    //                }
    //            }
    //        } else {
    //            for (int i = 0; i < length; i++) {
    //                for (int j = 0; j < audio_output_channels; ++j) {
    //                    output[j][i] = 0.0f;
    //                }
    //            }
    //            drwav_seek_to_first_pcm_frame(&fWAVFile);
    //            std::cout << "( rewind ): " << mReadFrames << std::endl;
    //        }
    //        delete[] mFrames;
};

PApplet* umgebung::instance() {
    return new UmgebungApp();
}