//  aweBuffer.h
//  Copyright 2012 - 2013 Keigen Shu

#ifndef AWE_BUFFER_H
#define AWE_BUFFER_H

#include "aweCompileConfig.h"
#include "aweDefine.h"

namespace awe {

    template <typename T>
        class Abuffer
    {
        private:
            std::vector<T>      data;
            const unsigned char channels;

        public:
            /* STL Vector derived */
            typedef typename std::vector<T>                     container_type;
            typedef typename container_type::size_type          size_type;
            typedef typename container_type::reference          reference;
            typedef typename container_type::const_reference    const_reference;
            typedef typename container_type::iterator           iterator;
            typedef typename container_type::const_iterator     const_iterator;
            
            inline       reference at(const size_type index)       { return data.at(index); }
            inline const_reference at(const size_type index) const { return data.at(index); }
            inline       iterator  begin()       { return data.begin(); }
            inline const_iterator  begin() const { return data.begin(); }
            inline const_iterator cbegin() const { return data.cbegin(); }
            inline       iterator  end()       { return data.end(); }
            inline const_iterator  end() const { return data.end(); }
            inline const_iterator cend() const { return data.cend(); }
            inline       iterator  rbegin()       { return data.rbegin(); }
            inline const_iterator  rbegin() const { return data.rbegin(); }
            inline const_iterator crbegin() const { return data.crbegin(); }
            inline       iterator  rend()       { return data.rend(); }
            inline const_iterator  rend() const { return data.rend(); }
            inline const_iterator crend() const { return data.crend(); }
            inline void push_back(const T&  value) { data.push_back(value); }
            inline void push_back(      T&& value) { data.push_back(value); }
            inline void swap(Abuffer& other) { data.swap(other.getContainer()); }
            /**
             * New buffer constructor
             * @param _channels number of channels on buffer
             * @param _frames   number of frames to reserve
             * @param _init     initialze samples in container or leave it empty?
             */
            Abuffer (const unsigned char &_channels, const size_type &_frames, const bool &_init = true) :
                data(), channels(_channels)
                {
                    if (_init == true) {
                        data.reserve(_channels * _frames);
                        data.resize (_channels * _frames);
                    } else {
                        data.reserve(_channels * _frames);
                    }
                }

            ~Abuffer ()
            {
#ifdef AWE_NUKE_BUFFERS
                data.clear();
#endif
            }
            
            
            inline void reset(const bool _init = true)
            {
                if (_init == true) {
                    size_type samples = data.size();
                    data.clear();
                    data.resize(samples);
                } else {
                    data.clear();
                }
            }
            
            inline       container_type&  getContainer()       { return data; }
            inline const container_type&  getContainer() const { return data; }
            
            inline unsigned  char      getChannelCount() const { return channels;                } /** @return number of channels in the buffer             */
            inline           size_type getBufferSize  () const { return data.size() * sizeof(T); } /** @return size of the entire buffer       (in bytes)   */
            inline           size_type getSampleCount () const { return data.size();             } /** @return number of samples in the buffer (in samples) */
            inline constexpr size_type getSampleSize  () const { return sizeof(T);               } /** @return size of a single sample         (in bytes)   */
            inline           size_type getFrameCount  () const { return data.size() / channels;  } /** @return number of frames in the buffer  (in frames)  */
            inline           size_type getFrameSize   () const { return sizeof(T) * channels;    } /** @return size of a single frame          (in bytes)   */

            inline       T  getSample(const size_type index = 0) const { return (data.size() > index) ? data.at(index) : T(); } /** @return copy of sample in buffer at sample offset index or an uninitialized value if offset is out-of-bounds */
            inline       T* getFrame (const size_type index = 0)       { return (data.size() > index * channels) ? &data.at(index * channels) : nullptr; } /** @return          pointer to frame in buffer at frame offset index or nullptr if offset is out-of-bounds */
            inline const T* getFrame (const size_type index = 0) const { return (data.size() > index * channels) ? &data.at(index * channels) : nullptr; } /** @return constant pointer to frame in buffer at frame offset index or nullptr if offset is out-of-bounds */
    };

    typedef Abuffer<Aint>   AiBuffer;
    typedef Abuffer<Afloat> AfBuffer;

} // namespace awe

#endif // AWE_BUFFER_H
