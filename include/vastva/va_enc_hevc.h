/*
 * Copyright (c) 2007-2014 Intel Corporation. All Rights Reserved.
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

/**
 * \file va_enc_hevc.h
 * \brief The HEVC encoding API
 *
 * This file contains the \ref api_enc_hevc "HEVC encoding API".
 *
 */

#ifndef VAST_ENC_HEVCAI_H
#define VAST_ENC_HEVCAI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * \defgroup api_enc_hevc HEVC encoding API
 *
 * @{
 */

/**
 * @name Picture flags
 *
 * Those flags flags are meant to signal when a picture marks the end
 * of a sequence, a stream, or even both at once.
 *
 * @{
 */
/**
 * \brief Marks the last picture in the sequence.
 *
 * i.e. the driver appends \c end_of_seq() NAL unit to the encoded frame.
 */
#define HEVC_LAST_PICTURE_EOSEQ_VAST     0x01
/**
 * \brief Marks the last picture in the stream.
 *
 * i.e. the driver appends \c end_of_stream() NAL unit to the encoded frame.
 */
#define HEVC_LAST_PICTURE_EOSTREAM_VAST  0x02
/**@}*/

/**
 * \brief Packed header types specific to HEVC encoding.
 *
 * Types of packed headers generally used for HEVC encoding. Each
 * associated packed header data buffer shall contain the start code
 * prefix 0x000001 followed by the complete NAL unit, thus also
 * including the \c nal_unit_type.
 *
 * Note: the start code prefix can contain an arbitrary number of leading
 * zeros. The driver will skip them for emulation prevention bytes insertion,
 * if necessary.
 */
typedef enum {
    /**
     * \brief Packed Video Parameter Set (VPS).
     *
     * The corresponding packed header data buffer shall contain the
     * complete video_parameter_set_rbsp() syntax element.
     *
     * Note: packed \c nal_unit_type shall be equal to 32.
     */
    VASTEncPackedHeaderHEVC_VPS   = VASTEncPackedHeaderSequence,
    /**
     * \brief Packed Sequence Parameter Set (SPS).
     *
     * The corresponding packed header data buffer shall contain the
     * complete seq_parameter_set_rbsp() syntax element.
     *
     * Note: packed \c nal_unit_type shall be equal to 33.
     */
    VASTEncPackedHeaderHEVC_SPS   = VASTEncPackedHeaderSequence,
    /**
     * \brief Packed Picture Parameter Set (PPS).
     *
     * The corresponding packed header data buffer shall contain the
     * complete pic_parameter_set_rbsp() syntax element.
     *
     * Note: packed \c nal_unit_type shall be equal to 34.
     */
    VASTEncPackedHeaderHEVC_PPS   = VASTEncPackedHeaderPicture,
    /**
     * \brief Packed slice header.
     *
     * The corresponding packed header data buffer shall contain the
     * \c slice_header() syntax element only, along with any start
     * code prefix and NAL unit type preceeding it. i.e. this means
     * that the buffer does not contain any of the \c slice_data() or
     * the \c rbsp_slice_trailing_bits().
     *
     * Note: packed \c nal_unit_type shall be equal to 0 to 9 (non-IRAP
     * picture), or 16 to 21 (IRAP picture).
     */
    VASTEncPackedHeaderHEVC_Slice = VASTEncPackedHeaderSlice,
    /**
     * \brief Packed Supplemental Enhancement Information (SEI).
     *
     * The corresponding packed header data buffer shall contain the
     * complete sei_rbsp() syntax element, thus including several
     * sei_message() elements if necessary.
     *
     * Note: packed \c nal_unit_type shall be equal to 39 or 40.
     *
     * @deprecated
     * This is a deprecated packed header flag, All applications can use
     * \c VAST_ENC_PACKED_HEADER_RAW_DATA to pass the corresponding packed
     * SEI header data buffer to the driver
     */
    VASTEncPackedHeaderHEVC_SEI = (0x80000000 | 1),
} VASTEncPackedHeaderTypeHEVC;

/**
 * \brief Sequence parameter for HEVC encoding in main & main 10
 * profiles.
 *
 * This structure holds information for \c seq_parameter_set_data() as
 * defined by the HEVC specification.
 *
 * If packed sequence headers mode is used, i.e. if the encoding
 * pipeline was configured with the #VAST_ENC_PACKED_HEADER_SEQUENCE
 * flag, then the driver expects two more buffers to be provided to
 * the same \c vastRenderPicture() as this buffer:
 * - a #VASTEncPackedHeaderParameterBuffer with type set to
 *   VASTEncPackedHeaderType::VASTEncPackedHeaderSequence ;
 * - a #VAEncPackedHeaderDataBuffer which holds the actual packed
 *   header data.
 *
 * If \c seq_scaling_matrix_present_flag is set to \c 1, then a
 * #VASTQMatrixBufferHEVC buffer shall also be provided within the same
 * \c vastRenderPicture() call as this sequence parameter buffer.
 */
typedef struct _VASTEncSequenceParameterBufferHEVC {
    /** \brief Same as the HEVC bitstream syntax element.
     *  value range [1..2].
     */
    uint8_t     general_profile_idc;
    /** \brief Same as the HEVC bitstream syntax element.
     *  general_level_idc shall be set equal to a value of 30 times the level
     *  numbers allowed [1, 2, 2.1, 3, 3.1, 4, 4.1, 5, 5.1, 5.2, 6, 6.1, 6.2]
     */
    uint8_t     general_level_idc;
    /** \brief Same as the HEVC bitstream syntax element.
     *  Only value 0 is allowed for level value below 4, exclusive.
     */
    uint8_t     general_tier_flag;
    /** \brief Period between I frames. */
    uint32_t    intra_period;
    /** \brief Period between IDR frames. */
    uint32_t    intra_idr_period;
    /** \brief Period between I/P frames. */
    uint32_t    ip_period;
    /**
     * \brief Initial bitrate set for this sequence in CBR or VBR modes.
     *
     * This field represents the initial bitrate value for this
     * sequence if CBR or VBR mode is used, i.e. if the encoder
     * pipeline was created with a #VASTConfigAttribRateControl
     * attribute set to either \ref VAST_RC_CBR or \ref VAST_RC_VBR.
     *
     * The bitrate can be modified later on through
     * #VASTEncMiscParameterRateControl buffers.
     */
    uint32_t    bits_per_second;
    /** \brief Picture width in pixel samples.
     *  Its value must be multiple of min CU size.
     */
    uint16_t    pic_width_in_luma_samples;
    /** \brief Picture height in pixel samples.
     *  Its value must be multiple of min CU size.
     */
    uint16_t    pic_height_in_luma_samples;

    union {
        struct {
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    chroma_format_idc                              : 2;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    separate_colour_plane_flag                     : 1;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    bit_depth_luma_minus8                          : 3;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    bit_depth_chroma_minus8                        : 3;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    scaling_list_enabled_flag                      : 1;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    strong_intra_smoothing_enabled_flag            : 1;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    amp_enabled_flag                               : 1;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    sample_adaptive_offset_enabled_flag            : 1;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    pcm_enabled_flag                               : 1;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    pcm_loop_filter_disabled_flag                  : 1;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    sps_temporal_mvp_enabled_flag                  : 1;

            uint32_t    reserved_bits                                  : 16;
        } bits;
        uint32_t value;
    } seq_fields;

    /** \brief Same as the HEVC bitstream syntax element.
     *  value range [0..3]
     */
    uint8_t     log2_min_luma_coding_block_size_minus3;

    /** \brief Same as the HEVC bitstream syntax element.
     */
    uint8_t     log2_diff_max_min_luma_coding_block_size;

    /** \brief Same as the HEVC bitstream syntax element.
     *  value range [0..3]
     */
    uint8_t     log2_min_transform_block_size_minus2;

    /** \brief Same as the HEVC bitstream syntax element.
     */
    uint8_t     log2_diff_max_min_transform_block_size;

    /** \brief Same as the HEVC bitstream syntax element.
     *  value range [2]
     */
    uint8_t     max_transform_hierarchy_depth_inter;

    /** \brief Same as the HEVC bitstream syntax element.
     *  value range [2]
     */
    uint8_t     max_transform_hierarchy_depth_intra;

    /** \brief Same as the HEVC bitstream syntax element. */
    uint32_t    pcm_sample_bit_depth_luma_minus1;

    /** \brief Same as the HEVC bitstream syntax element. */
    uint32_t    pcm_sample_bit_depth_chroma_minus1;

    /** \brief Same as the HEVC bitstream syntax element. */
    uint32_t    log2_min_pcm_luma_coding_block_size_minus3;

    /** \brief Derived from the HEVC bitstream syntax element.
    *  log2_min_pcm_luma_coding_block_size_minus3 +
    *  log2_diff_max_min_pcm_luma_coding_block_size
    */
    uint32_t    log2_max_pcm_luma_coding_block_size_minus3;

    /** @name VUI parameters (optional) */
    /**@{*/
    /** \brief Same as the HEVC bitstream syntax element. */
    uint8_t     vui_parameters_present_flag;
    union {
        struct {
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    aspect_ratio_info_present_flag                 : 1;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    neutral_chroma_indication_flag                 : 1;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    field_seq_flag                                 : 1;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    vui_timing_info_present_flag                   : 1;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    bitstream_restriction_flag                     : 1;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    tiles_fixed_structure_flag                     : 1;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    motion_vectors_over_pic_boundaries_flag        : 1;
            /** \brief Same as the HEVC bitstream syntax element. */
            uint32_t    restricted_ref_pic_lists_flag                  : 1;
            /** \brief Range: 0 to 16, inclusive. */
            uint32_t    log2_max_mv_length_horizontal                  : 5;
            /** \brief Range: 0 to 16, inclusive. */
            uint32_t    log2_max_mv_length_vertical                    : 5;
        } bits;
        uint32_t value;
    } vui_fields;
    /** \brief Same as the HEVC bitstream syntax element. */
    uint8_t     aspect_ratio_idc;
    /** \brief Same as the HEVC bitstream syntax element. */
    uint32_t    sar_width;
    /** \brief Same as the HEVC bitstream syntax element. */
    uint32_t    sar_height;
    /** \brief Same as the HEVC bitstream syntax element. */
    uint32_t    vui_num_units_in_tick;
    /** \brief Same as the HEVC bitstream syntax element. */
    uint32_t    vui_time_scale;
    /** \brief Same as the HEVC bitstream syntax element. */
    uint16_t    min_spatial_segmentation_idc;
    /** \brief Same as the HEVC bitstream syntax element. */
    uint8_t     max_bytes_per_pic_denom;
    /** \brief Same as the HEVC bitstream syntax element. */
    uint8_t     max_bits_per_min_cu_denom;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_MEDIUM];
    /**@}*/
} VASTEncSequenceParameterBufferHEVC;

/****************************
 * HEVC data structures
 ****************************/
/**
 * \brief Picture parameter for HEVC encoding in main & main 10
 * profiles.
 *
 * This structure holds information for \c pic_parameter_set_rbsp() as
 * defined by the HEVC specification.
 *
 * If packed picture headers mode is used, i.e. if the encoding
 * pipeline was configured with the #VAST_ENC_PACKED_HEADER_PICTURE
 * flag, then the driver expects two more buffers to be provided to
 * the same \c vastRenderPicture() as this buffer:
 * - a #VASTEncPackedHeaderParameterBuffer with type set to
 *   VASTEncPackedHeaderType::VASTEncPackedHeaderPicture ;
 * - a #VAEncPackedHeaderDataBuffer which holds the actual packed
 *   header data.
 *
 * If \c pic_scaling_matrix_present_flag is set to \c 1, then a
 * #VASTQMatrixBufferHEVC buffer shall also be provided within the same
 * \c vastRenderPicture() call as this picture parameter buffer.
 */
typedef struct _VASTEncPictureParameterBufferHEVC {
    /**
     * \brief Information about the picture to be encoded.
     *
     * See #VASTPictureHEVC for further description of each field.
     * Note that decoded_curr_pic.picture_id represents the reconstructed
     * (decoded) picture. User provides a scratch VA surface ID here.
     * Long term reference and RPS related fields should be set to 0
     * and ignored.
     */
    VASTPictureHEVC   decoded_curr_pic;
    /**
     * \brief Decoded Picture Buffer (DPB).
     *
     * This array represents the list of reconstructed (decoded)
     * frames used as reference. It is important to keep track of
     * reconstructed frames so that they can be used later on as
     * reference for P or B-frames encoding.
     */
    VASTPictureHEVC   reference_frames[15];
    /**
     * \brief Output encoded bitstream.
     *
     * \ref coded_buf has type #VASTEncCodedBufferType. It should be
     * large enough to hold the compressed NAL slice and possibly VPS, SPS
     * and PPS NAL units, and other NAL units such as SEI.
     */
    VASTBufferID      coded_buf;

    /** \brief collocated reference picture buffer index of ReferenceFrames[].
     * Please note it is different from HEVC syntac element collocated_ref_idx.
     * When  the HEVC syntax element slice_temporal_mvp_enable_flag takes value 0,
     * collocated_ref_pic_index should take value 0xFF. .
     * Range: [0..14, 0xFF]
     */
    uint8_t         collocated_ref_pic_index;

    /**
     * \brief OR'd flags describing whether the picture is the last one or not.
     *
     * This fields holds 0 if the picture to be encoded is not the last
     * one in the stream or sequence. Otherwise, it is a combination of
     * \ref HEVC_LAST_PICTURE_EOSEQ_VAST or \ref HEVC_LAST_PICTURE_EOSTREAM_VAST.
     */
    uint8_t         last_picture;

    /** \brief \c init_qp_minus26 + 26. */
    uint8_t         pic_init_qp;

    /** \brief Corresponds to HEVC syntax element of the same name. */
    uint8_t         diff_cu_qp_delta_depth;

    /** \brief Corresponds to HEVC syntax element of the same name. */
    int8_t          pps_cb_qp_offset;

    /** \brief Corresponds to HEVC syntax element of the same name. */
    int8_t          pps_cr_qp_offset;

    /** \brief Corresponds to HEVC syntax element of the same name. */
    uint8_t         num_tile_columns_minus1;

    /** \brief Corresponds to HEVC syntax element of the same name. */
    uint8_t         num_tile_rows_minus1;

    /** \brief Corresponds to HEVC syntax element of the same name. */
    uint8_t         column_width_minus1[19];

    /** \brief Corresponds to HEVC syntax element of the same name. */
    uint8_t         row_height_minus1[21];

    /** \brief Corresponds to HEVC syntax element of the same name. */
    uint8_t         log2_parallel_merge_level_minus2;

    /** \brief Application may set the CTU bit size limit based on
     *  spec requirement (A.3.2), or other value for special purpose.
     *  If the value is set 0, no bit size limit is checked.
     */
    uint8_t         ctu_max_bitsize_allowed;

    /** \brief Maximum reference index for reference picture list 0.
     *   value range: [0..14].
     */
    uint8_t         num_ref_idx_l0_default_active_minus1;

    /** \brief Maximum reference index for reference picture list 1.
     *  value range: [0..14].
     */
    uint8_t         num_ref_idx_l1_default_active_minus1;

    /** \brief PPS header
     *  Used by GPU to generate new slice headers in slice size control.
     *  value range: [0..63].
     */
    uint8_t         slice_pic_parameter_set_id;

    /** \brief NAL unit type
     *  Used by GPU to generate new slice headers in slice size control.
     *  value range: [0..63].
     */
    uint8_t         nal_unit_type;

    union {
        struct {
            /** \brief Is picture an IDR picture? */
            uint32_t    idr_pic_flag                                   : 1;
            /** \brief Picture type.
             *  I  - 1;
             *  P  - 2;
             *  B  - 3;
             *  B1 - 4;
             *  B2 - 5;
             * B1 and B2 are frame types for hierachical B, explanation
             * can refer to num_b_in_gop[].
             */
            uint32_t    coding_type                                    : 3;
            /** \brief Is picture a reference picture? */
            uint32_t    reference_pic_flag                             : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    dependent_slice_segments_enabled_flag          : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    sign_data_hiding_enabled_flag                  : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    constrained_intra_pred_flag                    : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    transform_skip_enabled_flag                    : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    cu_qp_delta_enabled_flag	               : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    weighted_pred_flag                             : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    weighted_bipred_flag                           : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    transquant_bypass_enabled_flag                 : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    tiles_enabled_flag                             : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    entropy_coding_sync_enabled_flag               : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    loop_filter_across_tiles_enabled_flag          : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    pps_loop_filter_across_slices_enabled_flag     : 1;
            /** \brief A combination of HEVC syntax element of
             *  sps_scaling_list_data_present_flag and
             *  pps_scaling_list_data_present_flag
             *  when scaling_list_enable_flag is 0, it must be 0.
             */
            uint32_t    scaling_list_data_present_flag                 : 1;
            /** \brief indicate the current picture contains significant
             *  screen contents (text, characters, etc.) or animated image.
             *  GPU may want to treat them differently from normal video.
             *  For example, encoder may choose a small transform unit size
             *  and may use transform skip mode.
             */
            uint32_t    screen_content_flag                            : 1;
            /**
             *  When either weighted_pred_flag or weighted_bipred_flag is
             *  turned on, the flag enable_gpu_weighted_prediction requests
             *  GPU to determine weighted prediction factors. In this case,
             *  the following parameters in slice control data structure
             *  shall be ignored:
             *  luma_log2_weight_denom, delta_chroma_log2_weight_denom,
             *  luma_offset_l0[15], luma_offset_l1[15],
             *  delta_luma_weight_l0[15], delta_luma_weight_l1[15],
             *  chroma_offset_l0[15][2], chroma_offset_l1[15][2],
             *  and delta_chroma_weight_l0[15][2], delta_chroma_weight_l1[15][2].
             */
            uint32_t    enable_gpu_weighted_prediction                 : 1;
            /** \brief HEVC syntax element in slice segment header
             *  GPU uses it to generate new slice headers in slice size control.
             */
            uint32_t    no_output_of_prior_pics_flag                   : 1;
            uint32_t    reserved                                       : 11;
        } bits;
        uint32_t        value;
    } pic_fields;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_HIGH];
} VASTEncPictureParameterBufferHEVC;

/**
 * \brief Slice parameter for HEVC encoding in main & main 10 profiles.
 *
 * This structure holds information for \c
 * slice_segment_layer_rbsp() as defined by the HEVC
 * specification.
 *
 * If packed slice headers mode is used, i.e. if the encoding
 * pipeline was configured with the #VAST_ENC_PACKED_HEADER_SLICE
 * flag, then the driver expects two more buffers to be provided to
 * the same \c vastRenderPicture() as this buffer:
 * - a #VASTEncPackedHeaderParameterBuffer with type set to
 *   VASTEncPackedHeaderType::VASTEncPackedHeaderSlice ;
 * - a #VAEncPackedHeaderDataBuffer which holds the actual packed
 *   header data.
 *
 */
typedef struct _VASTEncSliceParameterBufferHEVC {
    /** \brief Starting CTU address for this slice. */
    uint32_t        slice_segment_address;
    /** \brief Number of CTUs in this slice. */
    uint32_t        num_ctu_in_slice;

    /** \brief Slice type.
     *  Corresponds to HEVC syntax element of the same name.
     */
    uint8_t         slice_type;
    /** \brief Same as the HEVC bitstream syntax element. */
    uint8_t         slice_pic_parameter_set_id;

    /** \brief Maximum reference index for reference picture list 0.
     *  Range: 0 to 14, inclusive.
     */
    uint8_t         num_ref_idx_l0_active_minus1;
    /** \brief Maximum reference index for reference picture list 1.
     *  Range: 0 to 14, inclusive.
     */
    uint8_t         num_ref_idx_l1_active_minus1;
    /** \brief Reference picture list 0 (for P slices). */
    VASTPictureHEVC   ref_pic_list0[15];
    /** \brief Reference picture list 1 (for B slices). */
    VASTPictureHEVC   ref_pic_list1[15];
    /**@}*/

    /** @name pred_weight_table() */
    /**@{*/
    /** \brief Same as the HEVC bitstream syntax element. */
    uint8_t         luma_log2_weight_denom;
    /** \brief Same as the HEVC bitstream syntax element. */
    int8_t          delta_chroma_log2_weight_denom;
    /** \brief Same as the HEVC bitstream syntax element. */
    int8_t          delta_luma_weight_l0[15];
    /** \brief Same as the HEVC bitstream syntax element. */
    int8_t          luma_offset_l0[15];
    /** \brief Same as the HEVC bitstream syntax element. */
    int8_t          delta_chroma_weight_l0[15][2];
    /** \brief Same as the HEVC spec variable ChromaOffsetL0[]. */
    int8_t          chroma_offset_l0[15][2];
    /** \brief Same as the HEVC bitstream syntax element. */
    int8_t          delta_luma_weight_l1[15];
    /** \brief Same as the HEVC bitstream syntax element. */
    int8_t          luma_offset_l1[15];
    /** \brief Same as the HEVC bitstream syntax element. */
    int8_t          delta_chroma_weight_l1[15][2];
    /** \brief Same as the HEVC spec variable ChromaOffsetL1[]. */
    int8_t          chroma_offset_l1[15][2];
    /**@}*/

    /** \brief Corresponds to HEVC spec variable MaxNumMergeCand.
     *  Range: [1..5].
     */
    uint8_t         max_num_merge_cand;

    /** \brief Same as the HEVC bitstream syntax element. */
    int8_t          slice_qp_delta;

    /** \brief Same as the HEVC bitstream syntax element. */
    int8_t          slice_cb_qp_offset;

    /** \brief Same as the HEVC bitstream syntax element. */
    int8_t          slice_cr_qp_offset;

    /** \brief Same as the HEVC bitstream syntax element. */
    int8_t          slice_beta_offset_div2;

    /** \brief Same as the HEVC bitstream syntax element. */
    int8_t          slice_tc_offset_div2;

    union {
        struct {
            /** \brief Indicates if current slice is the last one in picture */
            uint32_t    last_slice_of_pic_flag                         : 1;
            /** \brief Corresponds to HEVC syntax element of the same name */
            uint32_t    dependent_slice_segment_flag                   : 1;
            /** \brief Corresponds to HEVC syntax element of the same name */
            uint32_t    colour_plane_id                                : 2;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    slice_temporal_mvp_enabled_flag                : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    slice_sao_luma_flag                            : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    slice_sao_chroma_flag                          : 1;
            /** \brief Corresponds to HEVC syntax element of the same name.
             *  if this flag is set to 0, num_ref_idx_l0_active_minus1 should be
             *  equal to num_ref_idx_l0_default_active_minus1
             *  as well as for that for l1.
             */
            uint32_t    num_ref_idx_active_override_flag               : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    mvd_l1_zero_flag                               : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    cabac_init_flag        	                    : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    slice_deblocking_filter_disabled_flag          : 2;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    slice_loop_filter_across_slices_enabled_flag   : 1;
            /** \brief Corresponds to HEVC syntax element of the same name. */
            uint32_t    collocated_from_l0_flag                        : 1;
        } bits;
        uint32_t        value;
    } slice_fields;

    /** \brief Reserved bytes for future use, must be zero */
    uint32_t                va_reserved[VAST_PADDING_MEDIUM];
    /**@}*/
} VASTEncSliceParameterBufferHEVC;

#ifdef __cplusplus
}
#endif

#endif /* VA_ENC_HEVC_H */
