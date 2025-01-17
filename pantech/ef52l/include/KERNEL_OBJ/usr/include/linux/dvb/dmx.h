/*
 * dmx.h
 *
 * Copyright (C) 2000 Marcus Metzler <marcus@convergence.de>
 *                  & Ralph  Metzler <ralph@convergence.de>
 *                    for convergence integrated media GmbH
 *
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _DVBDMX_H_
#define _DVBDMX_H_

#include <linux/types.h>
#include <time.h>


#define DMX_FILTER_SIZE 16

/* Min recording chunk upon which event is generated */
#define DMX_REC_BUFF_CHUNK_MIN_SIZE		(100*188)

#define DMX_MAX_DECODER_BUFFER_NUM		(32)

typedef enum
{
	DMX_OUT_DECODER, /* Streaming directly to decoder. */
	DMX_OUT_TAP,     /* Output going to a memory buffer */
			 /* (to be retrieved via the read command).*/
	DMX_OUT_TS_TAP,  /* Output multiplexed into a new TS  */
			 /* (to be retrieved by reading from the */
			 /* logical DVR device).                 */
	DMX_OUT_TSDEMUX_TAP /* Like TS_TAP but retrieved from the DMX device */
} dmx_output_t;


typedef enum
{
	DMX_IN_FRONTEND, /* Input from a front-end device.  */
	DMX_IN_DVR       /* Input from the logical DVR device.  */
} dmx_input_t;


typedef enum
{
	DMX_PES_AUDIO0,
	DMX_PES_VIDEO0,
	DMX_PES_TELETEXT0,
	DMX_PES_SUBTITLE0,
	DMX_PES_PCR0,

	DMX_PES_AUDIO1,
	DMX_PES_VIDEO1,
	DMX_PES_TELETEXT1,
	DMX_PES_SUBTITLE1,
	DMX_PES_PCR1,

	DMX_PES_AUDIO2,
	DMX_PES_VIDEO2,
	DMX_PES_TELETEXT2,
	DMX_PES_SUBTITLE2,
	DMX_PES_PCR2,

	DMX_PES_AUDIO3,
	DMX_PES_VIDEO3,
	DMX_PES_TELETEXT3,
	DMX_PES_SUBTITLE3,
	DMX_PES_PCR3,

	DMX_PES_OTHER
} dmx_pes_type_t;

#define DMX_PES_AUDIO    DMX_PES_AUDIO0
#define DMX_PES_VIDEO    DMX_PES_VIDEO0
#define DMX_PES_TELETEXT DMX_PES_TELETEXT0
#define DMX_PES_SUBTITLE DMX_PES_SUBTITLE0
#define DMX_PES_PCR      DMX_PES_PCR0


typedef struct dmx_filter
{
	__u8  filter[DMX_FILTER_SIZE];
	__u8  mask[DMX_FILTER_SIZE];
	__u8  mode[DMX_FILTER_SIZE];
} dmx_filter_t;


/* Filter flags */
#define DMX_CHECK_CRC		0x01
#define DMX_ONESHOT		0x02
#define DMX_IMMEDIATE_START	0x04
#define DMX_ENABLE_INDEXING	0x08
#define DMX_KERNEL_CLIENT	0x8000

struct dmx_sct_filter_params
{
	__u16          pid;
	dmx_filter_t   filter;
	__u32          timeout;
	__u32          flags;
};


/* Indexing: supported video standards */
enum dmx_indexing_video_standard {
	DMX_INDEXING_MPEG2,
	DMX_INDEXING_H264,
	DMX_INDEXING_VC1
};

/* Indexing: Supported video profiles */
enum dmx_indexing_video_profile {
	DMX_INDEXING_MPEG2_ANY,
	DMX_INDEXING_H264_ANY,
	DMX_INDEXING_VC1_ANY
};

/* Indexing: video configuration parameters */
struct dmx_indexing_video_params {
	enum dmx_indexing_video_standard standard;
	enum dmx_indexing_video_profile profile;
};

struct dmx_pes_filter_params
{
	__u16          pid;
	dmx_input_t    input;
	dmx_output_t   output;
	dmx_pes_type_t pes_type;
	__u32          flags;

	/*
	 * The following configures when the event
	 * DMX_EVENT_NEW_REC_CHUNK will be triggered.
	 * When new recorded data is received with size
	 * equal or larger than this value a new event
	 * will be triggered. This is relevent when
	 * output is DMX_OUT_TS_TAP or DMX_OUT_TSDEMUX_TAP,
	 * size must be at least DMX_REC_BUFF_CHUNK_MIN_SIZE
	 * and smaller than buffer size.
	 */
	__u32          rec_chunk_size;

	struct dmx_indexing_video_params video_params;
};

struct dmx_buffer_status {
	/* size of buffer in bytes */
	unsigned int size;

	/* fullness of buffer in bytes */
	unsigned int fullness;

	/*
	 * How many bytes are free
	 * It's the same as: size-fullness-1
	 */
	unsigned int free_bytes;

	/* read pointer offset in bytes */
	unsigned int read_offset;

	/* write pointer offset in bytes */
	unsigned int write_offset;

	/* non-zero if data error occured */
	int error;
};

/* Events associated with each demux filter */
enum dmx_event {
	/* New PES packet is ready to be consumed */
	DMX_EVENT_NEW_PES,

	/* New section is ready to be consumed */
	DMX_EVENT_NEW_SECTION,

	/* New recording chunk is ready to be consumed */
	DMX_EVENT_NEW_REC_CHUNK,

	/* New PCR value is ready */
	DMX_EVENT_NEW_PCR,

	/* Overflow */
	DMX_EVENT_BUFFER_OVERFLOW,

	/* Section was dropped due to CRC error */
	DMX_EVENT_SECTION_CRC_ERROR,

	/* End-of-stream, no more data from this filter */
	DMX_EVENT_EOS,

	/* New Elementary Stream data is ready */
	DMX_EVENT_NEW_ES_DATA
};

/* Flags passed in filter events */

/* Continuity counter error was detected */
#define DMX_FILTER_CC_ERROR			0x01

/* Discontinuity indicator was set */
#define DMX_FILTER_DISCONTINUITY_INDICATOR	0x02

/* PES legnth in PES header is not correct */
#define DMX_FILTER_PES_LENGTH_ERROR		0x04


/* PES info associated with DMX_EVENT_NEW_PES event */
struct dmx_pes_event_info {
	/* Offset at which PES information starts */
	__u32 base_offset;

	/*
	 * Start offset at which PES data
	 * from the stream starts.
	 * Equal to base_offset if PES data
	 * starts from the beginning.
	 */
	__u32 start_offset;

	/* Total length holding the PES information */
	__u32 total_length;

	/* Actual length holding the PES data */
	__u32 actual_length;

	/* Local receiver timestamp in 27MHz */
	__u64 stc;

	/* Flags passed in filter events */
	__u32 flags;

	/*
	 * Number of TS packets with Transport Error Indicator (TEI)
	 * found while constructing the PES.
	 */
	__u32 transport_error_indicator_counter;

	/* Number of continuity errors found while constructing the PES */
	__u32 continuity_error_counter;

	/* Total number of TS packets holding the PES */
	__u32 ts_packets_num;
};

/* Section info associated with DMX_EVENT_NEW_SECTION event */
struct dmx_section_event_info {
	/* Offset at which section information starts */
	__u32 base_offset;

	/*
	 * Start offset at which section data
	 * from the stream starts.
	 * Equal to base_offset if section data
	 * starts from the beginning.
	 */
	__u32 start_offset;

	/* Total length holding the section information */
	__u32 total_length;

	/* Actual length holding the section data */
	__u32 actual_length;

	/* Flags passed in filter events */
	__u32 flags;
};

/* Recording info associated with DMX_EVENT_NEW_REC_CHUNK event */
struct dmx_rec_chunk_event_info {
	/* Offset at which recording chunk starts */
	__u32 offset;

	/* Size of recording chunk in bytes */
	__u32 size;
};

/* PCR info associated with DMX_EVENT_NEW_PCR event */
struct dmx_pcr_event_info {
	/* Local timestamp in 27MHz
	 * when PCR packet was received
	 */
	__u64 stc;

	/* PCR value in 27MHz */
	__u64 pcr;

	/* Flags passed in filter events */
	__u32 flags;
};

/*
 * Elementary stream data information associated
 * with DMX_EVENT_NEW_ES_DATA event
 */
struct dmx_es_data_event_info {
	/* Buffer user-space handle */
	int buf_handle;

	/*
	 * Cookie to provide when releasing the buffer
	 * using the DMX_RELEASE_DECODER_BUFFER ioctl command
	 */
	int cookie;

	/* Offset of data from the beginning of the buffer */
	__u32 offset;

	/* Length of data in buffer (in bytes) */
	__u32 data_len;

	/* Indication whether PTS value is valid */
	int pts_valid;

	/* PTS value associated with the buffer */
	__u64 pts;

	/* Indication whether DTS value is valid */
	int dts_valid;

	/* DTS value associated with the buffer */
	__u64 dts;

	/*
	 * Number of TS packets with Transport Error Indicator (TEI) set
	 * in the TS packet header since last reported event
	 */
	__u32 transport_error_indicator_counter;

	/* Number of continuity errors since last reported event */
	__u32 continuity_error_counter;

	/* Total number of TS packets processed since last reported event */
	__u32 ts_packets_num;

	/*
	 * Number of dropped bytes due to insufficient buffer space,
	 * since last reported event
	 */
	__u32 ts_dropped_bytes;
};

/*
 * Filter's event returned through DMX_GET_EVENT.
 * poll with POLLPRI would block until events are available.
 */
struct dmx_filter_event {
	enum dmx_event type;

	union {
		struct dmx_pes_event_info pes;
		struct dmx_section_event_info section;
		struct dmx_rec_chunk_event_info recording_chunk;
		struct dmx_pcr_event_info pcr;
		struct dmx_es_data_event_info es_data;
	} params;
};

/* Filter's buffer requirement returned in dmx_caps */
struct dmx_buffer_requirement {
	/* Buffer size alignment, 0 means no special requirement */
	__u32 size_alignment;

	/* Maximum buffer size allowed */
	__u32 max_size;

	/* Maximum number of linear buffers handled by demux */
	__u32 max_buffer_num;

	/* Feature support bitmap as detailed below */
	__u32 flags;

/* Buffer must be allocated as physically contiguous memory */
#define DMX_BUFFER_CONTIGUOUS_MEM		0x1

/* If the filter's data is decrypted, the buffer should be secured one */
#define DMX_BUFFER_SECURED_IF_DECRYPTED		0x2

/* Buffer can be allocated externally */
#define DMX_BUFFER_EXTERNAL_SUPPORT		0x4

/* Buffer can be allocated internally */
#define DMX_BUFFER_INTERNAL_SUPPORT		0x8

/* Filter output can be output to a linear buffer group */
#define DMX_BUFFER_LINEAR_GROUP_SUPPORT		0x10
};

typedef struct dmx_caps {
	__u32 caps;

/* Indicates whether demux support playback from memory in pull mode */
#define DMX_CAP_PULL_MODE				0x01

/* Indicates whether demux support indexing of recorded video stream */
#define DMX_CAP_VIDEO_INDEXING			0x02

/* Indicates whether demux support sending data directly to video decoder */
#define DMX_CAP_VIDEO_DECODER_DATA		0x04

/* Indicates whether demux support sending data directly to audio decoder */
#define DMX_CAP_AUDIO_DECODER_DATA		0x08

/* Indicates whether demux support sending data directly to subtitle decoder */
#define DMX_CAP_SUBTITLE_DECODER_DATA	0x10

	/* Number of decoders demux can output data to */
	int num_decoders;

	/* Number of demux devices */
	int num_demux_devices;

	/* Max number of PID filters */
	int num_pid_filters;

	/* Max number of section filters */
	int num_section_filters;

	/*
	 * Max number of section filters using same PID,
	 * 0 if not supported
	 */
	int num_section_filters_per_pid;

	/*
	 * Length of section filter, not including section
	 * length field (2 bytes).
	 */
	int section_filter_length;

	/* Max number of demod based input */
	int num_demod_inputs;

	/* Max number of memory based input */
	int num_memory_inputs;

	/* Overall bitrate from all inputs concurrently. Mbit/sec */
	int max_bitrate;

	/* Max bitrate from single demod input. Mbit/sec */
	int demod_input_max_bitrate;

	/* Max bitrate from single memory input. Mbit/sec */
	int memory_input_max_bitrate;

	struct dmx_buffer_requirement section;

	/* For PES not sent to decoder */
	struct dmx_buffer_requirement pes;

	/* For PES sent to decoder */
	struct dmx_buffer_requirement decoder;

	/* Recording buffer for recording of 188 bytes packets */
	struct dmx_buffer_requirement recording_188_tsp;

	/* Recording buffer for recording of 192 bytes packets */
	struct dmx_buffer_requirement recording_192_tsp;

	/* DVR input buffer for playback of 188 bytes packets */
	struct dmx_buffer_requirement playback_188_tsp;

	/* DVR input buffer for playback of 192 bytes packets */
	struct dmx_buffer_requirement playback_192_tsp;
} dmx_caps_t;

typedef enum {
	DMX_SOURCE_FRONT0 = 0,
	DMX_SOURCE_FRONT1,
	DMX_SOURCE_FRONT2,
	DMX_SOURCE_FRONT3,
	DMX_SOURCE_DVR0   = 16,
	DMX_SOURCE_DVR1,
	DMX_SOURCE_DVR2,
	DMX_SOURCE_DVR3
} dmx_source_t;

enum dmx_tsp_format_t {
	DMX_TSP_FORMAT_188 = 0,
	DMX_TSP_FORMAT_192_TAIL,
	DMX_TSP_FORMAT_192_HEAD,
	DMX_TSP_FORMAT_204,
};

enum dmx_playback_mode_t {
	/*
	 * In push mode, if one of output buffers
	 * is full, the buffer would overflow
	 * and demux continue processing incoming stream.
	 * This is the default mode. When playing from frontend,
	 * this is the only mode that is allowed.
	 */
	DMX_PB_MODE_PUSH = 0,

	/*
	 * In pull mode, if one of output buffers
	 * is full, demux stalls waiting for free space,
	 * this would cause DVR input buffer fullness
	 * to accumulate.
	 * This mode is possible only when playing
	 * from DVR.
	 */
	DMX_PB_MODE_PULL,
};

struct dmx_stc {
	unsigned int num; /* input : which STC? 0..N */
	unsigned int base; /* output: divisor for stc to get 90 kHz clock */
	__u64 stc; /* output: stc in 'base'*90 kHz units */
};

enum dmx_buffer_mode {
	/*
	 * demux buffers are allocated internally
	 * by the demux driver. This is the default mode.
	 * DMX_SET_BUFFER_SIZE can be used to set the size of
	 * this buffer.
	 */
	DMX_BUFFER_MODE_INTERNAL,

	/*
	 * demux buffers are allocated externally and provided
	 * to demux through DMX_SET_BUFFER.
	 * When this mode is used DMX_SET_BUFFER_SIZE and
	 * mmap are prohibited.
	 */
	DMX_BUFFER_MODE_EXTERNAL,
};

struct dmx_buffer {
	unsigned int size;
	int handle;
};


struct dmx_decoder_buffers {
	/*
	 * Specify if linear buffer support is requested. If set, buffers_num
	 * must be greater than 1
	 */
	int is_linear;

	/*
	 * Specify number of external buffers allocated by user.
	 * If set to 0 means internal buffer allocation is requested
	 */
	__u32 buffers_num;

	/* Specify buffer size, either external or internal */
	__u32 buffers_size;

	/* Array of externally allocated buffer handles */
	int handles[DMX_MAX_DECODER_BUFFER_NUM];
};

struct dmx_secure_mode {
	/*
	 * Specifies whether secure mode should be set or not for the filter's
	 * pid. Note that DMX_OUT_TSDEMUX_TAP filters can have more than 1 pid
	 */
	int is_secured;

	/* PID to associate with key ladder id */
	__u16 pid;

	/* key ladder information to associate with the specified pid */
	__u32 key_ladder_id;
};

#define DMX_START                _IO('o', 41)
#define DMX_STOP                 _IO('o', 42)
#define DMX_SET_FILTER           _IOW('o', 43, struct dmx_sct_filter_params)
#define DMX_SET_PES_FILTER       _IOW('o', 44, struct dmx_pes_filter_params)
#define DMX_SET_BUFFER_SIZE      _IO('o', 45)
#define DMX_GET_PES_PIDS         _IOR('o', 47, __u16[5])
#define DMX_GET_CAPS             _IOR('o', 48, dmx_caps_t)
#define DMX_SET_SOURCE           _IOW('o', 49, dmx_source_t)
#define DMX_GET_STC              _IOWR('o', 50, struct dmx_stc)
#define DMX_ADD_PID              _IOW('o', 51, __u16)
#define DMX_REMOVE_PID           _IOW('o', 52, __u16)
#define DMX_SET_TS_PACKET_FORMAT _IOW('o', 53, enum dmx_tsp_format_t)
#define DMX_SET_TS_OUT_FORMAT	 _IOW('o', 54, enum dmx_tsp_format_t)
#define DMX_SET_DECODER_BUFFER_SIZE	_IO('o', 55)
#define DMX_GET_BUFFER_STATUS	 _IOR('o', 56, struct dmx_buffer_status)
#define DMX_RELEASE_DATA		 _IO('o', 57)
#define DMX_FEED_DATA			 _IO('o', 58)
#define DMX_SET_PLAYBACK_MODE	 _IOW('o', 59, enum dmx_playback_mode_t)
#define DMX_GET_EVENT		 _IOR('o', 60, struct dmx_filter_event)
#define DMX_SET_BUFFER_MODE	 _IOW('o', 61, enum dmx_buffer_mode)
#define DMX_SET_BUFFER		 _IOW('o', 62, struct dmx_buffer)
#define DMX_SET_DECODER_BUFFER	 _IOW('o', 63, struct dmx_decoder_buffers)
#define DMX_REUSE_DECODER_BUFFER _IO('o', 64)
#define DMX_SET_SECURE_MODE	_IOW('o', 65, struct dmx_secure_mode)


#endif /*_DVBDMX_H_*/
