#ifndef __VASTAPI_DYNLINK_LOADER_H__
#define __VASTAPI_DYNLINK_LOADER_H__

#include <stdlib.h>
#include "vastva/vastapi.h"

#if defined(_WIN32) || defined(__CYGWIN__)
#define VASTAPICALL __stdcall
#else
#define VASTAPICALL
#endif


#if defined(_WIN32) && (!defined(VASTAPI_LOAD_FUNC) || !defined(VASTAPI_SYM_FUNC) || !defined(VASTAPI_LIB_HANDLE))
# include <windows.h>
#endif

#ifndef VASTAPI_LIB_HANDLE
# if defined(_WIN32)
#  define VASTAPI_LIB_HANDLE HMODULE
# else
#  define VASTAPI_LIB_HANDLE void*
# endif
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#  define VASTAPI_ENC_LIBNAME "vastai_video.dll"
#else
# define VASTAPI_ENC_LIBNAME "vastai_drv_video.so"
#endif

#if !defined(VASTAPI_LOAD_FUNC) || !defined(VASTAPI_SYM_FUNC)
# ifdef _WIN32
#  define VASTAPI_LOAD_FUNC(path) LoadLibrary(TEXT(path))
#  define VASTAPI_SYM_FUNC(lib, sym) GetProcAddress((lib), (sym))
#  define VASTAPI_FREE_FUNC(lib) FreeLibrary(lib)
# else
#  include <dlfcn.h>
#  define VASTAPI_LOAD_FUNC(path) dlopen((path), RTLD_LAZY)
#  define VASTAPI_SYM_FUNC(lib, sym) dlsym((lib), (sym))
#  define VASTAPI_FREE_FUNC(lib) dlclose(lib)
# endif
#endif

#if !defined(VASTAPI_LOG_FUNC)
# include <stdio.h>
//#define VASTAPI_LOG_FUNC(msg, ...) fprintf(stderr, (msg), __VA_ARGS__)
#define VASTAPI_LOG_FUNC
#endif

#define LOAD_LIBRARY(l, path)                          \
    do {                                               \
        if (!((l) = VASTAPI_LOAD_FUNC(path))) {        \
            VASTAPI_LOG_FUNC("Cannot load %s\n", path);\
            ret = -1;                                  \
            goto error;                                \
        }                                              \
        VASTAPI_LOG_FUNC("Loaded lib: %s\n", path);    \
    } while (0)

#define LOAD_SYMBOL(fun, tp, symbol)                               \
    do {                                                           \
        if (!((f->fun) = (tp*)VASTAPI_SYM_FUNC(f->lib, symbol))) { \
            VASTAPI_LOG_FUNC("Cannot load %s\n", symbol);          \
            ret = -1;                                              \
            goto error;                                            \
        }                                                          \
        VASTAPI_LOG_FUNC("Loaded sym: %s\n", symbol);              \
    } while (0)

#define LOAD_SYMBOL_OPT(fun, tp, symbol)                           \
    do {                                                           \
        if (!((f->fun) = (tp*)VASTAPI_SYM_FUNC(f->lib, symbol))) { \
            VASTAPI_LOG_FUNC("Cannot load optional %s\n", symbol); \
        } else {                                                   \
            VASTAPI_LOG_FUNC("Loaded sym: %s\n", symbol);          \
        }                                                          \
    } while (0)


#define GENERIC_FREE_FUNC()                  \
    if (!functions)                          \
        return;                              \
    if (*functions && (*functions)->lib)     \
        VASTAPI_FREE_FUNC((*functions)->lib);\
    free(*functions);                        \
    *functions = NULL;


#define GENERIC_LOAD_FUNC_PREAMBLE(T, n, N)     \
    T *f;                                       \
    int ret=0;                                  \
                                                \
    n##_free_functions(functions);              \
                                                \
    f = *functions = (T*)calloc(1, sizeof(*f)); \
    if (!f)                                     \
        return -1;                              \
                                                \
    LOAD_LIBRARY(f->lib, N);

#define GENERIC_LOAD_FUNC_FINALE(n) \
    return 0;                       \
error:                              \
    n##_free_functions(functions);  \
    return ret;

//vastapi encoder api
typedef int   VastapiEncAllowOptimizeDelay(VASTAPIEncodeContext *ctx);
typedef int   VastapiEncWait(VASTAPIEncodeContext *ctx, VASTDisplay display, VASTAPIEncodePicture *pic);
typedef  int  VastapiEncPickNext(VASTAPIEncodeContext *ctx, int width, int height, int is_av1, VASTAPIEncodePicture **pic_out);
typedef void  VastapiEncFreeVastBuff2(VASTAPIEncodeContext *ctx, VASTDisplay display);
typedef void  VastapiEncH264DefaultRefPicList(VASTAPIEncodePicture *pic, VASTAPIEncodePicture **rpl0, VASTAPIEncodePicture **rpl1, int *rpl_size);
typedef int   VastapiEncAV1InitPicParams(VASTAPIEncodeContext *ctx, VASTAPIEncodePicture *pic);
typedef int   VastapiEncAV1InitSliceParam(VASTAPIEncodePicture *pic, VASTAPIEncodeSlice *slice);
typedef int   VastapiEncIssuePrep(VASTAPIEncodeContext *ctx, VASTDisplay display, VASTAPIEncodePicture *pic, VaEncFrameSideData* sd_list, int nb_item, 
                                void* opaque, int is_lt_n50, int frame_number, int width, int height);
typedef int VastapiEncIssue(VASTAPIEncodeContext *ctx, VASTDisplay display, VASTAPIEncodePicture *pic,VaEncFrameSideData* sd_list, int nb_item, 
                              int width, int height, enum VaEncCodecID codec_id, int is_ge_n44, VASTAPIEncodeParam* params, int count);
typedef int VastapiEncConfigCreate(VASTAPIEncodeContext *ctx, VASTDisplay display, int fr_num, int fr_den, int is_qsacle, int* avctx_profile);
typedef int VastapiEncInitVastParam(VASTAPIEncodeContext *ctx, int is_range_jpeg, int is_av1, int is_10bit,
                                     int* avctx_gop_size, int* avctx_max_b_frames, int is_vui, int num, int den);
typedef int VastapiEncFree(VASTAPIEncodeContext *avctx, VASTAPIEncodePicture *pic);
typedef int VastapiEncFlushEncoder(VASTAPIEncodeContext *ctx, void *pkt);
typedef int VastapiEncCheckAV11Pass(VASTAPIEncodeContext *ctx, void *pkt);
typedef int VastapiEnc2PassOnlyIssue(VASTAPIEncodeContext *ctx, VASTAPIEncodePicture *pic, void * pkt);
typedef int VastapiEnc1PassIssue(VASTAPIEncodeContext *ctx, VASTAPIEncodePicture *pic, void * pkt);
typedef int VastapiEncGetEncoderOutput(VASTAPIEncodeContext *ctx, VASTAPIEncodePicture *pic, void *pkt);
typedef int VastapiEncAllocOutputBuff(VASTAPIEncodeContext *ctx, VASTDisplay display, int is_av1, int width, int height, VASTBufferID *buf_id);



//vastapi hwcontext api
typedef VAST_PIX_FTM  VastapiHwPixFmtFromFourcc(unsigned int fourcc);
typedef VASTAPIFormatDescriptor*  VastapiHwFmtFromFourcc(unsigned int fourcc);
typedef int   VastapiHwGetImgFmt(VASTAPIDeviceContext *ctx, VAST_PIX_FTM  vst_fmt, VASTImageFormat **image_format);
typedef int   VastapiPresetLoadBL(char *preset);
typedef int   VastapiHwDeviceInit(VASTAPIDeviceContext *ctx,  AVVASTAPIDeviceContext *hwctx);
typedef void* VastapiHwDeviceCreate(AVVASTAPIDeviceContext *hwctx, const char *device);
typedef void  VastapiHwDeviceFree(AVVASTAPIDeviceContext *hwctx, void *user_opaque);
typedef void  VastapiHwDmaBuffFree(void *opaque, VASTAPIDmaHandle *data);
typedef void  VastapiHwBuffFree(void *opaque, uint8_t *data);
typedef void  VastapiHwUnmapFrame(VASTAPIContext *vstCtx, void* data, int width, int height);
typedef int   VastapiHwMapFrame(VASTAPIContext *vstCtx, VAST_PIX_FTM dstFtm, int width, int height, int flags);
typedef int   VastapiHwGetConstraints(VASTAPIContext *vstCtx, VastapiConstraint *constraints, VASTSurfaceAttrib **attr_list);
typedef int   VastapiHwSurfaceAddr(AVVASTAPIDeviceContext *hwctx, uint8_t *data, uint64_t *frame_addr, int isGetAddress);
typedef int   VastapiHwTransferData(VASTAPIContext *vstCtx, uint64_t dma_addr, int dma_size, uint8_t *data, int fd, int src_type, int isHostToHw);
typedef int   VastapiHwFrameInit(VASTAPIContext *vstCtx, VAST_PIX_FTM pix_fmt, int pool_size, int frame_flags);
typedef int   VastapiHwTestDeriveWork(VASTAPIContext *vstCtx, VAST_PIX_FTM pix_fmt, uint8_t *data);
typedef VASTAPIDmaHandle*  VastapiHwAllocDmaBuff(VASTAPIContext *vstCtx);


//vastapi decoder api
typedef int   VastapiDecMakeParamBuffer(VASTVADecCtx *vaCtx, VASTAPIDecodePicture *pic, int type, const void *data, size_t size);
typedef int   VastapiDecPicutre(VASTVADecCtx *vaCtx, VASTAPIDecodePicture *pic);
typedef void  VastapiDecUnint(VASTVADecCtx *vaCtx);
typedef void  VastapiDecDestroyBuff(VASTVADecCtx *vaCtx, VASTAPIDecodePicture *pic);
typedef int   VastapiDecMakeSliceBuff(VASTVADecCtx *vaCtx, VASTAPIDecodePicture *pic, const void *params_data,
                                        size_t params_size, const void *slice_data, size_t slice_size);
typedef int   VastapiDecQuerySurfaceAttr(VASTVADecCtx *vaCtx, VASTSurfaceAttrib  **attr);
typedef int   VastapiDecQueryProfileList(VASTVADecCtx *vaCtx, VASTProfile  **list);
typedef int   VastapiDecCreateConfig(VASTVADecCtx *vaCtx, VASTProfile profile);
typedef int   VastapiDecSyncSurface(VASTVADecCtx *vaCtx, VASTSurfaceID surface_id);
typedef int   VastapiDecCreateContext(VASTVADecCtx *vaCtx, int width, int height, int flag, VASTSurfaceID *render_targets,int num_render);
typedef int   VastapiDecDestoryConfig(VASTVADecCtx *vaCtx);
typedef VAST_PIX_FTM VastapiDecGetFmt(VASTSurfaceAttrib attr);


//vastapi filter  api
typedef void  VastapiFilterPipelineUnint(VASTFilterParamer *vastfilter_params,int nb_filter);
typedef int  VastapiFilterRenderPicture(VASTFilterParamer *vastfilter_params);
typedef int VastapiFilterConfigCreate(VASTFilterParamer *vastfilter_params);
typedef int VastapiFilterContextCreate(VASTFilterParamer *vastfilter_params,uint32_t output_width,uint32_t output_height,AVVASTAPIFramesContext *va_frames);


//vastapi api
typedef const char*   VastapiQueryVendorString(VASTDisplay dpy);
typedef VASTStatus    VastapiDestroyConfig (VASTDisplay dpy, VASTConfigID config_id);
typedef VASTStatus    VastapiCreateSurfaces(VASTDisplay dpy, unsigned int format, unsigned int width, unsigned int height, VASTSurfaceID *surfaces, unsigned int num_surfaces,
                                        VASTSurfaceAttrib *attrib_list, unsigned int num_attribs);
typedef VASTStatus    VastapiCreateContext (VASTDisplay dpy, VASTConfigID config_id, int picture_width, int picture_height, int flag,
                                VASTSurfaceID *render_targets, int num_render_targets, VASTContextID *context);
typedef VASTStatus VastapiDestroyContext(VASTDisplay dpy, VASTContextID context);

typedef VASTStatus VastapiCreateBuffer(VASTDisplay dpy, VASTContextID context, VASTBufferType type, unsigned int size, unsigned int num_elements,
                                        void *data,	VASTBufferID *buf_id);
typedef VASTStatus VastapiCreateBuffer2(VASTDisplay dpy, VASTContextID context, VASTBufferType type, unsigned int width, unsigned int height,
                                unsigned int *unit_size, unsigned int *pitch, VASTBufferID *buf_id);
typedef VASTStatus VastapiMapBuffer(VASTDisplay dpy,VASTBufferID buf_id, void **pbuf);
typedef VASTStatus VastapiUnmapBuffer (VASTDisplay dpy, VASTBufferID buf_id);
typedef VASTStatus VastapiDestroyBuffer(VASTDisplay dpy, VASTBufferID buffer_id);
typedef VASTStatus VastapiSyncSurface(VASTDisplay dpy, VASTSurfaceID render_target);
typedef VASTStatus VastapiCreateImage(VASTDisplay dpy, VASTImageFormat *format, int width, int height,VASTImage *image);
typedef VASTStatus VastapiDestroyImage (VASTDisplay dpy, VASTImageID image);
typedef VASTStatus VastapiDeriveImage (VASTDisplay dpy, VASTSurfaceID surface, VASTImage *image);
typedef VASTStatus VastapiDestroyDmaHandle(VASTDisplay dpy, void *dma_handle);  
typedef VASTStatus VastapiDmaWriteBuf(VASTDisplay dpy, uint64_t dst_soc_addr, int buf_size, void *dma_handle );  
typedef VASTStatus VastapiDmaReadBuf(VASTDisplay dpy, uint64_t src_soc_addr, int buf_size, void *dma_handle);
typedef VASTStatus VastapiFilterParamParse(void * filt_params, const char * key,const char * value);

//common tool api
typedef void*      VastapiGetMemory(int len);
typedef void       VastapiFreeMemory(void *ptr);

typedef struct VastapiFunctions{
    VastapiEncAllowOptimizeDelay    *vastapiEncAllowOptimizeDelay;
    VastapiEncWait                  *vastapiEncWait;
    VastapiEncPickNext              *vastapiEncPickNext;
    VastapiEncFreeVastBuff2         *vastapiEncFreeVastBuff2;
    VastapiEncH264DefaultRefPicList *vastapiEncH264DefaultRefPicList;
    VastapiEncAV1InitPicParams      *vastapiEncAV1InitPicParams;
    VastapiEncAV1InitSliceParam     *vastapiEncAV1InitSliceParam;
    VastapiEncIssuePrep             *vastapiEncIssuePrep;
    VastapiEncIssue                 *vastapiEncIssue;
    VastapiEncConfigCreate          *vastapiEncConfigCreate;
    VastapiEncInitVastParam         *vastapiEncInitVastParam;
    VastapiEncFree                  *vastapiEncFree;
    VastapiEncFlushEncoder          *vastapiEncFlushEncoder;
    VastapiEncCheckAV11Pass         *vastapiEncCheckAV11Pass;
    VastapiEnc2PassOnlyIssue        *vastapiEnc2PassOnlyIssue;
    VastapiEnc1PassIssue            *vastapiEnc1PassIssue;
    VastapiEncGetEncoderOutput      *vastapiEncGetEncoderOutput;
    VastapiEncAllocOutputBuff       *vastapiEncAllocOutputBuff;



    VastapiHwPixFmtFromFourcc   *vastapiHwPixFmtFromFourcc;
    VastapiHwFmtFromFourcc      *vastapiHwFmtFromFourcc;
    VastapiHwGetImgFmt          *vastapiHwGetImgFmt;
    VastapiHwDeviceInit         *vastapiHwDeviceInit;
    VastapiHwDeviceCreate       *vastapiHwDeviceCreate;
    VastapiHwDeviceFree         *vastapiHwDeviceFree;
    VastapiHwDmaBuffFree        *vastapiHwDmaBuffFree;
    VastapiHwBuffFree           *vastapiHwBuffFree;
    VastapiHwUnmapFrame         *vastapiHwUnmapFrame;
    VastapiHwMapFrame           *vastapiHwMapFrame;
    VastapiHwGetConstraints     *vastapiHwGetConstraints;
    VastapiHwSurfaceAddr        *vastapiHwSurfaceAddr;
    VastapiHwTransferData       *vastapiHwTransferData;
    VastapiHwFrameInit          *vastapiHwFrameInit;
    VastapiHwTestDeriveWork     *vastapiHwTestDeriveWork;
    VastapiHwAllocDmaBuff       *vastapiHwAllocDmaBuff;


    VastapiDecMakeParamBuffer    *vastapiDecMakeParamBuffer;
    VastapiDecPicutre            *vastapiDecPicutre;
    VastapiDecUnint              *vastapiDecUnint;
    VastapiDecDestroyBuff        *vastapiDecDestroyBuff;
    VastapiDecMakeSliceBuff      *vastapiDecMakeSliceBuff;
    VastapiDecQuerySurfaceAttr   *vastapiDecQuerySurfaceAttr;
    VastapiDecQueryProfileList   *vastapiDecQueryProfileList;
    VastapiDecCreateConfig       *vastapiDecCreateConfig;
    VastapiDecSyncSurface        *vastapiDecSyncSurface;
    VastapiDecCreateContext      *vastapiDecCreateContext;
    VastapiDecDestoryConfig      *vastapiDecDestoryConfig;
    VastapiDecGetFmt             *vastapiDecGetFmt;

    VastapiFilterPipelineUnint   *vastapiFilterPipelineUnint;
    VastapiFilterRenderPicture   *vastapiFilterRenderPicture;
    VastapiFilterConfigCreate    *vastapiFilterConfigCreate;
    VastapiFilterContextCreate   *vastapiFilterContextCreate;

    VastapiQueryVendorString     *vastapiQueryVendorString;
    VastapiDestroyConfig         *vastapiDestroyConfig;
    VastapiCreateSurfaces        *vastapiCreateSurfaces;
    VastapiCreateContext         *vastapiCreateContext;
    VastapiDestroyContext        *vastapiDestroyContext;
    VastapiCreateBuffer          *vastapiCreateBuffer;
    VastapiCreateBuffer2         *vastapiCreateBuffer2;
    VastapiMapBuffer             *vastapiMapBuffer;
    VastapiUnmapBuffer           *vastapiUnmapBuffer;
    VastapiDestroyBuffer         *vastapiDestroyBuffer;
    VastapiSyncSurface           *vastapiSyncSurface;
    VastapiCreateImage           *vastapiCreateImage;
    VastapiDestroyImage          *vastapiDestroyImage;
    VastapiDeriveImage           *vastapiDeriveImage;
    VastapiDestroyDmaHandle      *vastapiDestroyDmaHandle;
    VastapiDmaWriteBuf           *vastapiDmaWriteBuf;
    VastapiDmaReadBuf            *vastapiDmaReadBuf;

    VastapiGetMemory             *vastapiGetMemory;
    VastapiFreeMemory            *vastapiFreeMemory;

    VASTAPI_LIB_HANDLE           lib;

}VastapiFunctions;

typedef struct VastapiFunctionsNoDev{

    VastapiPresetLoadBL      *vastapiPresetLoadBL;
    VastapiFilterParamParse  *vastapiFilterParamParse;
    VastapiGetMemory         *vastapiGetMemory;
    VastapiFreeMemory        *vastapiFreeMemory;

    VASTAPI_LIB_HANDLE           lib;
}VastapiFunctionsNoDev;



static inline void vastapi_free_functions(VastapiFunctions **functions)
{
    GENERIC_FREE_FUNC();
}


static inline int vastapi_load_functions(VastapiFunctions **functions)
{

    GENERIC_LOAD_FUNC_PREAMBLE(VastapiFunctions, vastapi, VASTAPI_ENC_LIBNAME);


    LOAD_SYMBOL(vastapiEncAllowOptimizeDelay,   VastapiEncAllowOptimizeDelay, "allow_optimize_delay");
    LOAD_SYMBOL(vastapiEncWait,                 VastapiEncWait, "vaenc_wait");
    LOAD_SYMBOL(vastapiEncPickNext,             VastapiEncPickNext, "vastapi_encode_pick_next");
    LOAD_SYMBOL(vastapiEncFreeVastBuff2,        VastapiEncFreeVastBuff2, "vastapi_encode_free_vast_buffer2");
    LOAD_SYMBOL(vastapiEncH264DefaultRefPicList,VastapiEncH264DefaultRefPicList, "vastapi_encode_h264_default_ref_pic_list");
    LOAD_SYMBOL(vastapiEncAV1InitPicParams,     VastapiEncAV1InitPicParams, "vaenc_av1_init_picture_params");
    LOAD_SYMBOL(vastapiEncAV1InitSliceParam,    VastapiEncAV1InitSliceParam, "vaenc_av1_init_slice_params");
    LOAD_SYMBOL(vastapiEncIssuePrep,            VastapiEncIssuePrep, "vaenc_issue_prep");
    LOAD_SYMBOL(vastapiEncIssue,                VastapiEncIssue, "vaenc_issue");
    LOAD_SYMBOL(vastapiEncConfigCreate,         VastapiEncConfigCreate, "vaenc_create_config");
    LOAD_SYMBOL(vastapiEncInitVastParam,        VastapiEncInitVastParam, "vastapi_encode_init_vast_params");
    LOAD_SYMBOL(vastapiEncFree,                 VastapiEncFree, "vastapi_encode_free");
    LOAD_SYMBOL(vastapiEncFlushEncoder,         VastapiEncFlushEncoder, "vaenc_flush_encoder");
    LOAD_SYMBOL(vastapiEncCheckAV11Pass,        VastapiEncCheckAV11Pass, "vaenc_check_av1_1pass");
    LOAD_SYMBOL(vastapiEnc2PassOnlyIssue,       VastapiEnc2PassOnlyIssue, "vaenc_2passonly_issue");
    LOAD_SYMBOL(vastapiEnc1PassIssue,           VastapiEnc1PassIssue, "vaenc_1pass_issue");
    LOAD_SYMBOL(vastapiEncGetEncoderOutput,     VastapiEncGetEncoderOutput, "vaenc_get_encode_output");
    LOAD_SYMBOL(vastapiEncAllocOutputBuff,      VastapiEncAllocOutputBuff, "vaenc_alloc_output_buffer");



    LOAD_SYMBOL(vastapiHwPixFmtFromFourcc,  VastapiHwPixFmtFromFourcc, "vastapi_pix_fmt_from_fourcc");
    LOAD_SYMBOL(vastapiHwFmtFromFourcc,  VastapiHwFmtFromFourcc, "vastapi_format_from_fourcc");
    LOAD_SYMBOL(vastapiHwGetImgFmt,  VastapiHwGetImgFmt, "vastapi_get_image_format");
    LOAD_SYMBOL(vastapiHwDeviceInit,  VastapiHwDeviceInit, "vastapi_device_init_private");
    LOAD_SYMBOL(vastapiHwDeviceCreate,  VastapiHwDeviceCreate, "vastapi_device_create_private");
    LOAD_SYMBOL(vastapiHwDeviceFree,  VastapiHwDeviceFree, "vastapi_device_free_private");
    LOAD_SYMBOL(vastapiHwDmaBuffFree,  VastapiHwDmaBuffFree, "vastapi_dmabuffer_free");
    LOAD_SYMBOL(vastapiHwBuffFree,  VastapiHwBuffFree, "vastapi_buffer_free");
    LOAD_SYMBOL(vastapiHwUnmapFrame,  VastapiHwUnmapFrame, "vastapi_unmap_frame_private");
    LOAD_SYMBOL(vastapiHwMapFrame,  VastapiHwMapFrame, "vastapi_map_frame_private");
    LOAD_SYMBOL(vastapiHwGetConstraints,  VastapiHwGetConstraints, "vastapi_get_constraints");
    LOAD_SYMBOL(vastapiHwSurfaceAddr,  VastapiHwSurfaceAddr, "vastapi_surface_address");
    LOAD_SYMBOL(vastapiHwTransferData,  VastapiHwTransferData, "vastapi_transfer_data");
    LOAD_SYMBOL(vastapiHwFrameInit,  VastapiHwFrameInit, "vastapi_frames_init_private");
    LOAD_SYMBOL(vastapiHwTestDeriveWork,  VastapiHwTestDeriveWork, "vastapi_test_derive_work");
    LOAD_SYMBOL(vastapiHwAllocDmaBuff,  VastapiHwAllocDmaBuff, "vastapi_dmabuff_alloc");


    LOAD_SYMBOL(vastapiDecMakeParamBuffer, VastapiDecMakeParamBuffer, "vastapi_decode_make_param_buffer");
    LOAD_SYMBOL(vastapiDecPicutre,         VastapiDecPicutre, "vastapi_decode_picutre");
    LOAD_SYMBOL(vastapiDecUnint,           VastapiDecUnint, "vastapi_decode_uninit");
    LOAD_SYMBOL(vastapiDecDestroyBuff,     VastapiDecDestroyBuff, "vastapi_decode_destroy_buffers");
    LOAD_SYMBOL(vastapiDecMakeSliceBuff,   VastapiDecMakeSliceBuff, "vastapi_decode_make_slice_buffer");
    LOAD_SYMBOL(vastapiDecQuerySurfaceAttr,VastapiDecQuerySurfaceAttr, "vastapi_query_surface_attr");
    LOAD_SYMBOL(vastapiDecQueryProfileList,VastapiDecQueryProfileList, "vastapi_query_pofiles_list");
    LOAD_SYMBOL(vastapiDecCreateConfig,    VastapiDecCreateConfig, "vastapi_create_dec_config");
    LOAD_SYMBOL(vastapiDecSyncSurface,     VastapiDecSyncSurface, "vastapi_sync_surface");
    LOAD_SYMBOL(vastapiDecCreateContext,   VastapiDecCreateContext, "vastapi_create_dec_context");
    LOAD_SYMBOL(vastapiDecDestoryConfig,   VastapiDecDestoryConfig, "vastapi_destroy_config");
    LOAD_SYMBOL(vastapiDecGetFmt,          VastapiDecGetFmt, "vastapi_dec_get_format");

    LOAD_SYMBOL(vastapiFilterPipelineUnint,  VastapiFilterPipelineUnint, "vastfilter_pipeline_uninit");
    LOAD_SYMBOL(vastapiFilterRenderPicture,  VastapiFilterRenderPicture, "vastfilter_render_picture");
    LOAD_SYMBOL(vastapiFilterConfigCreate,   VastapiFilterConfigCreate, "vafilter_create_config");
    LOAD_SYMBOL(vastapiFilterContextCreate,  VastapiFilterContextCreate, "vafilter_creat_context");

    LOAD_SYMBOL(vastapiQueryVendorString, VastapiQueryVendorString, "vastQueryVendorString");
    LOAD_SYMBOL(vastapiDestroyConfig,     VastapiDestroyConfig, "vastDestroyConfig");
    LOAD_SYMBOL(vastapiCreateSurfaces,    VastapiCreateSurfaces, "vastCreateSurfaces");
    LOAD_SYMBOL(vastapiCreateContext,     VastapiCreateContext, "vastCreateContext");
    LOAD_SYMBOL(vastapiDestroyContext,    VastapiDestroyContext, "vastDestroyContext");
    LOAD_SYMBOL(vastapiCreateBuffer,      VastapiCreateBuffer, "vastCreateBuffer");
    LOAD_SYMBOL(vastapiCreateBuffer2,     VastapiCreateBuffer2, "vastCreateBuffer2");
    LOAD_SYMBOL(vastapiMapBuffer,         VastapiMapBuffer, "vastMapBuffer");
    LOAD_SYMBOL(vastapiUnmapBuffer,       VastapiUnmapBuffer, "vastUnmapBuffer");
    LOAD_SYMBOL(vastapiDestroyBuffer,     VastapiDestroyBuffer, "vastDestroyBuffer");
    LOAD_SYMBOL(vastapiSyncSurface,       VastapiSyncSurface, "vastSyncSurface");
    LOAD_SYMBOL(vastapiCreateImage,       VastapiCreateImage, "vastCreateImage");
    LOAD_SYMBOL(vastapiDestroyImage,      VastapiDestroyImage, "vastDestroyImage");
    LOAD_SYMBOL(vastapiDeriveImage,       VastapiDeriveImage, "vastDeriveImage");
    LOAD_SYMBOL(vastapiDestroyDmaHandle,  VastapiDestroyDmaHandle, "vastDestroyDmaHandle");
    LOAD_SYMBOL(vastapiDmaWriteBuf,       VastapiDmaWriteBuf, "vastDmaWriteBuf");
    LOAD_SYMBOL(vastapiDmaReadBuf,        VastapiDmaReadBuf, "vastDmaReadBuf");

    LOAD_SYMBOL(vastapiGetMemory,         VastapiGetMemory, "vastapi_malloc_memory");
    LOAD_SYMBOL(vastapiFreeMemory,        VastapiFreeMemory, "vastapi_free_memory");

    GENERIC_LOAD_FUNC_FINALE(vastapi);
}


static inline void vastapi_nodev_free_functions(VastapiFunctionsNoDev **functions)
{
    GENERIC_FREE_FUNC();
}

static inline int vastapi_nodev_load_functions(VastapiFunctionsNoDev **functions)
{

    GENERIC_LOAD_FUNC_PREAMBLE(VastapiFunctionsNoDev, vastapi_nodev, VASTAPI_ENC_LIBNAME);

    LOAD_SYMBOL(vastapiPresetLoadBL,       VastapiPresetLoadBL, "vastapi_preset_loadbalance");
    LOAD_SYMBOL(vastapiFilterParamParse,   VastapiFilterParamParse, "vastFilterParamParse");

    LOAD_SYMBOL(vastapiGetMemory,          VastapiGetMemory, "vastapi_malloc_memory");
    LOAD_SYMBOL(vastapiFreeMemory,         VastapiFreeMemory, "vastapi_free_memory");

    GENERIC_LOAD_FUNC_FINALE(vastapi_nodev);
}

#endif