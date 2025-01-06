/*
 * Copyright (c) 2007-2009 Intel Corporation. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL INTEL AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * Video Acceleration (VA) API Specification
 *
 * Rev. 0.30
 * <jonathan.bian@intel.com>
 *
 * Revision History:
 * rev 0.10 (12/10/2006 Jonathan Bian) - Initial draft
 * rev 0.11 (12/15/2006 Jonathan Bian) - Fixed some errors
 * rev 0.12 (02/05/2007 Jonathan Bian) - Added VC-1 data structures for slice level decode
 * rev 0.13 (02/28/2007 Jonathan Bian) - Added GetDisplay()
 * rev 0.14 (04/13/2007 Jonathan Bian) - Fixed MPEG-2 PictureParameter structure, cleaned up a few funcs.
 * rev 0.15 (04/20/2007 Jonathan Bian) - Overhauled buffer management
 * rev 0.16 (05/02/2007 Jonathan Bian) - Added error codes and fixed some issues with configuration
 * rev 0.17 (05/07/2007 Jonathan Bian) - Added H.264/AVC data structures for slice level decode.
 * rev 0.18 (05/14/2007 Jonathan Bian) - Added data structures for MPEG-4 slice level decode 
 *                                       and MPEG-2 motion compensation.
 * rev 0.19 (08/06/2007 Jonathan Bian) - Removed extra type for bitplane data.
 * rev 0.20 (08/08/2007 Jonathan Bian) - Added missing fields to VC-1 PictureParameter structure.
 * rev 0.21 (08/20/2007 Jonathan Bian) - Added image and subpicture support.
 * rev 0.22 (08/27/2007 Jonathan Bian) - Added support for chroma-keying and global alpha.
 * rev 0.23 (09/11/2007 Jonathan Bian) - Fixed some issues with images and subpictures.
 * rev 0.24 (09/18/2007 Jonathan Bian) - Added display attributes.
 * rev 0.25 (10/18/2007 Jonathan Bian) - Changed to use IDs only for some types.
 * rev 0.26 (11/07/2007 Waldo Bastian) - Change vastCreateBuffer semantics
 * rev 0.27 (11/19/2007 Matt Sottek)   - Added DeriveImage
 * rev 0.28 (12/06/2007 Jonathan Bian) - Added new versions of PutImage and AssociateSubpicture 
 *                                       to enable scaling
 * rev 0.29 (02/07/2008 Jonathan Bian) - VC1 parameter fixes,
 *                                       added VAST_STATUS_ERROR_RESOLUTION_NOT_SUPPORTED
 * rev 0.30 (03/01/2009 Jonathan Bian) - Added encoding support for H.264 BP and MPEG-4 SP and fixes
 *                                       for ISO C conformance.
 * rev 0.31 (09/02/2009 Gwenole Beauchesne) - VC-1/H264 fields change for VDPAU and XvBA backend
 *                                       Application needs to relink with the new library.
 *
 * rev 0.31.1 (03/29/2009)              - Data structure for JPEG encode
 * rev 0.31.2 (01/13/2011 Anthony Pabon)- Added a flag to indicate Subpicture coordinates are screen
 *                                        screen relative rather than source video relative.
 * rev 0.32.0 (01/13/2011 Xiang Haihao) - Add profile into VASTPictureParameterBufferVC1
 *                                        update VAAPI to 0.32.0
 *
 * Acknowledgements:
 *  Some concepts borrowed from XvMC and XvImage.
 *  Waldo Bastian (Intel), Matt Sottek (Intel),  Austin Yuan (Intel), and Gwenole Beauchesne (SDS)
 *  contributed to various aspects of the API.
 */

/**
 * \file va.h
 * \brief The Core API
 *
 * This file contains the \ref api_core "Core API".
 */

#ifndef _VASTVA_H_
#define _VASTVA_H_

#include <stddef.h>
#include <stdint.h>
#include <vastva/va_version.h>
#include <vastva/va_public.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __GNUC__
#define va_deprecated __attribute__((deprecated))
#if __GNUC__ >= 6
#define va_deprecated_enum va_deprecated
#else
#define va_deprecated_enum
#endif
#else
#define va_deprecated
#define va_deprecated_enum
#endif

/**
 * \mainpage Video Acceleration (VA) API
 *
 * \section intro Introduction
 *
 * The main motivation for VA-API (Video Acceleration API) is to
 * enable hardware accelerated video decode and encode at various
 * entry-points (VLD, IDCT, Motion Compensation etc.) for the
 * prevailing coding standards today (MPEG-2, MPEG-4 ASP/H.263, MPEG-4
 * AVC/H.264, VC-1/VMW3, and JPEG, HEVC/H265, VP8, VP9) and video pre/post
 * processing
 *
 * VA-API is split into several modules:
 * - \ref api_core
 * - Encoder (H264, HEVC, JPEG, MPEG2, VP8, VP9)
 * 	- \ref api_enc_h264
 * 	- \ref api_enc_hevc
 * 	- \ref api_enc_jpeg
 * 	- \ref api_enc_mpeg2
 * 	- \ref api_enc_vp8
 * 	- \ref api_enc_vp9
 * - Decoder (HEVC, JPEG, VP8, VP9)
 *      - \ref api_dec_hevc
 *      - \ref api_dec_jpeg
 *      - \ref api_dec_vp8
 *      - \ref api_dec_vp9
 * - \ref api_vpp
 * - FEI (H264, HEVC)
 * 	- \ref api_fei
 * 	- \ref api_fei_h264
 * 	- \ref api_fei_hevc
 */

/**
 * \defgroup api_core Core API
 *
 * @{
 */

/**
Overview 

The VA API is intended to provide an interface between a video decode/encode/processing
application (client) and a hardware accelerator (server), to off-load 
video decode/encode/processing operations from the host to the hardware accelerator at various
entry-points.

The basic operation steps are:

- Negotiate a mutually acceptable configuration with the server to lock
  down profile, entrypoints, and other attributes that will not change on 
  a frame-by-frame basis.
- Create a video decode, encode or processing context which represents a
  "virtualized" hardware device
- Get and fill the render buffers with the corresponding data (depending on
  profiles and entrypoints)
- Pass the render buffers to the server to handle the current frame

Initialization & Configuration Management 

- Find out supported profiles
- Find out entrypoints for a given profile
- Find out configuration attributes for a given profile/entrypoint pair
- Create a configuration for use by the application

*/


/** Values for the return status */
#define VAST_STATUS_ERROR_OPERATION_FAILED	0x00000001
#define VAST_STATUS_ERROR_ALLOCATION_FAILED	0x00000002
#define VAST_STATUS_ERROR_INVALID_DISPLAY		0x00000003
#define VAST_STATUS_ERROR_INVALID_CONFIG		0x00000004
#define VAST_STATUS_ERROR_INVALID_CONTEXT		0x00000005
#define VAST_STATUS_ERROR_INVALID_SURFACE		0x00000006
#define VAST_STATUS_ERROR_INVALID_BUFFER		0x00000007
#define VAST_STATUS_ERROR_INVALID_IMAGE		0x00000008
#define VAST_STATUS_ERROR_INVALID_SUBPICTURE	0x00000009
#define VAST_STATUS_ERROR_ATTR_NOT_SUPPORTED	0x0000000a
#define VAST_STATUS_ERROR_MAX_NUM_EXCEEDED	0x0000000b
#define VAST_STATUS_ERROR_UNSUPPORTED_PROFILE	0x0000000c
#define VAST_STATUS_ERROR_UNSUPPORTED_ENTRYPOINT	0x0000000d
#define VAST_STATUS_ERROR_UNSUPPORTED_RT_FORMAT	0x0000000e
#define VAST_STATUS_ERROR_UNSUPPORTED_BUFFERTYPE	0x0000000f
#define VAST_STATUS_ERROR_SURFACE_BUSY		0x00000010
#define VAST_STATUS_ERROR_FLAG_NOT_SUPPORTED      0x00000011
#define VAST_STATUS_ERROR_INVALID_PARAMETER	0x00000012
#define VAST_STATUS_ERROR_RESOLUTION_NOT_SUPPORTED 0x00000013
#define VAST_STATUS_ERROR_UNIMPLEMENTED           0x00000014
#define VAST_STATUS_ERROR_SURFACE_IN_DISPLAYING   0x00000015
#define VAST_STATUS_ERROR_INVALID_IMAGE_FORMAT    0x00000016
#define VAST_STATUS_ERROR_DECODING_ERROR          0x00000017
#define VAST_STATUS_ERROR_ENCODING_ERROR          0x00000018
#define VAST_STATUS_ERROR_PROCESSING_ERROR          0x00000019
/**
 * \brief An invalid/unsupported value was supplied.
 *
 * This is a catch-all error code for invalid or unsupported values.
 * e.g. value exceeding the valid range, invalid type in the context
 * of generic attribute values.
 */
#define VAST_STATUS_ERROR_INVALID_VALUE           0x00000019
/** \brief An unsupported filter was supplied. */
#define VAST_STATUS_ERROR_UNSUPPORTED_FILTER      0x00000020
/** \brief An invalid filter chain was supplied. */
#define VAST_STATUS_ERROR_INVALID_FILTER_CHAIN    0x00000021
/** \brief Indicate HW busy (e.g. run multiple encoding simultaneously). */
#define VAST_STATUS_ERROR_HW_BUSY	                0x00000022
/** \brief An unsupported memory type was supplied. */
#define VAST_STATUS_ERROR_UNSUPPORTED_MEMORY_TYPE 0x00000024
#define VAST_STATUS_ERROR_INSTANCE_MISMATCH  0x00000025
#define VAST_STATUS_ERROR_UNKNOWN			0xFFFFFFFF

/** De-interlacing flags for vastPutSurface() */
#define VAST_FRAME_PICTURE        0x00000000 
#define VAST_TOP_FIELD            0x00000001
#define VAST_BOTTOM_FIELD         0x00000002

/**
 * Enabled the positioning/cropping/blending feature:
 * 1, specify the video playback position in the isurface
 * 2, specify the cropping info for video playback
 * 3, encoded video will blend with background color
 */
#define VAST_ENABLE_BLEND         0x00000004 /* video area blend with the constant color */ 
    
/**
 * Clears the drawable with background color.
 * for hardware overlay based implementation this flag
 * can be used to turn off the overlay
 */
#define VAST_CLEAR_DRAWABLE       0x00000008

/** Color space conversion flags for vastPutSurface() */
#define VAST_SRC_COLOR_MASK       0x000000f0
#define VAST_SRC_BT601            0x00000010
#define VAST_SRC_BT709            0x00000020
#define VAST_SRC_SMPTE_240        0x00000040

/** Scaling flags for vastPutSurface() */
#define VAST_FILTER_SCALING_DEFAULT       0x00000000
#define VAST_FILTER_SCALING_FAST          0x00000100
#define VAST_FILTER_SCALING_HQ            0x00000200
#define VAST_FILTER_SCALING_NL_ANAMORPHIC 0x00000300
#define VAST_FILTER_SCALING_MASK          0x00000f00




/** \brief Generic motion vector data structure. */
typedef struct _VASTMotionVector {
    /** \mv0[0]: horizontal motion vector for past reference */
    /** \mv0[1]: vertical motion vector for past reference */
    /** \mv1[0]: horizontal motion vector for future reference */
    /** \mv1[1]: vertical motion vector for future reference */
    int16_t  mv0[2];  /* past reference */
    int16_t  mv1[2];  /* future reference */
} VASTMotionVector;


/** Extension of VASTBufferType */
typedef enum {

    VASTHANTRODecEmbeddedPostprocessParameterBufferType = 100,
    VASTHANTRODecMiscParameterBufferType,
    VASTHANTROEncROIMapBufferType,
    VASTHANTROEncCuCtrlBufferType,
    VASTHANTROEncCuCtrlIndexBufferType,
    VASTHANTROEncMBCUOutputInfoBufferType,
    VASTHANTROEncPSNROutputInfoBufferType,
    VASTHANTROEncUserDataBufferType,
    VASTHANTROEncAdaptiveGOPDecisionBufferType

} VASTHANTROBufferType;



/** attribute value for VASTConfigAttribRTFormat */
#define VAST_RT_FORMAT_YUV420	0x00000001	
#define VAST_RT_FORMAT_YUV422	0x00000002
#define VAST_RT_FORMAT_YUV444	0x00000004
#define VAST_RT_FORMAT_YUV411	0x00000008
#define VAST_RT_FORMAT_YUV400	0x00000010
#define VAST_RT_FORMAT_YUVJ420	0x00000011
#define VAST_RT_FORMAT_YUVJ422	0x00000012
/** YUV formats with more than 8 bpp */
#define VAST_RT_FORMAT_YUV420_10BPP	0x00000100
/** RGB formats */
#define VAST_RT_FORMAT_RGB16	0x00010000
#define VAST_RT_FORMAT_RGB32	0x00020000
/* RGBP covers RGBP and BGRP fourcc */ 
#define VAST_RT_FORMAT_RGBP	0x00100000
/**
 * RGB 10-bit packed format with upper 2 bits as alpha channel.
 * The existing pre-defined fourcc codes can be used to signal
 * the position of each component for this RT format.
 */
#define VAST_RT_FORMAT_RGB32_10BPP 0x00200000
#define VAST_RT_FORMAT_BGGR8 0x00300000
#define VAST_RT_FORMAT_RGGB8 0x00300001
#define VAST_RT_FORMAT_GBRG8 0x00300002
#define VAST_RT_FORMAT_GRBG8 0x00300003

#define VAST_RT_FORMAT_PROTECTED	0x80000000

/** @name Attribute values for VASTConfigAttribRateControl */
/**@{*/
/** \brief Driver does not support any form of rate control. */
#define VAST_RC_NONE                      0x00000001
/** \brief Constant bitrate. */
#define VAST_RC_CBR                       0x00000002
/** \brief Variable bitrate. */
#define VAST_RC_VBR                       0x00000004
/** \brief Video conference mode. */
#define VAST_RC_VCM                       0x00000008
/** \brief Constant QP. */
#define VAST_RC_CQP                       0x00000010
/** \brief Variable bitrate with peak rate higher than average bitrate. */
#define VAST_RC_VBR_CONSTRAINED           0x00000020
/** \brief Intelligent Constant Quality. Provided an initial ICQ_quality_factor,
 *  adjusts QP at a frame and MB level based on motion to improve subjective quality. */
#define VAST_RC_ICQ			0x00000040
/** \brief Macroblock based rate control.  Per MB control is decided
 *  internally in the encoder. It may be combined with other RC modes, except CQP. */
#define VAST_RC_MB                        0x00000080
/** \brief Constant Frame Size, it is used for small tolerent  */
#define VAST_RC_CFS                       0x00000100
/** \brief Parallel BRC, for hierachical B.
 *
 *  For hierachical B, B frames can be refered by other B frames.
 *  Currently three layers of hierachy are defined:
 *  B0 - regular B, no reference to other B frames.
 *  B1 - reference to only I, P and regular B0 frames.
 *  B2 - reference to any other frames, including B1.
 *  In Hierachical B structure, B frames on the same layer can be processed
 *  simultaneously. And BRC would adjust accordingly. This is so called
 *  Parallel BRC. */
#define VAST_RC_PARALLEL                  0x00000200

/**@}*/

/** @name Attribute values for VASTConfigAttribDecSliceMode */
/**@{*/
/** \brief Driver supports normal mode for slice decoding */
#define VAST_DEC_SLICE_MODE_NORMAL       0x00000001
/** \brief Driver supports base mode for slice decoding */
#define VAST_DEC_SLICE_MODE_BASE         0x00000002

/** @name Attribute values for VASTConfigAttribDecJPEG */
/**@{*/
typedef union _VASTConfigAttribValDecJPEG {
    struct{
    /** \brief Set to (1 << VA_ROTATION_xxx) for supported rotation angles. */
    uint32_t rotation : 4;
    /** \brief Reserved for future use. */
    uint32_t reserved : 28;
    }bits;
    uint32_t value;
    uint32_t va_reserved[VAST_PADDING_LOW];
} VASTConfigAttribValDecJPEG;
/** @name Attribute values for VASTConfigAttribDecProcessing */
/**@{*/
/** \brief No decoding + processing in a single decoding call. */
#define VAST_DEC_PROCESSING_NONE     0x00000000
/** \brief Decode + processing in a single decoding call. */
#define VAST_DEC_PROCESSING          0x00000001
/**@}*/

/** @name Attribute values for VASTConfigAttribEncPackedHeaders */
/**@{*/
/** \brief Driver does not support any packed headers mode. */
#define VAST_ENC_PACKED_HEADER_NONE       0x00000000
/**
 * \brief Driver supports packed sequence headers. e.g. SPS for H.264.
 *
 * Application must provide it to driver once this flag is returned through
 * vastGetConfigAttributes()
 */
#define VAST_ENC_PACKED_HEADER_SEQUENCE   0x00000001
/**
 * \brief Driver supports packed picture headers. e.g. PPS for H.264.
 *
 * Application must provide it to driver once this falg is returned through
 * vastGetConfigAttributes()
 */
#define VAST_ENC_PACKED_HEADER_PICTURE    0x00000002
/**
 * \brief Driver supports packed slice headers. e.g. slice_header() for H.264.
 *
 * Application must provide it to driver once this flag is returned through
 * vastGetConfigAttributes()
 */
#define VAST_ENC_PACKED_HEADER_SLICE      0x00000004
/**
 * \brief Driver supports misc packed headers. e.g. SEI for H.264.
 *
 * @deprecated
 * This is a deprecated packed header flag, All applications can use
 * \c VAST_ENC_PACKED_HEADER_RAW_DATA to pass the corresponding packed
 * header data buffer to the driver
 */
#define VAST_ENC_PACKED_HEADER_MISC       0x00000008
/** \brief Driver supports raw packed header, see VASTEncPackedHeaderRawData */
#define VAST_ENC_PACKED_HEADER_RAW_DATA   0x00000010
/**@}*/

/** @name Attribute values for VASTConfigAttribEncInterlaced */
/**@{*/
/** \brief Driver does not support interlaced coding. */
#define VAST_ENC_INTERLACED_NONE          0x00000000
/** \brief Driver supports interlaced frame coding. */
#define VAST_ENC_INTERLACED_FRAME         0x00000001
/** \brief Driver supports interlaced field coding. */
#define VAST_ENC_INTERLACED_FIELD         0x00000002
/** \brief Driver supports macroblock adaptive frame field coding. */
#define VAST_ENC_INTERLACED_MBAFF         0x00000004
/** \brief Driver supports picture adaptive frame field coding. */
#define VAST_ENC_INTERLACED_PAFF          0x00000008
/**@}*/

/** @name Attribute values for VASTConfigAttribEncSliceStructure */
/**@{*/
/** \brief Driver supports a power-of-two number of rows per slice. */
#define VAST_ENC_SLICE_STRUCTURE_POWER_OF_TWO_ROWS        0x00000001
/** \brief Driver supports an arbitrary number of macroblocks per slice. */
#define VAST_ENC_SLICE_STRUCTURE_ARBITRARY_MACROBLOCKS    0x00000002
/** \brief Dirver support 1 rows  per slice */
#define VAST_ENC_SLICE_STRUCTURE_EQUAL_ROWS               0x00000004
/** \brief Dirver support max encoded slice size per slice */
#define VAST_ENC_SLICE_STRUCTURE_MAX_SLICE_SIZE           0x00000008
/** \brief Driver supports an arbitrary number of rows per slice. */
#define VAST_ENC_SLICE_STRUCTURE_ARBITRARY_ROWS           0x00000010
/**@}*/

/** \brief Attribute value for VASTConfigAttribEncJPEG */
typedef union _VASTConfigAttribValEncJPEG {
    struct {
        /** \brief set to 1 for arithmatic coding. */
        uint32_t arithmatic_coding_mode : 1;
        /** \brief set to 1 for progressive dct. */
        uint32_t progressive_dct_mode : 1;
        /** \brief set to 1 for non-interleaved. */
        uint32_t non_interleaved_mode : 1;
        /** \brief set to 1 for differential. */
        uint32_t differential_mode : 1;
        uint32_t max_num_components : 3;
        uint32_t max_num_scans : 4;
        uint32_t max_num_huffman_tables : 3;
        uint32_t max_num_quantization_tables : 3;
    } bits;
    uint32_t value;
} VASTConfigAttribValEncJPEG;

/** @name Attribute values for VASTConfigAttribEncQuantization */
/**@{*/
/** \brief Driver does not support special types of quantization */
#define VAST_ENC_QUANTIZATION_NONE                        0x00000000
/** \brief Driver supports trellis quantization */
#define VAST_ENC_QUANTIZATION_TRELLIS_SUPPORTED           0x00000001
/**@}*/

/** @name Attribute values for VASTConfigAttribEncIntraRefresh */
/**@{*/
/** \brief Driver does not support intra refresh */
#define VAST_ENC_INTRA_REFRESH_NONE                       0x00000000
/** \brief Driver supports column based rolling intra refresh */
#define VAST_ENC_INTRA_REFRESH_ROLLING_COLUMN             0x00000001
/** \brief Driver supports row based rolling intra refresh */
#define VAST_ENC_INTRA_REFRESH_ROLLING_ROW                0x00000002
/** \brief Driver supports adaptive intra refresh */
#define VAST_ENC_INTRA_REFRESH_ADAPTIVE                   0x00000010
/** \brief Driver supports cyclic intra refresh */
#define VAST_ENC_INTRA_REFRESH_CYCLIC                     0x00000020
/** \brief Driver supports intra refresh of P frame*/
#define VAST_ENC_INTRA_REFRESH_P_FRAME                    0x00010000
/** \brief Driver supports intra refresh of B frame */
#define VAST_ENC_INTRA_REFRESH_B_FRAME                    0x00020000
/** \brief Driver supports intra refresh of multiple reference encoder */
#define VAST_ENC_INTRA_REFRESH_MULTI_REF                  0x00040000

/**@}*/

/** \brief Attribute value for VASTConfigAttribEncROI */
typedef union _VASTConfigAttribValEncROI {
    struct {
        /** \brief The number of ROI regions supported, 0 if ROI is not supported. */
        uint32_t num_roi_regions 		: 8;
        /**
         * \brief A flag indicates whether ROI priority is supported
         *
         * \ref roi_rc_priority_support equal to 1 specifies the underlying driver supports
         * ROI priority when VASTConfigAttribRateControl != VAST_RC_CQP , user can use \c roi_value
         * in #VASTEncROI to set ROI priority. \ref roi_rc_priority_support equal to 0 specifies
         * the underlying driver doesn't support ROI priority.
         *
         * User should ignore \ref roi_rc_priority_support when VASTConfigAttribRateControl == VAST_RC_CQP 
         * because ROI delta QP is always required when VASTConfigAttribRateControl == VAST_RC_CQP .
         */
        uint32_t roi_rc_priority_support	: 1;
        /**
         * \brief A flag indicates whether ROI delta QP is supported
         *
         * \ref roi_rc_qp_delta_support equal to 1 specifies the underlying driver supports
         * ROI delta QP when VASTConfigAttribRateControl != VAST_RC_CQP , user can use \c roi_value
         * in #VASTEncROI to set ROI delta QP. \ref roi_rc_qp_delta_support equal to 0 specifies
         * the underlying driver doesn't support ROI delta QP.
         *
         * User should ignore \ref roi_rc_qp_delta_support when VASTConfigAttribRateControl == VAST_RC_CQP 
         * because ROI delta QP is always required when VASTConfigAttribRateControl == VAST_RC_CQP .
         */
        uint32_t roi_rc_qp_delta_support    : 1;
        uint32_t reserved                   : 22;
     } bits;
     uint32_t value;
} VASTConfigAttribValEncROI;

/** \brief Attribute value for VASTConfigAttribEncRateControlExt */
typedef union _VASTConfigAttribValEncRateControlExt {
    struct {
        /**
         * \brief The maximum number of temporal layers minus 1
         *
         * \ref max_num_temporal_layers_minus1 plus 1 specifies the maximum number of temporal
         * layers that supported by the underlying driver. \ref max_num_temporal_layers_minus1
         * equal to 0 implies the underlying driver doesn't support encoding with temporal layer.
         */
        uint32_t max_num_temporal_layers_minus1      : 8;

        /**
         * /brief support temporal layer bit-rate control flag
         *
         * \ref temporal_layer_bitrate_control_flag equal to 1 specifies the underlying driver
         * can support bit-rate control per temporal layer when (#VASTConfigAttribRateControl == #VAST_RC_CBR ||
         * #VASTConfigAttribRateControl == #VAST_RC_VBR).
         *
         * The underlying driver must set \ref temporal_layer_bitrate_control_flag to 0 when
         * \c max_num_temporal_layers_minus1 is equal to 0
         *
         * To use bit-rate control per temporal layer, an application must send the right layer
         * structure via #VASTEncMiscParameterTemporalLayerStructure at the beginning of a coded sequence
         * and then followed by #VASTEncMiscParameterRateControl and #VASTEncMiscParameterFrameRate structures
         * for each layer, using the \c temporal_id field as the layer identifier. Otherwise
         * the driver doesn't use bitrate control per temporal layer if an application doesn't send the
         * layer structure via #VASTEncMiscParameterTemporalLayerStructure to the driver. The driver returns
         * VAST_STATUS_ERROR_INVALID_PARAMETER if an application sends a wrong layer structure or doesn't send
         * #VASTEncMiscParameterRateControl and #VASTEncMiscParameterFrameRate for each layer.
         *
         * The driver will ignore #VASTEncMiscParameterTemporalLayerStructure and the \c temporal_id field
         * in #VASTEncMiscParameterRateControl and #VASTEncMiscParameterFrameRate if
         * \ref temporal_layer_bitrate_control_flag is equal to 0 or #VASTConfigAttribRateControl == #VAST_RC_CQP 
         */
        uint32_t temporal_layer_bitrate_control_flag : 1;
        uint32_t reserved                            : 23;
    } bits;
    uint32_t value;
} VASTConfigAttribValEncRateControlExt;

/** @name Attribute values for VASTConfigAttribProcessingRate. */
/**@{*/
/** \brief Driver does not support processing rate report */
#define VAST_PROCESSING_RATE_NONE                       0x00000000
/** \brief Driver supports encode processing rate report  */
#define VAST_PROCESSING_RATE_ENCODE                     0x00000001
/** \brief Driver supports decode processing rate report  */
#define VAST_PROCESSING_RATE_DECODE                     0x00000002
/**@}*/
/**
 * if an attribute is not applicable for a given
 * profile/entrypoint pair, then set the value to the following 
 */
#define VAST_ATTRIB_NOT_SUPPORTED 0x80000000


/** @name Surface attribute flags */
/**@{*/
/** \brief Surface attribute is not supported. */
#define VAST_SURFACE_ATTRIB_NOT_SUPPORTED 0x00000000
/** \brief Surface attribute can be got through vastQuerySurfaceAttributes(). */
#define VAST_SURFACE_ATTRIB_GETTABLE      0x00000001
/** \brief Surface attribute can be set through vastCreateSurfaces(). */
#define VAST_SURFACE_ATTRIB_SETTABLE      0x00000002
/**@}*/

/** 
 * @name VASTSurfaceAttribMemoryType values in bit fields. 
 * Bit 0:7 are reserved for generic types, Bit 31:28 are reserved for 
 * Linux DRM, Bit 23:20 are reserved for Android. DRM and Android specific
 * types are defined in DRM and Android header files.
 */
/**@{*/
/** \brief VA memory type (default) is supported. */
#define VAST_SURFACE_ATTRIB_MEM_TYPE_VA             0x00000001
/** \brief V4L2 buffer memory type is supported. */
#define VAST_SURFACE_ATTRIB_MEM_TYPE_V4L2           0x00000002
/** \brief User set VA memory type is supported. */
#define VAST_SURFACE_ATTRIB_MEM_TYPE_VA_EXT         0x00000004
/** \brief User pointer memory type is supported. */
#define VAST_SURFACE_ATTRIB_MEM_TYPE_USER_PTR       0x00000008
/**@}*/

/** 
 * \brief VASTSurfaceAttribExternalBuffers structure for 
 * the VASTSurfaceAttribExternalBufferDescriptor attribute.
 */
typedef struct _VASTSurfaceAttribExternalBuffers {
    /** \brief pixel format in fourcc. */
    uint32_t pixel_format;
    /** \brief width in pixels. */
    uint32_t width;
    /** \brief height in pixels. */
    uint32_t height;
    /** \brief total size of the buffer in bytes. */
    uint32_t data_size;
    /** \brief number of planes for planar layout */
    uint32_t num_planes;
    /** \brief pitch for each plane in bytes */
    uint32_t pitches[4];
    /** \brief offset for each plane in bytes */
    uint32_t offsets[4];
    /** \brief buffer handles or user pointers */
    uintptr_t *buffers;
    /** \brief number of elements in the "buffers" array */
    uint32_t num_buffers;
    /** \brief flags. See "Surface external buffer descriptor flags". */
    uint32_t flags;
    /** \brief reserved for passing private data */
    void *private_data;
} VASTSurfaceAttribExternalBuffers;

/** @name VASTSurfaceAttribExternalBuffers flags */
/**@{*/
/** \brief Enable memory tiling */
#define VAST_SURFACE_EXTBUF_DESC_ENABLE_TILING	0x00000001
/** \brief Memory is cacheable */
#define VAST_SURFACE_EXTBUF_DESC_CACHED		0x00000002
/** \brief Memory is non-cacheable */
#define VAST_SURFACE_EXTBUF_DESC_UNCACHED		0x00000004
/** \brief Memory is write-combined */
#define VAST_SURFACE_EXTBUF_DESC_WC		0x00000008
/** \brief Memory is protected */
#define VAST_SURFACE_EXTBUF_DESC_PROTECTED        0x80000000

/** @name VASTSurfaceAttribUsageHint attribute usage hint flags */
/**@{*/
/** \brief Surface usage not indicated. */
#define VAST_SURFACE_ATTRIB_USAGE_HINT_GENERIC 	0x00000000
/** \brief Surface used by video decoder. */
#define VAST_SURFACE_ATTRIB_USAGE_HINT_DECODER 	0x00000001
/** \brief Surface used by video encoder. */
#define VAST_SURFACE_ATTRIB_USAGE_HINT_ENCODER 	0x00000002
/** \brief Surface read by video post-processing. */
#define VAST_SURFACE_ATTRIB_USAGE_HINT_VPP_READ 	0x00000004
/** \brief Surface written by video post-processing. */
#define VAST_SURFACE_ATTRIB_USAGE_HINT_VPP_WRITE 	0x00000008
/** \brief Surface used for display. */
#define VAST_SURFACE_ATTRIB_USAGE_HINT_DISPLAY 	0x00000010

/**
 * Processing rate parameter for encode.
 */
typedef struct _VASTProcessingRateParameterEnc {
    /** \brief Profile level */
    uint8_t         level_idc;
    uint8_t         reserved[3];
    /** \brief quality level. When set to 0, default quality
     * level is used.
     */
    uint32_t        quality_level;
    /** \brief Period between I frames. */
    uint32_t        intra_period;
    /** \brief Period between I/P frames. */
    uint32_t        ip_period;
} VASTProcessingRateParameterEnc;

/**
 * Processing rate parameter for decode.
 */
typedef struct _VASTProcessingRateParameterDec {
    /** \brief Profile level */
    uint8_t         level_idc;
    uint8_t         reserved0[3];
    uint32_t        reserved;
} VASTProcessingRateParameterDec;

typedef struct _VASTProcessingRateParameter {
    union {
        VASTProcessingRateParameterEnc proc_buf_enc;
        VASTProcessingRateParameterDec proc_buf_dec;
    };
} VASTProcessingRateParameter;

/**
 * \brief Queries processing rate for the supplied config.
 *
 * This function queries the processing rate based on parameters in
 * \c proc_buf for the given \c config. Upon successful return, the processing
 * rate value will be stored in \c processing_rate. Processing rate is
 * specified as the number of macroblocks/CTU per second.
 *
 * If NULL is passed to the \c proc_buf, the default processing rate for the
 * given configuration will be returned.
 *
 * @param[in] dpy               the VA display
 * @param[in] config            the config identifying a codec or a video
 *     processing pipeline
 * @param[in] proc_buf       the buffer that contains the parameters for
        either the encode or decode processing rate
 * @param[out] processing_rate  processing rate in number of macroblocks per
        second constrained by parameters specified in proc_buf
 *
 */
VASTStatus
vastQueryProcessingRate(
    VASTDisplay           dpy,
    VASTConfigID          config,
    VASTProcessingRateParameter *proc_buf,
    unsigned int       *processing_rate
);

typedef enum
{
    VASTEncMiscParameterTypeFrameRate 	= 0,
    VASTEncMiscParameterTypeRateControl  	= 1,
    VASTEncMiscParameterTypeMaxSliceSize	= 2,
    VASTEncMiscParameterTypeAIR    	= 3,
    /** \brief Buffer type used to express a maximum frame size (in bits). */
    VASTEncMiscParameterTypeMaxFrameSize  = 4,
    /** \brief Buffer type used for HRD parameters. */
    VASTEncMiscParameterTypeHRD           = 5,
    VASTEncMiscParameterTypeQualityLevel  = 6,
    /** \brief Buffer type used for Rolling intra refresh */
    VASTEncMiscParameterTypeRIR           = 7,
    /** \brief Buffer type used for quantization parameters, it's per-sequence parameter*/
    VASTEncMiscParameterTypeQuantization  = 8,
    /** \brief Buffer type used for sending skip frame parameters to the encoder's
      * rate control, when the user has externally skipped frames. */
    VASTEncMiscParameterTypeSkipFrame     = 9,
    /** \brief Buffer type used for region-of-interest (ROI) parameters. */
    VASTEncMiscParameterTypeROI           = 10,
    /** \brief Buffer type used for temporal layer structure */
    VASTEncMiscParameterTypeTemporalLayerStructure   = 12,
    /** \brief Buffer type used for dirty region-of-interest (ROI) parameters. */
    VASTEncMiscParameterTypeDirtyRect      = 13,
    /** \brief Buffer type used for parallel BRC parameters. */
    VASTEncMiscParameterTypeParallelBRC   = 14,
    /** \brief Set MB partion mode mask and Half-pel/Quant-pel motion search */
    VASTEncMiscParameterTypeSubMbPartPel = 15,
    /** \brief set encode quality tuning */
    VASTEncMiscParameterTypeEncQuality = 16,
    /** \brief Buffer type used for encoder rounding offset parameters. */
    VASTEncMiscParameterTypeCustomRoundingControl = 17,
    /** \brief Buffer type used for FEI input frame level parameters */
    VASTEncMiscParameterTypeFEIFrameControl = 18,
    /** \brief encode extension buffer, ect. MPEG2 Sequence extenstion data */
    VASTEncMiscParameterTypeExtensionData = 19,

    VASTSTEncMiscParameterType = 20,
    VASTEncMiscParameterTypeVFR = 21,
    /** \brief Buffer type used for input frame vir addr*/
    VASTEncMiscParameterTypeInputVirAddr = 22,

    VASTEncMiscParameterTypeCRF = 23,
    VASTEncMiscPassModel = 24,

} VASTEncMiscParameterTypeai;



/** \brief Packed header parameter. */
typedef struct _VASTEncPackedHeaderParameterBuffer {
    /** Type of the packed header buffer. See #VASTEncPackedHeaderType. */
    uint32_t                type;
    /** \brief Size of the #VAEncPackedHeaderDataBuffer in bits. */
    uint32_t                bit_length;
    /** \brief Flag: buffer contains start code emulation prevention bytes? */
    uint8_t               has_emulation_bytes;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncPackedHeaderParameterBuffer;

/**
 *  For application, e.g. set a new bitrate
 *    VASTBufferID buf_id;
 *    VASTEncMiscParameterBuffer *misc_param;
 *    VASTEncMiscParameterRateControl *misc_rate_ctrl;
 * 
 *    vastCreateBuffer(dpy, context, VASTEncMiscParameterBufferType,
 *              sizeof(VASTEncMiscParameterBuffer) + sizeof(VASTEncMiscParameterRateControl),
 *              1, NULL, &buf_id);
 *
 *    vastMapBuffer(dpy,buf_id,(void **)&misc_param);
 *    misc_param->type = VASTEncMiscParameterTypeRateControl;
 *    misc_rate_ctrl= (VASTEncMiscParameterRateControl *)misc_param->data;
 *    misc_rate_ctrl->bits_per_second = 6400000;
 *    vastUnmapBuffer(dpy, buf_id);
 *    vastRenderPicture(dpy, context, &buf_id, 1);
 */
typedef struct _VASTEncMiscParameterBuffer
{
    VASTEncMiscParameterTypeai type;
    uint32_t data[];
} VASTEncMiscParameterBuffer;

/** \brief Temporal layer Structure*/
typedef struct _VASTEncMiscParameterTemporalLayerStructure
{
    /** \brief The number of temporal layers */
    uint32_t number_of_layers;
    /** \brief The length of the array defining frame layer membership. Should be 1-32 */
    uint32_t periodicity;
    /**
     * \brief The array indicating the layer id for each frame
     *
     * The layer id for the first frame in a coded sequence is always 0, so layer_id[] specifies the layer
     * ids for frames starting from the 2nd frame.
     */
    uint32_t layer_id[32];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterTemporalLayerStructure;





typedef struct _VASTEncMiscParameterInputVirAddr
{
    uint64_t input_vir_addr;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterInputVirAddr;

typedef struct _VASTEncMiscParameter2PassModel
{
    uint64_t passmodel;
    uint64_t rate_emu;

} VASTEncMiscParameter2PassModel;

/**
 * Allow a maximum slice size to be specified (in bits).
 * The encoder will attempt to make sure that individual slices do not exceed this size
 * Or to signal applicate if the slice size exceed this size, see "status" of VASTCodedBufferSegment
 */
typedef struct _VASTEncMiscParameterMaxSliceSize
{
    uint32_t max_slice_size;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterMaxSliceSize;

typedef struct _VASTEncMiscParameterAIR
{
    uint32_t air_num_mbs;
    uint32_t air_threshold;
    uint32_t air_auto; /* if set to 1 then hardware auto-tune the AIR threshold */

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterAIR;

/*
 * \brief Rolling intra refresh data structure for encoding.
 */
typedef struct _VASTEncMiscParameterRIR
{
    union
    {
        struct
	/**
	 * \brief Indicate if intra refresh is enabled in column/row.
	 *
	 * App should query VASTConfigAttribEncIntraRefresh to confirm RIR support
	 * by the driver before sending this structure.
         */
        {
	    /* \brief enable RIR in column */
            uint32_t enable_rir_column : 1;
	    /* \brief enable RIR in row */
            uint32_t enable_rir_row : 1;
	    uint32_t reserved : 30;
        } bits;
        uint32_t value;
    } rir_flags;
    /**
     * \brief Indicates the column or row location in MB. It is ignored if
     * rir_flags is 0.
     */
    uint16_t intra_insertion_location;
    /**
     * \brief Indicates the number of columns or rows in MB. It is ignored if
     * rir_flags is 0.
     */
    uint16_t intra_insert_size;
    /**
     * \brief indicates the Qp difference for inserted intra columns or rows.
     * App can use this to adjust intra Qp based on bitrate & max frame size.
     */
    uint8_t  qp_delta_for_inserted_intra;
    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterRIR;


/**
 * \brief Defines a maximum frame size (in bits).
 *
 * This misc parameter buffer defines the maximum size of a frame (in
 * bits). The encoder will try to make sure that each frame does not
 * exceed this size. Otherwise, if the frame size exceeds this size,
 * the \c status flag of #VASTCodedBufferSegment will contain
 * #VA_CODED_BUF_STATUS_FRAME_SIZE_OVERFLOW.
 */
typedef struct _VASTEncMiscParameterBufferMaxFrameSize {
    /** \brief Type. Shall be set to #VASTEncMiscParameterTypeMaxFrameSize. */
    VASTEncMiscParameterTypeai      type;
    /** \brief Maximum size of a frame (in bits). */
    uint32_t                max_frame_size;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterBufferMaxFrameSize;

/**
 * \brief Encoding quality level.
 *
 * The encoding quality could be set through this structure, if the implementation  
 * supports multiple quality levels. The quality level set through this structure is 
 * persistent over the entire coded sequence, or until a new structure is being sent.
 * The quality level range can be queried through the VASTConfigAttribEncQualityRange 
 * attribute. A lower value means higher quality, and a value of 1 represents the highest 
 * quality. The quality level setting is used as a trade-off between quality and speed/power 
 * consumption, with higher quality corresponds to lower speed and higher power consumption. 
 */


/**
 * \brief Quantization settings for encoding.
 *
 * Some encoders support special types of quantization such as trellis, and this structure
 * can be used by the app to control these special types of quantization by the encoder.
 */
typedef struct _VASTEncMiscParameterQuantization
{
    union
    {
    /* if no flags is set then quantization is determined by the driver */
        struct
        {
	    /* \brief disable trellis for all frames/fields */
            uint64_t disable_trellis : 1;
	    /* \brief enable trellis for I frames/fields */
            uint64_t enable_trellis_I : 1;
	    /* \brief enable trellis for P frames/fields */
            uint64_t enable_trellis_P : 1;
	    /* \brief enable trellis for B frames/fields */
            uint64_t enable_trellis_B : 1;
            uint64_t reserved : 28;
        } bits;
        uint64_t value;
    } quantization_flags;
} VASTEncMiscParameterQuantization;

/**
 * \brief Encoding skip frame.
 *
 * The application may choose to skip frames externally to the encoder (e.g. drop completely or 
 * code as all skip's). For rate control purposes the encoder will need to know the size and number 
 * of skipped frames.  Skip frame(s) indicated through this structure is applicable only to the 
 * current frame.  It is allowed for the application to still send in packed headers for the driver to 
 * pack, although no frame will be encoded (e.g. for HW to encrypt the frame).  
 */
typedef struct _VASTEncMiscParameterSkipFrame {
    /** \brief Indicates skip frames as below.
      * 0: Encode as normal, no skip.
      * 1: One or more frames were skipped prior to the current frame, encode the current frame as normal.  
      * 2: The current frame is to be skipped, do not encode it but pack/encrypt the packed header contents
      *    (all except VASTEncPackedHeaderSlice) which could contain actual frame contents (e.g. pack the frame 
      *    in VASTEncPackedHeaderPicture).  */
    uint8_t               skip_frame_flag;
    /** \brief The number of frames skipped prior to the current frame.  Valid when skip_frame_flag = 1. */
    uint8_t               num_skip_frames;
    /** \brief When skip_frame_flag = 1, the size of the skipped frames in bits.   When skip_frame_flag = 2, 
      * the size of the current skipped frame that is to be packed/encrypted in bits. */
    uint32_t                size_skip_frames;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterSkipFrame;

/**
 * \brief Encoding region-of-interest (ROI).
 *
 * The encoding ROI can be set through VASTEncMiscParameterBufferROI, if the implementation
 * supports ROI input. The ROI set through this structure is applicable only to the
 * current frame or field, so must be sent every frame or field to be applied.  The number of
 * supported ROIs can be queried through the VASTConfigAttribEncROI.  The encoder will use the
 * ROI information to adjust the QP values of the MB's that fall within the ROIs.
 */


typedef struct _VASTEncMiscParameterBufferROI {
    /** \brief Number of ROIs being sent.*/
    uint32_t        num_roi;

    /** \brief Valid when VASTConfigAttribRateControl != VAST_RC_CQP , then the encoder's
     *  rate control will determine actual delta QPs.  Specifies the max/min allowed delta
     *  QPs. */
    int8_t                max_delta_qp;
    int8_t                min_delta_qp;

   /** \brief Pointer to a VASTEncROI array with num_roi elements.  It is relative to frame
     *  coordinates for the frame case and to field coordinates for the field case.*/
    VASTEncROI            *roi;
    union {
        struct {
            /**
             * \brief An indication for roi value.
             *
             * \ref roi_value_is_qp_delta equal to 1 indicates \c roi_value in #VASTEncROI should
             * be used as ROI delta QP. \ref roi_value_is_qp_delta equal to 0 indicates \c roi_value
             * in #VASTEncROI should be used as ROI priority.
             *
             * \ref roi_value_is_qp_delta is only available when VASTConfigAttribRateControl != VAST_RC_CQP ,
             * the setting must comply with \c roi_rc_priority_support and \c roi_rc_qp_delta_support in
             * #VASTConfigAttribValEncROI. The underlying driver should ignore this field
             * when VASTConfigAttribRateControl == VAST_RC_CQP .
             */
            uint32_t  roi_value_is_qp_delta    : 1;
            uint32_t  reserved                 : 31;
        } bits;
        uint32_t value;
    } roi_flags;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncMiscParameterBufferROI;
/*
 * \brief Dirty rectangle data structure for encoding.
 *
 * The encoding dirty rect can be set through VASTEncMiscParameterBufferDirtyRect, if the
 * implementation supports dirty rect input. The rect set through this structure is applicable
 * only to the current frame or field, so must be sent every frame or field to be applied.
 * The number of supported rects can be queried through the VASTConfigAttribEncDirtyRect.  The
 * encoder will use the rect information to know those rectangle areas have changed while the
 * areas not covered by dirty rect rectangles are assumed to have not changed compared to the
 * previous picture.  The encoder may do some internal optimizations.
 */
typedef struct _VASTEncMiscParameterBufferDirtyRect
{
    /** \brief Number of Rectangle being sent.*/
    uint32_t    num_roi_rectangle;

    /** \brief Pointer to a VASTRectangle array with num_roi_rectangle elements.*/
     VASTRectangle    *roi_rectangle;
} VASTEncMiscParameterBufferDirtyRect;

/** \brief Attribute value for VASTConfigAttribEncParallelRateControl */
typedef struct _VASTEncMiscParameterParallelRateControl {
    /** brief Number of layers*/
    uint32_t num_layers;
    /** brief Number of B frames per layer per GOP.
     *
     * it should be allocated by application, and the is num_layers.
     *  num_b_in_gop[0] is the number of regular B which refers to only I or P frames. */
    uint32_t *num_b_in_gop;
} VASTEncMiscParameterParallelRateControl;

/** per frame encoder quality controls, once set they will persist for all future frames
  *till it is updated again. */
typedef struct _VASTEncMiscParameterEncQuality
{
    union
    {
        struct
        {
            /** Use raw frames for reference instead of reconstructed frames.
              * it only impact motion estimation (ME)  stage, and will not impact MC stage
              * so the reconstruct picture will can match with decode side */
            uint32_t useRawPicForRef                    : 1;
            /**  Disables skip check for ME stage, it will increase the bistream size
              * but will improve the qulity */
            uint32_t skipCheckDisable                   : 1;
            /**  Indicates app will override default driver FTQ settings using FTQEnable.
              *  FTQ is forward transform quantization */
            uint32_t FTQOverride                        : 1;
            /** Enables/disables FTQ. */
            uint32_t FTQEnable                          : 1;
            /** Indicates the app will provide the Skip Threshold LUT to use when FTQ is
              * enabled (FTQSkipThresholdLUT), else default driver thresholds will be used. */
            uint32_t FTQSkipThresholdLUTInput           : 1;
            /** Indicates the app will provide the Skip Threshold LUT to use when FTQ is
              * disabled (NonFTQSkipThresholdLUT), else default driver thresholds will be used. */
            uint32_t NonFTQSkipThresholdLUTInput        : 1;
            uint32_t ReservedBit                        : 1;
            /** Control to enable the ME mode decision algorithm to bias to fewer B Direct/Skip types.
              * Applies only to B frames, all other frames will ignore this setting.  */
            uint32_t directBiasAdjustmentEnable         : 1;
            /** Enables global motion bias. global motion also is called HME (Heirarchical Motion Estimation )
              * HME is used to handle large motions and avoiding local minima in the video encoding process
              * down scaled the input and reference picture, then do ME. the result will be a predictor to next level HME or ME
              * current interface divide the HME to 3 level. UltraHME , SuperHME, and HME, result of UltraHME will be input of SurperHME,
              * result of superHME will be a input for HME. HME result will be input of ME. it is a switch for HMEMVCostScalingFactor
              * can change the HME bias inside RDO stage*/
            uint32_t globalMotionBiasAdjustmentEnable   : 1;
            /** MV cost scaling ratio for HME ( predictors.  It is used when
              * globalMotionBiasAdjustmentEnable == 1, else it is ignored.  Values are:
              *     0: set MV cost to be 0 for HME predictor.
              *     1: scale MV cost to be 1/2 of the default value for HME predictor.
              *     2: scale MV cost to be 1/4 of the default value for HME predictor.
              *     3: scale MV cost to be 1/8 of the default value for HME predictor. */
            uint32_t HMEMVCostScalingFactor             : 2;
            /**disable HME, if it is disabled. Super*ultraHME should also be disabled  */
            uint32_t HMEDisable                         : 1;
            /**disable Super HME, if it is disabled, ultraHME should be disabled */
            uint32_t SuperHMEDisable                    : 1;
            /** disable Ultra HME */
            uint32_t UltraHMEDisable                    : 1;
            /** disable panic mode. Panic mode happened when there are extreme BRC (bit rate control) requirement
              * frame size cant achieve the target of BRC.  when Panic mode is triggered, Coefficients will
              *  be set to zero. disable panic mode will improve quality but will impact BRC */
            uint32_t PanicModeDisable                   : 1;
            /** Force RepartitionCheck
             *  0: DEFAULT - follow driver default settings.
             *  1: FORCE_ENABLE - enable this feature totally for all cases.
             *  2: FORCE_DISABLE - disable this feature totally for all cases. */
            uint32_t ForceRepartitionCheck              : 2;

        };
        uint32_t encControls;
    };

    /** Maps QP to skip thresholds when FTQ is enabled.  Valid range is 0-255. */
    uint8_t FTQSkipThresholdLUT[52];
    /** Maps QP to skip thresholds when FTQ is disabled.  Valid range is 0-65535. */
    uint16_t NonFTQSkipThresholdLUT[52];

    uint32_t reserved[VAST_PADDING_HIGH];  // Reserved for future use.

} VASTEncMiscParameterEncQuality;

/**
 *  \brief Custom Encoder Rounding Offset Control.
 *  Application may use this structure to set customized rounding
 *  offset parameters for quantization.
 *  Valid when \c VASTConfigAttribCustomRoundingControl equals 1.
 */
typedef struct _VASTEncMiscParameterCustomRoundingControl
{
    union {
        struct {
            /** \brief Enable customized rounding offset for intra blocks.
             *  If 0, default value would be taken by driver for intra
             *  rounding offset.
             */
            uint32_t    enable_custom_rouding_intra     : 1 ;

            /** \brief Intra rounding offset
             *  Ignored if \c enable_custom_rouding_intra equals 0.
             */
            uint32_t    rounding_offset_intra           : 7;

            /** \brief Enable customized rounding offset for inter blocks.
             *  If 0, default value would be taken by driver for inter
             *  rounding offset.
             */
            uint32_t    enable_custom_rounding_inter    : 1 ;

            /** \brief Inter rounding offset
             *  Ignored if \c enable_custom_rouding_inter equals 0.
             */
            uint32_t    rounding_offset_inter           : 7;

           /* Reserved */
            uint32_t    reserved                        :16;
        }  bits;
        uint32_t    value;
    }   rounding_offset_setting;
} VASTEncMiscParameterCustomRoundingControl;

typedef struct _VASTEncMiscParameterVFR
{
    uint32_t pts;
    uint32_t timebaseDen;
    uint32_t timebaseNum;
    uint32_t temporal_id;
    uint32_t reserved[VAST_PADDING_HIGH];  // Reserved for future use.

} VASTEncMiscParameterVFR;

typedef struct _VASTEncMiscParameterCRF
{
    int32_t crf;
    uint32_t reserved[VAST_PADDING_HIGH];  // Reserved for future use.
} VASTEncMiscParameterCRF;

/* Codec-independent Slice Parameter Buffer base */
typedef struct _VASTSliceParameterBufferBase
{
    uint32_t slice_data_size;	/* number of bytes in the slice data buffer for this slice */
    uint32_t slice_data_offset;	/* the offset to the first byte of slice data */
    uint32_t slice_data_flag;	/* see VA_SLICE_DATA_FLAG_XXX definitions */
} VASTSliceParameterBufferBase;

/****************************
 * MPEG-2 data structures
 ****************************/
 
/* MPEG-2 Picture Parameter Buffer */
/* 
 * For each frame or field, and before any slice data, a single
 * picture parameter buffer must be send.
 */
typedef struct _VASTPictureParameterBufferMPEG2
{
    uint16_t horizontal_size;
    uint16_t vertical_size;
    VASTSurfaceID forward_reference_picture;
    VASTSurfaceID backward_reference_picture;
    /* meanings of the following fields are the same as in the standard */
    int32_t picture_coding_type;
    int32_t f_code; /* pack all four fcode into this */
    union {
        struct {
            uint32_t intra_dc_precision		: 2; 
            uint32_t picture_structure		: 2; 
            uint32_t top_field_first		: 1; 
            uint32_t frame_pred_frame_dct		: 1; 
            uint32_t concealment_motion_vectors	: 1;
            uint32_t q_scale_type			: 1;
            uint32_t intra_vlc_format		: 1;
            uint32_t alternate_scan			: 1;
            uint32_t repeat_first_field		: 1;
            uint32_t progressive_frame		: 1;
            uint32_t is_first_field			: 1; /* indicate whether the current field
                                                              * is the first field for field picture
                                                              */
        } bits;
        uint32_t value;
    } picture_coding_extension;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTPictureParameterBufferMPEG2;

/** MPEG-2 Inverse Quantization Matrix Buffer */
typedef struct _VASTIQMatrixBufferMPEG2
{
    /** \brief Same as the MPEG-2 bitstream syntax element. */
    int32_t load_intra_quantiser_matrix;
    /** \brief Same as the MPEG-2 bitstream syntax element. */
    int32_t load_non_intra_quantiser_matrix;
    /** \brief Same as the MPEG-2 bitstream syntax element. */
    int32_t load_chroma_intra_quantiser_matrix;
    /** \brief Same as the MPEG-2 bitstream syntax element. */
    int32_t load_chroma_non_intra_quantiser_matrix;
    /** \brief Luminance intra matrix, in zig-zag scan order. */
    uint8_t intra_quantiser_matrix[64];
    /** \brief Luminance non-intra matrix, in zig-zag scan order. */
    uint8_t non_intra_quantiser_matrix[64];
    /** \brief Chroma intra matrix, in zig-zag scan order. */
    uint8_t chroma_intra_quantiser_matrix[64];
    /** \brief Chroma non-intra matrix, in zig-zag scan order. */
    uint8_t chroma_non_intra_quantiser_matrix[64];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTIQMatrixBufferMPEG2;

/** MPEG-2 Slice Parameter Buffer */
typedef struct _VASTSliceParameterBufferMPEG2
{
    uint32_t slice_data_size;/* number of bytes in the slice data buffer for this slice */
    uint32_t slice_data_offset;/* the offset to the first byte of slice data */
    uint32_t slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    uint32_t macroblock_offset;/* the offset to the first bit of MB from the first byte of slice data */
    uint32_t slice_horizontal_position;
    uint32_t slice_vertical_position;
    int32_t quantiser_scale_code;
    int32_t intra_slice_flag;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTSliceParameterBufferMPEG2;

/** MPEG-2 Macroblock Parameter Buffer */
typedef struct _VASTMacroblockParameterBufferMPEG2
{
    uint16_t macroblock_address;
    /* 
     * macroblock_address (in raster scan order)
     * top-left: 0
     * bottom-right: picture-height-in-mb*picture-width-in-mb - 1
     */
    uint8_t macroblock_type;  /* see definition below */
    union {
        struct {
            uint32_t frame_motion_type		: 2; 
            uint32_t field_motion_type		: 2; 
            uint32_t dct_type			: 1; 
        } bits;
        uint32_t value;
    } macroblock_modes;
    uint8_t motion_vertical_field_select; 
    /* 
     * motion_vertical_field_select:
     * see section 6.3.17.2 in the spec
     * only the lower 4 bits are used
     * bit 0: first vector forward
     * bit 1: first vector backward
     * bit 2: second vector forward
     * bit 3: second vector backward
     */
    int16_t PMV[2][2][2]; /* see Table 7-7 in the spec */
    uint16_t coded_block_pattern;
    /* 
     * The bitplanes for coded_block_pattern are described 
     * in Figure 6.10-12 in the spec
     */
     
    /* Number of skipped macroblocks after this macroblock */
    uint16_t num_skipped_macroblocks;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTMacroblockParameterBufferMPEG2;

/* 
 * OR'd flags for macroblock_type (section 6.3.17.1 in the spec)
 */
#define VAST_MB_TYPE_MOTION_FORWARD	0x02
#define VAST_MB_TYPE_MOTION_BACKWARD	0x04
#define VAST_MB_TYPE_MOTION_PATTERN	0x08
#define VAST_MB_TYPE_MOTION_INTRA		0x10

/**
 * MPEG-2 Residual Data Buffer 
 * For each macroblock, there wil be 64 shorts (16-bit) in the 
 * residual data buffer
 */

/****************************
 * MPEG-4 Part 2 data structures
 ****************************/
 
/* MPEG-4 Picture Parameter Buffer */
/* 
 * For each frame or field, and before any slice data, a single
 * picture parameter buffer must be send.
 */
typedef struct _VASTPictureParameterBufferMPEG4
{
    uint16_t vop_width;
    uint16_t vop_height;
    VASTSurfaceID forward_reference_picture;
    VASTSurfaceID backward_reference_picture;
    union {
        struct {
            uint32_t short_video_header		: 1; 
            uint32_t chroma_format			: 2; 
            uint32_t interlaced			: 1; 
            uint32_t obmc_disable			: 1; 
            uint32_t sprite_enable			: 2; 
            uint32_t sprite_warping_accuracy	: 2; 
            uint32_t quant_type			: 1; 
            uint32_t quarter_sample			: 1; 
            uint32_t data_partitioned		: 1; 
            uint32_t reversible_vlc			: 1; 
            uint32_t resync_marker_disable		: 1; 
        } bits;
        uint32_t value;
    } vol_fields;
    uint8_t no_of_sprite_warping_points;
    int16_t sprite_trajectory_du[3];
    int16_t sprite_trajectory_dv[3];
    uint8_t quant_precision;
    union {
        struct {
            uint32_t vop_coding_type		: 2; 
            uint32_t backward_reference_vop_coding_type	: 2; 
            uint32_t vop_rounding_type		: 1; 
            uint32_t intra_dc_vlc_thr		: 3; 
            uint32_t top_field_first		: 1; 
            uint32_t alternate_vertical_scan_flag	: 1; 
        } bits;
        uint32_t value;
    } vop_fields;
    uint8_t vop_fcode_forward;
    uint8_t vop_fcode_backward;
    uint16_t vop_time_increment_resolution;
    /* short header related */
    uint8_t num_gobs_in_vop;
    uint8_t num_macroblocks_in_gob;
    /* for direct mode prediction */
    int16_t TRB;
    int16_t TRD;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTPictureParameterBufferMPEG4;

/** MPEG-4 Inverse Quantization Matrix Buffer */
typedef struct _VASTIQMatrixBufferMPEG4
{
    /** Same as the MPEG-4:2 bitstream syntax element. */
    int32_t load_intra_quant_mat;
    /** Same as the MPEG-4:2 bitstream syntax element. */
    int32_t load_non_intra_quant_mat;
    /** The matrix for intra blocks, in zig-zag scan order. */
    uint8_t intra_quant_mat[64];
    /** The matrix for non-intra blocks, in zig-zag scan order. */
    uint8_t non_intra_quant_mat[64];

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTIQMatrixBufferMPEG4;

/** MPEG-4 Slice Parameter Buffer */
typedef struct _VASTSliceParameterBufferMPEG4
{
    uint32_t slice_data_size;/* number of bytes in the slice data buffer for this slice */
    uint32_t slice_data_offset;/* the offset to the first byte of slice data */
    uint32_t slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    uint32_t macroblock_offset;/* the offset to the first bit of MB from the first byte of slice data */
    uint32_t macroblock_number;
    int32_t quant_scale;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTSliceParameterBufferMPEG4;

/**
 VC-1 data structures
*/

typedef enum   /* see 7.1.1.32 */
{
    VASTMvMode1Mv                        = 0,
    VASTMvMode1MvHalfPel                 = 1,
    VASTMvMode1MvHalfPelBilinear         = 2,
    VASTMvModeMixedMv                    = 3,
    VASTMvModeIntensityCompensation      = 4 
} VASTMvModeVC1;

/** VC-1 Picture Parameter Buffer */
/* 
 * For each picture, and before any slice data, a picture parameter
 * buffer must be send. Multiple picture parameter buffers may be
 * sent for a single picture. In that case picture parameters will
 * apply to all slice data that follow it until a new picture
 * parameter buffer is sent.
 *
 * Notes:
 *   pic_quantizer_type should be set to the applicable quantizer
 *   type as defined by QUANTIZER (J.1.19) and either
 *   PQUANTIZER (7.1.1.8) or PQINDEX (7.1.1.6)
 */
typedef struct _VASTPictureParameterBufferVC1
{
    VASTSurfaceID forward_reference_picture;
    VASTSurfaceID backward_reference_picture;
    /* if out-of-loop post-processing is done on the render
       target, then we need to keep the in-loop decoded 
       picture as a reference picture */
    VASTSurfaceID inloop_decoded_picture;

    /* sequence layer for AP or meta data for SP and MP */
    union {
        struct {
            uint32_t pulldown	: 1; /* SEQUENCE_LAYER::PULLDOWN */
            uint32_t interlace	: 1; /* SEQUENCE_LAYER::INTERLACE */
            uint32_t tfcntrflag	: 1; /* SEQUENCE_LAYER::TFCNTRFLAG */
            uint32_t finterpflag	: 1; /* SEQUENCE_LAYER::FINTERPFLAG */
            uint32_t psf		: 1; /* SEQUENCE_LAYER::PSF */
            uint32_t multires	: 1; /* METADATA::MULTIRES */
            uint32_t overlap	: 1; /* METADATA::OVERLAP */
            uint32_t syncmarker	: 1; /* METADATA::SYNCMARKER */
            uint32_t rangered	: 1; /* METADATA::RANGERED */
            uint32_t max_b_frames	: 3; /* METADATA::MAXBFRAMES */
            uint32_t profile	: 2; /* SEQUENCE_LAYER::PROFILE or The MSB of METADATA::PROFILE */
        } bits;
        uint32_t value;
    } sequence_fields;

    uint16_t coded_width;		/* ENTRY_POINT_LAYER::CODED_WIDTH */
    uint16_t coded_height;	/* ENTRY_POINT_LAYER::CODED_HEIGHT */
    union {
	struct {
            uint32_t broken_link	: 1; /* ENTRY_POINT_LAYER::BROKEN_LINK */
            uint32_t closed_entry	: 1; /* ENTRY_POINT_LAYER::CLOSED_ENTRY */
            uint32_t panscan_flag	: 1; /* ENTRY_POINT_LAYER::PANSCAN_FLAG */
            uint32_t loopfilter	: 1; /* ENTRY_POINT_LAYER::LOOPFILTER */
	} bits;
	uint32_t value;
    } entrypoint_fields;
    uint8_t conditional_overlap_flag; /* ENTRY_POINT_LAYER::CONDOVER */
    uint8_t fast_uvmc_flag;	/* ENTRY_POINT_LAYER::FASTUVMC */
    union {
        struct {
            uint32_t luma_flag	: 1; /* ENTRY_POINT_LAYER::RANGE_MAPY_FLAG */
            uint32_t luma		: 3; /* ENTRY_POINT_LAYER::RANGE_MAPY */
            uint32_t chroma_flag	: 1; /* ENTRY_POINT_LAYER::RANGE_MAPUV_FLAG */
            uint32_t chroma		: 3; /* ENTRY_POINT_LAYER::RANGE_MAPUV */
        } bits;
        uint32_t value;
    } range_mapping_fields;

    uint8_t b_picture_fraction;	/* Index for PICTURE_LAYER::BFRACTION value in Table 40 (7.1.1.14) */
    uint8_t cbp_table;		/* PICTURE_LAYER::CBPTAB/ICBPTAB */
    uint8_t mb_mode_table;	/* PICTURE_LAYER::MBMODETAB */
    uint8_t range_reduction_frame;/* PICTURE_LAYER::RANGEREDFRM */
    uint8_t rounding_control;	/* PICTURE_LAYER::RNDCTRL */
    uint8_t post_processing;	/* PICTURE_LAYER::POSTPROC */
    uint8_t picture_resolution_index;	/* PICTURE_LAYER::RESPIC */
    uint8_t luma_scale;		/* PICTURE_LAYER::LUMSCALE */
    uint8_t luma_shift;		/* PICTURE_LAYER::LUMSHIFT */

    union {
        struct {
            uint32_t picture_type		: 3; /* PICTURE_LAYER::PTYPE */
            uint32_t frame_coding_mode	: 3; /* PICTURE_LAYER::FCM */
            uint32_t top_field_first	: 1; /* PICTURE_LAYER::TFF */
            uint32_t is_first_field		: 1; /* set to 1 if it is the first field */
            uint32_t intensity_compensation	: 1; /* PICTURE_LAYER::INTCOMP */
        } bits;
        uint32_t value;
    } picture_fields;
    union {
        struct {
            uint32_t mv_type_mb	: 1; 	/* PICTURE::MVTYPEMB */
            uint32_t direct_mb	: 1; 	/* PICTURE::DIRECTMB */
            uint32_t skip_mb	: 1; 	/* PICTURE::SKIPMB */
            uint32_t field_tx	: 1; 	/* PICTURE::FIELDTX */
            uint32_t forward_mb	: 1;	/* PICTURE::FORWARDMB */
            uint32_t ac_pred	: 1;	/* PICTURE::ACPRED */
            uint32_t overflags	: 1;	/* PICTURE::OVERFLAGS */
        } flags;
        uint32_t value;
    } raw_coding;
    union {
        struct {
            uint32_t bp_mv_type_mb   : 1;    /* PICTURE::MVTYPEMB */
            uint32_t bp_direct_mb    : 1;    /* PICTURE::DIRECTMB */
            uint32_t bp_skip_mb      : 1;    /* PICTURE::SKIPMB */  
            uint32_t bp_field_tx     : 1;    /* PICTURE::FIELDTX */ 
            uint32_t bp_forward_mb   : 1;    /* PICTURE::FORWARDMB */
            uint32_t bp_ac_pred      : 1;    /* PICTURE::ACPRED */   
            uint32_t bp_overflags    : 1;    /* PICTURE::OVERFLAGS */
        } flags;
        uint32_t value;
    } bitplane_present; /* signal what bitplane is being passed via the bitplane buffer */
    union {
        struct {
            uint32_t reference_distance_flag : 1;/* PICTURE_LAYER::REFDIST_FLAG */
            uint32_t reference_distance	: 5;/* PICTURE_LAYER::REFDIST */
            uint32_t num_reference_pictures: 1;/* PICTURE_LAYER::NUMREF */
            uint32_t reference_field_pic_indicator	: 1;/* PICTURE_LAYER::REFFIELD */
        } bits;
        uint32_t value;
    } reference_fields;
    union {
        struct {
            uint32_t mv_mode		: 3; /* PICTURE_LAYER::MVMODE */
            uint32_t mv_mode2		: 3; /* PICTURE_LAYER::MVMODE2 */
            uint32_t mv_table		: 3; /* PICTURE_LAYER::MVTAB/IMVTAB */
            uint32_t two_mv_block_pattern_table: 2; /* PICTURE_LAYER::2MVBPTAB */
            uint32_t four_mv_switch		: 1; /* PICTURE_LAYER::4MVSWITCH */
            uint32_t four_mv_block_pattern_table : 2; /* PICTURE_LAYER::4MVBPTAB */
            uint32_t extended_mv_flag	: 1; /* ENTRY_POINT_LAYER::EXTENDED_MV */
            uint32_t extended_mv_range	: 2; /* PICTURE_LAYER::MVRANGE */
            uint32_t extended_dmv_flag	: 1; /* ENTRY_POINT_LAYER::EXTENDED_DMV */
            uint32_t extended_dmv_range	: 2; /* PICTURE_LAYER::DMVRANGE */
        } bits;
        uint32_t value;
    } mv_fields;
    union {
        struct {
            uint32_t dquant	: 2; 	/* ENTRY_POINT_LAYER::DQUANT */
            uint32_t quantizer     : 2; 	/* ENTRY_POINT_LAYER::QUANTIZER */
            uint32_t half_qp	: 1; 	/* PICTURE_LAYER::HALFQP */
            uint32_t pic_quantizer_scale : 5;/* PICTURE_LAYER::PQUANT */
            uint32_t pic_quantizer_type : 1;/* PICTURE_LAYER::PQUANTIZER */
            uint32_t dq_frame	: 1; 	/* VOPDQUANT::DQUANTFRM */
            uint32_t dq_profile	: 2; 	/* VOPDQUANT::DQPROFILE */
            uint32_t dq_sb_edge	: 2; 	/* VOPDQUANT::DQSBEDGE */
            uint32_t dq_db_edge 	: 2; 	/* VOPDQUANT::DQDBEDGE */
            uint32_t dq_binary_level : 1; 	/* VOPDQUANT::DQBILEVEL */
            uint32_t alt_pic_quantizer : 5;/* VOPDQUANT::ALTPQUANT */
        } bits;
        uint32_t value;
    } pic_quantizer_fields;
    union {
        struct {
            uint32_t variable_sized_transform_flag	: 1;/* ENTRY_POINT_LAYER::VSTRANSFORM */
            uint32_t mb_level_transform_type_flag	: 1;/* PICTURE_LAYER::TTMBF */
            uint32_t frame_level_transform_type	: 2;/* PICTURE_LAYER::TTFRM */
            uint32_t transform_ac_codingset_idx1	: 2;/* PICTURE_LAYER::TRANSACFRM */
            uint32_t transform_ac_codingset_idx2	: 2;/* PICTURE_LAYER::TRANSACFRM2 */
            uint32_t intra_transform_dc_table	: 1;/* PICTURE_LAYER::TRANSDCTAB */
        } bits;
        uint32_t value;
    } transform_fields;

    uint8_t luma_scale2;                  /* PICTURE_LAYER::LUMSCALE2 */
    uint8_t luma_shift2;                  /* PICTURE_LAYER::LUMSHIFT2 */
    uint8_t intensity_compensation_field; /* Index for PICTURE_LAYER::INTCOMPFIELD value in Table 109 (9.1.1.48) */

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_MEDIUM - 1];
} VASTPictureParameterBufferVC1;

/** VC-1 Bitplane Buffer
There will be at most three bitplanes coded in any picture header. To send 
the bitplane data more efficiently, each byte is divided in two nibbles, with
each nibble carrying three bitplanes for one macroblock.  The following table
shows the bitplane data arrangement within each nibble based on the picture
type.

Picture Type	Bit3		Bit2		Bit1		Bit0
I or BI				OVERFLAGS	ACPRED		FIELDTX
P				MYTYPEMB	SKIPMB		DIRECTMB
B				FORWARDMB	SKIPMB		DIRECTMB

Within each byte, the lower nibble is for the first MB and the upper nibble is 
for the second MB.  E.g. the lower nibble of the first byte in the bitplane
buffer is for Macroblock #1 and the upper nibble of the first byte is for 
Macroblock #2 in the first row.
*/

/* VC-1 Slice Parameter Buffer */
typedef struct _VASTSliceParameterBufferVC1
{
    uint32_t slice_data_size;/* number of bytes in the slice data buffer for this slice */
    uint32_t slice_data_offset;/* the offset to the first byte of slice data */
    uint32_t slice_data_flag; /* see VA_SLICE_DATA_FLAG_XXX defintions */
    uint32_t macroblock_offset;/* the offset to the first bit of MB from the first byte of slice data */
    uint32_t slice_vertical_position;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTSliceParameterBufferVC1;

/****************************
 * Common encode data structures 
 ****************************/
typedef enum
{
    VASTEncPictureTypeIntra		= 0,
    VASTEncPictureTypePredictive		= 1,
    VASTEncPictureTypeBidirectional	= 2,
} VASTEncPictureType;

/**
 * \brief Encode Slice Parameter Buffer.
 *
 * @deprecated
 * This is a deprecated encode slice parameter buffer, All applications
 * \c can use VAEncSliceParameterBufferXXX (XXX = MPEG2, HEVC, H264, JPEG)
 */
typedef struct _VASTEncSliceParameterBuffer
{
    uint32_t start_row_number;	/* starting MB row number for this slice */
    uint32_t slice_height;	/* slice height measured in MB */
    union {
        struct {
            uint32_t is_intra	: 1;
            uint32_t disable_deblocking_filter_idc : 2;
            uint32_t uses_long_term_ref		:1;
            uint32_t is_long_term_ref		:1;
        } bits;
        uint32_t value;
    } slice_flags;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncSliceParameterBuffer;


/****************************
 * H.263 specific encode data structures
 ****************************/

typedef struct _VASTEncSequenceParameterBufferH263
{
    uint32_t intra_period;
    uint32_t bits_per_second;
    uint32_t frame_rate;
    uint32_t initial_qp;
    uint32_t min_qp;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncSequenceParameterBufferH263;

typedef struct _VASTEncPictureParameterBufferH263
{
    VASTSurfaceID reference_picture;
    VASTSurfaceID reconstructed_picture;
    VASTBufferID coded_buf;
    uint16_t picture_width;
    uint16_t picture_height;
    VASTEncPictureType picture_type;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncPictureParameterBufferH263;

/****************************
 * MPEG-4 specific encode data structures
 ****************************/

typedef struct _VASTEncSequenceParameterBufferMPEG4
{
    uint8_t profile_and_level_indication;
    uint32_t intra_period;
    uint32_t video_object_layer_width;
    uint32_t video_object_layer_height;
    uint32_t vop_time_increment_resolution;
    uint32_t fixed_vop_rate;
    uint32_t fixed_vop_time_increment;
    uint32_t bits_per_second;
    uint32_t frame_rate;
    uint32_t initial_qp;
    uint32_t min_qp;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncSequenceParameterBufferMPEG4;

typedef struct _VASTEncPictureParameterBufferMPEG4
{
    VASTSurfaceID reference_picture;
    VASTSurfaceID reconstructed_picture;
    VASTBufferID coded_buf;
    uint16_t picture_width;
    uint16_t picture_height;
    uint32_t modulo_time_base; /* number of 1s */
    uint32_t vop_time_increment;
    VASTEncPictureType picture_type;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTEncPictureParameterBufferMPEG4;

/* 
 * FICTURE_AVE_QP(bit7-0): The average Qp value used during this frame
 * LARGE_SLICE(bit8):At least one slice in the current frame was large
 *              enough for the encoder to attempt to limit its size.
 * SLICE_OVERFLOW(bit9): At least one slice in the current frame has
 *              exceeded the maximum slice size specified.
 * BITRATE_OVERFLOW(bit10): The peak bitrate was exceeded for this frame.
 * BITRATE_HIGH(bit11): The frame size got within the safety margin of the maximum size (VCM only)
 * AIR_MB_OVER_THRESHOLD: the number of MBs adapted to Intra MB
 */
#define VAST_CODED_BUF_STATUS_PICTURE_AVE_QP_MASK         0xff
#define VAST_CODED_BUF_STATUS_LARGE_SLICE_MASK            0x100
#define VAST_CODED_BUF_STATUS_SLICE_OVERFLOW_MASK         0x200
#define VAST_CODED_BUF_STATUS_BITRATE_OVERFLOW		0x400
#define VAST_CODED_BUF_STATUS_BITRATE_HIGH		0x800
/**
 * \brief The frame has exceeded the maximum requested size.
 *
 * This flag indicates that the encoded frame size exceeds the value
 * specified through a misc parameter buffer of type
 * #VASTEncMiscParameterTypeMaxFrameSize.
 */
#define VAST_CODED_BUF_STATUS_FRAME_SIZE_OVERFLOW         0x1000
/**
 * \brief the bitstream is bad or corrupt.
 */
#define VAST_CODED_BUF_STATUS_BAD_BITSTREAM               0x8000
#define VAST_CODED_BUF_STATUS_AIR_MB_OVER_THRESHOLD	0xff0000
#define VAST_CODED_BUF_STATUS_EXEPTION	0x2000

/**
 * \brief The coded buffer segment status contains frame encoding passes number
 *
 * This is the mask to get the number of encoding passes from the coded
 * buffer segment status. 
 * NUMBER_PASS(bit24~bit27): the number for encoding passes executed for the coded frame.
 * 
 */
#define VAST_CODED_BUF_STATUS_NUMBER_PASSES_MASK          0xf000000

/**
 * \brief The coded buffer segment contains a single NAL unit. 
 *
 * This flag indicates that the coded buffer segment contains a
 * single NAL unit. This flag might be useful to the user for 
 * processing the coded buffer.
 */
#define VAST_CODED_BUF_STATUS_SINGLE_NALU                 0x10000000	

/** \brief VA buffer information */
typedef struct {
    /** \brief Buffer handle */
    uintptr_t           handle;
    /** \brief Buffer type (See \ref VASTBufferType). */
    uint32_t            type;
    /**
     * \brief Buffer memory type (See \ref VASTSurfaceAttribMemoryType).
     *
     * On input to vastAcquireBufferHandle(), this field can serve as a hint
     * to specify the set of memory types the caller is interested in.
     * On successful return from vastAcquireBufferHandle(), the field is
     * updated with the best matching memory type.
     */
    uint32_t            mem_type;
    /** \brief Size of the underlying buffer. */
    size_t              mem_size;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTBufferInfo;

/**
 * \brief Acquires buffer handle for external API usage
 *
 * Locks the VA buffer object \ref buf_id for external API usage like
 * EGL or OpenCL (OCL). This function is a synchronization point. This
 * means that any pending operation is guaranteed to be completed
 * prior to returning from the function.
 *
 * If the referenced VA buffer object is the backing store of a VA
 * surface, then this function acts as if vastSyncSurface() on the
 * parent surface was called first.
 *
 * The \ref VASTBufferInfo argument shall be zero'ed on input. On
 * successful output, the data structure is filled in with all the
 * necessary buffer level implementation details like handle, type,
 * memory type and memory size.
 *
 * Note: the external API implementation, or the application, can
 * express the memory types it is interested in by filling in the \ref
 * mem_type field accordingly. On successful output, the memory type
 * that fits best the request and that was used is updated in the \ref
 * VASTBufferInfo data structure. If none of the supplied memory types
 * is supported, then a \ref VAST_STATUS_ERROR_UNSUPPORTED_MEMORY_TYPE
 * error is returned.
 *
 * The \ref VASTBufferInfo data is valid until vastReleaseBufferHandle()
 * is called. Besides, no additional operation is allowed on any of
 * the buffer parent object until vastReleaseBufferHandle() is called.
 * e.g. decoding into a VA surface backed with the supplied VA buffer
 * object \ref buf_id would fail with a \ref VAST_STATUS_ERROR_SURFACE_BUSY
 * error.
 *
 * Possible errors:
 * - \ref VAST_STATUS_ERROR_UNIMPLEMENTED: the VA driver implementation
 *   does not support this interface
 * - \ref VAST_STATUS_ERROR_INVALID_DISPLAY: an invalid display was supplied
 * - \ref VAST_STATUS_ERROR_INVALID_BUFFER: an invalid buffer was supplied
 * - \ref VAST_STATUS_ERROR_UNSUPPORTED_BUFFERTYPE: the implementation
 *   does not support exporting buffers of the specified type
 * - \ref VAST_STATUS_ERROR_UNSUPPORTED_MEMORY_TYPE: none of the requested
 *   memory types in \ref VASTBufferInfo.mem_type was supported
 *
 * @param[in] dpy               the VA display
 * @param[in] buf_id            the VA buffer
 * @param[in,out] buf_info      the associated VA buffer information
 * @return VAST_STATUS_SUCCESS if successful
 */
VASTStatus
vastAcquireBufferHandle(VASTDisplay dpy, VASTBufferID buf_id, VASTBufferInfo *buf_info);

/**
 * \brief Releases buffer after usage from external API
 *
 * Unlocks the VA buffer object \ref buf_id from external API usage like
 * EGL or OpenCL (OCL). This function is a synchronization point. This
 * means that any pending operation is guaranteed to be completed
 * prior to returning from the function.
 *
 * The \ref VASTBufferInfo argument shall point to the original data
 * structure that was obtained from vastAcquireBufferHandle(), unaltered.
 * This is necessary so that the VA driver implementation could
 * deallocate any resources that were needed.
 *
 * In any case, returning from this function invalidates any contents
 * in \ref VASTBufferInfo. i.e. the underlyng buffer handle is no longer
 * valid. Therefore, VA driver implementations are free to reset this
 * data structure to safe defaults.
 *
 * Possible errors:
 * - \ref VAST_STATUS_ERROR_UNIMPLEMENTED: the VA driver implementation
 *   does not support this interface
 * - \ref VAST_STATUS_ERROR_INVALID_DISPLAY: an invalid display was supplied
 * - \ref VAST_STATUS_ERROR_INVALID_BUFFER: an invalid buffer was supplied
 * - \ref VAST_STATUS_ERROR_UNSUPPORTED_BUFFERTYPE: the implementation
 *   does not support exporting buffers of the specified type
 *
 * @param[in] dpy               the VA display
 * @param[in] buf_id            the VA buffer
 * @return VAST_STATUS_SUCCESS if successful
 */
VASTStatus
vastReleaseBufferHandle(VASTDisplay dpy, VASTBufferID buf_id);

/** @name vastExportSurfaceHandle() flags
 *
 * @{
 */
/** Export surface to be read by external API. */
#define VAST_EXPORT_SURFACE_READ_ONLY        0x0001
/** Export surface to be written by external API. */
#define VAST_EXPORT_SURFACE_WRITE_ONLY       0x0002
/** Export surface to be both read and written by external API. */
#define VAST_EXPORT_SURFACE_READ_WRITE       0x0003
/** Export surface with separate layers.
 *
 * For example, NV12 surfaces should be exported as two separate
 * planes for luma and chroma.
 */
#define VAST_EXPORT_SURFACE_SEPARATE_LAYERS  0x0004
/** Export surface with composed layers.
 *
 * For example, NV12 surfaces should be exported as a single NV12
 * composed object.
 */
#define VAST_EXPORT_SURFACE_COMPOSED_LAYERS  0x0008

/** @} */

/**
 * \brief Export a handle to a surface for use with an external API
 *
 * The exported handles are owned by the caller, and the caller is
 * responsible for freeing them when no longer needed (e.g. by closing
 * DRM PRIME file descriptors).
 *
 * This does not perform any synchronisation.  If the contents of the
 * surface will be read, vastSyncSurface() must be called before doing so.
 * If the contents of the surface are written, then all operations must
 * be completed externally before using the surface again by via VA-API
 * functions.
 *
 * @param[in] dpy          VA display.
 * @param[in] surface_id   Surface to export.
 * @param[in] mem_type     Memory type to export to.
 * @param[in] flags        Combination of flags to apply
 *   (VA_EXPORT_SURFACE_*).
 * @param[out] descriptor  Pointer to the descriptor structure to fill
 *   with the handle details.  The type of this structure depends on
 *   the value of mem_type.
 *
 * @return Status code:
 * - VAST_STATUS_SUCCESS:    Success.
 * - VAST_STATUS_ERROR_INVALID_DISPLAY:  The display is not valid.
 * - VAST_STATUS_ERROR_UNIMPLEMENTED:  The driver does not implement
 *     this interface.
 * - VAST_STATUS_ERROR_INVALID_SURFACE:  The surface is not valid, or
 *     the surface is not exportable in the specified way.
 * - VAST_STATUS_ERROR_UNSUPPORTED_MEMORY_TYPE:  The driver does not
 *     support exporting surfaces to the specified memory type.
 */
VASTStatus vastExportSurfaceHandle(VASTDisplay dpy,
                               VASTSurfaceID surface_id,
                               uint32_t mem_type, uint32_t flags,
                               void *descriptor);

typedef enum
{
    VASTSurfaceRendering	= 1, /* Rendering in progress */ 
    VASTSurfaceDisplaying	= 2, /* Displaying in progress (not safe to render into it) */ 
                             /* this status is useful if surface is used as the source */
                             /* of an overlay */
    VASTSurfaceReady	= 4, /* not being rendered or displayed */
    VASTSurfaceSkipped	= 8  /* Indicate a skipped frame during encode */
} VASTSurfaceStatus;

/**
 * Find out any pending ops on the render target 
 */
VASTStatus vastQuerySurfaceStatus (
    VASTDisplay dpy,
    VASTSurfaceID render_target,
    VASTSurfaceStatus *status	/* out */
);

typedef enum
{
    VASTDecodeSliceMissing            = 0,
    VASTDecodeMBError                 = 1,
} VASTDecodeErrorType;

/**
 * Client calls vastQuerySurfaceError with VAST_STATUS_ERROR_DECODING_ERROR, server side returns
 * an array of structure VASTSurfaceDecodeMBErrors, and the array is terminated by setting status=-1
*/
typedef struct _VASTSurfaceDecodeMBErrors
{
    int32_t status; /* 1 if hardware has returned detailed info below, -1 means this record is invalid */
    uint32_t start_mb; /* start mb address with errors */
    uint32_t end_mb;  /* end mb address with errors */
    VASTDecodeErrorType decode_error_type;
    uint32_t num_mb;   /* number of mbs with errors */
    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW - 1];
} VASTSurfaceDecodeMBErrors;

/**
 * After the application gets VAST_STATUS_ERROR_DECODING_ERROR after calling vastSyncSurface(),
 * it can call vastQuerySurfaceError to find out further details on the particular error.
 * VAST_STATUS_ERROR_DECODING_ERROR should be passed in as "error_status",
 * upon the return, error_info will point to an array of _VASurfaceDecodeMBErrors structure,
 * which is allocated and filled by libVA with detailed information on the missing or error macroblocks.
 * The array is terminated if "status==-1" is detected.
 */
VASTStatus vastQuerySurfaceError(
    VASTDisplay dpy,
    VASTSurfaceID surface,
    VASTStatus error_status,
    void **error_info
);

/**
 * Images and Subpictures
 * VASTImage is used to either get the surface data to client memory, or 
 * to copy image data in client memory to a surface. 
 * Both images, subpictures and surfaces follow the same 2D coordinate system where origin 
 * is at the upper left corner with positive X to the right and positive Y down
 */
#define VAST_FOURCC(ch0, ch1, ch2, ch3) \
    ((unsigned long)(unsigned char) (ch0) | ((unsigned long)(unsigned char) (ch1) << 8) | \
    ((unsigned long)(unsigned char) (ch2) << 16) | ((unsigned long)(unsigned char) (ch3) << 24 ))

/* byte order */
#define VAST_LSB_FIRST		1
#define VAST_MSB_FIRST		2

VASTStatus vastSetImagePalette (
    VASTDisplay dpy,
    VASTImageID image,
    /* 
     * pointer to an array holding the palette data.  The size of the array is 
     * num_palette_entries * entry_bytes in size.  The order of the components 
     * in the palette is described by the component_order in VASTImage struct    
     */
    unsigned char *palette 
);

/** Get maximum number of subpicture formats supported by the implementation */
int vastMaxNumSubpictureFormats (
    VASTDisplay dpy
);

/** flags for subpictures */
#define VAST_SUBPICTURE_CHROMA_KEYING			0x0001
#define VAST_SUBPICTURE_GLOBAL_ALPHA			0x0002
#define VAST_SUBPICTURE_DESTINATION_IS_SCREEN_COORD	0x0004
/**
 * Query supported subpicture formats 
 * The caller must provide a "format_list" array that can hold at
 * least vastMaxNumSubpictureFormats() entries. The flags arrary holds the flag 
 * for each format to indicate additional capabilities for that format. The actual 
 * number of formats returned in "format_list" is returned in "num_formats".
 *  flags: returned value to indicate addtional capabilities
 *         VAST_SUBPICTURE_CHROMA_KEYING - supports chroma-keying
 *         VAST_SUBPICTURE_GLOBAL_ALPHA - supports global alpha
 * 	   VAST_SUBPICTURE_DESTINATION_IS_SCREEN_COORD - supports unscaled screen relative subpictures for On Screen Display
 */

VASTStatus vastQuerySubpictureFormats (
    VASTDisplay dpy,
    VASTImageFormat *format_list,	/* out */
    unsigned int *flags,	/* out */
    unsigned int *num_formats	/* out */
);

/**
 * Subpictures are created with an image associated. 
 */
VASTStatus vastCreateSubpicture (
    VASTDisplay dpy,
    VASTImageID image,
    VASTSubpictureID *subpicture	/* out */
);

/**
 * Destroy the subpicture before destroying the image it is assocated to
 */
VASTStatus vastDestroySubpicture (
    VASTDisplay dpy,
    VASTSubpictureID subpicture
);

/**
 * Bind an image to the subpicture. This image will now be associated with 
 * the subpicture instead of the one at creation.
 */
VASTStatus vastSetSubpictureImage (
    VASTDisplay dpy,
    VASTSubpictureID subpicture,
    VASTImageID image
);

/**
 * If chromakey is enabled, then the area where the source value falls within
 * the chromakey [min, max] range is transparent
 * The chromakey component format is the following:
 *  For RGB: [0:7] Red [8:15] Blue [16:23] Green   
 *  For YUV: [0:7] V [8:15] U [16:23] Y
 * The chromakey mask can be used to mask out certain components for chromakey
 * comparision
 */
VASTStatus vastSetSubpictureChromakey (
    VASTDisplay dpy,
    VASTSubpictureID subpicture,
    unsigned int chromakey_min,
    unsigned int chromakey_max,
    unsigned int chromakey_mask
);

/**
 * Global alpha value is between 0 and 1. A value of 1 means fully opaque and 
 * a value of 0 means fully transparent. If per-pixel alpha is also specified then
 * the overall alpha is per-pixel alpha multiplied by the global alpha
 */
VASTStatus vastSetSubpictureGlobalAlpha (
    VASTDisplay dpy,
    VASTSubpictureID subpicture,
    float global_alpha 
);

/**
 * vastAssociateSubpicture associates the subpicture with target_surfaces.
 * It defines the region mapping between the subpicture and the target  
 * surfaces through source and destination rectangles (with the same width and height).
 * Both will be displayed at the next call to vastPutSurface.  Additional
 * associations before the call to vastPutSurface simply overrides the association.
 */
VASTStatus vastAssociateSubpicture (
    VASTDisplay dpy,
    VASTSubpictureID subpicture,
    VASTSurfaceID *target_surfaces,
    int num_surfaces,
    int16_t src_x, /* upper left offset in subpicture */
    int16_t src_y,
    uint16_t src_width,
    uint16_t src_height,
    int16_t dest_x, /* upper left offset in surface */
    int16_t dest_y,
    uint16_t dest_width,
    uint16_t dest_height,
    /*
     * whether to enable chroma-keying, global-alpha, or screen relative mode
     * see VA_SUBPICTURE_XXX values
     */
    uint32_t flags
);

/**
 * vastDeassociateSubpicture removes the association of the subpicture with target_surfaces.
 */
VASTStatus vastDeassociateSubpicture (
    VASTDisplay dpy,
    VASTSubpictureID subpicture,
    VASTSurfaceID *target_surfaces,
    int num_surfaces
);

/**
 * Display attributes
 * Display attributes are used to control things such as contrast, hue, saturation,
 * brightness etc. in the rendering process.  The application can query what
 * attributes are supported by the driver, and then set the appropriate attributes
 * before calling vastPutSurface()
 */
/* PowerVR IEP Lite attributes */
typedef enum
{
    VASTDISPLAYATTRIB_BLE_OFF              = 0x00,
    VASTDISPLAYATTRIB_BLE_LOW,
    VASTDISPLAYATTRIB_BLE_MEDIUM,
    VASTDISPLAYATTRIB_BLE_HIGH,
    VASTDISPLAYATTRIB_BLE_NONE,
} VASTDisplayAttribBLEMode;

/** attribute value for VADisplayAttribRotation   */
#define VAST_ROTATION_NONE        0x00000000
#define VAST_ROTATION_90          0x00000001
#define VAST_ROTATION_180         0x00000002
#define VAST_ROTATION_270         0x00000003
/**@}*/

/**
 * @name Mirroring directions
 *
 * Those values could be used for VADisplayAttribMirror attribute or
 * VASTProcPipelineParameterBuffer::mirror_state.

 */
/**@{*/
/** \brief No Mirroring. */
#define VAST_MIRROR_NONE              0x00000000
/** \brief Horizontal Mirroring. */
#define VAST_MIRROR_HORIZONTAL        0x00000001
/** \brief Vertical Mirroring. */
#define VAST_MIRROR_VERTICAL          0x00000002
/**@}*/

/** attribute value for VADisplayAttribOutOfLoopDeblock */
#define VAST_OOL_DEBLOCKING_FALSE 0x00000000
#define VAST_OOL_DEBLOCKING_TRUE  0x00000001

/** Render mode */
#define VAST_RENDER_MODE_UNDEFINED           0
#define VAST_RENDER_MODE_LOCAL_OVERLAY       1
#define VAST_RENDER_MODE_LOCAL_GPU           2
#define VAST_RENDER_MODE_EXTERNAL_OVERLAY    4
#define VAST_RENDER_MODE_EXTERNAL_GPU        8

/** Render device */
#define VAST_RENDER_DEVICE_UNDEFINED  0
#define VAST_RENDER_DEVICE_LOCAL      1
#define VAST_RENDER_DEVICE_EXTERNAL   2

/** Currently defined display attribute types */
typedef enum
{
    VASTDisplayAttribBrightness		= 0,
    VASTDisplayAttribContrast		= 1,
    VASTDisplayAttribHue			= 2,
    VASTDisplayAttribSaturation		= 3,
    /* client can specifiy a background color for the target window
     * the new feature of video conference,
     * the uncovered area of the surface is filled by this color
     * also it will blend with the decoded video color
     */
    VASTDisplayAttribBackgroundColor      = 4,
    /*
     * this is a gettable only attribute. For some implementations that use the
     * hardware overlay, after PutSurface is called, the surface can not be    
     * re-used until after the subsequent PutSurface call. If this is the case 
     * then the value for this attribute will be set to 1 so that the client   
     * will not attempt to re-use the surface right after returning from a call
     * to PutSurface.
     *
     * Don't use it, use flag VASTSurfaceDisplaying of vastQuerySurfaceStatus since
     * driver may use overlay or GPU alternatively
     */
    VASTDisplayAttribDirectSurface       = 5,
    VASTDisplayAttribRotation            = 6,	
    VASTDisplayAttribOutofLoopDeblock    = 7,

    /* PowerVR IEP Lite specific attributes */
    VASTDisplayAttribBLEBlackMode        = 8,
    VASTDisplayAttribBLEWhiteMode        = 9,
    VASTDisplayAttribBlueStretch         = 10,
    VASTDisplayAttribSkinColorCorrection = 11,
    /*
     * For type VADisplayAttribCSCMatrix, "value" field is a pointer to the color
     * conversion matrix. Each element in the matrix is float-point
     */
    VASTDisplayAttribCSCMatrix           = 12,
    /* specify the constant color used to blend with video surface
     * Cd = Cv*Cc*Ac + Cb *(1 - Ac) C means the constant RGB
     *      d: the final color to overwrite into the frame buffer 
     *      v: decoded video after color conversion, 
     *      c: video color specified by VADisplayAttribBlendColor
     *      b: background color of the drawable
     */
    VASTDisplayAttribBlendColor          = 13,
    /*
     * Indicate driver to skip painting color key or not.
     * only applicable if the render is overlay
     */
    VASTDisplayAttribOverlayAutoPaintColorKey   = 14,
    /*
     * customized overlay color key, the format is RGB888
     * [23:16] = Red, [15:08] = Green, [07:00] = Blue.
     */
    VASTDisplayAttribOverlayColorKey	= 15,
    /*
     * The hint for the implementation of vastPutSurface
     * normally, the driver could use an overlay or GPU to render the surface on the screen
     * this flag provides APP the flexibity to switch the render dynamically
     */
    VASTDisplayAttribRenderMode           = 16,
    /*
     * specify if vastPutSurface needs to render into specified monitors
     * one example is that one external monitor (e.g. HDMI) is enabled, 
     * but the window manager is not aware of it, and there is no associated drawable
     */
    VASTDisplayAttribRenderDevice        = 17,
    /*
     * specify vastPutSurface render area if there is no drawable on the monitor
     */
    VASTDisplayAttribRenderRect          = 18,
} VASTDisplayAttribType;

/* flags for VASTDisplayAttribute */
#define VAST_DISPLAY_ATTRIB_NOT_SUPPORTED	0x0000
#define VAST_DISPLAY_ATTRIB_GETTABLE	0x0001
#define VAST_DISPLAY_ATTRIB_SETTABLE	0x0002

typedef struct _VASTDisplayAttribute
{
    VASTDisplayAttribType type;
    int32_t min_value;
    int32_t max_value;
    int32_t value;	/* used by the set/get attribute functions */
/* flags can be VA_DISPLAY_ATTRIB_GETTABLE or VA_DISPLAY_ATTRIB_SETTABLE or OR'd together */
    uint32_t flags;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_LOW];
} VASTDisplayAttribute;

/** Get maximum number of display attributs supported by the implementation */
int vastMaxNumDisplayAttributes (
    VASTDisplay dpy
);

/**
 * Query display attributes 
 * The caller must provide a "attr_list" array that can hold at
 * least vastMaxNumDisplayAttributes() entries. The actual number of attributes
 * returned in "attr_list" is returned in "num_attributes".
 */
VASTStatus vastQueryDisplayAttributes (
    VASTDisplay dpy,
    VASTDisplayAttribute *attr_list,	/* out */
    int *num_attributes			/* out */
);

/**
 * Get display attributes 
 * This function returns the current attribute values in "attr_list".
 * Only attributes returned with VA_DISPLAY_ATTRIB_GETTABLE set in the "flags" field
 * from vastQueryDisplayAttributes() can have their values retrieved.  
 */
VASTStatus vastGetDisplayAttributes (
    VASTDisplay dpy,
    VASTDisplayAttribute *attr_list,	/* in/out */
    int num_attributes
);

/**
 * Set display attributes 
 * Only attributes returned with VA_DISPLAY_ATTRIB_SETTABLE set in the "flags" field
 * from vastQueryDisplayAttributes() can be set.  If the attribute is not settable or 
 * the value is out of range, the function returns VAST_STATUS_ERROR_ATTR_NOT_SUPPORTED
 */
VASTStatus vastSetDisplayAttributes (
    VASTDisplay dpy,
    VASTDisplayAttribute *attr_list,
    int num_attributes
);

/*Initialize the library*/
VASTStatus vastInitialize (VASTDisplay dpy);

/* After this call, all library internal resources will be cleaned up*/ 
VASTStatus vastTerminate (VASTDisplay dpy);

VASTDisplay vastGetDisplayDRM(int fd);


VASTStatus vastQueryIdleDie(
    void * cap_buf,
    int fd,
    int multi_load
);

VASTStatus vastDmaTransExternalBuf(
    VASTDisplay dpy,
    int fd,
    int die_id,
    uint64_t dmabuffer_addr,
    uint64_t axi_addr,
    unsigned int dmabuff_size,
    int src_type,
    int is_dev_to_host
);

VASTStatus vastCreateDmaHandle(
    VASTDisplay dpy,
    int die_id /* in */,
    unsigned int dmabuff_size,
    void *dma_handle
);

VASTStatus vastGetDieinfo (
    VASTDisplay dpy,
    int* die_id	/* out */
);

VASTStatus vastGetSurfaceIdAddress (
    VASTDisplay dpy,
    VASTSurfaceID surface,
    uint64_t *address	/* out */
);

VASTStatus vastSetSurfaceIdAddress (
    VASTDisplay dpy,
    VASTSurfaceID surface,
    uint64_t address	/* out */
);

/**
 * Copy data from a VASTImage to a surface
 * Image must be in a format supported by the implementation
 * Returns a VAST_STATUS_ERROR_SURFACE_BUSY if the surface
 * shouldn't be rendered into when this is called
 */
VASTStatus vastPutImage (
    VASTDisplay dpy,
    VASTSurfaceID surface,
    VASTImageID image,
    int src_x,
    int src_y,
    unsigned int src_width,
    unsigned int src_height,
    int dest_x,
    int dest_y,
    unsigned int dest_width,
    unsigned int dest_height
);

/**
 * Retrive surface data into a VASTImage
 * Image must be in a format supported by the implementation
 */
VASTStatus vastGetImage (
    VASTDisplay dpy,
    VASTSurfaceID surface,
    int x,	/* coordinates of the upper left source pixel */
    int y,
    unsigned int width, /* width and height of the region */
    unsigned int height,
    VASTImageID image
);

/**
 * Query supported image formats 
 * The caller must provide a "format_list" array that can hold at
 * least vastMaxNumImageFormats() entries. The actual number of formats
 * returned in "format_list" is returned in "num_formats".
 */
VASTStatus vastQueryImageFormats (
    VASTDisplay dpy,
    VASTImageFormat *format_list,	/* out */
    int *num_formats		/* out */
);

/** Get maximum number of image formats supported by the implementation */
int vastMaxNumImageFormats (VASTDisplay dpy);

/**
 * Get ready for a video pipeline
 * - decode a picture to a target surface
 * - encode a picture from a target surface
 * - process a picture to a target surface
 */
VASTStatus vastBeginPicture (
    VASTDisplay dpy,
    VASTContextID context,
    VASTSurfaceID render_target
);

/**
 * Send video decode, encode or processing buffers to the server.
 */
VASTStatus vastRenderPicture (
    VASTDisplay dpy,
    VASTContextID context,
    VASTBufferID *buffers,
    int num_buffers
);

/**
 * Make the end of rendering for a picture. 
 * The server should start processing all pending operations for this 
 * surface. This call is non-blocking. The client can start another 
 * Begin/Render/End sequence on a different render target.
 * if VASTContextID used in this function previously successfully passed
 * vastMFAddContext call, real processing will be started during vastMFSubmit
 */
VASTStatus vastEndPicture (VASTDisplay dpy, VASTContextID context);

VASTStatus vastEncodeHead (VASTDisplay dpy, VASTContextID context);

VASTStatus vastSyncSurfaceFlush (VASTDisplay dpy, VASTSurfaceID render_target);

/**
 * Convey to the server how many valid elements are in the buffer. 
 * e.g. if multiple slice parameters are being held in a single buffer,
 * this will communicate to the server the number of slice parameters
 * that are valid in the buffer.
 */
VASTStatus vastBufferSetNumElements (
    VASTDisplay dpy,
    VASTBufferID buf_id,	/* in */
    unsigned int num_elements /* in */
);

/**
 * vastDestroySurfaces - Destroy resources associated with surfaces. 
 *  Surfaces can only be destroyed after all contexts using these surfaces have been
 *  destroyed.  
 *  dpy: display
 *  surfaces: array of surfaces to destroy
 *  num_surfaces: number of surfaces in the array to be destroyed.
 */
VASTStatus vastDestroySurfaces (
    VASTDisplay dpy,
    VASTSurfaceID *surfaces,
    int num_surfaces
);

/**
 * \brief Queries surface attributes for the supplied config.
 *
 * This function queries for all supported attributes for the
 * supplied VA @config. In particular, if the underlying hardware
 * supports the creation of VA surfaces in various formats, then
 * this function will enumerate all pixel formats that are supported.
 *
 * The \c attrib_list array is allocated by the user and \c
 * num_attribs shall be initialized to the number of allocated
 * elements in that array. Upon successful return, the actual number
 * of attributes will be overwritten into \c num_attribs. Otherwise,
 * \c VAST_STATUS_ERROR_MAX_NUM_EXCEEDED is returned and \c num_attribs
 * is adjusted to the number of elements that would be returned if
 * enough space was available.
 *
 * Note: it is perfectly valid to pass NULL to the \c attrib_list
 * argument when vastQuerySurfaceAttributes() is used to determine the
 * actual number of elements that need to be allocated.
 *
 * @param[in] dpy               the VA display
 * @param[in] config            the config identifying a codec or a video
 *     processing pipeline
 * @param[out] attrib_list      the output array of #VASTSurfaceAttrib elements
 * @param[in,out] num_attribs   the number of elements allocated on
 *      input, the number of elements actually filled in output
 */
VASTStatus vastQuerySurfaceAttributes(
    VASTDisplay           dpy,
    VASTConfigID          config,
    VASTSurfaceAttrib    *attrib_list,
    unsigned int       *num_attribs
);

/**
 * Query all attributes for a given configuration 
 * The profile of the configuration is returned in "profile"
 * The entrypoint of the configuration is returned in "entrypoint"
 * The caller must provide an "attrib_list" array that can hold at least 
 * vastMaxNumConfigAttributes() entries. The actual number of attributes 
 * returned in "attrib_list" is returned in "num_attribs"
 */
VASTStatus vastQueryConfigAttributes (
    VASTDisplay dpy,
    VASTConfigID config_id, 
    VASTProfile *profile, 	/* out */
    VASTEntrypoint *entrypoint, 	/* out */
    VASTConfigAttrib *attrib_list,/* out */
    int *num_attribs 		/* out */
);

/**
 * Create a configuration for the video decode/encode/processing pipeline
 * it passes in the attribute list that specifies the attributes it cares 
 * about, with the rest taking default values.  
 */
VASTStatus vastCreateConfig (
                VASTDisplay dpy,
                VASTProfile profile, 
                VASTEntrypoint entrypoint, 
                VASTConfigAttrib *attrib_list,
                int num_attribs,
                VASTConfigID *config_id /* out */
);

/**
 * Get attributes for a given profile/entrypoint pair 
 * The caller must provide an "attrib_list" with all attributes to be 
 * retrieved.  Upon return, the attributes in "attrib_list" have been 
 * updated with their value.  Unknown attributes or attributes that are 
 * not supported for the given profile/entrypoint pair will have their 
 * value set to VAST_ATTRIB_NOT_SUPPORTED
 */
VASTStatus vastGetConfigAttributes (
                VASTDisplay dpy,
                VASTProfile profile,
                VASTEntrypoint entrypoint,
                VASTConfigAttrib *attrib_list, /* in/out */
                int num_attribs
);

/**
 * Query supported entrypoints for a given profile 
 * The caller must provide an "entrypoint_list" array that can hold at
 * least vastMaxNumEntrypoints() entries. The actual number of entrypoints 
 * returned in "entrypoint_list" is returned in "num_entrypoints".
 */
VASTStatus vastQueryConfigEntrypoints (
                VASTDisplay dpy,
                VASTProfile profile,
                VASTEntrypoint *entrypoint_list,/* out */
                int *num_entrypoints/* out */
);

/**
 * Query supported profiles 
 * The caller must provide a "profile_list" array that can hold at
 * least vaMaxNumProfile() entries. The actual number of profiles
 * returned in "profile_list" is returned in "num_profile".
 */
VASTStatus vastQueryConfigProfiles (
                VASTDisplay dpy,
                VASTProfile *profile_list,/* out */
                int *num_profiles/* out */
);

/** Get maximum number of profiles supported by the implementation */
int vastMaxNumProfiles (VASTDisplay dpy);

/** Get maximum number of entrypoints supported by the implementation */
int vastMaxNumEntrypoints (VASTDisplay dpy);

/** Get maximum number of attributs supported by the implementation */
int vastMaxNumConfigAttributes (VASTDisplay dpy);

VASTStatus vaQueWriteDmaBufSg(
        VASTDisplay dpy,
        void *channel,
        int channel_num,
        uint64_t axi_addr,
        unsigned int die_index
);  

/**
 * vastQueryVendorString returns a pointer to a zero-terminated string
 * describing some aspects of the VA implemenation on a specific    
 * hardware accelerator. The format of the returned string is vendor
 * specific and at the discretion of the implementer.
 * e.g. for the Intel GMA500 implementation, an example would be:
 * "Intel GMA500 - 2.0.0.32L.0005"
 */
const char *vastQueryVendorString (VASTDisplay dpy);

/* Free resources associdated with a given config */
VASTStatus vastDestroyConfig (VASTDisplay dpy, VASTConfigID config_id);

/**
 * \brief Creates an array of surfaces
 *
 * Creates an array of surfaces. The optional list of attributes shall
 * be constructed based on what the underlying hardware could expose
 * through vastQuerySurfaceAttributes().
 *
 * @param[in] dpy               the VA display
 * @param[in] format            the desired surface format. See \c VA_RT_FORMAT_*
 * @param[in] width             the surface width
 * @param[in] height            the surface height
 * @param[out] surfaces         the array of newly created surfaces
 * @param[in] num_surfaces      the number of surfaces to create
 * @param[in] attrib_list       the list of (optional) attributes, or \c NULL
 * @param[in] num_attribs       the number of attributes supplied in
 *     \c attrib_list, or zero
 */
VASTStatus vastCreateSurfaces(
    VASTDisplay           dpy,
    unsigned int        format,
    unsigned int        width,
    unsigned int        height,
    VASTSurfaceID        *surfaces,
    unsigned int        num_surfaces,
    VASTSurfaceAttrib    *attrib_list,
    unsigned int        num_attribs
);

/**
 * vastCreateContext - Create a context
 *  dpy: display
 *  config_id: configuration for the context
 *  picture_width: coded picture width
 *  picture_height: coded picture height
 *  flag: any combination of the following:
 *    VA_PROGRESSIVE (only progressive frame pictures in the sequence when set)
 *  render_targets: a hint for render targets (surfaces) tied to the context
 *  num_render_targets: number of render targets in the above array
 *  context: created context id upon return
 */
VASTStatus vastCreateContext (
            VASTDisplay dpy,
            VASTConfigID config_id,
            int picture_width,
            int picture_height,
            int flag,
            VASTSurfaceID *render_targets,
            int num_render_targets,
            VASTContextID *context		/* out */
);

/**
 * vastDestroyContext - Destroy a context 
 *  dpy: display
 *  context: context to be destroyed
 */
VASTStatus vastDestroyContext (
            VASTDisplay dpy,
            VASTContextID context
);


/**
 * Creates a buffer for "num_elements" elements of "size" bytes and 
 * initalize with "data".
 * if "data" is null, then the contents of the buffer data store
 * are undefined.
 * Basically there are two ways to get buffer data to the server side. One is 
 * to call vastCreateBuffer() with a non-null "data", which results the data being
 * copied to the data store on the server side.  A different method that 
 * eliminates this copy is to pass null as "data" when calling vastCreateBuffer(),
 * and then use vastMapBuffer() to map the data store from the server side to the
 * client address space for access.
 * The user must call vastDestroyBuffer() to destroy a buffer.
 *  Note: image buffers are created by the library, not the client. Please see 
 *        vastCreateImage on how image buffers are managed.
 */
VASTStatus vastCreateBuffer (
            VASTDisplay dpy,
            VASTContextID context,
            VASTBufferType type,	/* in */
            unsigned int size,	/* in */
            unsigned int num_elements, /* in */
            void *data,		/* in */
            VASTBufferID *buf_id	/* out */
);


/**
 * Create a buffer for given width & height get unit_size, pitch, buf_id for 2D buffer
 * for permb qp buffer, it will return unit_size for one MB or LCU and the pitch for alignments
 * can call vastMapBuffer with this Buffer ID to get virtual address.
 * e.g. AVC 1080P encode, 1920x1088, the size in MB is 120x68,but inside driver,
 * maybe it should align with 256, and one byte present one Qp.so, call the function.
 * then get unit_size = 1, pitch = 256. call vastMapBuffer to get the virtual address (pBuf).
 * then read write the memory like 2D. the size is 256x68, application can only use 120x68
 * pBuf + 256 is the start of next line.
 * different driver implementation maybe return different unit_size and pitch
 */
VASTStatus vastCreateBuffer2(
    VASTDisplay dpy,
    VASTContextID context,
    VASTBufferType type,
    unsigned int width,
    unsigned int height,
    unsigned int *unit_size,
    unsigned int *pitch,
    VASTBufferID *buf_id
);

/**
 * Map data store of the buffer into the client's address space
 * vastCreateBuffer() needs to be called with "data" set to NULL before
 * calling vastMapBuffer()
 *
 * if buffer type is VASTEncCodedBufferType, pbuf points to link-list of
 * VASTCodedBufferSegment, and the list is terminated if "next" is NULL
 */
VASTStatus vastMapBuffer (
    VASTDisplay dpy,
    VASTBufferID buf_id,	/* in */
    void **pbuf 	/* out */
);

/**
 * After client making changes to a mapped data store, it needs to
 * "Unmap" it to let the server know that the data is ready to be
 * consumed by the server
 */
VASTStatus vastUnmapBuffer (VASTDisplay dpy, VASTBufferID buf_id);

/**
 * After this call, the buffer is deleted and this buffer_id is no longer valid
 *
 * A buffer can be re-used and sent to the server by another Begin/Render/End
 * sequence if vastDestroyBuffer() is not called with this buffer.
 *
 * Note re-using a shared buffer (e.g. a slice data buffer) between the host and the
 * hardware accelerator can result in performance dropping.
 */
VASTStatus vastDestroyBuffer (VASTDisplay dpy, VASTBufferID buffer_id);

/**
 * This function blocks until all pending operations on the render target
 * have been completed.  Upon return it is safe to use the render target for a 
 * different picture. 
 */
VASTStatus vastSyncSurface (VASTDisplay dpy, VASTSurfaceID render_target);

/**
 * Create a VASTImage structure
 * The width and height fields returned in the VASTImage structure may get 
 * enlarged for some YUV formats. Upon return from this function, 
 * image->buf has been created and proper storage allocated by the library. 
 * The client can access the image through the Map/Unmap calls.
 */
VASTStatus vastCreateImage (
    VASTDisplay dpy,
    VASTImageFormat *format,
    int width,
    int height,
    VASTImage *image	/* out */
);

/**
 * Should call DestroyImage before destroying the surface it is bound to
 */
VASTStatus vastDestroyImage (VASTDisplay dpy, VASTImageID image);

VASTStatus vastDeriveImage (
    VASTDisplay dpy,
    VASTSurfaceID surface,
    VASTImage *image	/* out */
);

VASTStatus vastDestroyDmaHandle(
    VASTDisplay dpy,
    void *dma_handle
);    

VASTStatus vastDmaWriteBuf(
    VASTDisplay dpy,
    uint64_t dst_soc_addr,
    int buf_size,
    void *dma_handle
);  

VASTStatus vastDmaReadBuf(
    VASTDisplay dpy,
    uint64_t src_soc_addr,
    int buf_size,
    void *dma_handle
);

VASTStatus vastDeviceMemcpy(VASTDisplay dpy,
                            uint32_t dev_id, 
                            const void *addr_from, 
                            size_t size, 
                            void *addr_to,
                            int direction,
                            void *dma_handle
);


VASTStatus vastQueWriteDmaBufSg(
    VASTDisplay dpy,
    void *channel,
    int channel_num,
    uint64_t axi_addr,
    unsigned int die_index
);


#define DEFAULT_VAST -255
#define MAX_TEMPORAL_LAYERS_VAST         4
#define MAX_SCENE_CHANGE_VAST 20
#define HANTRO_MAX_NUM_ROI_REGIONS_VAST                     8
#define HANTRO_MAX_NUM_IPCM_REGIONS_VAST                    8
#define HANTRO_MAX_NUM_INTRA_AREA_REGIONS_VAST              1
#define HANTRO_MAX_SUBPIC_SUM_VAST                     8


typedef enum {
    VA_SINGLE_CORE_MODE,    //one stream encode on one encoder core
    VA_MULTI_CORE_MODE,     //one stream encode on multi encoder core
    VA_WORK_MODE_MAX,
}va_encoder_workmode_t;

#define VA_PICTURE_AV1_INVALID                  0x00000001


/**@}*/

#ifdef __cplusplus
}
#endif

#endif /* _VA_H_ */
