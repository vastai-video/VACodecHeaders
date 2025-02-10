#ifndef VA_PUBLIC_H
#define VA_PUBLIC_H

#include <stddef.h>
#include <stdint.h>
#include <vastva/va_version.h>

/** Padding size in 4-bytes */
#define VAST_PADDING_LOW          4
#define VAST_PADDING_MEDIUM       8
#define VAST_PADDING_HIGH         16
#define VAST_PADDING_LARGE        32

#define DEFAULT_VAST -255
#define MAX_TEMPORAL_LAYERS_VAST         4
#define MAX_SCENE_CHANGE_VAST 20
#define HANTRO_MAX_NUM_ROI_REGIONS_VAST                     8
#define HANTRO_MAX_NUM_IPCM_REGIONS_VAST                    8
#define HANTRO_MAX_NUM_INTRA_AREA_REGIONS_VAST              1
#define HANTRO_MAX_SUBPIC_SUM_VAST                     8

#define MAX_SEI_COUNT  8

/** Currently defined profiles */
typedef enum
{
    /** \brief Profile ID used for video processing. */
    VASTProfileNone                       = -1,
    VASTProfileMPEG2Simple		= 0,
    VASTProfileMPEG2Main			= 1,
    VASTProfileMPEG4Simple		= 2,
    VASTProfileMPEG4AdvancedSimple	= 3,
    VASTProfileMPEG4Main			= 4,
    VASTProfileH264Baseline = 5,
    VASTProfileH264Main			= 6,
    VASTProfileH264High			= 7,
    VASTProfileVC1Simple			= 8,
    VASTProfileVC1Main			= 9,
    VASTProfileVC1Advanced		= 10,
    VASTProfileH263Baseline		= 11,
    VASTProfileJPEGBaseline               = 12,
    VASTProfileH264ConstrainedBaseline    = 13,
    VASTProfileVP8Version0_3              = 14,
    VASTProfileH264MultiviewHigh          = 15,
    VASTProfileH264StereoHigh             = 16,
    VASTProfileHEVCMain                   = 17,
    VASTProfileHEVCMain10                 = 18,
    VASTProfileHEVCMainIntra              = 19,
    VASTProfileVP9Profile0                = 20,
    VASTProfileVP9Profile1                = 21,
    VASTProfileVP9Profile2                = 22,
    VASTProfileVP9Profile3                = 23,
    VASTProfileAV1Main                    = 24,
    VASTProfileAV1Hign                    = 25,
    VASTProfileVPP                        = 26,
    HANTROProfileHEVCMainStill            = 27,
    HANTROProfileH264High10               = 28,
    HANTROProfileVPP                      = 29,

    VASTProfileMAX                        = 30,  
} VASTProfile;

 typedef enum
{
    VAST_FTM_NONE = -1,

    NV12 = 1,
    YUV420P,
    YUV422P,
    UYVY422,
    YUYV422,
    YUV411P,
    YUV440P,
    YUV444P,
    GRAY8,
    P010,
    BGRA,
    BGR0,
    RGBA,
    RGB0,
    ABGR,
    XBGR, /*0BGR*/
    ARGB,
    XRGB, /*0RGB*/
    YUV420P10LE,
    YUVJ420P,
    BAYER_BGGR8,
    BAYER_RGGB8,
    BAYER_GBRG8,
    BAYER_GRBG8,
    Y210,
    YUV420P10,

}VAST_PIX_FTM;

/**
 *  Currently defined entrypoints 
 */
typedef enum
{
    VASTEntrypointVLD		= 1,
    VASTEntrypointIZZ		= 2,
    VASTEntrypointIDCT		= 3,
    VASTEntrypointMoComp		= 4,
    VASTEntrypointDeblocking	= 5,
    VASTEntrypointEncSlice	= 6,	/* slice level encode */
    VASTEntrypointEncPicture 	= 7,	/* pictuer encode, JPEG, etc */
    /*
     * For an implementation that supports a low power/high performance variant
     * for slice level encode, it can choose to expose the
     * VASTEntrypointEncSliceLP entrypoint. Certain encoding tools may not be
     * available with this entrypoint (e.g. interlace, MBAFF) and the
     * application can query the encoding configuration attributes to find
     * out more details if this entrypoint is supported.
     */
    VASTEntrypointEncSliceLP 	= 8,
    VASTEntrypointVideoProc       = 10,   /**< Video pre/post-processing. */
    /**
     * \brief VASTEntrypointFEI
     *
     * The purpose of FEI (Flexible Encoding Infrastructure) is to allow applications to
     * have more controls and trade off quality for speed with their own IPs.
     * The application can optionally provide input to ENC for extra encode control
     * and get the output from ENC. Application can chose to modify the ENC
     * output/PAK input during encoding, but the performance impact is significant.
     *
     * On top of the existing buffers for normal encode, there will be
     * one extra input buffer (VAEncMiscParameterFEIFrameControl) and
     * three extra output buffers (VASTEncFEIMVBufferType, VAEncFEIMBModeBufferType
     * and VASTEncFEIDistortionBufferType) for VASTEntrypointFEI entry function.
     * If separate PAK is set, two extra input buffers
     * (VASTEncFEIMVBufferType, VAEncFEIMBModeBufferType) are needed for PAK input.
     **/
    VASTEntrypointFEI         = 11,
    /**
     * \brief VASTEntrypointStats
     *
     * A pre-processing function for getting some statistics and motion vectors is added,
     * and some extra controls for Encode pipeline are provided. The application can
     * optionally call the statistics function to get motion vectors and statistics like
     * variances, distortions before calling Encode function via this entry point.
     *
     * Checking whether Statistics is supported can be performed with vastQueryConfigEntrypoints().
     * If Statistics entry point is supported, then the list of returned entry-points will
     * include #VASTEntrypointStats. Supported pixel format, maximum resolution and statistics
     * specific attributes can be obtained via normal attribute query. One input buffer
     * (VASTStatsStatisticsParameterBufferType) and one or two output buffers
     * (VASTStatsStatisticsBufferType, VASTStatsStatisticsBottomFieldBufferType (for interlace only)
     * and VASTStatsMVBufferType) are needed for this entry point.
     **/
    VASTEntrypointStats       = 12,
} VASTEntrypoint;

/** Currently defined configuration attribute types */
typedef enum
{
    VASTConfigAttribRTFormat		= 0,
    VASTConfigAttribSpatialResidual	= 1,
    VASTConfigAttribSpatialClipping	= 2,
    VASTConfigAttribIntraResidual		= 3,
    VASTConfigAttribEncryption		= 4,
    VASTConfigAttribRateControl		= 5,

    /** @name Attributes for decoding */
    /**@{*/
    /**
     * \brief Slice Decoding mode. Read/write.
     *
     * This attribute determines what mode the driver supports for slice
     * decoding, through vastGetConfigAttributes(); and what mode the user
     * will be providing to the driver, through vastCreateConfig(), if the
     * driver supports those. If this attribute is not set by the user then
     * it is assumed that VAST_DEC_SLICE_MODE_NORMAL mode is used.
     *
     * See \c VA_DEC_SLICE_MODE_xxx for the list of slice decoding modes.
     */
    VASTConfigAttribDecSliceMode		= 6,
   /**
     * \brief JPEG decoding attribute. Read-only.
     *
     * This attribute exposes a number of capabilities of the underlying
     * JPEG implementation. The attribute value is partitioned into fields as defined in the
     * VASTConfigAttribValDecJPEG union.
     */
    VASTConfigAttribDecJPEG             = 7,
    /**
     * \brief Decode processing support. Read/write.
     *
     * This attribute determines if the driver supports video processing
     * with decoding using the decoding context in a single call, through
     * vastGetConfigAttributes(); and if the user may use this feature,
     * through vastCreateConfig(), if the driver supports the user scenario.
     * The user will essentially create a regular decode VAContext.  Therefore,
     * the parameters of vastCreateContext() such as picture_width, picture_height
     * and render_targets are in relation to the decode output parameters
     * (not processing output parameters) as normal.
     * If this attribute is not set by the user then it is assumed that no
     * extra processing is done after decoding for this decode context.
     *
     * Since essentially the application is creating a decoder config and context,
     * all function calls that take in the config (e.g. vastQuerySurfaceAttributes())
     * or context are in relation to the decoder, except those video processing
     * function specified in the next paragraph.
     *
     * Once the decode config and context are created, the user must further
     * query the supported processing filters using vastQueryVideoProcFilters(),
     * vastQueryVideoProcFilterCaps(), vastQueryVideoProcPipelineCaps() by specifying
     * the created decode context.  The user must provide processing information
     * and extra processing output surfaces as "additional_outputs" to the driver
     * through VASTProcPipelineParameterBufferType.  The render_target specified
     * at vastBeginPicture() time refers to the decode output surface.  The
     * target surface for the output of processing needs to be a different
     * surface since the decode process requires the original reconstructed buffer.
     * The “surface” member of VASTProcPipelineParameterBuffer should be set to the
     * same as “render_target” set in vastBeginPicture(), but the driver may choose
     * to ignore this parameter.
     */
    VASTConfigAttribDecProcessing		= 8,
    /** @name Attributes for encoding */
    /**@{*/
    /**
     * \brief Packed headers mode. Read/write.
     *
     * This attribute determines what packed headers the driver supports,
     * through vastGetConfigAttributes(); and what packed headers the user
     * will be providing to the driver, through vastCreateConfig(), if the
     * driver supports those.
     *
     * See \c VA_ENC_PACKED_HEADER_xxx for the list of packed headers.
     */
    VASTConfigAttribEncPackedHeaders      = 10,
    /**
     * \brief Interlaced mode. Read/write.
     *
     * This attribute determines what kind of interlaced encoding mode
     * the driver supports.
     *
     * See \c VA_ENC_INTERLACED_xxx for the list of interlaced modes.
     */
    VASTConfigAttribEncInterlaced         = 11,
    /**
     * \brief Maximum number of reference frames. Read-only.
     *
     * This attribute determines the maximum number of reference
     * frames supported for encoding.
     *
     * Note: for H.264 encoding, the value represents the maximum number
     * of reference frames for both the reference picture list 0 (bottom
     * 16 bits) and the reference picture list 1 (top 16 bits).
     */
    VASTConfigAttribEncMaxRefFrames       = 13,
    /**
     * \brief Maximum number of slices per frame. Read-only.
     *
     * This attribute determines the maximum number of slices the
     * driver can support to encode a single frame.
     */
    VASTConfigAttribEncMaxSlices          = 14,
    /**
     * \brief Slice structure. Read-only.
     *
     * This attribute determines slice structures supported by the
     * driver for encoding. This attribute is a hint to the user so
     * that he can choose a suitable surface size and how to arrange
     * the encoding process of multiple slices per frame.
     *
     * More specifically, for H.264 encoding, this attribute
     * determines the range of accepted values to
     * VASTEncSliceParameterBufferH264::macroblock_address and
     * VASTEncSliceParameterBufferH264::num_macroblocks.
     *
     * See \c VA_ENC_SLICE_STRUCTURE_xxx for the supported slice
     * structure types.
     */
    VASTConfigAttribEncSliceStructure     = 15,
    /**
     * \brief Macroblock information. Read-only.
     *
     * This attribute determines whether the driver supports extra
     * encoding information per-macroblock. e.g. QP.
     *
     * More specifically, for H.264 encoding, if the driver returns a non-zero
     * value for this attribute, this means the application can create
     * additional #VASTEncMacroblockParameterBufferH264 buffers referenced
     * through VASTEncSliceParameterBufferH264::macroblock_info.
     */
    VASTConfigAttribEncMacroblockInfo     = 16,
    /**
     * \brief Maximum picture width. Read-only.
     *
     * This attribute determines the maximum picture width the driver supports
     * for a given configuration.
     */
    VASTConfigAttribMaxPictureWidth     = 18,
    /**
     * \brief Maximum picture height. Read-only.
     *
     * This attribute determines the maximum picture height the driver supports
     * for a given configuration.
     */
    VASTConfigAttribMaxPictureHeight    = 19,
    /**
     * \brief JPEG encoding attribute. Read-only.
     *
     * This attribute exposes a number of capabilities of the underlying
     * JPEG implementation. The attribute value is partitioned into fields as defined in the 
     * VAConfigAttribValEncJPEG_VAST union.
     */
    VASTConfigAttribEncJPEG             = 20,
    /**
     * \brief Encoding quality range attribute. Read-only.
     *
     * This attribute conveys whether the driver supports different quality level settings
     * for encoding. A value less than or equal to 1 means that the encoder only has a single
     * quality setting, and a value greater than 1 represents the number of quality levels 
     * that can be configured. e.g. a value of 2 means there are two distinct quality levels. 
     */
    VASTConfigAttribEncQualityRange     = 21,
    /**
     * \brief Encoding quantization attribute. Read-only.
     *
     * This attribute conveys whether the driver supports certain types of quantization methods
     * for encoding (e.g. trellis). See \c VA_ENC_QUANTIZATION_xxx for the list of quantization methods
     */
    VASTConfigAttribEncQuantization     = 22,
    /**
     * \brief Encoding intra refresh attribute. Read-only.
     *
     * This attribute conveys whether the driver supports certain types of intra refresh methods
     * for encoding (e.g. adaptive intra refresh or rolling intra refresh).
     * See \c VA_ENC_INTRA_REFRESH_xxx for intra refresh methods
     */
    VASTConfigAttribEncIntraRefresh     = 23,
    /**
     * \brief Encoding skip frame attribute. Read-only.
     *
     * This attribute conveys whether the driver supports sending skip frame parameters 
     * (VASTEncMiscParameterTypeSkipFrame) to the encoder's rate control, when the user has 
     * externally skipped frames. 
     */
    VASTConfigAttribEncSkipFrame        = 24,
    /**
     * \brief Encoding region-of-interest (ROI) attribute. Read-only.
     *
     * This attribute conveys whether the driver supports region-of-interest (ROI) encoding,
     * based on user provided ROI rectangles.  The attribute value is partitioned into fields
     * as defined in the VASTConfigAttribValEncROI union.
     *
     * If ROI encoding is supported, the ROI information is passed to the driver using
     * VASTEncMiscParameterTypeROI.
     */
    VASTConfigAttribEncROI              = 25,
    /**
     * \brief Encoding extended rate control attribute. Read-only.
     *
     * This attribute conveys whether the driver supports any extended rate control features
     * The attribute value is partitioned into fields as defined in the
     * VASTConfigAttribValEncRateControlExt union.
     */
    VASTConfigAttribEncRateControlExt   = 26,
    /**
     * \brief Processing rate reporting attribute. Read-only.
     *
     * This attribute conveys whether the driver supports reporting of
     * encode/decode processing rate based on certain set of parameters
     * (i.e. levels, I frame internvals) for a given configuration.
     * If this is supported, vastQueryProcessingRate() can be used to get
     * encode or decode processing rate.
     * See \c VA_PROCESSING_RATE_xxx for encode/decode processing rate
     */
    VASTConfigAttribProcessingRate    = 27,
    /**
     * \brief Encoding dirty rectangle. Read-only.
     *
     * This attribute conveys whether the driver supports dirty rectangle.
     * encoding, based on user provided ROI rectangles which indicate the rectangular areas
     * where the content has changed as compared to the previous picture.  The regions of the
     * picture that are not covered by dirty rect rectangles are assumed to have not changed
     * compared to the previous picture.  The encoder may do some optimizations based on
     * this information.  The attribute value returned indicates the number of regions that
     * are supported.  e.g. A value of 0 means dirty rect encoding is not supported.  If dirty
     * rect encoding is supported, the ROI information is passed to the driver using
     * VASTEncMiscParameterTypeDirtyRect.
     */
     VASTConfigAttribEncDirtyRect       = 28,
    /**
     * \brief Parallel Rate Control (hierachical B) attribute. Read-only.
     *
     * This attribute conveys whether the encoder supports parallel rate control.
     * It is a integer value 0 - unsupported, > 0 - maximum layer supported.
     * This is the way when hireachical B frames are encoded, multiple independent B frames
     * on the same layer may be processed at same time. If supported, app may enable it by
     * setting enable_parallel_brc in VASTEncMiscParameterRateControl,and the number of B frames
     * per layer per GOP will be passed to driver through VASTEncMiscParameterParallelRateControl
     * structure.Currently three layers are defined.
     */
     VASTConfigAttribEncParallelRateControl   = 29,
     /**
     * \brief Dynamic Scaling Attribute. Read-only.
     *
     * This attribute conveys whether encoder is capable to determine dynamic frame
     * resolutions adaptive to bandwidth utilization and processing power, etc.
     * It is a boolean value 0 - unsupported, 1 - supported.
     * If it is supported,for VP9, suggested frame resolution can be retrieved from VASTCodedBufferVP9Status.
     */
     VASTConfigAttribEncDynamicScaling        = 30,
     /**
     * \brief frame size tolerance support
     * it indicates the tolerance of frame size
     */
     VASTConfigAttribFrameSizeToleranceSupport = 31,
     /**
     * \brief Encode function type for FEI.
     *
     * This attribute conveys whether the driver supports different function types for encode.
     * It can be VAST_FEI_FUNCTION_ENC, VAST_FEI_FUNCTION_PAK, or VAST_FEI_FUNCTION_ENC_PAK. Currently
     * it is for FEI entry point only.
     * Default is VAST_FEI_FUNCTION_ENC_PAK.
     */
     VASTConfigAttribFEIFunctionType     = 32,
    /**
     * \brief Maximum number of FEI MV predictors. Read-only.
     *
     * This attribute determines the maximum number of MV predictors the driver
     * can support to encode a single frame. 0 means no MV predictor is supported.
     * Currently it is for FEI entry point only.
     */
    VASTConfigAttribFEIMVPredictors     = 33,
    /**
     * \brief Statistics attribute. Read-only.
     *
     * This attribute exposes a number of capabilities of the VASTEntrypointStats entry
     * point. The attribute value is partitioned into fields as defined in the
     * VASTConfigAttribValStats union. Currently it is for VASTEntrypointStats only.
     */
    VASTConfigAttribStats               = 34,
     /**
     * \brief Tile Support Attribute. Read-only.
     *
     * This attribute conveys whether encoder is capable to support tiles.
     * If not supported, the tile related parameters sent to encoder, such as
     * tiling structure, should be ignored. 0 - unsupported, 1 - supported.
     */
     VASTConfigAttribEncTileSupport        = 35,
    /**
     * \brief whether accept rouding setting from application. Read-only.
     * This attribute is for encode quality, if it is report,
     * application can change the rounding setting by VASTEncMiscParameterTypeCustomRoundingControl
     */
    VASTConfigAttribCustomRoundingControl = 36,
    /**
     * \brief Encoding QP info block size attribute. Read-only.
     * This attribute conveys the block sizes that underlying driver
     * support for QP info for buffer #VAEncQpBuffer.
     */
    VASTConfigAttribQPBlockSize            = 37,
    VASTConfigAttribGPUNodes               = 38,
    /**@}*/
    VASTConfigAttribTypeMax
} VASTConfigAttribType;


/** \brief Surface attribute types. */
typedef enum {
    VASTSurfaceAttribNone = 0,
    /**
     * \brief Pixel format (fourcc).
     *
     * The value is meaningful as input to vastQuerySurfaceAttributes().
     * If zero, the driver returns the optimal pixel format for the
     * specified config. Otherwise, if non-zero, the value represents
     * a pixel format (FOURCC) that is kept as is on output, if the
     * driver supports it. Otherwise, the driver sets the value to
     * zero and drops the \c VAST_SURFACE_ATTRIB_SETTABLE flag.
     */
    VASTSurfaceAttribPixelFormat,
    /** \brief Minimal width in pixels (int, read-only). */
    VASTSurfaceAttribMinWidth,
    /** \brief Maximal width in pixels (int, read-only). */
    VASTSurfaceAttribMaxWidth,
    /** \brief Minimal height in pixels (int, read-only). */
    VASTSurfaceAttribMinHeight,
    /** \brief Maximal height in pixels (int, read-only). */
    VASTSurfaceAttribMaxHeight,
    /** \brief Surface memory type expressed in bit fields (int, read/write). */
    VASTSurfaceAttribMemoryType,
    /** \brief External buffer descriptor (pointer, write). */
    VASTSurfaceAttribExternalBufferDescriptor,
    /** \brief Surface usage hint, gives the driver a hint of intended usage 
     *  to optimize allocation (e.g. tiling) (int, read/write). */
    VASTSurfaceAttribUsageHint,
    /** \brief this attrib info surface belong to which slice, should be added by intel. */
    VASTSurfaceAttribGPUNodes,
    /** \brief Number of surface attributes. */
    VASTSurfaceAttribCount
} VASTSurfaceAttribType;

/** \brief Generic value types. */
typedef enum  {
    VASTGenericValueTypeInteger = 1,      /**< 32-bit signed integer. */
    VASTGenericValueTypeFloat,            /**< 32-bit floating-point value. */
    VASTGenericValueTypePointer,          /**< Generic pointer type */
    VASTGenericValueTypeFunc              /**< Pointer to function */
} VASTGenericValueType;


typedef void* VASTDisplay;	/* window system dependent */

typedef unsigned int VASTGenericID;

typedef VASTGenericID VASTConfigID;

typedef VASTGenericID VASTContextID;

typedef VASTGenericID VASTSurfaceID;

typedef VASTGenericID VASTBufferID;

/** \brief Generic function type. */
typedef void (*VASTGenericFunc)(void);

/** \brief Generic value. */
typedef struct _VASTGenericValue {
    /** \brief Value type. See #VASTGenericValueType. */
    VASTGenericValueType  type;
    /** \brief Value holder. */
    union {
        /** \brief 32-bit signed integer. */
        int32_t             i;
        /** \brief 32-bit float. */
        float           f;
        /** \brief Generic pointer. */
        void           *p;
        /** \brief Pointer to function. */
        VASTGenericFunc   fn;
    }                   value;
} VASTGenericValue;

/** \brief Surface attribute. */
typedef struct _VASTSurfaceAttrib {
    /** \brief Type. */
    VASTSurfaceAttribType type;
    /** \brief Flags. See "Surface attribute flags". */
    uint32_t        flags;
    /** \brief Value. See "Surface attribute types" for the expected types. */
    VASTGenericValue      value;
} VASTSurfaceAttrib;

/**
 * Configuration attributes
 * If there is more than one value for an attribute, a default
 * value will be assigned to the attribute if the client does not
 * specify the attribute when creating a configuration
 */
typedef struct _VASTConfigAttrib {
    VASTConfigAttribType type;
    uint32_t value; /* OR'd flags (bits) for this attribute */
} VASTConfigAttrib;

/** \brief Rate control parameters */
typedef struct _VASTEncMiscParameterRateControl
{
    /* this is the maximum bit-rate to be constrained by the rate control implementation */
    uint32_t bits_per_second;
    /* this is the bit-rate the rate control is targeting, as a percentage of the maximum
     * bit-rate for example if target_percentage is 95 then the rate control will target
     * a bit-rate that is 95% of the maximum bit-rate
     */
    uint32_t target_percentage;
    /* windows size in milliseconds. For example if this is set to 500,
     * then the rate control will guarantee the target bit-rate over a 500 ms window
     */
    uint32_t window_size;
    /* initial QP at I frames */
    uint32_t initial_qp;
    uint32_t min_qp;
    uint32_t basic_unit_size;
    union
    {
        struct
        {
            uint32_t reset : 1;
            uint32_t disable_frame_skip : 1; /* Disable frame skip in rate control mode */
            uint32_t disable_bit_stuffing : 1; /* Disable bit stuffing in rate control mode */
            uint32_t mb_rate_control : 4; /* Control VA_RC_MB 0: default, 1: enable, 2: disable, other: reserved*/
            /*
             * The temporal layer that the rate control parameters are specified for.
             */
            uint32_t temporal_id : 8;
            uint32_t cfs_I_frames : 1; /* I frame also follows CFS */
            uint32_t enable_parallel_brc    : 1;
            uint32_t enable_dynamic_scaling : 1;
             /**  \brief Frame Tolerance Mode
             *  Indicates the tolerance the application has to variations in the frame size.
             *  For example, wireless display scenarios may require very steady bit rate to
             *  reduce buffering time. It affects the rate control algorithm used,
             *  but may or may not have an effect based on the combination of other BRC
             *  parameters.  Only valid when the driver reports support for
             *  #VASTConfigAttribFrameSizeToleranceSupport.
             *
             *  equals 0    -- normal mode;
             *  equals 1    -- maps to sliding window;
             *  equals 2    -- maps to low delay mode;
             *  other       -- invalid.
             */
            uint32_t frame_tolerance_mode   : 2;
            uint32_t reserved               : 12;
        } bits;
        uint32_t value;
    } rc_flags;
    uint32_t ICQ_quality_factor; /* Initial ICQ quality factor: 1-51. */
    /** \brief Reserved bytes for future use, must be zero */
    uint32_t max_qp;
    uint32_t va_reserved[VAST_PADDING_MEDIUM - 2];
    char reserved[1024];
} VASTEncMiscParameterRateControl;


typedef struct _VASTEncMiscParameterHRD
{
    uint32_t initial_buffer_fullness;       /* in bits */
    uint32_t buffer_size;                   /* in bits */

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterHRD;

typedef struct _VASTEncMiscParameterFrameRate
{
    /*
     * The framerate is specified as a number of frames per second, as a
     * fraction.  The denominator of the fraction is given in the top half
     * (the high two bytes) of the framerate field, and the numerator is
     * given in the bottom half (the low two bytes).
     *
     * That is:
     * denominator = framerate >> 16 & 0xffff;
     * numerator   = framerate & 0xffff;
     * fps         = numerator / denominator;
     *
     * For example, if framerate is set to (100 << 16 | 750), this is
     * 750 / 100, hence 7.5fps.
     *
     * If the denominator is zero (the high two bytes are both zero) then
     * it takes the value one instead, so the framerate is just the integer
     * in the low 2 bytes.
     */
    uint32_t framerate;
    union
    {
        struct
        {
            /*
             * The temporal id the framerate parameters are specified for.
             */
            uint32_t temporal_id : 8;
            uint32_t reserved : 24;
         } bits;
         uint32_t value;
     } framerate_flags;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterFrameRate;

/* HDR10 */
typedef struct _Hdr10DisplaySeiVast
{
    uint8_t hdr10_display_enable;
    uint16_t hdr10_dx0;
    uint16_t hdr10_dy0;
    uint16_t hdr10_dx1;
    uint16_t hdr10_dy1;
    uint16_t hdr10_dx2;
    uint16_t hdr10_dy2;
    uint16_t hdr10_wx;
    uint16_t hdr10_wy;
    uint32_t hdr10_maxluma;
    uint32_t hdr10_minluma;
}VASTHdr10DisplaySei;

typedef struct _Hdr10LightLevelSeiVast
{
    uint8_t  hdr10_lightlevel_enable;
    uint16_t hdr10_maxlight;
    uint16_t hdr10_avglight;
}VASTHdr10LightLevelSei;

typedef struct _VuiColorDescriptionVast
{
    uint8_t vuiColorDescripPresentFlag;                        /* color description present in the vui.0- not present, 1- present */
    uint8_t vuiColorPrimaries;                                 /* Color's Primaries */
    uint8_t vuiTransferCharacteristics;                        /* Transfer Characteristics */
    uint8_t vuiMatrixCoefficients;                             /* Matrix Coefficients */
}VASTVuiColorDescription;


typedef struct _VASTRectangle
{
    int16_t x;
    int16_t y;
    uint16_t width;
    uint16_t height;
} VASTRectangle;

typedef struct _VASTEncROI
{
        /** \brief Defines the ROI boundary in pixels, the driver will map it to appropriate
         *  codec coding units.  It is relative to frame coordinates for the frame case and
         *  to field coordinates for the field case. */
        VASTRectangle     roi_rectangle;
        /**
         * \brief ROI value
         *
         * \ref roi_value specifies ROI delta QP or ROI priority.
         * --  ROI delta QP is the value that will be added on top of the frame level QP.
         * --  ROI priority specifies the priority of a region, it can be positive (more important)
         * or negative (less important) values and is compared with non-ROI region (taken as value 0),
         * E.g. ROI region with \ref roi_value -3 is less important than the non-ROI region (\ref roi_value
         * implied to be 0) which is less important than ROI region with roi_value +2. For overlapping
         * regions, the roi_value that is first in the ROI array will have priority.
         *
         * \ref roi_value always specifes ROI delta QP when VASTConfigAttribRateControl == VAST_RC_CQP , no matter
         * the value of \c roi_value_is_qp_delta in #VASTEncMiscParameterBufferROI.
         *
         * \ref roi_value depends on \c roi_value_is_qp_delta in #VASTEncMiscParameterBufferROI when
         * VASTConfigAttribRateControl != VAST_RC_CQP . \ref roi_value specifies ROI_delta QP if \c roi_value_is_qp_delta
         * in VASTEncMiscParameterBufferROI is 1, otherwise \ref roi_value specifies ROI priority.
         */
        int32_t            roi_value;//vastai fix bug
} VASTEncROI;

typedef struct _VASTEncMiscParameterBufferQualityLevel {
    /** \brief Encoding quality level setting. When set to 0, default quality
     * level is used.
     */
    uint32_t                quality_level;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterBufferQualityLevel;

/**
 * \brief Coded buffer segment.
 *
 * #VASTCodedBufferSegment is an element of a linked list describing
 * some information on the coded buffer. The coded buffer segment
 * could contain either a single NAL unit, or more than one NAL unit. 
 * It is recommended (but not required) to return a single NAL unit 
 * in a coded buffer segment, and the implementation should set the 
 * VA_CODED_BUF_STATUS_SINGLE_NALU status flag if that is the case.
 */
typedef  struct _VASTCodedBufferSegment  {
    /**
     * \brief Size of the data buffer in this segment (in bytes).
     */
    uint32_t        size;
    /** \brief Bit offset into the data buffer where the video data starts. */
    uint32_t        bit_offset;
    /** \brief Status set by the driver. See \c VA_CODED_BUF_STATUS_*. */
    uint32_t        status;
    /** \brief Reserved for future use. */
    uint32_t        reserved;
    /** \brief Pointer to the start of the data buffer. */
    void               *buf;
    /**
     * \brief Pointer to the next #VASTCodedBufferSegment element,
     * or \c NULL if there is none.
     */
    void               *next;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTCodedBufferSegment;

typedef struct _VAEncMiscParameter
{
    int use_vast_params;
    unsigned int rate_control_mode;
    unsigned int quality_level;
    unsigned int quality_range;
    unsigned int num_frames_in_sequence;
    unsigned int frame_width_in_pixel;
    unsigned int frame_height_in_pixel;
    unsigned int max_slice_or_seg_num;

    unsigned int recon_frame_compression_mode;
    unsigned int enable_P010_ref;

    struct {
        unsigned int num_layers;
        unsigned int size_frame_layer_ids;
        unsigned int frame_layer_ids[32];
        unsigned int curr_frame_layer_id;
    } layer;

    struct {
        unsigned short gop_size;
        unsigned short num_iframes_in_gop;
        unsigned short num_pframes_in_gop;
        unsigned short num_bframes_in_gop;
        unsigned int bits_per_second[MAX_TEMPORAL_LAYERS_VAST];

        int32_t inputRateNumer;
        int32_t inputRateDenom;
        int32_t outputRateNumer;
        int32_t outputRateDenom;
        unsigned int mb_rate_control[MAX_TEMPORAL_LAYERS_VAST];
        unsigned int target_percentage[MAX_TEMPORAL_LAYERS_VAST];
        unsigned int hrd_buffer_size;
        unsigned int hrd_initial_buffer_fullness;
        unsigned int window_size;
        unsigned int initial_qp;
        unsigned int min_qp;
        unsigned int need_reset;
        unsigned int need_reset_new_sps;

        unsigned int smooth_psnr_in_GOP;
        int          intra_qp_delta;
        unsigned int fixedIntraQp;
        unsigned int basic_unit_size;
        unsigned int disable_frame_skip;

        unsigned int num_roi;
        unsigned int roi_max_delta_qp;
        unsigned int roi_min_delta_qp;
        unsigned int roi_value_is_qp_delta;
        VASTEncROI roi[HANTRO_MAX_NUM_ROI_REGIONS_VAST];

        unsigned int roimap_is_enabled;
        unsigned int roimap_block_unit;
        unsigned int roi_map_version;
        VASTBufferID roimap_ipcmmap_buf_id;

        unsigned int cu_ctrl_version;


        unsigned int cu_ctrl_index_enable;


        unsigned int num_ipcm;
        VASTRectangle ipcm[HANTRO_MAX_NUM_IPCM_REGIONS_VAST];

        unsigned int num_intra_area;
        VASTRectangle intra_area[HANTRO_MAX_NUM_INTRA_AREA_REGIONS_VAST];

        unsigned int cir_start;
        unsigned int cir_interval;

        unsigned int gdr_duration;
        int video_signal_type_present;   
        unsigned int video_full_range;
        
        //  Video format of the source video.  0 = component, 1 = PAL, 2 = NTSC,
        //  * 3 = SECAM, 4 = MAC, 5 = unspecified video format is the default 
        int video_format;

        unsigned int ipcmmap_is_enabled;
        unsigned int skipmap_is_enabled;
        unsigned int pcm_loop_filter_disabled_flag;

        unsigned int mb_cu_encoding_info_output_is_enabled;
 

        int use_extend_rc_params;
        int tolMovingBitRate;
        int monitorFrames;
        int bitVarRangeI;
        int bitVarRangeP;
        int bitVarRangeB;
        unsigned int u32StaticSceneIbitPercent;
        unsigned int rcQpDeltaRange;
        unsigned int rcBaseMBComplexity;
        float tolCtbRcInter;
        float tolCtbRcIntra;
        float maxFrameSizeMultiple; /**< deprecated! maximum multiple to average target frame size */
        int maxFrameSize;       /* max frame size, only valid in llrc mode*/
        int picQpDeltaMin;
        int picQpDeltaMax;
        unsigned int hrd;
        unsigned int bitrateWindow;
        unsigned int pictureSkip;
        int qpHdr;
        unsigned int qpMax;

        unsigned int psnr_info_output_is_enabled;
        VASTBufferID psnr_info_buf_id;
        int32_t cqp;
        int32_t ctbRcRowQpStep;
        uint32_t longTermGap;
        uint32_t longTermGapOffset;
        uint32_t ltrInterval;
        int32_t longTermQpDelta;
        int32_t gopLowdelay;
        unsigned int qpMaxI;
        unsigned int qpMinI;
        int32_t bFrameQpDelta;
        int32_t vbr;
        char * gopCfg;
        char * roiMapDeltaQpFile;
        char * roiMapDeltaQpBinFile;
        char * ipcmMapFile;
        char * roiMapInfoBinFile;
        char * RoimapCuCtrlInfoBinFile;
        char * RoimapCuCtrlIndexBinFile;
        uint32_t skipMapBlockUnit;
        char * skipMapFile;
        int adaptiveIntraQpDelta;
        int targetPsnr;
        int32_t bitrateBalanceLevel;
        int32_t inLoopDSRatio;
        int32_t aq_mode;
        float aq_strength;
        float psyFactor;
        double qCompress;
        double iQpFactor;
    } brc;

    struct {
        int use_extend_coding_ctrl_params;

        unsigned int cabacInitFlag;
        unsigned int enableCabac;
        unsigned int disableDeblockingFilter;
        unsigned int enableSao;
        int tc_Offset;
        int beta_Offset;
        unsigned int enableDeblockOverride;
        unsigned int deblockOverride;
        int32_t tiles_enabled_flag;
        int num_tile_columns;
        int num_tile_rows;
        int loop_filter_across_tiles_enabled_flag;
        unsigned int enableScalingList;
        unsigned int RpsInSliceHeader;
        int chroma_qp_offset;

        uint32_t streamMultiSegmentMode;
        uint32_t streamMultiSegmentAmount;

        uint32_t noiseReductionEnable;
        uint32_t noiseLow;
        uint32_t firstFrameSigma;
        unsigned int fieldOrder;
        int32_t smartModeEnable;
        /* dynamic rdo params */
        uint32_t dynamicRdoEnable;
        uint32_t dynamicRdoCu16Bias;
        uint32_t dynamicRdoCu16Factor;
        uint32_t dynamicRdoCu32Bias;
        uint32_t dynamicRdoCu32Factor;
        int32_t rdoLevel;
        int32_t enableRdoQuant;
    } coding_ctrl;

    //line buffer
    struct {
        unsigned int inputLineBufMode;
        unsigned int inputLineBufDepth;
        unsigned int amountPerLoopBack;
    } low_latency;

    struct {
        int use_gmv_params;
        int16_t  mv0[2];  /* past reference */
        int16_t  mv1[2];  /* future reference */
        char * gmvFileName[2];
    } gmv;

    struct {
        /** \brief cropping offset x, in pixel unit, in original picture*/
        unsigned int        cropping_offset_x;
        /** \brief cropping offset y,in pixel unit, in original picture*/
        unsigned int        cropping_offset_y;

        /** \brief cropped width in pixel unit, in original picture. if 0, equal to input surface orig_width*/
        unsigned int        cropped_width;
        /** \brief cropped height in pixel unit, in original picture. if 0, equal to input surface orig_height*/
        unsigned int        cropped_height;


        /** \brief rotation. VA_PREPROCESS_ROTATION_NONE - none, VA_PREPROCESS_ROTATION_270 - 270, VA_PREPROCESS_ROTATION_90 - 90, VA_PREPROCESS_ROTATION_180 - 180 */
        unsigned int        rotation;
        /** \brief mirror. 0 - none, 1 - mirror */
        unsigned int        mirror;
        /** \brief constant chroma enabled. */
        unsigned int        preprocess_constant_chroma_is_enabled;
        /** \brief constant Cb value. */
        unsigned int        constCb;
        /** \brief constant Cr value. */
        unsigned int        constCr;
        /** \brief colorConversion value. */
        unsigned int        colorConversion;
        /** \brief down-scaled width value.
            *Optional down-scaled output picture width,multiple of 4. 0=disabled. [16..width] */
        unsigned int        scaledWidth;
        /** \brief down-scaled height value.
            * Optional down-scaled output picture height,multiple of 2. [96..height] */
        unsigned int        scaledHeight;
        unsigned int interlacedFrame;
        unsigned int codedChromaIdc;
        unsigned int scaledOutputFormat;
        unsigned int inputFrameCropFlag;
        unsigned int inputFrameCropTop;
        unsigned int inputFrameCropBottom;
        unsigned int inputFrameCropLeft;
        unsigned int inputFrameCropRight;        
    } preprocess;
    /**packed header output control**/
    struct {
        unsigned int sps_reoutput_enable;
        unsigned int pps_reoutput_enable;
        unsigned int sei_output_enable;
        unsigned int aud_output_enable;
    } phoc;


    void* vsi_private_context;
    //VASTProfile profile;

    unsigned int is_tmp_id: 1;
    unsigned int low_power_mode: 1;
    unsigned int soft_batch_force: 1;
    unsigned int context_roi: 1;
    unsigned int is_new_sequence: 1; /* Currently only valid for H.264, TODO for other codecs */

    int temporal_id;

    unsigned char special_size;
    unsigned char ltrcnt;
    int skipFramePOC;
    unsigned int enableOutputCuInfo;
    unsigned int hashtype;
    unsigned int ssim;
    unsigned int enableVuiTimingInfo;
    unsigned int verbose;

    /* for HDR10 */
    unsigned int use_hdr10_params;
    VASTHdr10DisplaySei    Hdr10Display;
    VASTHdr10LightLevelSei Hdr10LightLevel;
    VASTVuiColorDescription      Hdr10Color;
    int sar_width;
    int sar_height;    

    /**used for JPEG Lossless. */
    unsigned int losslessEn;
    unsigned int predictMode;
    unsigned int ptransValue;

    struct {
        int codingWidth;
        int codingHeight;
        int lumWidthSrc;
        int lumHeightSrc;
        int inputFormat;
        int horOffsetSrc;
        int verOffsetSrc;
        int input_alignment;
        int ref_alignment;
        int ref_ch_alignment;
        int aqInfoAlignment;
    } image_info;

    unsigned int flushRemainingFrames;
    unsigned int extDSRatio;
    unsigned int frames2Encode;
    unsigned int encodedFramesCnt;
    unsigned int lookaheadDepth;

    VASTBufferID adaptive_gop_decision_buf_id;
    uint32_t adaptive_gop_buffer;
    unsigned int adaptiveGOPEn;

    unsigned int numberMultiCore;
    unsigned int *dualBufMultiCore[2];

    unsigned int has_dec400;
    unsigned int intraPicRate;
    int32_t sceneChange[MAX_SCENE_CHANGE_VAST];
    unsigned int tier;
    uint32_t cpbMaxRate;
    int32_t outReconFrame;
    int32_t formatCustomizedType;
    int32_t multimode;
    uint32_t bitDepthLuma;
    uint32_t bitDepthChroma;
    int32_t cuInfoVersion;
    uint32_t rasterscan;
    char * halfDsInput;
    unsigned int streamBufChain;
    unsigned int MEVertRange;
    uint32_t picOrderCntType;
    uint32_t log2MaxPicOrderCntLsb;
    uint32_t log2MaxFrameNum;
    uint32_t preset;
    uint32_t tune;
    uint32_t firstPic;
    uint32_t lastPic;
    uint32_t lastVastFrame;
    uint32_t lastCodingType;
    int32_t crf;
    uint32_t userCoreID;
    int32_t insertIDR;
    int32_t P2B;
    int32_t bBPyramid; // 0: non-referece B Frames 1: reference B Frames
    int32_t sei_buf_count;
    VASTBufferID sei_buf_id[MAX_SEI_COUNT];
    int32_t sei_buf_size[MAX_SEI_COUNT];
    int32_t llRc;
    int32_t ctbRc;
    int32_t gopChangeIdr;//for insertIDR
    int32_t psnrType;//for psnr,411 or 611
    int32_t speedLimit;//for multi-mode speed limit;
    unsigned int maxBFrames;
    unsigned int stillpicture; // for avif still picture
    int32_t vfr;
    uint64_t in_tbn;
    int32_t sliceSize; // Slice size in CTB/MB rows for multiSlice
    int32_t openGop;
    int32_t smartEnc;
    int32_t passmodel;
    int32_t qLevel;

    int32_t lkLatency;
    char reserved[1024];
} VAEncMiscParameter;

#define VAST_INVALID_ID		0xffffffff
#define VAST_INVALID_SURFACE	VAST_INVALID_ID
#define VAST_STATUS_SUCCESS			0x00000000
#define VA_PROGRESSIVE 0x1

typedef int VASTStatus;

typedef VASTGenericID VASTSubpictureID;

typedef VASTGenericID VASTImageID;

/* 
 * Pre-defined fourcc codes
 */
#define VAST_FOURCC_NV12		0x3231564E
#define VAST_FOURCC_NV21		0x3132564E
#define VAST_FOURCC_AI44		0x34344149
#define VAST_FOURCC_RGBA		0x41424752
#define VAST_FOURCC_RGBX		0x58424752
#define VAST_FOURCC_BGRA		0x41524742
#define VAST_FOURCC_BGRX		0x58524742
#define VAST_FOURCC_RGB565        0x36314752
#define VAST_FOURCC_BGR565        0x36314742
#define VAST_FOURCC_ARGB		0x42475241
#define VAST_FOURCC_XRGB		0x42475258
#define VAST_FOURCC_ABGR          0x52474241
#define VAST_FOURCC_XBGR          0x52474258
#define VAST_FOURCC_UYVY          0x59565955
#define VAST_FOURCC_YUY2          0x32595559
#define VAST_FOURCC_AYUV          0x56555941
#define VAST_FOURCC_NV11          0x3131564e
#define VAST_FOURCC_YV12          0x32315659
#define VAST_FOURCC_P208          0x38303250
/* IYUV same as I420, but most user perfer I420, will deprecate it */
#define VAST_FOURCC_IYUV          0x56555949
#define VAST_FOURCC_I420          0x30323449
#define VAST_FOURCC_YV24          0x34325659
#define VAST_FOURCC_YV32          0x32335659
#define VAST_FOURCC_Y800          0x30303859
#define VAST_FOURCC_IMC3          0x33434D49
#define VAST_FOURCC_411P          0x50313134
#define VAST_FOURCC_422H          0x48323234
#define VAST_FOURCC_422V          0x56323234
#define VAST_FOURCC_444P          0x50343434
#define VAST_FOURCC_RGBP          0x50424752
#define VAST_FOURCC_BGRP          0x50524742
#define VAST_FOURCC_411R          0x52313134 /* rotated 411P */
#define VAST_FOURCC_BGGR8           0x42565559
#define VAST_FOURCC_RGGB8           0x42575559
#define VAST_FOURCC_GBRG8           0x42585559
#define VAST_FOURCC_GRBG8           0x42595559
/**
 * Planar YUV 4:2:2.
 * 8-bit Y plane, followed by 8-bit 2x1 subsampled V and U planes
 */
#define VAST_FOURCC_YV16          0x36315659
/**
 * 10-bit and 16-bit Planar YUV 4:2:0. 
 */
#define VAST_FOURCC_P010          0x30313050
#define VAST_FOURCC_P016          0x36313050

/**
 * 10-bit Planar YUV 420 and occupy the lower 10-bit.
 */
#define VAST_FOURCC_I010          0x30313049
#define VAST_FOURCC_YUVJ          0x4A565559   

#define VAST_SLICE_DATA_FLAG_ALL	0x00	/* whole slice is in the buffer */
#define VAST_SLICE_DATA_FLAG_BEGIN	0x01	/* The beginning of the slice is in the buffer but the end if not */
#define VAST_SLICE_DATA_FLAG_MIDDLE	0x02	/* Neither beginning nor end of the slice is in the buffer */
#define VAST_SLICE_DATA_FLAG_END	0x04	/* end of the slice is in the buffer */

typedef enum
{
    VASTPictureParameterBufferType	= 0,
    VASTIQMatrixBufferType		= 1,
    VASTBitPlaneBufferType		= 2,
    VASTSliceGroupMapBufferType		= 3,
    VASTSliceParameterBufferType		= 4,
    VASTSliceDataBufferType		= 5,
    VASTMacroblockParameterBufferType	= 6,
    VASTResidualDataBufferType		= 7,
    VASTDeblockingParameterBufferType	= 8,
    VASTImageBufferType			= 9,
    VASTProtectedSliceDataBufferType	= 10,
    VASTQMatrixBufferType                 = 11,
    VASTHuffmanTableBufferType            = 12,
    VASTProbabilityBufferType             = 13,

/* Following are encode buffer types */
    VASTEncCodedBufferType		= 21,
    VASTEncSequenceParameterBufferType	= 22,
    VASTEncPictureParameterBufferType	= 23,
    VASTEncSliceParameterBufferType	= 24,
    VASTEncPackedHeaderParameterBufferType = 25,
    VASTEncPackedHeaderDataBufferType     = 26,
    VASTEncMiscParameterBufferType	= 27,
    VASTEncMacroblockParameterBufferType	= 28,
    VASTEncMacroblockMapBufferType        = 29,

    /**
     * \brief Encoding QP buffer
     *
     * This buffer contains QP per MB for encoding. Currently
     * VASTEncQPBufferH264 is defined for H.264 encoding, see
     * #VASTEncQPBufferH264 for details
     */
    VASTEncQPBufferType                   = 30,
/* Following are video processing buffer types */
    /**
     * \brief Video processing pipeline parameter buffer.
     *
     * This buffer describes the video processing pipeline. See
     * #VASTProcPipelineParameterBuffer for details.
     */
    VASTProcPipelineParameterBufferType   = 41,
    /**
     * \brief Video filter parameter buffer.
     *
     * This buffer describes the video filter parameters. All buffers
     * inherit from #VASTProcFilterParameterBufferBase, thus including
     * a unique filter buffer type.
     *
     * The default buffer used by most filters is #VASTProcFilterParameterBuffer.
     * Filters requiring advanced parameters include, but are not limited to,
     * deinterlacing (#VASTProcFilterParameterBufferDeinterlacing),
     * color balance (#VASTProcFilterParameterBufferColorBalance), etc.
     */
    VASTProcFilterParameterBufferType     = 42,
    /**
     * \brief FEI specific buffer types
     */
    VASTEncFEIMVBufferType                = 43,
    VASTEncFEIMBCodeBufferType            = 44,
    VASTEncFEIDistortionBufferType        = 45,
    VASTEncFEIMBControlBufferType         = 46,
    VASTEncFEIMVPredictorBufferType       = 47,
    VASTStatsStatisticsParameterBufferType = 48,
    /** \brief Statistics output for VASTEntrypointStats progressive and top field of interlaced case*/
    VASTStatsStatisticsBufferType         = 49,
    /** \brief Statistics output for VASTEntrypointStats bottom field of interlaced case*/
    VASTStatsStatisticsBottomFieldBufferType = 50,
    VASTStatsMVBufferType                 = 51,
    VASTStatsMVPredictorBufferType        = 52,
    /** Force MB's to be non skip for encode.it's per-mb control buffer, The width of the MB map
     * Surface is (width of the Picture in MB unit) * 1 byte, multiple of 64 bytes.
     * The height is (height of the picture in MB unit). The picture is either
     * frame or non-interleaved top or bottom field.  If the application provides this
     *surface, it will override the "skipCheckDisable" setting in VASTEncMiscParameterEncQuality.
     */
    VASTEncMacroblockDisableSkipMapBufferType = 53,
    /**
     * \brief HEVC FEI CTB level cmd buffer
     * it is CTB level information for future usage.
     */
    VASTEncFEICTBCmdBufferType            = 54,
    /**
     * \brief HEVC FEI CU level data buffer
     * it's CTB level information for future usage
     */
    VASTEncFEICURecordBufferType          = 55,
    /** decode stream out buffer, intermedia data of decode, it may include MV, MB mode etc.
      * it can be used to detect motion and analyze the frame contain  */
    VASTDecodeStreamoutBufferType             = 56,
    VASTBufferTypeMax
} VASTBufferType;

typedef struct _VASTImageFormat
{
    uint32_t	fourcc;
    uint32_t	byte_order; /* VAST_LSB_FIRST, VAST_MSB_FIRST */
    uint32_t	bits_per_pixel;
    /* for RGB formats */
    uint32_t	depth; /* significant bits per pixel */
    uint32_t	red_mask;
    uint32_t	green_mask;
    uint32_t	blue_mask;
    uint32_t	alpha_mask;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTImageFormat;

typedef struct _VASTImage
{
    VASTImageID		image_id; /* uniquely identify this image */
    VASTImageFormat	format;
    VASTBufferID		buf;	/* image data buffer */
    /*
     * Image data will be stored in a buffer of type VASTImageBufferType to facilitate
     * data store on the server side for optimal performance. The buffer will be 
     * created by the CreateImage function, and proper storage allocated based on the image
     * size and format. This buffer is managed by the library implementation, and 
     * accessed by the client through the buffer Map/Unmap functions.
     */
    uint16_t	width; 
    uint16_t	height;
    uint32_t	data_size;
    uint32_t	num_planes;	/* can not be greater than 3 */
    /* 
     * An array indicating the scanline pitch in bytes for each plane.
     * Each plane may have a different pitch. Maximum 3 planes for planar formats
     */
    uint32_t	pitches[3];
    /* 
     * An array indicating the byte offset from the beginning of the image data 
     * to the start of each plane.
     */
    uint32_t	offsets[3];

    /* The following fields are only needed for paletted formats */
    int32_t num_palette_entries;   /* set to zero for non-palette images */
    /* 
     * Each component is one byte and entry_bytes indicates the number of components in 
     * each entry (eg. 3 for YUV palette entries). set to zero for non-palette images   
     */
    int32_t entry_bytes; 
    /*
     * An array of ascii characters describing the order of the components within the bytes.
     * Only entry_bytes characters of the string are used.
     */
    int8_t component_order[4];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTImage;


#define VAST_PICTURE_HEVC_INVALID                 0x00000001
#define VAST_PICTURE_HEVC_FIELD_PIC               0x00000002
#define VAST_PICTURE_HEVC_BOTTOM_FIELD            0x00000004
#define VAST_PICTURE_HEVC_LONG_TERM_REFERENCE     0x00000008
#define VAST_PICTURE_HEVC_RPS_ST_CURR_BEFORE      0x00000010
#define VAST_PICTURE_HEVC_RPS_ST_CURR_AFTER       0x00000020
#define VAST_PICTURE_HEVC_RPS_LT_CURR             0x00000040

#define VAST_PICTURE_H264_INVALID			0x00000001
#define VAST_PICTURE_H264_TOP_FIELD		0x00000002
#define VAST_PICTURE_H264_BOTTOM_FIELD		0x00000004
#define VAST_PICTURE_H264_SHORT_TERM_REFERENCE	0x00000008
#define VAST_PICTURE_H264_LONG_TERM_REFERENCE	0x00000010

typedef struct _VASTPictureH264
{
    VASTSurfaceID picture_id;
    uint32_t frame_idx;
    uint32_t flags;
    int32_t TopFieldOrderCnt;
    int32_t BottomFieldOrderCnt;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTPictureH264;

/** H.264 Picture Parameter Buffer */
/* 
 * For each picture, and before any slice data, a single
 * picture parameter buffer must be send.
 */
typedef struct _VASTPictureParameterBufferH264
{
    VASTPictureH264 CurrPic;
    VASTPictureH264 ReferenceFrames[16];	/* in DPB */
    uint16_t picture_width_in_mbs_minus1;
    uint16_t picture_height_in_mbs_minus1;
    uint8_t bit_depth_luma_minus8;
    uint8_t bit_depth_chroma_minus8;
    uint8_t num_ref_frames;
    union {
        struct {
            uint32_t chroma_format_idc			: 2; 
            uint32_t residual_colour_transform_flag		: 1; /* Renamed to separate_colour_plane_flag in newer standard versions. */
            uint32_t gaps_in_frame_num_value_allowed_flag	: 1; 
            uint32_t frame_mbs_only_flag			: 1; 
            uint32_t mb_adaptive_frame_field_flag		: 1; 
            uint32_t direct_8x8_inference_flag		: 1; 
            uint32_t MinLumaBiPredSize8x8			: 1; /* see A.3.3.2 */
            uint32_t log2_max_frame_num_minus4		: 4;
            uint32_t pic_order_cnt_type			: 2;
            uint32_t log2_max_pic_order_cnt_lsb_minus4	: 4;
            uint32_t delta_pic_order_always_zero_flag	: 1;
        } bits;
        uint32_t value;
    } seq_fields;
    // FMO is not supported.
    uint8_t num_slice_groups_minus1;
    uint8_t slice_group_map_type;
    uint16_t slice_group_change_rate_minus1;
    int8_t pic_init_qp_minus26;
    int8_t pic_init_qs_minus26;
    int8_t chroma_qp_index_offset;
    int8_t second_chroma_qp_index_offset;
    union {
        struct {
            uint32_t entropy_coding_mode_flag	: 1;
            uint32_t weighted_pred_flag		: 1;
            uint32_t weighted_bipred_idc		: 2;
            uint32_t transform_8x8_mode_flag	: 1;
            uint32_t field_pic_flag			: 1;
            uint32_t constrained_intra_pred_flag	: 1;
            uint32_t pic_order_present_flag			: 1; /* Renamed to bottom_field_pic_order_in_frame_present_flag in newer standard versions. */
            uint32_t deblocking_filter_control_present_flag : 1;
            uint32_t redundant_pic_cnt_present_flag		: 1;
            uint32_t reference_pic_flag			: 1; /* nal_ref_idc != 0 */
        } bits;
        uint32_t value;
    } pic_fields;
    uint16_t frame_num;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_MEDIUM];
} VASTPictureParameterBufferH264;

/** H.264 Inverse Quantization Matrix Buffer */
typedef struct _VASTIQMatrixBufferH264
{
    /** \brief 4x4 scaling list, in raster scan order. */
    uint8_t ScalingList4x4[6][16];
    /** \brief 8x8 scaling list, in raster scan order. */
    uint8_t ScalingList8x8[2][64];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTIQMatrixBufferH264;

typedef struct _VASTSliceParameterBufferH264
{
    uint32_t slice_data_size;/* number of bytes in the slice data buffer for this slice */
    /** \brief Byte offset to the NAL Header Unit for this slice. */
    uint32_t slice_data_offset;
    uint32_t slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    /**
     * \brief Bit offset from NAL Header Unit to the begining of slice_data().
     *
     * This bit offset is relative to and includes the NAL unit byte
     * and represents the number of bits parsed in the slice_header()
     * after the removal of any emulation prevention bytes in
     * there. However, the slice data buffer passed to the hardware is
     * the original bitstream, thus including any emulation prevention
     * bytes.
     */
    uint16_t slice_data_bit_offset;
    uint16_t first_mb_in_slice;
    uint8_t slice_type;
    uint8_t direct_spatial_mv_pred_flag;
    /**
     * H264/AVC syntax element
     *
     * if num_ref_idx_active_override_flag equals 0, host decoder should
     * set its value to num_ref_idx_l0_default_active_minus1.
     */
    uint8_t num_ref_idx_l0_active_minus1;
    /**
     * H264/AVC syntax element
     *
     * if num_ref_idx_active_override_flag equals 0, host decoder should
     * set its value to num_ref_idx_l1_default_active_minus1.
     */
    uint8_t num_ref_idx_l1_active_minus1;
    uint8_t cabac_init_idc;
    int8_t slice_qp_delta;
    uint8_t disable_deblocking_filter_idc;
    int8_t slice_alpha_c0_offset_div2;
    int8_t slice_beta_offset_div2;
    VASTPictureH264 RefPicList0[32];	/* See 8.2.4.2 */
    VASTPictureH264 RefPicList1[32];	/* See 8.2.4.2 */
    uint8_t luma_log2_weight_denom;
    uint8_t chroma_log2_weight_denom;
    uint8_t luma_weight_l0_flag;
    int16_t luma_weight_l0[32];
    int16_t luma_offset_l0[32];
    uint8_t chroma_weight_l0_flag;
    int16_t chroma_weight_l0[32][2];
    int16_t chroma_offset_l0[32][2];
    uint8_t luma_weight_l1_flag;
    int16_t luma_weight_l1[32];
    int16_t luma_offset_l1[32];
    uint8_t chroma_weight_l1_flag;
    int16_t chroma_weight_l1[32][2];
    int16_t chroma_offset_l1[32][2];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTSliceParameterBufferH264;

typedef struct _VASTPictureHEVC
{
    /** \brief reconstructed picture buffer surface index 
     * invalid when taking value VAST_INVALID_SURFACE.
     */
    VASTSurfaceID             picture_id;
    /** \brief picture order count. 
     * in HEVC, POCs for top and bottom fields of same picture should
     * take different values.
     */
    int32_t                 pic_order_cnt;
    /* described below */
    uint32_t                flags;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTPictureHEVC;

typedef struct  _VASTPictureParameterBufferHEVC
{
    /** \brief buffer description of decoded current picture
     * only VAST_PICTURE_HEVC_FIELD_PIC and VAST_PICTURE_HEVC_BOTTOM_FIELD
     * of "flags" fields are meaningful.
     */
    VASTPictureHEVC           CurrPic;
    /** \brief buffer description of reference frames in DPB */
    VASTPictureHEVC           ReferenceFrames[15];
    /** \brief picture width, shall be integer multiple of minimum CB size. */
    uint16_t                pic_width_in_luma_samples;
    /** \brief picture height, shall be integer multiple of minimum CB size. */
    uint16_t                pic_height_in_luma_samples;


    union
    {
        struct
        {
        /** following flags have same syntax and semantic as those in HEVC spec */
            uint32_t        chroma_format_idc                           : 2;
            uint32_t        separate_colour_plane_flag                  : 1;
            uint32_t        pcm_enabled_flag                            : 1;
            uint32_t        scaling_list_enabled_flag                   : 1;
            uint32_t        transform_skip_enabled_flag                 : 1;
            uint32_t        amp_enabled_flag                            : 1;
            uint32_t        strong_intra_smoothing_enabled_flag         : 1;
            uint32_t        sign_data_hiding_enabled_flag               : 1;
            uint32_t        constrained_intra_pred_flag                 : 1;
            uint32_t        cu_qp_delta_enabled_flag                    : 1;
            uint32_t        weighted_pred_flag                          : 1;
            uint32_t        weighted_bipred_flag                        : 1;
            uint32_t        transquant_bypass_enabled_flag              : 1;
            uint32_t        tiles_enabled_flag                          : 1;
            uint32_t        entropy_coding_sync_enabled_flag            : 1;
            uint32_t        pps_loop_filter_across_slices_enabled_flag  : 1;
            uint32_t        loop_filter_across_tiles_enabled_flag       : 1;
            uint32_t        pcm_loop_filter_disabled_flag               : 1;
            /** set based on sps_max_num_reorder_pics of current temporal layer. */
            uint32_t        NoPicReorderingFlag                         : 1;
            /** picture has no B slices */
            uint32_t        NoBiPredFlag                                : 1;

            uint32_t        ReservedBits                                : 11;
        } bits;
        uint32_t            value;
    } pic_fields;

    /** following parameters have same syntax with those in HEVC spec */
    /** \brief DPB size for current temporal layer */
    uint8_t                 sps_max_dec_pic_buffering_minus1;
    uint8_t                 bit_depth_luma_minus8;
    uint8_t                 bit_depth_chroma_minus8;
    uint8_t                 pcm_sample_bit_depth_luma_minus1;
    uint8_t                 pcm_sample_bit_depth_chroma_minus1;
    uint8_t                 log2_min_luma_coding_block_size_minus3;
    uint8_t                 log2_diff_max_min_luma_coding_block_size;
    uint8_t                 log2_min_transform_block_size_minus2;
    uint8_t                 log2_diff_max_min_transform_block_size;
    uint8_t                 log2_min_pcm_luma_coding_block_size_minus3;
    uint8_t                 log2_diff_max_min_pcm_luma_coding_block_size;
    uint8_t                 max_transform_hierarchy_depth_intra;
    uint8_t                 max_transform_hierarchy_depth_inter;
    int8_t                  init_qp_minus26;
    uint8_t                 diff_cu_qp_delta_depth;
    int8_t                  pps_cb_qp_offset;
    int8_t                  pps_cr_qp_offset;
    uint8_t                 log2_parallel_merge_level_minus2;
    uint8_t                 num_tile_columns_minus1;
    uint8_t                 num_tile_rows_minus1;
    /**
     * when uniform_spacing_flag equals 1, application should populate
     * column_width_minus[], and row_height_minus1[] with approperiate values.
     */
    uint16_t                column_width_minus1[19];
    uint16_t                row_height_minus1[21];

    /**
     *  The Following Parameters are needed for Short Slice Format Only.
     *  Only format decoding can ignore them.
     */

    /**
     * \brief Parameters needed for parsing slice segment headers
     */
    union
    {
        struct
        {
            /** following parameters have same syntax with those in HEVC spec */
            uint32_t        lists_modification_present_flag             : 1;
            uint32_t        long_term_ref_pics_present_flag             : 1;
            uint32_t        sps_temporal_mvp_enabled_flag               : 1;
            uint32_t        cabac_init_present_flag                     : 1;
            uint32_t        output_flag_present_flag                    : 1;
            uint32_t        dependent_slice_segments_enabled_flag       : 1;
            uint32_t        pps_slice_chroma_qp_offsets_present_flag    : 1;
            uint32_t        sample_adaptive_offset_enabled_flag         : 1;
            uint32_t        deblocking_filter_override_enabled_flag     : 1;
            uint32_t        pps_disable_deblocking_filter_flag          : 1;
            uint32_t        slice_segment_header_extension_present_flag : 1;

            /** current picture with NUT between 16 and 21 inclusive */
            uint32_t        RapPicFlag                                  : 1;
            /** current picture with NUT between 19 and 20 inclusive */
            uint32_t        IdrPicFlag                                  : 1;
            /** current picture has only intra slices */
            uint32_t        IntraPicFlag                                : 1;

            uint32_t        ReservedBits                                : 18;
        } bits;
        uint32_t            value;
    } slice_parsing_fields;

    /** following parameters have same syntax with those in HEVC spec */
    uint8_t                 log2_max_pic_order_cnt_lsb_minus4;
    uint8_t                 num_short_term_ref_pic_sets;
    uint8_t                 num_long_term_ref_pic_sps;
    uint8_t                 num_ref_idx_l0_default_active_minus1;
    uint8_t                 num_ref_idx_l1_default_active_minus1;
    int8_t                  pps_beta_offset_div2;
    int8_t                  pps_tc_offset_div2;
    uint8_t                 num_extra_slice_header_bits;

    /**
     * \brief number of bits that structure
     * short_term_ref_pic_set( num_short_term_ref_pic_sets ) takes in slice
     * segment header when short_term_ref_pic_set_sps_flag equals 0.
     * if short_term_ref_pic_set_sps_flag equals 1, the value should be 0.
     * the bit count is calculated after emulation prevention bytes are removed
     * from bit streams.
     * This variable is used for accelorater to skip parsing the
     * short_term_ref_pic_set( num_short_term_ref_pic_sets ) structure.
     */
    uint32_t                st_rps_bits;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_MEDIUM];

    //debug for long term reference
    uint32_t               longTermRefPoc[16];
    uint32_t               longTermRefNum;
    uint8_t                LTMSBPresent[16];

    uint16_t lt_ref_pic_poc_lsb_sps[32];//HEVC_MAX_LONG_TERM_REF_PICS
    uint8_t used_by_curr_pic_lt_sps_flag[32];//HEVC_MAX_LONG_TERM_REF_PICS
} VASTPictureParameterBufferHEVC;

/**
 * \brief HEVC Slice Parameter Buffer Structure For Long Format
 *
 * VASTSliceParameterBufferHEVC structure should be accompanied by a
 * slice data buffer, which holds the whole raw slice NAL unit bit streams
 * including start code prefix and emulation prevention bytes not removed.
 *
 * This structure conveys parameters related to slice segment header and should
 * be sent once per slice.
 *
 * For short format, this data structure is not sent by application.
 *
 */
typedef struct  _VASTSliceParameterBufferHEVC
{
    /** @name Codec-independent Slice Parameter Buffer base. */

    /**@{*/

    /** \brief Number of bytes in the slice data buffer for this slice
     * counting from and including NAL unit header.
     */
    uint32_t                slice_data_size;
    /** \brief The offset to the NAL unit header for this slice */
    uint32_t                slice_data_offset;
    /** \brief Slice data buffer flags. See \c VA_SLICE_DATA_FLAG_XXX. */
    uint32_t                slice_data_flag;
    /**
     * \brief Byte offset from NAL unit header to the begining of slice_data().
     *
     * This byte offset is relative to and includes the NAL unit header
     * and represents the number of bytes parsed in the slice_header()
     * after the removal of any emulation prevention bytes in
     * there. However, the slice data buffer passed to the hardware is
     * the original bitstream, thus including any emulation prevention
     * bytes.
     */
    uint32_t                slice_data_byte_offset;
    uint32_t                slice_segment_address;
    /** \brief index into ReferenceFrames[]
     * RefPicList[0][] corresponds to RefPicList0[] of HEVC variable.
     * RefPicList[1][] corresponds to RefPicList1[] of HEVC variable.
     * value range [0..14, 0xFF], where 0xFF indicates invalid entry.
     */
    uint8_t                 RefPicList[2][15];
    union
    {
        uint32_t            value;
        struct
        {
            /** current slice is last slice of picture. */
            uint32_t        LastSliceOfPic                              : 1;
            uint32_t        dependent_slice_segment_flag                : 1;
            uint32_t        slice_type                                  : 2;
            uint32_t        color_plane_id                              : 2;
            uint32_t        slice_sao_luma_flag                         : 1;
            uint32_t        slice_sao_chroma_flag                       : 1;
            uint32_t        mvd_l1_zero_flag                            : 1;
            uint32_t        cabac_init_flag                             : 1;
            uint32_t        slice_temporal_mvp_enabled_flag             : 1;
            uint32_t        slice_deblocking_filter_disabled_flag       : 1;
            uint32_t        collocated_from_l0_flag                     : 1;
            uint32_t        slice_loop_filter_across_slices_enabled_flag : 1;
            uint32_t        reserved                                    : 18;
        } fields;
    } LongSliceFlags;

    /** HEVC syntax element. Collocated Reference Picture Index.
     * index to RefPicList[0][] or RefPicList[1][].
     * when slice_temporal_mvp_enabled_flag equals 0, it should take value 0xFF.
     * value range [0..14, 0xFF].
     */
    uint8_t                 collocated_ref_idx;
    /** HEVC syntax element.
     * if num_ref_idx_active_override_flag equals 0, host decoder should
     * set its value to num_ref_idx_l0_default_active_minus1.
     */
    uint8_t                 num_ref_idx_l0_active_minus1;
    /** HEVC syntax element.
     * if num_ref_idx_active_override_flag equals 0, host decoder should
     * set its value to num_ref_idx_l1_default_active_minus1.
     */
    uint8_t                 num_ref_idx_l1_active_minus1;
    int8_t                  slice_qp_delta;
    int8_t                  slice_cb_qp_offset;
    int8_t                  slice_cr_qp_offset;
    int8_t                  slice_beta_offset_div2;
    int8_t                  slice_tc_offset_div2;
    uint8_t                 luma_log2_weight_denom;
    int8_t                  delta_chroma_log2_weight_denom;
    int8_t                  delta_luma_weight_l0[15];
    int8_t                  luma_offset_l0[15];
    int8_t                  delta_chroma_weight_l0[15][2];
    int8_t                  ChromaOffsetL0[15][2];
    int8_t                  delta_luma_weight_l1[15];
    int8_t                  luma_offset_l1[15];
    int8_t                  delta_chroma_weight_l1[15][2];
    int8_t                  ChromaOffsetL1[15][2];
    uint8_t                 five_minus_max_num_merge_cand;
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTSliceParameterBufferHEVC;


typedef struct _VASTIQMatrixBufferHEVC
{
    /**
     * \brief scaling lists,
     * corresponds to same HEVC spec syntax element
     * ScalingList[ i ][ MatrixID ][ j ].
     *
     * \brief 4x4 scaling,
     * correspongs i = 0, MatrixID is in the range of 0 to 5,
     * inclusive. And j is in the range of 0 to 15, inclusive.
     */
    uint8_t                 ScalingList4x4[6][16];
    /**
     * \brief 8x8 scaling,
     * correspongs i = 1, MatrixID is in the range of 0 to 5,
     * inclusive. And j is in the range of 0 to 63, inclusive.
     */
    uint8_t                 ScalingList8x8[6][64];
    /**
     * \brief 16x16 scaling,
     * correspongs i = 2, MatrixID is in the range of 0 to 5,
     * inclusive. And j is in the range of 0 to 63, inclusive.
     */
    uint8_t                 ScalingList16x16[6][64];
    /**
     * \brief 32x32 scaling,
     * correspongs i = 3, MatrixID is in the range of 0 to 1,
     * inclusive. And j is in the range of 0 to 63, inclusive.
     */
    uint8_t                 ScalingList32x32[2][64];
    /**
     * \brief DC values of the 16x16 scaling lists,
     * corresponds to HEVC spec syntax
     * scaling_list_dc_coef_minus8[ sizeID - 2 ][ matrixID ] + 8
     * with sizeID = 2 and matrixID in the range of 0 to 5, inclusive.
     */
    uint8_t                 ScalingListDC16x16[6];
    /**
     * \brief DC values of the 32x32 scaling lists,
     * corresponds to HEVC spec syntax
     * scaling_list_dc_coef_minus8[ sizeID - 2 ][ matrixID ] + 8
     * with sizeID = 3 and matrixID in the range of 0 to 1, inclusive.
     */
    uint8_t                 ScalingListDC32x32[2];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTIQMatrixBufferHEVC;


typedef struct _VASegmentationStructAV1 {
    union {
        struct {
             /** Indicates whether segmentation map related syntax elements
             *  are present or not for current frame. If equal to 0,
             *  the segmentation map related syntax elements are
             *  not present for the current frame and the control flags of
             *  segmentation map related tables feature_data[][], and
             *  feature_mask[] are not valid and shall be ignored by accelerator.
             */
            uint32_t         enabled                                     : 1;
            /** Value 1 indicates that the segmentation map are updated
             *  during the decoding of this frame.
             *  Value 0 means that the segmentation map from the previous
             *  frame is used.
             */
            uint32_t         update_map                                  : 1;
            /** Value 1 indicates that the updates to the segmentation map
             *  are coded relative to the existing segmentation map.
             *  Value 0 indicates that the new segmentation map is coded
             *  without reference to the existing segmentation map.
             */
            uint32_t         temporal_update                             : 1;
            /** Value 1 indicates that new parameters are about to be
             *  specified for each segment.
             *  Value 0 indicates that the segmentation parameters
             *  should keep their existing values.
             */
            uint32_t         update_data                                 : 1;

            /** \brief Reserved bytes for future use, must be zero */
            uint32_t         reserved                                    : 28;
        } bits;
        uint32_t             value;
    } segment_info_fields;

    /** \brief Segmentation parameters for current frame.
     *  feature_data[segment_id][feature_id]
     *  where segment_id has value range [0..7] indicating the segment id.
     *  and feature_id is defined as
        typedef enum {
            SEG_LVL_ALT_Q,       // Use alternate Quantizer ....
            SEG_LVL_ALT_LF_Y_V,  // Use alternate loop filter value on y plane vertical
            SEG_LVL_ALT_LF_Y_H,  // Use alternate loop filter value on y plane horizontal
            SEG_LVL_ALT_LF_U,    // Use alternate loop filter value on u plane
            SEG_LVL_ALT_LF_V,    // Use alternate loop filter value on v plane
            SEG_LVL_REF_FRAME,   // Optional Segment reference frame
            SEG_LVL_SKIP,        // Optional Segment (0,0) + skip mode
            SEG_LVL_GLOBALMV,
            SEG_LVL_MAX
        } SEG_LVL_FEATURES;
     *  feature_data[][] is equivalent to variable FeatureData[][] in spec,
     *  which is after clip3() operation.
     *  Clip3(x, y, z) = (z < x)? x : ((z > y)? y : z);
     *  The limit is defined in Segmentation_Feature_Max[ SEG_LVL_MAX ] = {
     *  255, MAX_LOOP_FILTER, MAX_LOOP_FILTER, MAX_LOOP_FILTER, MAX_LOOP_FILTER, 7, 0, 0 }
     */
    int16_t                 feature_data[8][8];

    /** \brief indicates if a feature is enabled or not.
     *  Each bit field itself is the feature_id. Index is segment_id.
     *  feature_mask[segment_id] & (1 << feature_id) equal to 1 specify that the feature of
     *  feature_id for segment of segment_id is enabled, otherwise disabled.
     */
    uint8_t                 feature_mask[8];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];

} VASTSegmentationStructAV1;

/** \brief Film Grain Information
  */
typedef struct _VAFilmGrainStructAV1 {
    union {
        struct {
            /** \brief Specify whether or not film grain is applied on current frame.
             *  If set to 0, all the rest parameters should be set to zero
             *  and ignored.
             */
            uint32_t        apply_grain                                 : 1;
            uint32_t        chroma_scaling_from_luma                    : 1;
            uint32_t        grain_scaling_minus_8                       : 2;
            uint32_t        ar_coeff_lag                                : 2;
            uint32_t        ar_coeff_shift_minus_6                      : 2;
            uint32_t        grain_scale_shift                           : 2;
            uint32_t        overlap_flag                                : 1;
            uint32_t        clip_to_restricted_range                    : 1;
            /** \brief Reserved bytes for future use, must be zero */
            uint32_t        reserved                                    : 20;
        } bits;
        uint32_t            value;
    } film_grain_info_fields;

    uint16_t                grain_seed;
    /*  value range [0..14] */
    uint8_t                 num_y_points;
    uint8_t                 point_y_value[14];
    uint8_t                 point_y_scaling[14];
    /*  value range [0..10] */
    uint8_t                 num_cb_points;
    uint8_t                 point_cb_value[10];
    uint8_t                 point_cb_scaling[10];
    /*  value range [0..10] */
    uint8_t                 num_cr_points;
    uint8_t                 point_cr_value[10];
    uint8_t                 point_cr_scaling[10];
    /*  value range [-128..127] */
    int8_t                  ar_coeffs_y[24];
    int8_t                  ar_coeffs_cb[25];
    int8_t                  ar_coeffs_cr[25];
    uint8_t                 cb_mult;
    uint8_t                 cb_luma_mult;
    uint16_t                cb_offset;
    uint8_t                 cr_mult;
    uint8_t                 cr_luma_mult;
    uint16_t                cr_offset;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];

} VASTFilmGrainStructAV1;

typedef enum {
    /** identity transformation, 0-parameter */
    VAAV1TransformationIdentity           = 0,
    /** translational motion, 2-parameter */
    VAAV1TransformationTranslation        = 1,
    /** simplified affine with rotation + zoom only, 4-parameter */
    VAAV1TransformationRotzoom            = 2,
    /** affine, 6-parameter */
    VAAV1TransformationAffine             = 3,
    /** transformation count */
    VAAV1TransformationCount
} VASTAV1TransformationType;

typedef struct _VAWarpedMotionParamsAV1{

    /** \brief Specify the type of warped motion */
    VASTAV1TransformationType  wmtype;

    /** \brief Specify warp motion parameters
     *  wm.wmmat[] corresponds to gm_params[][] in spec.
     *  Details in AV1 spec section 5.9.24 or refer to libaom code
     *  https://aomedia.googlesource.com/aom/+/refs/heads/master/av1/decoder/decodeframe.c
     */
    int32_t                 wmmat[8];

    /* valid or invalid on affine set */
    uint8_t  invalid;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];

} VASTWarpedMotionParamsAV1;


typedef struct _VASliceParameterBufferAV1
{
    /**@{*/
    /** \brief The byte count of current tile in the bitstream buffer,
     *  starting from first byte of the buffer.
     *  It uses the name slice_data_size to be consistent with other codec,
     *  but actually means tile_data_size.
     */
    uint32_t                slice_data_size;
    /**
     * offset to the first byte of the data buffer.
     */
    uint32_t                slice_data_offset;
    /**
     * see VA_SLICE_DATA_FLAG_XXX definitions
     */
    uint32_t                slice_data_flag;

    uint16_t                tile_row;
    uint16_t                tile_column;

    uint16_t                tg_start;
    uint16_t                tg_end;
    /** \brief anchor frame index for large scale tile.
     *  index into an array AnchorFrames of the frames that the tile uses
     *  for prediction.
     *  valid only when large_scale_tile equals 1.
     */
    uint8_t                 anchor_frame_idx;

    /** \brief tile index in the tile list.
     *  Valid only when large_scale_tile is enabled.
     *  Driver uses this field to decide the tile output location.
     */
    uint16_t                tile_idx_in_tile_list;

    /**@}*/

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTSliceParameterBufferAV1;

typedef struct  _VADecPictureParameterBufferAV1
{
    /**@{*/

    /** \brief sequence level information
     */

    /** \brief AV1 bit stream profile
     */
    uint8_t                 profile;

    uint8_t                 order_hint_bits_minus_1;

    /** \brief bit depth index
     *  value range [0..2]
     *  0 - bit depth 8;
     *  1 - bit depth 10;
     *  2 - bit depth 12;
     */
    uint8_t                 bit_depth_idx;

    /** \brief corresponds to AV1 spec variable of the same name. */
    uint8_t                 matrix_coefficients;

    union {
        struct {
            uint32_t        still_picture                               : 1;
            uint32_t        use_128x128_superblock                      : 1;
            uint32_t        enable_filter_intra                         : 1;
            uint32_t        enable_intra_edge_filter                    : 1;

            /** read_compound_tools */
            uint32_t        enable_interintra_compound                  : 1;
            uint32_t        enable_masked_compound                      : 1;

            uint32_t        enable_dual_filter                          : 1;
            uint32_t        enable_order_hint                           : 1;
            uint32_t        enable_jnt_comp                             : 1;
            uint32_t        enable_cdef                                 : 1;
            uint32_t        mono_chrome                                 : 1;
            uint32_t        color_range                                 : 1;
            uint32_t        subsampling_x                               : 1;
            uint32_t        subsampling_y                               : 1;
            uint32_t        chroma_sample_position                      : 1;
            uint32_t        film_grain_params_present                   : 1;
            /** \brief Reserved bytes for future use, must be zero */
            uint32_t        reserved                                    : 16;
        } fields;
        uint32_t value;
    } seq_info_fields;

    /** \brief Picture level information
     */

    /** \brief buffer description of decoded current picture
     */
    VASTSurfaceID             current_frame;

    /** \brief display buffer of current picture
     *  Used for film grain applied decoded picture.
     *  Valid only when apply_grain equals 1.
     */
    VASTSurfaceID             current_display_picture;

    /** \brief number of anchor frames for large scale tile
     *  This parameter gives the number of entries of anchor_frames_list[].
     *  Value range [0..128].
     */
    uint8_t                anchor_frames_num;

    /** \brief anchor frame list for large scale tile
     *  For large scale tile applications, the anchor frames could come from
     *  previously decoded frames in current sequence (aka. internal), or
     *  from external sources.
     *  For external anchor frames, application should call API
     *  vaCreateBuffer() to generate frame buffers and populate them with
     *  pixel frames. And this process may happen multiple times.
     *  The array anchor_frames_list[] is used to register all the available
     *  anchor frames from both external and internal, up to the current
     *  frame instance. If a previously registerred anchor frame is no longer
     *  needed, it should be removed from the list. But it does not prevent
     *  applications from relacing the frame buffer with new anchor frames.
     *  Please note that the internal anchor frames may not still be present
     *  in the current DPB buffer. But if it is in the anchor_frames_list[],
     *  it should not be replaced with other frames or removed from memory
     *  until it is not shown in the list.
     *  This number of entries of the list is given by parameter anchor_frames_num.
     */
    VASTSurfaceID             *anchor_frames_list;

    /** \brief Picture resolution minus 1
     *  Picture original resolution. If SuperRes is enabled,
     *  this is the upscaled resolution.
     *  value range [0..65535]
     */
    uint16_t                frame_width_minus1;
    uint16_t                frame_height_minus1;

    /** \brief Output frame buffer size in unit of tiles
     *  Valid only when large_scale_tile equals 1.
     *  value range [0..65535]
     */
    uint16_t                output_frame_width_in_tiles_minus_1;
    uint16_t                output_frame_height_in_tiles_minus_1;

    /** \brief Surface indices of reference frames in DPB.
     *
     *  Contains a list of uncompressed frame buffer surface indices as references.
     *  Application needs to make sure all the entries point to valid frames
     *  except for intra frames by checking ref_frame_id[]. If missing frame
     *  is identified, application may choose to perform error recovery by
     *  pointing problematic index to an alternative frame buffer.
     *  Driver is not responsible to validate reference frames' id.
     */
    VASTSurfaceID             ref_frame_map[8];

    /** \brief Reference frame indices.
     *
     *  Contains a list of indices into ref_frame_map[8].
     *  It specifies the reference frame correspondence.
     *  The indices of the array are defined as [LAST_FRAME – LAST_FRAME,
     *  LAST2_FRAME – LAST_FRAME, …, ALTREF_FRAME – LAST_FRAME], where each
     *  symbol is defined as:
     *  enum{INTRA_FRAME = 0, LAST_FRAME, LAST2_FRAME, LAST3_FRAME, GOLDEN_FRAME,
     *  BWDREF_FRAME, ALTREF2_FRAME, ALTREF_FRAME};
     */
    uint8_t                 ref_frame_idx[7];

    /** \brief primary reference frame index
     *  Index into ref_frame_idx[], specifying which reference frame contains
     *  propagated info that should be loaded at the start of the frame.
     *  When value equals PRIMARY_REF_NONE (7), it indicates there is
     *  no primary reference frame.
     *  value range [0..7]
     */
    uint8_t                 primary_ref_frame;

    uint8_t                 order_hint;
    uint8_t                 refresh_frame_flags;

    VASTSegmentationStructAV1 seg_info;
    VASTFilmGrainStructAV1    film_grain_info;

    /** \brief tile structure
     *  When uniform_tile_spacing_flag == 1, width_in_sbs_minus_1[] and
     *  height_in_sbs_minus_1[] should be ignored, which will be generated
     *  by driver based on tile_cols and tile_rows.
     */
    uint8_t                 tile_cols;
    uint8_t                 tile_rows;

    /* The width/height of a tile minus 1 in units of superblocks. Though the
     * maximum number of tiles is 64, since ones of the last tile are computed
     * from ones of the other tiles and frame_width/height, they are not
     * necessarily specified.
     */
    uint16_t                width_in_sbs_minus_1[63];
    uint16_t                height_in_sbs_minus_1[63];

    /** \brief number of tiles minus 1 in large scale tile list
     *  Same as AV1 semantic element.
     *  Valid only when large_scale_tiles == 1.
     */
    uint16_t                tile_count_minus_1;

    /* specify the tile index for context updating */
    uint16_t                context_update_tile_id;

    union
    {
        struct
        {
            /** \brief flags for current picture
             *  same syntax and semantic as those in AV1 code
             */

             /** \brief Frame Type
              *  0:     KEY_FRAME;
              *  1:     INTER_FRAME;
              *  2:     INTRA_ONLY_FRAME;
              *  3:     SWITCH_FRAME
              *  For SWITCH_FRAME, application shall set error_resilient_mode = 1,
              *  refresh_frame_flags, etc. appropriately. And driver will convert it
              *  to INTER_FRAME.
              */
            uint32_t        frame_type                                  : 2;
            uint32_t        show_frame                                  : 1;
            uint32_t        showable_frame                              : 1;
            uint32_t        error_resilient_mode                        : 1;
            uint32_t        disable_cdf_update                          : 1;
            uint32_t        allow_screen_content_tools                  : 1;
            uint32_t        force_integer_mv                            : 1;
            uint32_t        allow_intrabc                               : 1;
            uint32_t        use_superres                                : 1;
            uint32_t        allow_high_precision_mv                     : 1;
            uint32_t        is_motion_mode_switchable                   : 1;
            uint32_t        use_ref_frame_mvs                           : 1;
            /* disable_frame_end_update_cdf is coded as refresh_frame_context. */
            uint32_t        disable_frame_end_update_cdf                : 1;
            uint32_t        uniform_tile_spacing_flag                   : 1;
            uint32_t        allow_warped_motion                         : 1;
            /** \brief indicate if current frame in large scale tile mode */
            uint32_t        large_scale_tile                            : 1;

            /** \brief Reserved bytes for future use, must be zero */
            uint32_t        reserved                                    : 15;
        } bits;
        uint32_t            value;
    } pic_info_fields;

    /** \brief Supper resolution scale denominator.
     *  When use_superres=1, superres_scale_denominator must be in the range [9..16].
     *  When use_superres=0, superres_scale_denominator must be 8.
     */
    uint8_t                 superres_scale_denominator;

    /** \brief Interpolation filter.
     *  value range [0..4]
     */
    uint8_t                 interp_filter;

    /** \brief luma loop filter levels.
     *  value range [0..63].
     */
    uint8_t                 filter_level[2];

    /** \brief chroma loop filter levels.
     *  value range [0..63].
     */
    uint8_t                 filter_level_u;
    uint8_t                 filter_level_v;

    union
    {
        struct
        {
            /** \brief flags for reference pictures
             *  same syntax and semantic as those in AV1 code
             */
            uint8_t         sharpness_level                             : 3;
            uint8_t         mode_ref_delta_enabled                      : 1;
            uint8_t         mode_ref_delta_update                       : 1;

            /** \brief Reserved bytes for future use, must be zero */
            uint8_t         reserved                                    : 3;
        } bits;
        uint8_t             value;
    } loop_filter_info_fields;

    /** \brief The adjustment needed for the filter level based on
     *  the chosen reference frame.
     *  value range [-64..63].
     */
    int8_t                  ref_deltas[8];

    /** \brief The adjustment needed for the filter level based on
     *  the chosen mode.
     *  value range [-64..63].
     */
    int8_t                  mode_deltas[2];

    /** \brief quantization
     */
    /** \brief Y AC index
     *  value range [0..255]
     */
    uint8_t                base_qindex;
    /** \brief Y DC delta from Y AC
     *  value range [-64..63]
     */
    int8_t                  y_dc_delta_q;
    /** \brief U DC delta from Y AC
     *  value range [-64..63]
     */
    int8_t                  u_dc_delta_q;
    /** \brief U AC delta from Y AC
     *  value range [-64..63]
     */
    int8_t                  u_ac_delta_q;
    /** \brief V DC delta from Y AC
     *  value range [-64..63]
     */
    int8_t                  v_dc_delta_q;
    /** \brief V AC delta from Y AC
     *  value range [-64..63]
     */
    int8_t                  v_ac_delta_q;

    /** \brief quantization_matrix
     */
    union
    {
        struct
        {
            uint16_t        using_qmatrix                               : 1;
            /** \brief qm level
             *  value range [0..15]
             *  Invalid if using_qmatrix equals 0.
             */
            uint16_t        qm_y                                        : 4;
            uint16_t        qm_u                                        : 4;
            uint16_t        qm_v                                        : 4;

            /** \brief Reserved bytes for future use, must be zero */
            uint16_t        reserved                                    : 3;
        } bits;
        uint16_t            value;
    } qmatrix_fields;

    union
    {
        struct
        {
            /** \brief delta_q parameters
             */
            uint32_t        delta_q_present_flag                        : 1;
            uint32_t        log2_delta_q_res                            : 2;

            /** \brief delta_lf parameters
             */
            uint32_t        delta_lf_present_flag                       : 1;
            uint32_t        log2_delta_lf_res                           : 2;

            /** \brief CONFIG_LOOPFILTER_LEVEL
             */
            uint32_t        delta_lf_multi                              : 1;

            /** \brief read_tx_mode
             *  value range [0..2]
             */
            uint32_t        tx_mode                                     : 2;

            /* AV1 frame reference mode semantic */
            uint32_t        reference_select                            : 1;

            uint32_t        reduced_tx_set_used                         : 1;

            uint32_t        skip_mode_present                           : 1;

            /** \brief Reserved bytes for future use, must be zero */
            uint32_t        reserved                                    : 20;
        } bits;
        uint32_t            value;
    } mode_control_fields;

    /** \brief CDEF parameters
     */
    /*  value range [0..3]  */
    uint8_t                 cdef_damping_minus_3;
    /*  value range [0..3]  */
    uint8_t                 cdef_bits;

    /** Encode cdef strength:
     *
     * The cdef_y_strengths[] and cdef_uv_strengths[] are expected to be packed
     * with both primary and secondary strength. The secondary strength is
     * given in the lower two bits and the primary strength is given in the next
     * four bits.
     *
     * cdef_y_strengths[] & cdef_uv_strengths[] should be derived as:
     * (cdef_y_strengths[]) = (cdef_y_pri_strength[] << 2) | (cdef_y_sec_strength[] & 0x03)
     * (cdef_uv_strengths[]) = (cdef_uv_pri_strength[] << 2) | (cdef_uv_sec_strength[] & 0x03)
     * In which, cdef_y_pri_strength[]/cdef_y_sec_strength[]/cdef_uv_pri_strength[]/cdef_uv_sec_strength[]
     * are variables defined in AV1 Spec 5.9.19. The cdef_y_strengths[] & cdef_uv_strengths[]
     * are corresponding to LIBAOM variables cm->cdef_strengths[] & cm->cdef_uv_strengths[] respectively.
     */
    /*  value range [0..63]  */
    uint8_t                 cdef_y_strengths[8];
    /*  value range [0..63]  */
    uint8_t                 cdef_uv_strengths[8];

    /** \brief loop restoration parameters
     */
    union
    {
        struct
        {
            uint16_t        yframe_restoration_type                     : 2;
            uint16_t        cbframe_restoration_type                    : 2;
            uint16_t        crframe_restoration_type                    : 2;
            uint16_t        lr_unit_shift                               : 2;
            uint16_t        lr_uv_shift                                 : 1;

            /** \brief Reserved bytes for future use, must be zero */
            uint16_t        reserved                                    : 7;
        } bits;
        uint16_t            value;
    } loop_restoration_fields;

    /** \brief global motion
     */
    VASTWarpedMotionParamsAV1 wm[7];

    /**@}*/

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_MEDIUM];
} VASTDecPictureParameterBufferAV1;

typedef struct _VASTHuffmanTableBufferJPEGBaseline {
    /** \brief Specifies which #huffman_table is valid. */
    uint8_t       load_huffman_table[2];
    /** \brief Huffman tables indexed by table identifier (Th). */
    struct {
        /** @name DC table (up to 12 categories) */
        /**@{*/
        /** \brief Number of Huffman codes of length i + 1 (Li). */
        uint8_t   num_dc_codes[16];
        /** \brief Value associated with each Huffman code (Vij). */
        uint8_t   dc_values[12];
        /**@}*/
        /** @name AC table (2 special codes + up to 16 * 10 codes) */
        /**@{*/
        /** \brief Number of Huffman codes of length i + 1 (Li). */
        uint8_t   num_ac_codes[16];
        /** \brief Value associated with each Huffman code (Vij). */
        uint8_t   ac_values[162];
        /** \brief Padding to 4-byte boundaries. Must be set to zero. */
        uint8_t   pad[2];
        /**@}*/
    }                   huffman_table[2];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTHuffmanTableBufferJPEGBaseline;


typedef struct _VASTPictureParameterBufferJPEGBaseline {
    /** \brief Picture width in pixels. */
    uint16_t      picture_width;
    /** \brief Picture height in pixels. */
    uint16_t      picture_height;

    struct {
        /** \brief Component identifier (Ci). */
        uint8_t   component_id;
        /** \brief Horizontal sampling factor (Hi). */
        uint8_t   h_sampling_factor;
        /** \brief Vertical sampling factor (Vi). */
        uint8_t   v_sampling_factor;
        /* \brief Quantization table selector (Tqi). */
        uint8_t   quantiser_table_selector;
    }                   components[255];
    /** \brief Number of components in frame (Nf). */
    uint8_t       num_components;

    /** \brief Input color space 0: YUV, 1: RGB, 2: BGR, others: reserved */
    uint8_t       color_space;
    /** \brief Set to VA_ROTATION_* for a single rotation angle reported by VASTConfigAttribDecJPEG. */
    uint32_t      rotation;
    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_MEDIUM - 1];
} VASTPictureParameterBufferJPEGBaseline;

/**
 * \brief Quantization table for JPEG decoding.
 *
 * This structure holds the complete quantization tables. This is an
 * aggregation of all quantization table (DQT) segments maintained by
 * the application. i.e. up to 4 quantization tables are stored in
 * there for baseline profile.
 *
 * The #load_quantization_table array can be used as a hint to notify
 * the VA driver implementation about which table(s) actually changed
 * since the last submission of this buffer.
 *
 * The #quantiser_table values are specified in zig-zag scan order.
 */
typedef struct _VASTIQMatrixBufferJPEGBaseline {
    /** \brief Specifies which #quantiser_table is valid. */
    uint8_t       load_quantiser_table[4];
    /** \brief Quanziation tables indexed by table identifier (Tqi). */
    uint8_t       quantiser_table[4][64];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTIQMatrixBufferJPEGBaseline;

/**
 * \brief Slice parameter for JPEG decoding.
 *
 * This structure holds information from the scan header, along with
 * definitions from additional segments. The associated slice data
 * buffer holds all entropy coded segments (ECS) in the scan.
 */
typedef struct _VASTSliceParameterBufferJPEGBaseline {
    /** @name Codec-independent Slice Parameter Buffer base. */
    /**@{*/
    /** \brief Number of bytes in the slice data buffer for this slice. */
    uint32_t        slice_data_size;
    /** \brief The offset to the first byte of the first MCU. */
    uint32_t        slice_data_offset;
    /** \brief Slice data buffer flags. See \c VA_SLICE_DATA_FLAG_xxx. */
    uint32_t        slice_data_flag;
    /**@}*/

    /** \brief Scan horizontal position. */
    uint32_t        slice_horizontal_position;
    /** \brief Scan vertical position. */
    uint32_t        slice_vertical_position;

    struct {
        /** \brief Scan component selector (Csj). */
        uint8_t   component_selector;
        /** \brief DC entropy coding table selector (Tdj). */
        uint8_t   dc_table_selector;
        /** \brief AC entropy coding table selector (Taj). */
        uint8_t   ac_table_selector;
    }                   components[4];
    /** \brief Number of components in scan (Ns). */
    uint8_t       num_components;

    /** \brief Restart interval definition (Ri). */
    uint16_t      restart_interval;
    /** \brief Number of MCUs in a scan. */
    uint32_t        num_mcus;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTSliceParameterBufferJPEGBaseline;

#define HANTRO_BUFFER_TYPE_START 100

typedef enum {

    HANTRODecEmbeddedPostprocessParameterBufferType = HANTRO_BUFFER_TYPE_START,
    HANTRODecMiscParameterBufferType,
    HANTROEncROIMapBufferType,
    HANTROEncCuCtrlBufferType,
    HANTROEncCuCtrlIndexBufferType,
    HANTROEncMBCUOutputInfoBufferType,
    HANTROEncPSNROutputInfoBufferType,
    HANTROEncUserDataBufferType,
    HANTROEncAdaptiveGOPDecisionBufferType

} HANTROBufferType;

/** \brief Packed header type. */
typedef enum {
    /** \brief Packed sequence header. */
    VASTEncPackedHeaderSequence   = 1,
    /** \brief Packed picture header. */
    VASTEncPackedHeaderPicture    = 2,
    /** \brief Packed slice header. */
    VASTEncPackedHeaderSlice      = 3,
    /** 
     * \brief Packed raw header. 
     * 
     * Packed raw data header can be used by the client to insert a header  
     * into the bitstream data buffer at the point it is passed, the driver 
     * will handle the raw packed header based on "has_emulation_bytes" field
     * in the packed header parameter structure.
     */
    VASTEncPackedHeaderRawData    = 4,
    /**
     * \brief Misc packed header. See codec-specific definitions.
     *
     * @deprecated
     * This is a deprecated packed header type. All applications can use
     * \c VASTEncPackedHeaderRawData to insert a codec-specific packed header
     */
    VASTEncPackedHeaderMiscMask  = 0x80000000,
} VASTEncPackedHeaderType;

typedef struct _VAPictureAV1
{
    /** \brief reconstructed picture buffer surface index
     * invalid when taking value VA_INVALID_SURFACE.
     */
    VASTSurfaceID             picture_id;
    /** \brief picture order count.
     * in HEVC, POCs for top and bottom fields of same picture should
     * take different values.
     */
    int32_t                 pic_order_cnt;
    /* described below */
    uint32_t                flags;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VAPictureAV1;

#endif // VA_PUBLIC_H
