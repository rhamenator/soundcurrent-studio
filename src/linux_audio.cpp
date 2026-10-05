// SPDX-License-Identifier: GPL-3.0-only
#include "linux_audio.h"
#include <pipewire/pipewire.h>
#include <spa/param/audio/format-utils.h>
#include <algorithm>
#include <array>
#include <cstring>
#include <stdexcept>

namespace soundcurrent::studio {
struct LinuxBridge::Impl {
    pw_thread_loop *loop = nullptr;
    pw_stream *capture = nullptr, *playback = nullptr;
    std::unique_ptr<AudioEngine> engine;
    std::unique_ptr<ChannelRouter> router;
    std::vector<float> raw, scratch, ring;
    std::size_t n = 0, read = 0, write = 0, queued = 0;
    static constexpr std::size_t capacity = 48000, block = 1024;
    std::string failure;
    std::vector<float> peaks;
    struct LoopLock {
        pw_thread_loop *loop;
        explicit LoopLock(pw_thread_loop *l):loop(l){pw_thread_loop_lock(loop);}
        ~LoopLock(){pw_thread_loop_unlock(loop);}
    };
    static void state(void *data, pw_stream_state, pw_stream_state now, const char *message) {
        auto &s=*static_cast<Impl *>(data);
        if(now==PW_STREAM_STATE_ERROR)s.failure=message?message:"PipeWire stream failed";
    }
    static void captureProcess(void *data) {
        auto &s=*static_cast<Impl *>(data);auto *buffer=pw_stream_dequeue_buffer(s.capture);if(!buffer)return;
        const auto *b=buffer->buffer;
        if(b->n_datas && b->datas[0].data && b->datas[0].chunk) {
            const auto &d=b->datas[0];const auto offset=std::min(d.chunk->offset,d.maxsize);
            const auto bytes=std::min(d.chunk->size,d.maxsize-offset);
            const auto frames=bytes/(sizeof(float)*s.n);const auto *input=reinterpret_cast<const float *>(static_cast<const unsigned char *>(d.data)+offset);
            for(std::size_t f=0;f<frames;) {
                const auto count=std::min(block,frames-f);auto raw=std::span(s.raw).first(count*s.n);auto output=std::span(s.scratch).first(count*s.n);
                std::memcpy(raw.data(),input+f*s.n,raw.size_bytes());s.router->process(raw,output);s.engine->process(output);
                const auto levels=s.engine->channelPeaks();for(std::size_t c=0;c<s.n;++c)s.peaks[c]=std::max(s.peaks[c],levels[c]);
                for(std::size_t frame=0;frame<count;++frame){if(s.queued==capacity){s.read=(s.read+1)%capacity;--s.queued;}
                    std::copy_n(output.data()+frame*s.n,s.n,s.ring.data()+s.write*s.n);s.write=(s.write+1)%capacity;++s.queued;}
                f+=count;
            }
        }
        pw_stream_queue_buffer(s.capture,buffer);
    }
    static void playbackProcess(void *data) {
        auto &s=*static_cast<Impl *>(data);auto *buffer=pw_stream_dequeue_buffer(s.playback);if(!buffer)return;
        auto *b=buffer->buffer;
        if(b->n_datas && b->datas[0].data && b->datas[0].chunk) {
            auto &d=b->datas[0];auto *output=static_cast<float *>(d.data);
            auto frames=d.maxsize/(sizeof(float)*s.n);if(buffer->requested)frames=std::min<std::size_t>(frames,buffer->requested);
            for(std::size_t f=0;f<frames;++f) {
                if(s.queued){std::copy_n(s.ring.data()+s.read*s.n,s.n,output+f*s.n);s.read=(s.read+1)%capacity;--s.queued;}
                else std::fill_n(output+f*s.n,s.n,0.0f);
            }
            d.chunk->offset=0;d.chunk->stride=std::int32_t(sizeof(float)*s.n);d.chunk->size=std::uint32_t(frames*sizeof(float)*s.n);
            buffer->size=frames;
        }
        pw_stream_queue_buffer(s.playback,buffer);
    }
    static const pw_stream_events &events(bool output) {
        static const pw_stream_events captureEvents=[]{pw_stream_events e{};e.version=PW_VERSION_STREAM_EVENTS;e.state_changed=state;e.process=captureProcess;return e;}();
        static const pw_stream_events playbackEvents=[]{pw_stream_events e{};e.version=PW_VERSION_STREAM_EVENTS;e.state_changed=state;e.process=playbackProcess;return e;}();
        return output?playbackEvents:captureEvents;
    }
};
LinuxBridge::LinuxBridge():impl_(std::make_unique<Impl>()) {pw_init(nullptr,nullptr);}
LinuxBridge::~LinuxBridge(){stop();}
void LinuxBridge::start(const std::string &target,const std::string &sink,const std::string &output,
                        const EngineSettings &settings,std::span<const double> matrix) {
    stop();auto &s=*impl_;s.n=settings.channels.size();
    if(!s.n||s.n>SPA_AUDIO_MAX_CHANNELS)throw std::runtime_error("PipeWire live streams support at most 64 channels; use offline rendering for larger layouts");
    s.engine=std::make_unique<AudioEngine>(48000,s.n);std::string error;
    if(!s.engine->configure(settings,&error))throw std::runtime_error(error);
    s.router=std::make_unique<ChannelRouter>(s.n,s.n);if(!s.router->setMatrix(matrix))throw std::runtime_error("Invalid Studio routing matrix");
    s.raw.resize(Impl::block*s.n);s.scratch.resize(s.raw.size());s.ring.assign(Impl::capacity*s.n,0.0f);s.peaks.assign(s.n,0.0f);s.read=s.write=s.queued=0;s.failure.clear();
    s.loop=pw_thread_loop_new("SoundCurrent Studio",nullptr);if(!s.loop)throw std::runtime_error("Cannot create PipeWire loop");
    auto *captureProps=pw_properties_new(PW_KEY_MEDIA_TYPE,"Audio",PW_KEY_MEDIA_CATEGORY,"Capture",PW_KEY_MEDIA_ROLE,"DSP",
        PW_KEY_NODE_NAME,sink.c_str(),PW_KEY_NODE_DESCRIPTION,"SoundCurrent Studio",PW_KEY_MEDIA_CLASS,"Audio/Sink",
        PW_KEY_NODE_VIRTUAL,"true",PW_KEY_NODE_AUTOCONNECT,"false",PW_KEY_NODE_LATENCY,"512/48000",nullptr);
    auto *outputProps=pw_properties_new(PW_KEY_MEDIA_TYPE,"Audio",PW_KEY_MEDIA_CATEGORY,"Playback",PW_KEY_MEDIA_ROLE,"DSP",
        PW_KEY_NODE_NAME,output.c_str(),PW_KEY_TARGET_OBJECT,target.c_str(),PW_KEY_NODE_PASSIVE,"false",
        "state.restore-props","false","state.default-volume","1.0",PW_KEY_NODE_LATENCY,"512/48000",nullptr);
    s.capture=pw_stream_new_simple(pw_thread_loop_get_loop(s.loop),"Studio capture",captureProps,&Impl::events(false),&s);
    s.playback=pw_stream_new_simple(pw_thread_loop_get_loop(s.loop),"Studio playback",outputProps,&Impl::events(true),&s);
    if(!s.capture||!s.playback){stop();throw std::runtime_error("Cannot create PipeWire streams");}
    std::array<std::uint8_t,4096> storage{};spa_pod_builder builder=SPA_POD_BUILDER_INIT(storage.data(),storage.size());
    spa_audio_info_raw info{};info.format=SPA_AUDIO_FORMAT_F32;info.rate=48000;info.channels=std::uint32_t(s.n);
    constexpr std::uint32_t surround[]{SPA_AUDIO_CHANNEL_FL,SPA_AUDIO_CHANNEL_FR,SPA_AUDIO_CHANNEL_FC,SPA_AUDIO_CHANNEL_LFE,SPA_AUDIO_CHANNEL_RL,SPA_AUDIO_CHANNEL_RR,SPA_AUDIO_CHANNEL_SL,SPA_AUDIO_CHANNEL_SR};
    for(std::size_t c=0;c<s.n;++c)info.position[c]=s.n==1?SPA_AUDIO_CHANNEL_MONO:(s.n==2||s.n==6||s.n==8)?surround[c]:std::uint32_t(SPA_AUDIO_CHANNEL_AUX0+c);
    const spa_pod *params[]{spa_format_audio_raw_build(&builder,SPA_PARAM_EnumFormat,&info)};
    const auto captureFlags=PW_STREAM_FLAG_MAP_BUFFERS;
    const auto outputFlags=pw_stream_flags(PW_STREAM_FLAG_AUTOCONNECT|PW_STREAM_FLAG_MAP_BUFFERS);
    if(pw_stream_connect(s.capture,PW_DIRECTION_INPUT,PW_ID_ANY,captureFlags,params,1)<0||
       pw_stream_connect(s.playback,PW_DIRECTION_OUTPUT,PW_ID_ANY,outputFlags,params,1)<0||pw_thread_loop_start(s.loop)<0){stop();throw std::runtime_error("Cannot connect PipeWire streams");}
}
void LinuxBridge::update(const EngineSettings &settings,std::span<const double> matrix) {
    auto &s=*impl_;if(!s.loop)return;Impl::LoopLock lock(s.loop);
    if(settings.channels.size()!=s.n)throw std::runtime_error("Turn playback off before applying a new live channel layout");
    ChannelRouter validate(s.n,s.n);if(!validate.setMatrix(matrix))throw std::runtime_error("Invalid Studio routing matrix");
    std::string error;if(!s.engine->configure(settings,&error))throw std::runtime_error(error);s.router->setMatrix(matrix);
}
void LinuxBridge::stop() {
    auto &s=*impl_;if(s.loop) {pw_thread_loop_stop(s.loop);if(s.capture)pw_stream_destroy(s.capture);if(s.playback)pw_stream_destroy(s.playback);pw_thread_loop_destroy(s.loop);}
    s.loop=nullptr;s.capture=s.playback=nullptr;s.engine.reset();s.router.reset();
}
bool LinuxBridge::running() const {auto &s=*impl_;if(!s.loop)return false;Impl::LoopLock lock(s.loop);return s.failure.empty();}
std::string LinuxBridge::error() const {auto &s=*impl_;if(!s.loop)return s.failure;Impl::LoopLock lock(s.loop);return s.failure;}
std::vector<float> LinuxBridge::levels() const {auto &s=*impl_;if(!s.loop)return {};Impl::LoopLock lock(s.loop);auto levels=s.peaks;std::fill(s.peaks.begin(),s.peaks.end(),0.0f);return levels;}
}
