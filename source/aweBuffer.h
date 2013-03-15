//  aweBuffer.h
//  Copyright 2012 - 2013 Keigen Shu

#ifndef AWE_BUFFER_H
#define AWE_BUFFER_H

#include <cassert>
#include "aweCompileConfig.h"
#include "aweDefine.h"

namespace awe {

    template <typename T>
        class Abuffer
    {
        private:
            std::vector<T>      pcm_data;
            const unsigned char channels;

        public:
            /* STL Vector derived */
            typedef typename std::vector<T>                     container_type;
            typedef typename container_type::size_type          size_type;
            typedef typename container_type::reference          reference;
            typedef typename container_type::const_reference    const_reference;
            typedef typename container_type::pointer            pointer;
            typedef typename container_type::const_pointer      const_pointer;
            typedef typename container_type::iterator           iterator;
            typedef typename container_type::const_iterator     const_iterator;
            
            inline       reference at(const size_type index)       { return pcm_data.at(index); }
            inline const_reference at(const size_type index) const { return pcm_data.at(index); }
            inline       iterator  begin()       { return pcm_data.begin(); }
            inline const_iterator  begin() const { return pcm_data.begin(); }
            inline const_iterator cbegin() const { return pcm_data.cbegin(); }
            inline       iterator  end()       { return pcm_data.end(); }
            inline const_iterator  end() const { return pcm_data.end(); }
            inline const_iterator cend() const { return pcm_data.cend(); }
            inline       iterator  rbegin()       { return pcm_data.rbegin(); }
            inline const_iterator  rbegin() const { return pcm_data.rbegin(); }
            inline const_iterator crbegin() const { return pcm_data.crbegin(); }
            inline       iterator  rend()       { return pcm_data.rend(); }
            inline const_iterator  rend() const { return pcm_data.rend(); }
            inline const_iterator crend() const { return pcm_data.crend(); }
            inline       pointer  data()       { return pcm_data.data(); }
            inline const_pointer cdata() const { return pcm_data.data(); }
            inline void push_back(const T&  value) { pcm_data.push_back(value); }
            inline void push_back(      T&& value) { pcm_data.push_back(value); }
            inline void swap(Abuffer& other) { pcm_data.swap(other.getContainer()); } 
            
            /**
             * New buffer constructor
             * @param _channels number of channels on buffer
             * @param _frames   number of frames to reserve
             * @param _init     initialze samples in container or leave it empty?
             */
            Abuffer (const unsigned char &_channels, const size_type &_frames, const bool &_init = true) :
                pcm_data(), channels(_channels)
                {
                    assert(channels != 0);

                    if (_init == true) {
                        pcm_data.reserve(_channels * _frames);
                        pcm_data.resize (_channels * _frames);
                    } else {
                        pcm_data.reserve(_channels * _frames);
                    }
                }

            ~Abuffer ()
            {
#ifdef AWE_NUKE_BUFFERS
                pcm_data.clear();
#endif
            }
            
            
            inline void reset(const bool _init = true)
            {
                if (_init == true) {
                    size_type samples = pcm_data.size();
                    pcm_data.clear();
                    pcm_data.resize(samples);
                } else {
                    pcm_data.clear();
                }
            }
            
            inline       container_type&  getContainer()       { return pcm_data; }
            inline const container_type&  getContainer() const { return pcm_data; }
            
            inline unsigned  char      getChannelCount() const { return channels;                    } /** @return number of channels in the buffer             */
            inline           size_type getBufferSize  () const { return pcm_data.size() * sizeof(T); } /** @return size of the entire buffer       (in bytes)   */
            inline           size_type getSampleCount () const { return pcm_data.size();             } /** @return number of samples in the buffer (in samples) */
            inline const     size_type getSampleSize  () const { return sizeof(T);                   } /** @return size of a single sample         (in bytes)   */
            inline           size_type getFrameCount  () const { return pcm_data.size() / channels;  } /** @return number of frames in the buffer  (in frames)  */
            inline           size_type getFrameSize   () const { return sizeof(T) * channels;        } /** @return size of a single frame          (in bytes)   */

            inline       T       getSample(const size_type index = 0) const { return (pcm_data.size() > index) ? pcm_data.at(index) : T(); } /** @return copy of sample in buffer at sample offset index or an uninitialized value if offset is out-of-bounds */
            inline       pointer getFrame (const size_type index = 0)       { return (pcm_data.size() > index * channels) ? &pcm_data.at(index * channels) : nullptr; } /** @return          pointer to frame in buffer at frame offset index or nullptr if offset is out-of-bounds */
            inline const_pointer getFrame (const size_type index = 0) const { return (pcm_data.size() > index * channels) ? &pcm_data.at(index * channels) : nullptr; } /** @return constant pointer to frame in buffer at frame offset index or nullptr if offset is out-of-bounds */
    };

    typedef Abuffer<Aint>   AiBuffer;
    typedef Abuffer<Afloat> AfBuffer;

} // namespace awe

#endif // AWE_BUFFER_H
