The libawe Engine Manual                                    {#EngineManual}
========================

The \ref awe::AEngine class is used to setup an operating environment to supply
audio data to an output device in real-time. The audio output host communication
functionality is provided by the PortAudio library, which this library
interfaces with through an \ref awe::APortAudio object in this class.

The main function of this class is to provide the application developer a simple
interface to hook libawe's audio data source (\ref awe::Asource) objects into a
single unified audio output stream and then move them into PortAudio's internal
buffering mechanism. This is done by buffering sources from a source pool ahead
of time inside a master output track (\ref awe::AEngine::mOutputDevice) contained
within this class through the \ref awe::AEngine::update() function.

The diagram below illustrates the flow of data from an audio source to the
master output track to the library output queue to the audio output host.
Clicking on the labels on the diagram will bring you to the definitions of the
actual member variable or function of that entity in the library.

\dot
digraph audioflow {
    fontsize=11;
    node[fontsize=7;];
    edge[fontsize=8; penwidth=0.5;];

    subgraph cluster_la {
        label="awe::AEngine"; URL="\ref awe::AEngine";

        subgraph cluster_la_e {
            label="Master Output Track" URL="\ref awe::AEngine::mMasterTrack";

            sp [ label="Source pool"    URL="\ref awe::Atrack::mPsources"; ];
            pb [ label="Pooling buffer" URL="\ref awe::Atrack::mPbuffer"; ];
            ob [ label="Output buffer"  URL="\ref awe::Atrack::mObuffer"; ];

            sp -> pb [ label="Mix sources"  URL="\ref awe::Atrack::pull()"; weight=0.5; ];
            pb -> ob [ label="Swap buffers" URL="\ref awe::Atrack::flip()"; ];

            { rank=same; pb ob; }
        }

        subgraph cluster_la_pa {
            label="PortAudio Interface" URL="\ref awe::AEngine::mOutputDevice";

            oq [ label="Output queue" URL="\ref awe::APortAudio::mOutputQueue" ];
        }

        ob -> oq [ label="Update" URL="\ref awe::AEngine::update()"; weight=0.5; tailport=s headport=e; ];
    }

    subgraph cluster_lpa {
        label="The PortAudio Library";

        cb [ label="PortAudio's internal circular buffer" ];
    }

    oq -> cb [ label="PortAudio callback" URL="\ref awe::PaCallback()"; ];
}
\enddot

Operation details
-----------------
When this object is constructed, an audio output stream will be
automatically configured and then started immediately. A thread
would be spawned by PortAudio which periodically invokes the
aforementioned callback to fill the <em>circular buffer</em> with
data from the <em>output queue</em> until either the <em>output
queue</em> is empty, or if the <em>circular buffer</em> is full.
The former would mean that the engine is not updated often enough,
which causes audio jitters as a result of <em>underflows</em> while
the latter would cause the callback return immediately so that the
output device uses up the buffer and calls the callback later.

Usage details
-------------
A typical application using this class should attach the relevant
audio sources into the master track and then invoke the
\ref update() function to load data into the output buffer.
The PortAudio object is intentionally hidden from the public scope
of this class to avoid unnecessary tinkering.
\note The PortAudio instance within this class has its own mutex
lock that determines the ownership of the is locked by the \ref update()


