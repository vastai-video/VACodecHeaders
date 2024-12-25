#ifndef VASTAPI_H
#define VASTAPI_H

#ifndef VAENC_API_H
#define VAENC_API_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <vastva/va_public.h>

#define MAX_ADAPTIVE_GOP_SIZE 8

#define CODETYPE_FLUSH 5 // vframe bug
#define VCENC_FRAME_EOS 8 // sync to VCEncRet VCENC_FRAME_EOS
#define BUFFER_CNT_FOR_REORDER 4
#define MAX_CORE_NUM 4
#define CUTREE_BUFFER_CNT(depth, gopSize)                                                                              \
    ((gopSize) <= MAX_ADAPTIVE_GOP_SIZE ? ((depth) + MAX_ADAPTIVE_GOP_SIZE - 1) : ((depth) + MAX_GOP_SIZE / 2))
#define MAX_GOP_SIZE 16

enum {
    MAX_CONFIG_ATTRIBUTES  = 5,
    MAX_GLOBAL_PARAMS      = 5,
    MAX_DPB_SIZE           = 16,
    MAX_PICTURE_REFERENCES = 2,
    MAX_REORDER_DELAY      = 16,
    MAX_PARAM_BUFFER_SIZE  = 1024,
};

enum {
    PICTURE_TYPE_IDR = 0,
    PICTURE_TYPE_I   = 1,
    PICTURE_TYPE_P   = 2,
    PICTURE_TYPE_B   = 3,
};

enum {
    RC_MODE_AUTO,
    RC_MODE_CQP,
    RC_MODE_CBR,
    RC_MODE_VBR,
    RC_MODE_ICQ,
    RC_MODE_QVBR,
    RC_MODE_AVBR,
    RC_MODE_MAX = RC_MODE_AVBR,
};

enum {
    NORMALPASS,
    ONLY2PASS,
    ONLY1PASS,
};

enum {
    SEI_TIMING              = 0x01,
    SEI_IDENTIFIER          = 0x02,
    SEI_RECOVERY_POINT      = 0x04,
    SEI_MASTERING_DISPLAY   = 0x08,
    SEI_CONTENT_LIGHT_LEVEL = 0x10,
};

enum AV1SliceType {
    AV1_SLICE_B = 0,
    AV1_SLICE_P = 1,
    AV1_SLICE_I = 2,
};

enum {
    // Codec supports controlling the subdivision of pictures into slices.
    FLAG_SLICE_CONTROL = 1 << 0,
    // Codec only supports constant quality (no rate control).
    FLAG_CONSTANT_QUALITY_ONLY = 1 << 1,
    // Codec is intra-only.
    FLAG_INTRA_ONLY = 1 << 2,
    // Codec supports B-pictures.
    FLAG_B_PICTURES = 1 << 3,
    // Codec supports referencing B-pictures.
    FLAG_B_PICTURE_REFERENCES = 1 << 4,
    // Codec supports non-IDR key pictures (that is, key pictures do
    // not necessarily empty the DPB).
    FLAG_NON_IDR_KEY_PICTURES = 1 << 5,
};

struct node {
    struct node *next;
#if defined(ANDROID_32BIT) || defined(X86_32)
    struct node *next_MSB;
#endif
};

struct queue {
    struct node *head;
#if defined(ANDROID_32BIT) || defined(X86_32)
    struct node *head_MSB;
#endif
    struct node *tail;
#if defined(ANDROID_32BIT) || defined(X86_32)
    struct node *tail_MSB;
#endif
    int length;
};


typedef struct VASTAPIEncodeSlice {
    int   index;
    int   row_start;
    int   row_size;
    int   block_start;
    int   block_size;
    void *priv_data;
    void *codec_slice_params;
} VASTAPIEncodeSlice;

typedef struct VASTAPIEncodePicture {
    struct VASTAPIEncodePicture *next;
    struct VASTAPIEncodePicture *next_output;

    int64_t display_order;
    int64_t encode_order;
    int64_t pts;
    int     force_idr;

    VASTEncROI *roi;

    int type;
    int b_depth;
    int encode_issued;
    int insertIDR;
    int encode_complete;

    void/*AVFrame*/      *input_image;
    VASTSurfaceID input_surface;

    void/*AVFrame*/      *recon_image;
    VASTSurfaceID recon_surface;

    int           nb_param_buffers;
    VASTBufferID *param_buffers;

    void/*AVBufferRef*/ *output_buffer_ref;
    VASTBufferID output_buffer;

    void *priv_data;
    void *codec_picture_params;

    // Whether this picture is a reference picture.
    int is_reference;

    // The contents of the DPB after this picture has been decoded.
    // This will contain the picture itself if it is a reference picture,
    // but not if it isn't.
    int                          nb_dpb_pics;
    struct VASTAPIEncodePicture *dpb[MAX_DPB_SIZE];
    // The reference pictures used in decoding this picture.  If they are
    // used by later pictures they will also appear in the DPB.
    int                          nb_refs;
    struct VASTAPIEncodePicture *refs[MAX_PICTURE_REFERENCES];
    // The previous reference picture in encode order.  Must be in at least
    // one of the reference list and DPB list.
    struct VASTAPIEncodePicture *prev;
    // Reference count for other pictures referring to this one through
    // the above pointers, directly from incomplete pictures and indirectly
    // through completed pictures.
    int ref_count[2];
    int ref_removed[2];

    int                 nb_slices;
    int                 isEnqueue; // add by vastai for 2pass
    VASTAPIEncodeSlice *slices;
    int                 timebaseDen;
    int                 timebaseNum;
    int                 passmodel;
    int                 do1pass;
    int                 should_do_2pass;
    int64_t             pts_internal;
    int                 ctnIgop;
} VASTAPIEncodePicture;


typedef struct VASTAPIEncodeRCMode {
    // Mode from above enum (RC_MODE_*).
    int mode;
    // Name.
    const char *name;
    // Supported in the compile-time VASTAPI version.
    int supported;
    // VA mode value (VA_RC_*).
    uint32_t va_mode;
    // Uses bitrate parameters.
    int bitrate;
    // Supports maxrate distinct from bitrate.
    int maxrate;
    // Uses quality value.
    int quality;
    // Supports HRD/VBV parameters.
    int hrd;
} VASTAPIEncodeRCMode;

typedef struct VASTAPIEncodeProfile {
    // lavc profile value (FF_PROFILE_*).
    int av_profile;
    // Supported bit depth.
    int depth;
    // Number of components.
    int nb_components;
    // Chroma subsampling in width dimension.
    int log2_chroma_w;
    // Chroma subsampling in height dimension.
    int log2_chroma_h;
    // VASTAPI profile value.
    VASTProfile va_profile;
} VASTAPIEncodeProfile;

typedef struct VASTAPIEncodePixFmtDesc {
    const char *name;
    uint8_t nb_components;
    uint8_t log2_chroma_w;
    uint8_t log2_chroma_h;
    int depth; // simplify AVComponentDescriptor comp[4];
} VASTAPIEncodePixFmtDesc; // AVPixFmtDescriptor

typedef struct VASTAPIEncodeParam {
    int type;
    char* data;
    size_t size;
    int is_packed;
} VASTAPIEncodeParam;

typedef struct VASTAPIEncodeExtra {
    int64_t bit_rate;
    int global_quality;
    int64_t rc_max_rate;
    int gop_size;
    int rc_buffer_size;
    int rc_initial_buffer_occupancy;
    int qmin;
    int qmax;
    //
    int compression_level;
    int max_b_frames;
    int width, height;
    int slices;

    int profile;
    int color_primaries;
    int color_trc;
    int colorspace;
    
    // ptr to struct
    void* time_base;
    void* framerate;
    
    VASTAPIEncodePixFmtDesc desc;
} VASTAPIEncodeExtra; // vars from avctx

typedef struct VASTAPIEncodeContext {
    const void/*AVClass*/ *class;

    // Codec-specific hooks.
    const struct VASTAPIEncodeType *codec;

    // Global options.
    int explicit_crf;

    // Use low power encoding mode.
    int low_power;

    // Number of I frames between IDR frames.
    int idr_interval;

    // Desired B frame reference depth.
    int desired_b_depth;

    // Explicitly set RC mode (otherwise attempt to pick from
    // available modes).
    int explicit_rc_mode;

    // Explicitly-set QP, for use with the "qp" options.
    // (Forces CQP mode when set, overriding everything else.)
    int explicit_qp;

    // Desired packed headers.
    unsigned int desired_packed_headers;

    // The required size of surfaces.  This is probably the input
    // size (AVCodecContext.width|height) aligned up to whatever
    // block size is required by the codec.
    int surface_width;
    int surface_height;

    // The block size for slice calculations.
    int slice_block_width;
    int slice_block_height;

    // Everything above this point must be set before calling
    // ff_vastapi_encode_init().

    // Chosen encoding profile details.
    const VASTAPIEncodeProfile *profile;

    // Chosen rate control mode details.
    const VASTAPIEncodeRCMode *rc_mode;
    // RC quality level - meaning depends on codec and RC mode.
    // In CQP mode this sets the fixed quantiser value.
    int rc_quality;

    // Encoding profile (VASTProfile*).
    VASTProfile va_profile;
    // Encoding entrypoint (VAEntryoint*).
    VASTEntrypoint va_entrypoint;
    // Rate control mode.
    unsigned int va_rc_mode;
    // Bitrate for codec-specific encoder parameters.
    unsigned int va_bit_rate;
    // Packed headers which will actually be sent.
    unsigned int va_packed_headers;

    // Configuration attributes to use when creating va_config.
    VASTConfigAttrib config_attributes[MAX_CONFIG_ATTRIBUTES];
    int              nb_config_attributes;

    VASTConfigID  va_config;
    VASTContextID va_context;

    void/*AVBufferRef*/            *device_ref;
    void/*AVHWDeviceContext*/      *device;
    void/*AVVASTAPIDeviceContext*/ *hwctx;

    // The hardware frame context containing the input frames.
    void/*AVBufferRef*/       *input_frames_ref;
    void/*AVHWFramesContext*/ *input_frames;

    // The hardware frame context containing the reconstructed frames.
    void/*AVBufferRef*/       *recon_frames_ref;
    void/*AVHWFramesContext*/ *recon_frames;

    // Pool of (reusable) bitstream output buffers.
    void/*AVBufferPool*/ *output_buffer_pool;

    // Global parameters which will be applied at the start of the
    // sequence (includes rate control parameters below).
    int         global_params_type[MAX_GLOBAL_PARAMS];
    const void *global_params[MAX_GLOBAL_PARAMS];
    size_t      global_params_size[MAX_GLOBAL_PARAMS];
    int         nb_global_params;

    // Rate control parameters.
    VASTEncMiscParameterRateControl rc_params;
    VASTEncMiscParameterHRD         hrd_params;
    VASTEncMiscParameterFrameRate   fr_params;
#if VA_CHECK_VERSION(0, 36, 0)
    VASTEncMiscParameterBufferQualityLevel quality_params;
#endif

    // Per-sequence parameter structure (VAEncSequenceParameterBuffer*).
    void *codec_sequence_params;

    // Per-sequence parameters found in the per-picture parameter
    // structure (VAEncPictureParameterBuffer*).
    void *codec_picture_params;

    // Current encoding window, in display (input) order.
    VASTAPIEncodePicture *pic_start, *pic_end;

    VASTAPIEncodePicture *outpic_start, *outpic_end;
    // The next picture to use as the previous reference picture in
    // encoding order.
    VASTAPIEncodePicture *next_prev;

    // Next input order index (display order).
    int64_t input_order;
    // Number of frames that output is behind input.
    int64_t output_delay;
    // Next encode order index.
    int64_t encode_order;
    // Number of frames decode output will need to be delayed.
    int64_t decode_delay;
    // Next output order index (in encode order).
    int64_t output_order;

    // Timestamp handling.
    int64_t first_pts;
    int64_t dts_pts_diff;
    int64_t ts_ring[156]; // add max lookahead 3*(40+8+4)s

    // Slice structure.
    int slice_block_rows;
    int slice_block_cols;
    int nb_slices;
    int slice_size;

    // Frame type decision.
    int gop_size;
    int closed_gop;
    int gop_per_idr;
    int p_per_i;
    int max_b_depth;
    int b_per_p;
    int force_idr;
    int idr_counter;
    int gop_counter;
    int end_of_stream;

    // Whether the driver supports ROI at all.
    int roi_allowed;
    // Maximum number of regions supported by the driver.
    int roi_max_regions;
    // Quantisation range for offset calculations.  Set by codec-specific
    // code, as it may change based on parameters.
    int roi_quant_range;

    // The encoder does not support cropping information, so warn about
    // it the first time we encounter any nonzero crop fields.
    int crop_warned;
    // If the driver does not support ROI then warn the first time we
    // encounter a frame with ROI side data.
    int                 roi_warned;
    int                 udu_sei;
    void/*AVDictionary*/       *vast_opts;  // vastai
    VAEncMiscParameter *vast_param; // vastai

    int     vast_last_frame; // vastai
    int64_t dts_pts_scale;   // vastai

    void/*AVFrame*/ *frame;
    int64_t  prev_dts; // vastai for vfr
    uint8_t *av1pkg;   // av1mp4
    int64_t  av1size;

    int64_t                 input_order_change_point;
    short                   rate_emu;
    int                     first_i_ready;
    int                     dts_index; // new dst simonz
    int64_t                 pts_internal;
    int                     flush_pkt_flag;
    int                     lookaheadPacket_size;
    struct queue            encodePktQ;
    int32_t                 av1_num_nalus;
    int32_t                 av1_nalu_size[2];
    int32_t                 av1_frame_not_show;
    int32_t                 av1_eos;
    int32_t                 av1_again;
    VASTCodedBufferSegment *flush_buf_list;
    VASTCodedBufferSegment *flush_buf_list_multi[MAX_CORE_NUM];
    int                     idr_change;
    int                     is_av1;
    VASTDisplay             display;
    int64_t                 pkt_pts;
    int                     *pframe_number;
    void                    *avctx;
    int                     pkt_size; 
    int                     ff_version;   
    unsigned char * (*get_encode_buffer)(void * pkt, int size);  
    void (*av_buffer_unref)(void **buf);  
    void (*av_frame_free)(void **buf);
    void (*av_buffer_pool_uninit)(void **ppool); 
    void *(*av_buffer_pool_get)(void *pool);
    int (*vastapi_encode_receive_packet)(void *avctx, void *pkt);
    int (*vastapi_encode_av1_split_and_combine)(void *avctx, void *pkt);
    int (*vastapi_encode_reset_flush)(void *avctx);
    void (*set_flags_and_pts)(VASTAPIEncodePicture *pic, void * pkt, int64_t pkt_pts);
    void (*update_pkt_dts)(void* avctx, void * pkt, int64_t dts, int delay);
    int (*vastapi_encode_issue)(void *avctx, void *pic);
    void (*av_packet_unref)(void *pkt);
    int (*update_av1_pkt)(void *ctx, void * avpkt);
    void (*ffmpeg_av_freep)(void *arg);
    void (*ff_av_log)(void* avcl, int level, const char *fmt, ...);
    VASTAPIEncodeExtra      ff_param;

    void* vst_func;
} VASTAPIEncodeContext;


typedef struct VASTAPIEncodeH264Picture {
    int frame_num;
    int pic_order_cnt;

    int64_t  last_idr_frame;
    uint16_t idr_pic_id;

    int primary_pic_type;
    int slice_type;

    int cpb_delay;
    int dpb_delay;
} VASTAPIEncodeH264Picture;

typedef struct VASTAPIEncodeH265Picture {
    int pic_order_cnt;

    int64_t last_idr_frame;

    int slice_nal_unit;
    int slice_type;
    int pic_type;
} VASTAPIEncodeH265Picture;

typedef struct VASTAPIEncodeAV1Picture {
    int pic_order_cnt;

    int64_t last_idr_frame;

    int slice_nal_unit;
    int slice_type;
    int pic_type;
} VASTAPIEncodeAV1Picture;


typedef struct VASTAPIEncodeType {
    // List of supported profiles and corresponding VASTAPI profiles.
    // (Must end with FF_PROFILE_UNKNOWN.)
    const VASTAPIEncodeProfile *profiles;

    // Codec feature flags.
    int flags;

    // Default quality for this codec - used as quantiser or RC quality
    // factor depending on RC mode.
    int default_quality;

    // Perform any extra codec-specific configuration after the
    // codec context is initialised (set up the private data and
    // add any necessary global parameters).
    int (*configure)(void/*AVCodecContext*/ *avctx);

    // The size of any private data structure associated with each
    // picture (can be zero if not required).
    size_t picture_priv_data_size;

    // The size of the parameter structures:
    // sizeof(VAEnc{type}ParameterBuffer{codec}).
    size_t sequence_params_size;
    size_t picture_params_size;
    size_t slice_params_size;

    // Fill the parameter structures.
    int (*init_sequence_params)(void/*AVCodecContext*/ *avctx);
    int (*init_picture_params)(void/*AVCodecContext*/ *avctx, VASTAPIEncodePicture *pic);
    int (*init_slice_params)(void/*AVCodecContext*/ *avctx, VASTAPIEncodePicture *pic, VASTAPIEncodeSlice *slice);

    // The type used by the packed header: this should look like
    // VAEncPackedHeader{something}.
    int sequence_header_type;
    int picture_header_type;
    int slice_header_type;

    // Write the packed header data to the provided buffer.
    // The sequence header is also used to fill the codec extradata
    // when the encoder is starting.
    int (*write_sequence_header)(void/*AVCodecContext*/ *avctx, char *data, size_t *data_len);
    int (*write_picture_header)(void/*AVCodecContext*/ *avctx, VASTAPIEncodePicture *pic, char *data, size_t *data_len);
    int (*write_slice_header)(void/*AVCodecContext*/ *avctx, VASTAPIEncodePicture *pic, VASTAPIEncodeSlice *slice, char *data,
                              size_t *data_len);

    // Fill an extra parameter structure, which will then be
    // passed to vastRenderPicture().  Will be called repeatedly
    // with increasing index argument until AVERROR_EOF is
    // returned.
    int (*write_extra_buffer)(void/*AVCodecContext*/ *avctx, VASTAPIEncodePicture *pic, int index, int *type, char *data,
                              size_t *data_len);

    // Write an extra packed header.  Will be called repeatedly
    // with increasing index argument until AVERROR_EOF is
    // returned.
    int (*write_extra_header)(void/*AVCodecContext*/ *avctx, VASTAPIEncodePicture *pic, int index, int *type, char *data,
                              size_t *data_len);
} VASTAPIEncodeType;

enum VaEncCodecID {
    VAENC_CODEC_ID_H264,
    VAENC_CODEC_ID_H265,
    VAENC_CODEC_ID_UNKOWN,
};


// Frame Side Data

enum VaEncFrameSideDataType {
    ///
    VAENC_FRAME_DATA_REGIONS_OF_INTEREST,

    /// @brief 
    VAENC_FRAME_DATE_VASTAI_BITRATE_EXT1,
    VAENC_FRAME_DATA_UDU_SEI,
    VAENC_FRAME_DATA_RESERVED_SEI,
    VAENC_FRAME_DATA_ROIMAP, // set user roimap
    VAENC_FRAME_DATA_VASTAI_FRAMERATE,
    VAENC_FRAME_DATA_VASTAI_CRF,
    VAENC_FRAME_DATA_VASTAI_KEYINT,
    VAENC_FRAME_DATA_SEI_UNREGISTERED,
    VAENC_FRAME_DATA_UNKOWN,
};

typedef struct {
    enum VaEncFrameSideDataType type;
    uint8_t *data;
    size_t   size;
} VaEncFrameSideData;

extern const VASTAPIEncodeRCMode vastapi_encode_rc_modes[];

void v_queue_init(struct queue *queue);
void v_queue_put(struct queue *queue, struct node *node);
struct node *v_queue_get(struct queue *queue);
void v_queue_free(struct queue *queue);

#endif // VAENC_API_H

#ifndef __VADEC_API_H_
#define __VADEC_API_H_

enum {
    MAX_PARAM_BUFFERS = 16,
};


typedef struct _VASTVADecCtx{
    VASTConfigID  va_config;
    VASTContextID va_context;
    VASTDisplay display;
    void*       vst_func;

    VASTSurfaceAttrib pixel_format_attribute;

} VASTVADecCtx;

typedef struct VASTAPIDecodePicture {
    VASTSurfaceID output_surface;

    int          nb_param_buffers;
    VASTBufferID param_buffers[MAX_PARAM_BUFFERS];

    int           nb_slices;
    VASTBufferID *slice_buffers;
    int           slices_allocated;
} VASTAPIDecodePicture;

#endif // __VADEC_API_H_

#ifndef __HWCTX_VASTAPI_API_H__
#define __HWCTX_VASTAPI_API_H__

#define VASTAI_DMA_BUFFER_MAX 128
#define VENC_INPUT_DMA


typedef struct VASTAPIDmaHandle_s {
    unsigned int device_id;
#if defined(_WIN32)
    unsigned long long  kchar_fd;
    unsigned long long  dmabuff_fd;
#else
    int          kchar_fd;
    int          dmabuff_fd;
#endif
    unsigned int dmabuff_size;
    void        *dmabuff_viraddr;
    uint64_t     bus_addr;
    int          inited;
    unsigned int data_size;
} VASTAPIDmaHandle;

typedef struct AVVASTAPIDeviceContext_s {
    VASTDisplay display;
    void*       vst_func;
} AVVASTAPIDeviceContext;

typedef struct AVVASTAPIFramesContext_s {
    VASTSurfaceAttrib *attributes;
    int                nb_attributes;
    VASTSurfaceID *surface_ids;
    int            nb_surfaces;
} AVVASTAPIFramesContext;


typedef struct AVVASTAPIHWConfig_s {
    VASTConfigID config_id;
} AVVASTAPIHWConfig;


typedef struct VASTAPISurfaceFormat_s {
    VAST_PIX_FTM       pix_fmt;
    VASTImageFormat    image_format;
} VASTAPISurfaceFormat;

typedef struct VASTAPIDeviceContext_s {
    /**
     * The public AVVAAPIDeviceContext. See hwcontext_vaapi.h for it.
     */
    AVVASTAPIDeviceContext p;
    // Surface formats which can be used with this device.
    VASTAPISurfaceFormat *formats;
    int                   nb_formats;
} VASTAPIDeviceContext;


typedef struct VASTAPIFormatDescriptor_s {
    unsigned int       fourcc;
    unsigned int       rt_format;
    VAST_PIX_FTM       pix_fmt;
    int                chroma_planes_swapped;
} VASTAPIFormatDescriptor;


typedef struct VASTAPIFramesContext_s {
    /**
     * The public AVVAAPIFramesContext. See hwcontext_vaapi.h for it.
     */
    AVVASTAPIFramesContext p;

    VASTSurfaceAttrib *attributes;
    int                nb_attributes;
    unsigned int rt_format;
    int derive_works;
    
    VASTAPIDmaHandle dma_transfer;
    void             *pool_dmabuffer;  /*AVBufferPool*/
    int              init_dmabuffer_pool_size;
    int              number_dmabuffer;
    int              buffer_elem_size;
    int              dma_hack_enable;
} VASTAPIFramesContext;

typedef struct VASTAPIMapping_s {
    VASTImage image;
    int flags;
} VASTAPIMapping;


typedef struct VastapiConstraint_s {
    int min_width;
    int min_height;
    int max_width;
    int max_height;

    int attr_count;
    int pix_fmt_count;

} VastapiConstraint;


typedef struct VASTAPIContext_s{
    AVVASTAPIDeviceContext *avVstDevCtx;
    AVVASTAPIFramesContext *avVstFrmCtx;
    AVVASTAPIHWConfig      *avVstHwCfg;

    VASTAPIDeviceContext   *vstDevCtx;
    VASTAPIFramesContext   *vstFrmCtx;

    VASTAPIDmaHandle       *dmahandle;
    VASTAPIMapping         *map;

    VASTSurfaceID           surfaceId;

}VASTAPIContext;

#endif // __HWCTX_VASTAPI_API_H__

#ifndef VAFILTER_API_H
#define VAFILTER_API_H

#include <vastva/va_vpp.h>
typedef struct _VASTVAFilter{
    VASTConfigID                            va_config;
    VASTContextID                           va_context;
    VASTSurfaceID                           va_surface;
    VASTBufferID                            filter_buffers[VASTProcFilterCount];
    AVVASTAPIDeviceContext                  *hwctx;
    FilterParams                            *filt_params;
    VASTProcPipelineParameterBuffer         pipeline_params;
}VASTFilterParamer;

#endif // VAFILTER_API_H

#endif // VASTAPI_H