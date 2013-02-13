#include <thread>
#include <cmath>
#include "aweEngine.h"
#include "awesndfile.h"

namespace awe {

    /* read data from SNDFILE into  */
    void readData ( Asample* sample, SNDFILE* sndf, SF_INFO* SFinfo )
    {
        // Two buffers needed for overclip bug workaround
        AiBuffer* bufi = new AiBuffer (SFinfo->channels, SFinfo->frames, false);
        AfBuffer* buff = new AfBuffer (SFinfo->channels, SFinfo->frames, true);

        // Read as float
        sf_readf_float (sndf, buff->getContainer().data(), SFinfo->frames);

        // Find peak sample value in file.
        double peakValue;

#ifdef AWE_ENGINE_USE_CXX11_ALGORITHM
        // C++11 solution, but leads to popping sounds.
        if (sf_command(sndf, SFC_GET_SIGNAL_MAX, &peakValue, sizeof(peakValue)) == SF_FALSE)
        {
            // Peak value not provided by file, find manually.
            auto temp = std::minmax_element(buff->begin(), buff->end());
            auto list = { 1.0, fabs(*temp.first), fabs(*temp.second) };
            peakValue = std::max(list);
        }
#else
        // Simpler classic solution, possibly slower
        if (sf_command(sndf, SFC_GET_SIGNAL_MAX, &peakValue, sizeof(peakValue)) == SF_FALSE)
        {
            // Peak value not provided by file, calculate ourself.
            peakValue = 1.0;

            for (const Afloat &x : *buff) {
                if (fabs(x) > peakValue)
                    peakValue = fabs(x);
            }
        }
#endif

        // Fix and copy buffer
        size_t i = 0;
        for (const Afloat &x : *buff) {
            bufi->push_back(to_Aint(x / static_cast<float>(peakValue)));
            i++;
        }

        // Save buffer into sample, clean up and return
        sample->setBuffer(bufi, peakValue);

        delete buff;
        sf_close(sndf);

        return;
    }


    Asample* AEngine::readFromFile ( const char* file, bool readNow )
    {
        SF_INFO* SFinfo = new SF_INFO;
        SNDFILE* sndf;

        sndf = sf_open(file, SFM_READ, SFinfo);

        if (sf_error(sndf) != SF_ERR_NO_ERROR) {
            fprintf(stderr,"libsndfile [error] %s: %s.\n", file, sf_strerror(sndf));
            sf_close(sndf);
            return nullptr;
        }

        if (SFinfo->channels > 2) {
            fprintf(stderr,"libawe [error] %s: libawe currently only supports mono and stereo. \n", file);
            sf_close(sndf);
            return nullptr;
        }

        Asample* sample = new Asample (file, 0, SFinfo->samplerate);

        if (readNow) {
            readData(sample, sndf, SFinfo);
        } else {
            std::thread readThread(readData, sample, sndf, SFinfo);
            readThread.detach();
        }

        return sample;
    }


    Asample* AEngine::readFromMemory ( const char* name, char* mptr, size_t size, bool readNow )
    {
        SF_INFO* SFinfo = new SF_INFO ();
        SNDFILE* sndf;
        SF_VIRTUAL_IO       SFvio;
        awe_sf_vmio_data    SFvmio;

        SFvio.get_filelen = awe_sf_vmio_get_filelen;
        SFvio.tell  = awe_sf_vmio_tell;
        SFvio.seek  = awe_sf_vmio_seek;
        SFvio.read  = awe_sf_vmio_read;
        SFvio.write = awe_sf_vmio_write;

        SFvmio.size = size;
        SFvmio.curr = 0;
        SFvmio.mptr = mptr;

        sndf = sf_open_virtual(&SFvio, SFM_READ, SFinfo, (void*)&SFvmio);

        if (sf_error(sndf) != SF_ERR_NO_ERROR) {
            fprintf(stderr,"libsndfile [error] %p: %s.\n", mptr, sf_strerror(sndf));
            sf_close(sndf);
            return nullptr;
        }

        if (SFinfo->channels > 2) {
            fprintf(stderr,"libawe [error] %p: libawe currently only supports mono and stereo. \n", mptr);
            sf_close(sndf);
            return nullptr;
        }

        Asample* sample = new Asample (name, 0, SFinfo->samplerate);

        if (readNow) {
            readData(sample, sndf, SFinfo);
        } else {
            std::thread readThread(readData, sample, sndf, SFinfo);
            readThread.detach();
        }

        return sample;
    }

    /** @TODO: sequencial update HERE. */
    bool AEngine::update()
    {
        // Engine and mixer must be ready
        if (outputDevice->paCalled() && (masterOutput->trackState == PAUSED || masterOutput->trackState == STOPPED))
        {
            this->push();
            return true;
        } else {
            masterOutput->work();
            return false;
        }
    }

    void AEngine::push()
    {
        unsigned lag = outputDevice->fplay(masterOutput->dine());

        while (lag --> 0)
            masterOutput->skip();
    }

};
