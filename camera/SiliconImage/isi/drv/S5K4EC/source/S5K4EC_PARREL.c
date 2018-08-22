//ov8825 the same with ov14825

/******************************************************************************
 *
 * Copyright 2010, Dream Chip Technologies GmbH. All rights reserved.
 * No part of this work may be reproduced, modified, distributed, transmitted,
 * transcribed, or translated into any language or computer format, in any form
 * or by any means without written permission of:
 * Dream Chip Technologies GmbH, Steinriede 10, 30827 Garbsen / Berenbostel,
 * Germany
 *
 *****************************************************************************/
/**
 * @file S5K4EC.c
 *
 * @brief
 *   ADD_DESCRIPTION_HERE
 *
 *****************************************************************************/
#include <ebase/types.h>
#include <ebase/trace.h>
#include <ebase/builtins.h>

#include <common/return_codes.h>
#include <common/misc.h>

#include "isi.h"
#include "isi_iss.h"
#include "isi_priv.h"

#include "S5K4EC_priv.h"


#define CC_OFFSET_SCALING  2.0f
#define I2C_COMPLIANT_STARTBIT 1U

/******************************************************************************
 * local macro definitions
 *****************************************************************************/
CREATE_TRACER( S5K4EC_INFO , "S5K4EC: ", INFO,    1U );
CREATE_TRACER( S5K4EC_WARN , "S5K4EC: ", WARNING, 1U );
CREATE_TRACER( S5K4EC_ERROR, "S5K4EC: ", ERROR,   1U );

CREATE_TRACER( S5K4EC_DEBUG, "S5K4EC: ", INFO,     1U );

CREATE_TRACER( S5K4EC_REG_INFO , "S5K4EC: ", INFO, 1);
CREATE_TRACER( S5K4EC_REG_DEBUG, "S5K4EC: ", INFO, 1U );

#define S5K4EC_SLAVE_ADDR       0x5aU                           /**< i2c slave address of the S5K4EC camera sensor */
#define S5K4EC_SLAVE_AF_ADDR    0x5aU                           /**< i2c slave address of the S5K4EC integrated AD5820 */

#define SOC_AF 0

#define AF_Address    0x3022U
#define AF_CMD        0x03U
#define AF_IDLE        0x08U
#define AF_ACK_Address 0x3023U
#define AF_ACK_VALUE   0x01U


#define LOG_TAG                                  "CameraHal"          

#undef TRACE

#undef S5K4EC_INFO
#undef S5K4EC_DEBUG
#undef S5K4EC_WARN
#undef S5K4EC_ERROR
#undef S5K4EC_REG_DEBUG
#undef S5K4EC_REG_INFO

#define S5K4EC_INFO      "S5K4EC" 
#define S5K4EC_DEBUG     "S5K4EC"
#define S5K4EC_WARN      "S5K4EC"
#define S5K4EC_ERROR     "S5K4EC"
#define S5K4EC_REG_DEBUG "S5K4EC"
#define S5K4EC_REG_INFO  "S5K4EC" 

#define TRACE(msg1,msg2,...)                 ALOGV("%s(%d): " msg2 ,__FUNCTION__,__LINE__,##__VA_ARGS__)

/*!<
 * Focus position values:
 * 65 logical positions ( 0 - 64 )
 * where 64 is the setting for infinity and 0 for macro
 * corresponding to
 * 1024 register settings (0 - 1023)
 * where 0 is the setting for infinity and 1023 for macro
 */
#define MAX_LOG   64U
#define MAX_REG 1023U
/*!<
 * Lens movement is triggered every 133ms (VGA, 7.5fps processed frames
 * worst case assumed, usually even much slower, see OV5630 driver for
 * details). Thus the lens has to reach the requested position after
 * max. 133ms. Minimum mechanical ringing is expected with mode 1 ,
 * 100us per step. A movement over the full range needs max. 102.3ms
 * (see table 9 AD5820 datasheet).
 */
#define MDI_SLEW_RATE_CTRL 2U /* S3..0 */

/******************************************************************************
 * local variable declarations
 *****************************************************************************/
const char S5K4EC_g_acName[] = "S5K4EC_ SOC_PARREL";
extern const IsiRegDescription_t S5K4EC_g_aRegDescription[];
extern const IsiRegDescription_t S5K4EC_g_svga[];
extern const IsiRegDescription_t S5K4EC_g_vga[];
extern const IsiRegDescription_t S5K4EC_g_2592x1944[];
extern const IsiRegDescription_t S5K4EC_g_video_720p[];
extern const IsiRegDescription_t S5K4EC_g_1080p[];
extern const IsiRegDescription_t S5K4EC_af_firmware_new[];
extern const IsiRegDescription_t S5K4EC_af_init[];


const IsiSensorCaps_t S5K4EC_g_IsiSensorDefaultConfig;


#define S5K4EC_I2C_START_BIT        (I2C_COMPLIANT_STARTBIT)    // I2C bus start condition
#define S5K4EC_I2C_NR_ADR_BYTES     (2U)                        // 1 byte base address and 2 bytes sub address
#define S5K4EC_I2C_NR_DAT_BYTES     (2U)                        // 8 bit registers


/******************************************************************************
 * local function prototypes
 *****************************************************************************/
static RESULT S5K4EC_IsiCreateSensorIss( IsiSensorInstanceConfig_t *pConfig );
static RESULT S5K4EC_IsiReleaseSensorIss( IsiSensorHandle_t handle );
static RESULT S5K4EC_IsiGetCapsIss( IsiSensorHandle_t handle, IsiSensorCaps_t *pIsiSensorCaps );
static RESULT S5K4EC_IsiSetupSensorIss( IsiSensorHandle_t handle, const IsiSensorConfig_t *pConfig );
static RESULT S5K4EC_IsiSensorSetStreamingIss( IsiSensorHandle_t handle, bool_t on );
static RESULT S5K4EC_IsiSensorSetPowerIss( IsiSensorHandle_t handle, bool_t on );
static RESULT S5K4EC_IsiCheckSensorConnectionIss( IsiSensorHandle_t handle );
static RESULT S5K4EC_IsiGetSensorRevisionIss( IsiSensorHandle_t handle, uint32_t *p_value);

static RESULT S5K4EC_IsiGetGainLimitsIss( IsiSensorHandle_t handle, float *pMinGain, float *pMaxGain);
static RESULT S5K4EC_IsiGetIntegrationTimeLimitsIss( IsiSensorHandle_t handle, float *pMinIntegrationTime, float *pMaxIntegrationTime );
static RESULT S5K4EC_IsiExposureControlIss( IsiSensorHandle_t handle, float NewGain, float NewIntegrationTime, uint8_t *pNumberOfFramesToSkip, float *pSetGain, float *pSetIntegrationTime );
static RESULT S5K4EC_IsiGetCurrentExposureIss( IsiSensorHandle_t handle, float *pSetGain, float *pSetIntegrationTime );
static RESULT S5K4EC_IsiGetAfpsInfoIss ( IsiSensorHandle_t handle, uint32_t Resolution, IsiAfpsInfo_t* pAfpsInfo);
static RESULT S5K4EC_IsiGetGainIss( IsiSensorHandle_t handle, float *pSetGain );
static RESULT S5K4EC_IsiGetGainIncrementIss( IsiSensorHandle_t handle, float *pIncr );
static RESULT S5K4EC_IsiSetGainIss( IsiSensorHandle_t handle, float NewGain, float *pSetGain );
static RESULT S5K4EC_IsiGetIntegrationTimeIss( IsiSensorHandle_t handle, float *pSetIntegrationTime );
static RESULT S5K4EC_IsiGetIntegrationTimeIncrementIss( IsiSensorHandle_t handle, float *pIncr );
static RESULT S5K4EC_IsiSetIntegrationTimeIss( IsiSensorHandle_t handle, float NewIntegrationTime, float *pSetIntegrationTime, uint8_t *pNumberOfFramesToSkip );
static RESULT S5K4EC_IsiGetResolutionIss( IsiSensorHandle_t handle, uint32_t *pSetResolution );


static RESULT S5K4EC_IsiRegReadIss( IsiSensorHandle_t handle, const uint32_t address, uint32_t *p_value );
static RESULT S5K4EC_IsiRegWriteIss( IsiSensorHandle_t handle, const uint32_t address, const uint32_t value );

static RESULT S5K4EC_IsiGetCalibKFactor( IsiSensorHandle_t handle, Isi1x1FloatMatrix_t **pIsiKFactor );
static RESULT S5K4EC_IsiGetCalibPcaMatrix( IsiSensorHandle_t   handle, Isi3x2FloatMatrix_t **pIsiPcaMatrix );
static RESULT S5K4EC_IsiGetCalibSvdMeanValue( IsiSensorHandle_t   handle, Isi3x1FloatMatrix_t **pIsiSvdMeanValue );
static RESULT S5K4EC_IsiGetCalibCenterLine( IsiSensorHandle_t   handle, IsiLine_t  **ptIsiCenterLine);
static RESULT S5K4EC_IsiGetCalibClipParam( IsiSensorHandle_t   handle, IsiAwbClipParm_t    **pIsiClipParam );
static RESULT S5K4EC_IsiGetCalibGlobalFadeParam( IsiSensorHandle_t       handle, IsiAwbGlobalFadeParm_t  **ptIsiGlobalFadeParam);
static RESULT S5K4EC_IsiGetCalibFadeParam( IsiSensorHandle_t   handle, IsiAwbFade2Parm_t   **ptIsiFadeParam);
static RESULT S5K4EC_IsiGetIlluProfile( IsiSensorHandle_t   handle, const uint32_t CieProfile, IsiIlluProfile_t **ptIsiIlluProfile );

static RESULT S5K4EC_IsiMdiInitMotoDriveMds( IsiSensorHandle_t handle );
static RESULT S5K4EC_IsiMdiSetupMotoDrive( IsiSensorHandle_t handle, uint32_t *pMaxStep );
static RESULT S5K4EC_IsiMdiFocusSet( IsiSensorHandle_t handle, const uint32_t Position );
static RESULT S5K4EC_IsiMdiFocusGet( IsiSensorHandle_t handle, uint32_t *pAbsStep );
static RESULT S5K4EC_IsiMdiFocusCalibrate( IsiSensorHandle_t handle );

static RESULT S5K4EC_IsiGetSensorMipiInfoIss( IsiSensorHandle_t handle, IsiSensorMipiInfo *ptIsiSensorMipiInfo);
static RESULT S5K4EC_IsiGetSensorIsiVersion(  IsiSensorHandle_t   handle, unsigned int* pVersion);

/*****************************************************************************/
/**
 *          S5K4EC_IsiCreateSensorIss
 *
 * @brief   This function creates a new S5K4EC sensor instance handle.
 *
 * @param   pConfig     configuration structure to create the instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 * @retval  RET_OUTOFMEM
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiCreateSensorIss
(
    IsiSensorInstanceConfig_t *pConfig
)
{
    RESULT result = RET_SUCCESS;

    S5K4EC_Context_t *pS5K4ECCtx;

    TRACE( S5K4EC_INFO, "%s (---------enter-----------)\n", __FUNCTION__);

    if ( (pConfig == NULL) || (pConfig->pSensor ==NULL) )
    {
        return ( RET_NULL_POINTER );
    }

    pS5K4ECCtx = ( S5K4EC_Context_t * )malloc ( sizeof (S5K4EC_Context_t) );
    if ( pS5K4ECCtx == NULL )
    {
        TRACE( S5K4EC_ERROR,  "%s: Can't allocate S5K4EC context\n",  __FUNCTION__ );
        return ( RET_OUTOFMEM );
    }
    MEMSET( pS5K4ECCtx, 0, sizeof( S5K4EC_Context_t ) );

    result = HalAddRef( pConfig->HalHandle );
    if ( result != RET_SUCCESS )
    {
        free ( pS5K4ECCtx );
        return ( result );
    }

    pS5K4ECCtx->IsiCtx.HalHandle              = pConfig->HalHandle;
    pS5K4ECCtx->IsiCtx.HalDevID               = pConfig->HalDevID;
    pS5K4ECCtx->IsiCtx.I2cBusNum              = pConfig->I2cBusNum;
    pS5K4ECCtx->IsiCtx.SlaveAddress           = ( pConfig->SlaveAddr == 0 ) ? S5K4EC_SLAVE_ADDR : pConfig->SlaveAddr;
    pS5K4ECCtx->IsiCtx.NrOfAddressBytes       = 2U;

    pS5K4ECCtx->IsiCtx.I2cAfBusNum            = 3;//pConfig->I2cAfBusNum;
    pS5K4ECCtx->IsiCtx.SlaveAfAddress         = ( pConfig->SlaveAfAddr == 0 ) ? S5K4EC_SLAVE_AF_ADDR : pConfig->SlaveAfAddr;
    pS5K4ECCtx->IsiCtx.NrOfAfAddressBytes     = 2U;

    pS5K4ECCtx->IsiCtx.pSensor                = pConfig->pSensor;

    pS5K4ECCtx->Configured             = BOOL_FALSE;
    pS5K4ECCtx->Streaming              = BOOL_FALSE;
    pS5K4ECCtx->TestPattern            = BOOL_FALSE;
    pS5K4ECCtx->isAfpsRun              = BOOL_FALSE;

    pConfig->hSensor = ( IsiSensorHandle_t )pS5K4ECCtx;

    result = HalSetCamConfig( pS5K4ECCtx->IsiCtx.HalHandle, pS5K4ECCtx->IsiCtx.HalDevID, true, true, false );
    RETURN_RESULT_IF_DIFFERENT( RET_SUCCESS, result );

    result = HalSetClock( pS5K4ECCtx->IsiCtx.HalHandle, pS5K4ECCtx->IsiCtx.HalDevID, 10000000U);
    RETURN_RESULT_IF_DIFFERENT( RET_SUCCESS, result );

    TRACE( S5K4EC_INFO, "%s (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiReleaseSensorIss
 *
 * @brief   This function destroys/releases an S5K4EC sensor instance.
 *
 * @param   handle      S5K4EC sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiReleaseSensorIss
(
    IsiSensorHandle_t handle
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    (void)S5K4EC_IsiSensorSetStreamingIss( pS5K4ECCtx, BOOL_FALSE );
    (void)S5K4EC_IsiSensorSetPowerIss( pS5K4ECCtx, BOOL_FALSE );

    (void)HalDelRef( pS5K4ECCtx->IsiCtx.HalHandle );

    MEMSET( pS5K4ECCtx, 0, sizeof( S5K4EC_Context_t ) );
    free ( pS5K4ECCtx );

    TRACE( S5K4EC_INFO, "%s (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetCapsIss
 *
 * @brief   fills in the correct pointers for the sensor description struct
 *
 * @param   param1      pointer to sensor capabilities structure
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/

static RESULT S5K4EC_IsiGetCapsIssInternal
(
    IsiSensorCaps_t   *pIsiSensorCaps    
)
{

    RESULT result = RET_SUCCESS;


    if ( pIsiSensorCaps == NULL )
    {
        return ( RET_NULL_POINTER );
    }
    else
    {
	ALOGD("lkd ------>>> Index = %d", pIsiSensorCaps->Index);
        switch (pIsiSensorCaps->Index) 
        {
            case 0:
            {
                //pIsiSensorCaps->Resolution = ISI_RES_2592_1944P7;
		        pIsiSensorCaps->Resolution = ISI_RES_TV1080P15;
                //pIsiSensorCaps->Resolution = ISI_RES_TV720P30;
                //pIsiSensorCaps->Resolution = ISI_RES_SVGAP15;
                break;
            }
            case 1:
            {
                //remove svga,preview is from 720p.just for increase fps.
                pIsiSensorCaps->Resolution = ISI_RES_TV720P30;
                //pIsiSensorCaps->Resolution = ISI_RES_SVGAP15;
                break;
            }
            default:
            {
                result = RET_OUTOFRANGE;
                goto end;
            }

        }
    
        pIsiSensorCaps->BusWidth        = ISI_BUSWIDTH_12BIT;
        pIsiSensorCaps->Mode            = ISI_MODE_PICT|ISI_MODE_BT601;
        pIsiSensorCaps->FieldSelection  = ISI_FIELDSEL_BOTH;
        pIsiSensorCaps->YCSequence      = ISI_YCSEQ_YCBYCR|ISI_YCSEQ_CBYCRY;           
        pIsiSensorCaps->Conv422         = ISI_CONV422_NOCOSITED;
        pIsiSensorCaps->BPat            = ISI_BPAT_RGRGGBGB ;
        pIsiSensorCaps->HPol            = ISI_HPOL_REFPOS;
        pIsiSensorCaps->VPol            = ISI_VPOL_NEG;
        pIsiSensorCaps->Edge            = ISI_EDGE_RISING;
        pIsiSensorCaps->Bls             = ISI_BLS_OFF;
        pIsiSensorCaps->Gamma           = ISI_GAMMA_ON;
        pIsiSensorCaps->CConv           = ISI_CCONV_ON;


        pIsiSensorCaps->BLC             = ( ISI_BLC_AUTO );
        pIsiSensorCaps->AGC             = ( ISI_AGC_AUTO );
        pIsiSensorCaps->AWB             = ( ISI_AWB_AUTO );
        pIsiSensorCaps->AEC             = ( ISI_AEC_AUTO );
        pIsiSensorCaps->DPCC            = ( ISI_DPCC_AUTO );

        pIsiSensorCaps->DwnSz           = ISI_DWNSZ_SUBSMPL;
        pIsiSensorCaps->CieProfile      = 0;
        pIsiSensorCaps->SmiaMode        = ISI_SMIA_OFF;
        pIsiSensorCaps->MipiMode        = ISI_MIPI_OFF;
        pIsiSensorCaps->AfpsResolutions = ( ISI_AFPS_NOTSUPP );
        pIsiSensorCaps->SensorOutputMode = ISI_SENSOR_OUTPUT_MODE_YUV;
    }
end:

    return ( result );
}

 
static RESULT S5K4EC_IsiGetCapsIss
(
    IsiSensorHandle_t handle,
    IsiSensorCaps_t   *pIsiSensorCaps
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    result = S5K4EC_IsiGetCapsIssInternal(pIsiSensorCaps);
    TRACE( S5K4EC_INFO, "%s (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_g_IsiSensorDefaultConfig
 *
 * @brief   recommended default configuration for application use via call
 *          to IsiGetSensorIss()
 *
 *****************************************************************************/
const IsiSensorCaps_t S5K4EC_g_IsiSensorDefaultConfig =
{
    ISI_BUSWIDTH_12BIT,         // BusWidth
    ISI_MODE_BT601,              // MIPI
    ISI_FIELDSEL_BOTH,          // FieldSel
    ISI_YCSEQ_CBYCRY,           // YCSeq
    ISI_CONV422_NOCOSITED,      // Conv422
    ISI_BPAT_RGRGGBGB,//ISI_BPAT_BGBGGRGR,          // BPat
    ISI_HPOL_REFPOS,            // HPol
    ISI_VPOL_NEG,               // VPol
    ISI_EDGE_RISING, //ISI_EDGE_RISING,            // Edge
    ISI_BLS_OFF,                // Bls
    ISI_GAMMA_ON,              // Gamma
    ISI_CCONV_ON,              // CConv
    ISI_RES_SVGAP15,          // Res
    ISI_DWNSZ_SUBSMPL,          // DwnSz
    ISI_BLC_AUTO,               // BLC
    ISI_AGC_AUTO,                // AGC
    ISI_AWB_AUTO,                // AWB
    ISI_AEC_AUTO,                // AEC
    ISI_DPCC_AUTO,               // DPCC
    0,            // CieProfile, this is also used as start profile for AWB (if not altered by menu settings)
    ISI_SMIA_OFF,               // SmiaMode
    ISI_MIPI_OFF,       // MipiMode
    ISI_AFPS_NOTSUPP,           // AfpsResolutions
    ISI_SENSOR_OUTPUT_MODE_YUV,
    0,
};



/*****************************************************************************/
/**
 *          S5K4EC_SetupOutputFormat
 *
 * @brief   Setup of the image sensor considering the given configuration.
 *
 * @param   handle      S5K4EC sensor instance handle
 * @param   pConfig     pointer to sensor configuration structure
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT S5K4EC_SetupOutputFormat
(
    S5K4EC_Context_t       *pS5K4ECCtx,
    const IsiSensorConfig_t *pConfig
)
{
    RESULT result = RET_SUCCESS;
    return result;

    TRACE( S5K4EC_INFO, "%s%s (enter)\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"" );

    /* bus-width */
    switch ( pConfig->BusWidth )        /* only ISI_BUSWIDTH_12BIT supported, no configuration needed here */
    {
        case ISI_BUSWIDTH_10BIT:
        case ISI_BUSWIDTH_8BIT_ZZ:
        case ISI_BUSWIDTH_12BIT:
        {
            break;
        }

        default:
        {
            TRACE( S5K4EC_ERROR, "%s%s: bus width not supported\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"" );
            return ( RET_NOTSUPP );
        }
    }

    /* mode */
    switch ( pConfig->Mode )            /* only ISI_MODE_BAYER supported, no configuration needed here */
    {
        case( ISI_MODE_MIPI ):
        case ISI_MODE_BAYER:
        case ISI_MODE_BT601:
        case ISI_MODE_PICT:
        case  ISI_MODE_DATA:
        {
            break;
        }

        default:
        {
            TRACE( S5K4EC_ERROR, "%s%s: mode not supported\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"" );
            return ( RET_NOTSUPP );
        }
    }

    /* field-selection */
    switch ( pConfig->FieldSelection )  /* only ISI_FIELDSEL_BOTH supported, no configuration needed */
    {
        case ISI_FIELDSEL_BOTH:
        {
            break;
        }

        default:
        {
            TRACE( S5K4EC_ERROR, "%s%s: field selection not supported\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"" );
            return ( RET_NOTSUPP );
        }
    }

    switch ( pConfig->YCSequence )
    {
        default:
        {
            break;
        }
    }

    /* 422 conversion */
    switch ( pConfig->Conv422 )         /* only ISI_CONV422_NOCOSITED supported, no configuration needed */
    {
        case ISI_CONV422_NOCOSITED:
        case ISI_CONV422_COSITED:
        case ISI_CONV422_INTER:
        {
            break;
        }

        default:
        {
            TRACE( S5K4EC_ERROR, "%s%s: 422 conversion not supported\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"" );
            return ( RET_NOTSUPP );
        }
    }

    /* bayer-pattern */
    switch ( pConfig->BPat )            /* only ISI_BPAT_BGBGGRGR supported, no configuration needed */
    {
        case ISI_BPAT_BGBGGRGR:
        case ISI_BPAT_RGRGGBGB:
        {
            break;
        }

        default:
        {
            TRACE( S5K4EC_ERROR, "%s%s: bayer pattern not supported\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"" );
            return ( RET_NOTSUPP );
        }
    }

    /* horizontal polarity */
    switch ( pConfig->HPol )            /* only ISI_HPOL_REFPOS supported, no configuration needed */
    {
        case ISI_HPOL_REFPOS:
        {
            break;
        }

        default:
        {
            TRACE( S5K4EC_ERROR, "%s%s: HPol not supported\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"" );
            return ( RET_NOTSUPP );
        }
    }

    /* vertical polarity */
    switch ( pConfig->VPol )            /* only ISI_VPOL_NEG supported, no configuration needed */
    {
        case ISI_VPOL_NEG:
        {
            break;
        }

        default:
        {
            TRACE( S5K4EC_ERROR, "%s%s: VPol not supported\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"" );
            return ( RET_NOTSUPP );

        }
    }


    /* edge */
    switch ( pConfig->Edge )            /* only ISI_EDGE_RISING supported, no configuration needed */
    {
        case ISI_EDGE_RISING:
        {
            break;
        }

        case ISI_EDGE_FALLING:          /*TODO for MIPI debug*/
        {
            break;
        }

        default:
        {
            TRACE( S5K4EC_ERROR, "%s%s:  edge mode not supported\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"" );
            return ( RET_NOTSUPP );
        }
    }

    /* gamma */
    switch ( pConfig->Gamma )           /* only ISI_GAMMA_OFF supported, no configuration needed */
    {
        case ISI_GAMMA_ON:
        case ISI_GAMMA_OFF:
        {
            break;
        }

        default:
        {
            TRACE( S5K4EC_ERROR, "%s%s:  gamma not supported\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"" );
            return ( RET_NOTSUPP );
        }
    }

    /* color conversion */
    switch ( pConfig->CConv )           /* only ISI_CCONV_OFF supported, no configuration needed */
    {
        case ISI_CCONV_OFF:
        case ISI_CCONV_ON:
        {
            break;
        }

        default:
        {
            TRACE( S5K4EC_ERROR, "%s%s: color conversion not supported\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"" );
            return ( RET_NOTSUPP );
        }
    }

    switch ( pConfig->SmiaMode )        /* only ISI_SMIA_OFF supported, no configuration needed */
    {
        case ISI_SMIA_OFF:
        {
            break;
        }

        default:
        {
            TRACE( S5K4EC_ERROR, "%s%s: SMIA mode not supported\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"" );
            return ( RET_NOTSUPP );
        }
    }

    switch ( pConfig->MipiMode )        /* only ISI_MIPI_MODE_RAW_12 supported, no configuration needed */
    {
        case ISI_MIPI_MODE_RAW_10:
        case ISI_MIPI_OFF:
        {
            break;
        }

        default:
        {
            TRACE( S5K4EC_ERROR, "%s%s: MIPI mode not supported\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"" );
            return ( RET_NOTSUPP );
        }
    }

    switch ( pConfig->AfpsResolutions ) /* no configuration needed */
    {
        case ISI_AFPS_NOTSUPP:
        {
            break;
        }
        default:
        {
            // don't care about what comes in here
            //TRACE( S5K4EC_ERROR, "%s%s: AFPS not supported\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"" );
            //return ( RET_NOTSUPP );
        }
    }

    TRACE( S5K4EC_INFO, "%s%s (exit)\n", __FUNCTION__, pS5K4ECCtx->isAfpsRun?"(AFPS)":"");

    return ( result );
}

int S5K4EC_get_PCLK( S5K4EC_Context_t *pS5K4ECCtx, int XVCLK)
{
    // calculate PCLK
    
    return 0;
}

/*****************************************************************************/
/**
 *          S5K4EC_SetupOutputWindow
 *
 * @brief   Setup of the image sensor considering the given configuration.
 *
 * @param   handle      S5K4EC sensor instance handle
 * @param   pConfig     pointer to sensor configuration structure
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_SetupOutputWindow
(
    S5K4EC_Context_t        *pS5K4ECCtx,
    const IsiSensorConfig_t *pConfig
)
{
    RESULT result     = RET_SUCCESS;
    static uint32_t oldRes;

        /* resolution */
    switch ( pConfig->Resolution )
    {
        case ISI_RES_SVGAP15:
        {
            if((result = IsiRegDefaultsApply((IsiSensorHandle_t)pS5K4ECCtx,S5K4EC_g_svga)) != RET_SUCCESS){
                TRACE( S5K4EC_ERROR, "%s: failed to set  ISI_RES_SVGAP30 \n", __FUNCTION__ );
            }else{

                TRACE( S5K4EC_INFO, "%s: success to set  ISI_RES_SVGAP30 \n", __FUNCTION__ );
            }
            break;
        }
        /*case ISI_RES_1600_1200:
        {
            if((result = IsiRegDefaultsApply((IsiSensorHandle_t)pS5K4ECCtx,S5K4EC_g_1600x1200)) != RET_SUCCESS){
                TRACE( S5K4EC_ERROR, "%s: failed to set  ISI_RES_1600_1200 \n", __FUNCTION__ );
            }else{

                TRACE( S5K4EC_INFO, "%s: success to set  ISI_RES_1600_1200  \n", __FUNCTION__ );
            }
            break;
        }*/
        case ISI_RES_TV720P30:
        {
            if((result = IsiRegDefaultsApply((IsiSensorHandle_t)pS5K4ECCtx,S5K4EC_g_video_720p)) != RET_SUCCESS){
                TRACE( S5K4EC_ERROR, "%s: failed to set  ISI_RES_TV720P30 \n", __FUNCTION__ );
            }else{

                TRACE( S5K4EC_INFO, "%s: success to set  ISI_RES_TV720P30  \n", __FUNCTION__ );
            }
            break;
        }
	case ISI_RES_TV1080P15:
	{
            if((result = IsiRegDefaultsApply((IsiSensorHandle_t)pS5K4ECCtx, S5K4EC_g_1080p)) != RET_SUCCESS){
                TRACE( S5K4EC_ERROR, "%s: failed to set  ISI_RES_TV1080P \n", __FUNCTION__ );
            }else{

                TRACE( S5K4EC_INFO, "%s: success to set  ISI_RES_TV1080P  \n", __FUNCTION__ );
            }
            break;
	}
	case ISI_RES_2592_1944P7:
        {
#if 0
          //  if(oldRes == ISI_RES_TV720P30){
                if((result = IsiRegDefaultsApply((IsiSensorHandle_t)pS5K4ECCtx,S5K4EC_g_video_720p)) != RET_SUCCESS){
                    TRACE( S5K4EC_ERROR, "%s: failed to set  ISI_RES_SVGAP30 \n", __FUNCTION__ );
                }else{
                    TRACE( S5K4EC_INFO, "%s: success to set  ISI_RES_SVGAP30 \n", __FUNCTION__ );
                }
          //  }
#endif
                
#if 1
            if((result = IsiRegDefaultsApply((IsiSensorHandle_t)pS5K4ECCtx,S5K4EC_g_2592x1944)) != RET_SUCCESS){
                TRACE( S5K4EC_ERROR, "%s: failed to set  ISI_RES_2592_1944P7 \n", __FUNCTION__ );
            }else{

                TRACE( S5K4EC_INFO, "%s: success to set  ISI_RES_2592_1944P7  \n", __FUNCTION__ );
            }
#endif
            break;
        }
        default:
        {
            TRACE( S5K4EC_ERROR, "%s: Resolution not supported\n", __FUNCTION__ );
            return ( RET_NOTSUPP );
        }
    }

    oldRes = pConfig->Resolution;

    return ( result );
}




/*****************************************************************************/
/**
 *          S5K4EC_SetupImageControl
 *
 * @brief   Sets the image control functions (BLC, AGC, AWB, AEC, DPCC ...)
 *
 * @param   handle      S5K4EC sensor instance handle
 * @param   pConfig     pointer to sensor configuration structure
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT S5K4EC_SetupImageControl
(
    S5K4EC_Context_t        *pS5K4ECCtx,
    const IsiSensorConfig_t *pConfig
)
{
    RESULT result = RET_SUCCESS;

    uint32_t RegValue = 0U;

    TRACE( S5K4EC_INFO, "%s (enter)\n", __FUNCTION__);


    return ( result );
}


/*****************************************************************************/
/**
 *          S5K4EC_AecSetModeParameters
 *
 * @brief   This function fills in the correct parameters in S5K4EC-Instances
 *          according to AEC mode selection in IsiSensorConfig_t.
 *
 * @note    It is assumed that IsiSetupOutputWindow has been called before
 *          to fill in correct values in instance structure.
 *
 * @param   handle      S5K4EC context
 * @param   pConfig     pointer to sensor configuration structure
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_AecSetModeParameters
(
    S5K4EC_Context_t       *pS5K4ECCtx,
    const IsiSensorConfig_t *pConfig
)
{
    RESULT result = RET_SUCCESS;

    return ( result );
}


/*****************************************************************************/
/**
 *          S5K4EC_IsiSetupSensorIss
 *
 * @brief   Setup of the image sensor considering the given configuration.
 *
 * @param   handle      S5K4EC sensor instance handle
 * @param   pConfig     pointer to sensor configuration structure
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiSetupSensorIss
(
    IsiSensorHandle_t       handle,
    const IsiSensorConfig_t *pConfig
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    uint32_t RegValue = 0;

    TRACE( S5K4EC_INFO, "%s (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid sensor handle (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_WRONG_HANDLE );
    }

    if ( pConfig == NULL )
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid configuration (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_NULL_POINTER );
    }

    if ( pS5K4ECCtx->Streaming != BOOL_FALSE )
    {
        return RET_WRONG_STATE;
    }

    MEMCPY( &pS5K4ECCtx->Config, pConfig, sizeof( IsiSensorConfig_t ) );

    /* 2.) write default values derived from datasheet and evaluation kit (static setup altered by dynamic setup further below) */
    result = IsiRegDefaultsApply( pS5K4ECCtx, S5K4EC_g_aRegDescription );
    if ( result != RET_SUCCESS )
    {
        return ( result );
    }
		

    /* sleep a while, that sensor can take over new default values */
    osSleep( 10 );
    #if 0


    /* 3.) verify default values to make sure everything has been written correctly as expected */
    result = IsiRegDefaultsVerify( pS5K4ECCtx, S5K4EC_g_aRegDescription );
    if ( result != RET_SUCCESS )
    {
        return ( result );
    }
    // output of pclk for measurement (only debugging)
    result = S5K4EC_IsiRegWriteIss( pS5K4ECCtx, 0x3009U, 0x10U );
    RETURN_RESULT_IF_DIFFERENT( RET_SUCCESS, result );
    #endif

    /* 4.) setup output format (RAW10|RAW12) */
    result = S5K4EC_SetupOutputFormat( pS5K4ECCtx, pConfig );
    if ( result != RET_SUCCESS )
    {
        TRACE( S5K4EC_ERROR, "%s: SetupOutputFormat failed.\n", __FUNCTION__);
        return ( result );
    }

    /* 5.) setup output window */
    result = S5K4EC_SetupOutputWindow( pS5K4ECCtx, pConfig );
    if ( result != RET_SUCCESS )
    {
        TRACE( S5K4EC_ERROR, "%s: SetupOutputWindow failed.\n", __FUNCTION__);
        return ( result );
    }

    result = S5K4EC_SetupImageControl( pS5K4ECCtx, pConfig );
    if ( result != RET_SUCCESS )
    {
        TRACE( S5K4EC_ERROR, "%s: SetupImageControl failed.\n", __FUNCTION__);
        return ( result );
    }

    result = S5K4EC_AecSetModeParameters( pS5K4ECCtx, pConfig );
    if ( result != RET_SUCCESS )
    {
        TRACE( S5K4EC_ERROR, "%s: AecSetModeParameters failed.\n", __FUNCTION__);
        return ( result );
    }
#if 0
    result = IsiRegDefaultsApply( pS5K4ECCtx, S5K4EC_af_firmware);
    if ( result != RET_SUCCESS )
    {
        TRACE( S5K4EC_ERROR, "%s: Download S5K4EC_af_firmware failed\n", __FUNCTION__ );
        //return ( result );
    }
    
    result = IsiRegDefaultsApply( pS5K4ECCtx, S5K4EC_af_init);
    if ( result != RET_SUCCESS )
    {
        TRACE( S5K4EC_ERROR, "%s: Download S5K4EC_af_init failed\n", __FUNCTION__ );
        //return ( result );
    }
    
    //osSleep( 1000 );
    uint32_t value;
    result = IsiReadRegister( pS5K4ECCtx, 0x3023, &value );
    if(value != 0x0){
    		TRACE( S5K4EC_ERROR, "%s: value:%d ;read TAG failed\n", __FUNCTION__,value);
    }
    
#endif
    if (result == RET_SUCCESS)
    {
        pS5K4ECCtx->Configured = BOOL_TRUE;
    }
    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiChangeSensorResolutionIss
 *
 * @brief   Change image sensor resolution while keeping all other static settings.
 *          Dynamic settings like current gain & integration time are kept as
 *          close as possible. Sensor needs 2 frames to engage (first 2 frames
 *          are not correctly exposed!).
 *
 * @note    Re-read current & min/max values as they will probably have changed!
 *
 * @param   handle                  Sensor instance handle
 * @param   Resolution              new resolution ID
 * @param   pNumberOfFramesToSkip   reference to storage for number of frames to skip
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_WRONG_STATE
 * @retval  RET_OUTOFRANGE
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiChangeSensorResolutionIss
(
    IsiSensorHandle_t   handle,
    uint32_t            Resolution,
    uint8_t             *pNumberOfFramesToSkip
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if (pNumberOfFramesToSkip == NULL)
    {
        return ( RET_NULL_POINTER );
    }

    if ( (pS5K4ECCtx->Configured != BOOL_TRUE) || (pS5K4ECCtx->Streaming != BOOL_FALSE) )
    {
        return RET_WRONG_STATE;
    }

    IsiSensorCaps_t Caps;    
    Caps.Index = 0;
    Caps.Resolution = 0;
    while (S5K4EC_IsiGetCapsIss( handle, &Caps) == RET_SUCCESS) {
        if (Resolution == Caps.Resolution) {            
            break;
        }
        Caps.Index++;
    }

    if ( (Resolution & Caps.Resolution) == 0 )
    {
        return RET_OUTOFRANGE;
    }

    if ( Resolution == pS5K4ECCtx->Config.Resolution )
    {
        // well, no need to worry
        *pNumberOfFramesToSkip = 0;
    }
    else
    {
        // change resolution
        char *szResName = NULL;
        result = IsiGetResolutionName( Resolution, &szResName );
        TRACE( S5K4EC_DEBUG, "%s: NewRes=0x%08x (%s)\n", __FUNCTION__, Resolution, szResName);

        // update resolution in copy of config in context
        pS5K4ECCtx->Config.Resolution = Resolution;

        // tell sensor about that
        result = S5K4EC_SetupOutputWindow( pS5K4ECCtx, &pS5K4ECCtx->Config );
        if ( result != RET_SUCCESS )
        {
            TRACE( S5K4EC_ERROR, "%s: SetupOutputWindow failed.\n", __FUNCTION__);
            return ( result );
        }

        // remember old exposure values
        float OldGain = pS5K4ECCtx->AecCurGain;
        float OldIntegrationTime = pS5K4ECCtx->AecCurIntegrationTime;

        // update limits & stuff (reset current & old settings)
        result = S5K4EC_AecSetModeParameters( pS5K4ECCtx, &pS5K4ECCtx->Config );
        if ( result != RET_SUCCESS )
        {
            TRACE( S5K4EC_ERROR, "%s: AecSetModeParameters failed.\n", __FUNCTION__);
            return ( result );
        }

        // restore old exposure values (at least within new exposure values' limits)
        uint8_t NumberOfFramesToSkip = 0;
        float   DummySetGain;
        float   DummySetIntegrationTime;
        result = S5K4EC_IsiExposureControlIss( handle, OldGain, OldIntegrationTime, &NumberOfFramesToSkip, &DummySetGain, &DummySetIntegrationTime );
        if ( result != RET_SUCCESS )
        {
            TRACE( S5K4EC_ERROR, "%s: S5K4EC_IsiExposureControlIss failed.\n", __FUNCTION__);
            return ( result );
        }

        // return number of frames that aren't exposed correctly
        *pNumberOfFramesToSkip = 0;//NumberOfFramesToSkip + 1;
    }

    TRACE( S5K4EC_INFO, "%s (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiSensorSetStreamingIss
 *
 * @brief   Enables/disables streaming of sensor data, if possible.
 *
 * @param   handle      Sensor instance handle
 * @param   on          new streaming state (BOOL_TRUE=on, BOOL_FALSE=off)
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_WRONG_STATE
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiSensorSetStreamingIss
(
    IsiSensorHandle_t   handle,
    bool_t              on
)
{
    uint32_t RegValue = 0;

    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if ( (pS5K4ECCtx->Configured != BOOL_TRUE) || (pS5K4ECCtx->Streaming == on) )
    {
        return RET_WRONG_STATE;
    }

    if (on == BOOL_TRUE)
    {
        /* enable streaming */
        // hkw add;
        //result = S5K4EC_IsiRegWriteIss( handle, 0x0100, 0x1);
       result = RET_SUCCESS;

    }
    else
    {
        /* disable streaming */
        //result = S5K4EC_IsiRegWriteIss( handle, 0x0100, 0x0);
				result = RET_SUCCESS;

    }

    if (result == RET_SUCCESS)
    {
        pS5K4ECCtx->Streaming = on;
    }

    TRACE( S5K4EC_INFO, "%s (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiSensorSetPowerIss
 *
 * @brief   Performs the power-up/power-down sequence of the camera, if possible.
 *
 * @param   handle      S5K4EC sensor instance handle
 * @param   on          new power state (BOOL_TRUE=on, BOOL_FALSE=off)
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiSensorSetPowerIss
(
    IsiSensorHandle_t   handle,
    bool_t              on
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    pS5K4ECCtx->Configured = BOOL_FALSE;
    pS5K4ECCtx->Streaming  = BOOL_FALSE;

    TRACE( S5K4EC_DEBUG, "%s power off \n", __FUNCTION__);
    result = HalSetPower( pS5K4ECCtx->IsiCtx.HalHandle, pS5K4ECCtx->IsiCtx.HalDevID, false );
    RETURN_RESULT_IF_DIFFERENT( RET_SUCCESS, result );

    TRACE( S5K4EC_DEBUG, "%s reset on\n", __FUNCTION__);
    result = HalSetReset( pS5K4ECCtx->IsiCtx.HalHandle, pS5K4ECCtx->IsiCtx.HalDevID, true );
    RETURN_RESULT_IF_DIFFERENT( RET_SUCCESS, result );

    if (on == BOOL_TRUE)
    {
        TRACE( S5K4EC_DEBUG, "%s power on \n", __FUNCTION__);
        result = HalSetPower( pS5K4ECCtx->IsiCtx.HalHandle, pS5K4ECCtx->IsiCtx.HalDevID, true );
        RETURN_RESULT_IF_DIFFERENT( RET_SUCCESS, result );

        osSleep( 20 );

        TRACE( S5K4EC_DEBUG, "%s reset off \n", __FUNCTION__);
        result = HalSetReset( pS5K4ECCtx->IsiCtx.HalHandle, pS5K4ECCtx->IsiCtx.HalDevID, false );
        RETURN_RESULT_IF_DIFFERENT( RET_SUCCESS, result );

        osSleep( 10 );

        TRACE( S5K4EC_DEBUG, "%s reset on \n", __FUNCTION__);
        result = HalSetReset( pS5K4ECCtx->IsiCtx.HalHandle, pS5K4ECCtx->IsiCtx.HalDevID, true );
        RETURN_RESULT_IF_DIFFERENT( RET_SUCCESS, result );

        osSleep( 20 );

        TRACE( S5K4EC_DEBUG, "%s reset off \n", __FUNCTION__);
        result = HalSetReset( pS5K4ECCtx->IsiCtx.HalHandle, pS5K4ECCtx->IsiCtx.HalDevID, false );

        osSleep( 50 );
    }

    TRACE( S5K4EC_INFO, "%s (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiCheckSensorConnectionIss
 *
 * @brief   Checks the I2C-Connection to sensor by reading sensor revision id.
 *
 * @param   handle      S5K4EC sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiCheckSensorConnectionIss
(
    IsiSensorHandle_t   handle
)
{
    uint32_t RevId;
    uint32_t value;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s (enter)\n", __FUNCTION__);

    if ( handle == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    return ( result );

    RevId = S5K4EC_CHIP_ID_HIGH_BYTE_DEFAULT;
    //RevId = (RevId << 16U) | (S5K4EC_CHIP_ID_MIDDLE_BYTE_DEFAULT<<8U);
    //RevId = RevId | S5K4EC_CHIP_ID_LOW_BYTE_DEFAULT;

    result = S5K4EC_IsiGetSensorRevisionIss( handle, &value );
    //if ( (result != RET_SUCCESS) || (RevId != value) )
    if ( (result != RET_SUCCESS) || ((RevId & 0x00ffff) != (value & 0x00ffff)) )
    {
        TRACE( S5K4EC_ERROR, "%s RevId = 0x%08x, value = 0x%08x \n", __FUNCTION__, RevId, value );
        return ( RET_FAILURE );
    }

    TRACE( S5K4EC_DEBUG, "%s RevId = 0x%08x, value = 0x%08x \n", __FUNCTION__, RevId, value );

    TRACE( S5K4EC_INFO, "%s (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetSensorRevisionIss
 *
 * @brief   reads the sensor revision register and returns this value
 *
 * @param   handle      pointer to sensor description struct
 * @param   p_value     pointer to storage value
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiGetSensorRevisionIss
(
    IsiSensorHandle_t   handle,
    uint32_t            *p_value
)
{
    RESULT result = RET_SUCCESS;

    uint32_t data;

    TRACE( S5K4EC_INFO, "%s (enter)\n", __FUNCTION__);

    if ( handle == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if ( p_value == NULL )
    {
        return ( RET_NULL_POINTER );
    }

    *p_value = 0U;
    result = S5K4EC_IsiRegReadIss ( handle, S5K4EC_CHIP_ID_HIGH_BYTE, &data );
    *p_value = data & 0xFFFF;
    /*
    *p_value = ( (data & 0xFF) << 16U );
    result = S5K4EC_IsiRegReadIss ( handle, S5K4EC_CHIP_ID_MIDDLE_BYTE, &data );
    *p_value |= ( (data & 0xFF) << 8U );
    result = S5K4EC_IsiRegReadIss ( handle, S5K4EC_CHIP_ID_LOW_BYTE, &data );
    *p_value |= ( (data & 0xFF));
    */

    TRACE( S5K4EC_INFO, "%s (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiRegReadIss
 *
 * @brief   grants user read access to the camera register
 *
 * @param   handle      pointer to sensor description struct
 * @param   address     sensor register to write
 * @param   p_value     pointer to value
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiRegReadIss
(
    IsiSensorHandle_t   handle,
    const uint32_t      address,
    uint32_t            *p_value
)
{
    RESULT result = RET_SUCCESS;

  //  TRACE( S5K4EC_INFO, "%s (enter)\n", __FUNCTION__);

    if ( handle == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if ( p_value == NULL )
    {
        return ( RET_NULL_POINTER );
    }
    else
    {
        uint8_t NrOfBytes = IsiGetNrDatBytesIss( address, S5K4EC_g_aRegDescription );
        if ( !NrOfBytes )
        {
            NrOfBytes = 1;
        }
 //       TRACE( S5K4EC_REG_DEBUG, "%s (IsiGetNrDatBytesIss %d 0x%08x)\n", __FUNCTION__, NrOfBytes, address);

        *p_value = 0;
        result = IsiI2cReadSensorRegister( handle, address, (uint8_t *)p_value, NrOfBytes, BOOL_TRUE );
    }

  //  TRACE( S5K4EC_INFO, "%s (exit: 0x%08x 0x%08x)\n", __FUNCTION__, address, *p_value);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiRegWriteIss
 *
 * @brief   grants user write access to the camera register
 *
 * @param   handle      pointer to sensor description struct
 * @param   address     sensor register to write
 * @param   value       value to write
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 *
 *****************************************************************************/
static RESULT IsiI2cSwapBytes_zxf
(
    uint8_t         *pData, 
    const uint8_t   NrOfDataBytes 
)
{
    RESULT result = RET_FAILURE;

    if ( pData == NULL  )
    {
        return ( RET_NULL_POINTER );
    }

    switch ( NrOfDataBytes )
    {
        case 1:
            {
                // nothing to do
                result = RET_SUCCESS;
                break;
            }

        case 2:
            {
                uint8_t *pSwapData = pData;
                uint8_t  SwapByte = 0;

                // advance to second byte
                pSwapData++;

                // save first byte
                SwapByte = *(pData);

                // copy second byte to first position
                *pData = *pSwapData;

                // restore first byte to second position
                *pSwapData = SwapByte;

                result = RET_SUCCESS;
                break;
            }

        case 4:
            {
                uint32_t *pSwapData = (uint32_t *)(pData);
                uint32_t  Help = 0UL;

                // get byte 1
                Help = (*pSwapData & 0x000000FF);
                Help <<= 8;
                *pSwapData >>= 8;

                // subjoin byte 2
                Help |= (*pSwapData & 0x000000FF);
                Help <<= 8;
                *pSwapData >>= 8;

                // subjoin byte 3
                Help |= (*pSwapData & 0x000000FF);
                Help <<= 8;
                *pSwapData >>= 8;

                // get byte 4
                *pSwapData  &= 0x000000FF;

                // subjoin bytes 1 to 3
                *pSwapData  |= Help;

                result = RET_SUCCESS;
                break;
            }

        default:
            {
                TRACE( ISI_ERROR, "%s: Wrong amount of bytes (%d) for swapping.\n", __FUNCTION__, NrOfDataBytes );
                break;
            }
    }

    return ( result );
}




static RESULT S5K4EC_IsiRegWriteIss
(
    IsiSensorHandle_t   handle,
    const uint32_t      address,
    const uint32_t      value
)
{
    RESULT result = RET_SUCCESS;

    uint8_t NrOfBytes;
    uint8_t* pValue = (uint8_t *)(&value);

    if ( handle == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }
		S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;
    NrOfBytes = IsiGetNrDatBytesIss( address, S5K4EC_g_aRegDescription );
    if ( !NrOfBytes )
    {
        NrOfBytes = 1;
    }
    //TRACE( S5K4EC_REG_DEBUG, "%s (IsiGetNrDatBytesIss %d 0x%08x 0x%08x)\n", __FUNCTION__, NrOfBytes, address, value);

    NrOfBytes = 2;
    //result = IsiI2cWriteSensorRegister( handle, address, (uint8_t *)(&value), NrOfBytes, BOOL_TRUE );
    //result = IsiI2cWriteSensorRegister( handle, address, (uint8_t *)(&value), NrOfBytes, BOOL_TRUE );
#if 1

        IsiI2cSwapBytes_zxf(pValue, NrOfBytes );
		result = HalWriteI2CMem_Rate( pS5K4ECCtx->IsiCtx.HalHandle,
                             pS5K4ECCtx->IsiCtx.I2cBusNum,
                             pS5K4ECCtx->IsiCtx.SlaveAddress,
                             address,
                             pS5K4ECCtx->IsiCtx.NrOfAddressBytes,
                             pValue,
                             2U,
			     			 200); 
#endif
    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetGainLimitsIss
 *
 * @brief   Returns the exposure minimal and maximal values of an
 *          S5K4EC instance
 *
 * @param   handle       S5K4EC sensor instance handle
 * @param   pMinExposure Pointer to a variable receiving minimal exposure value
 * @param   pMaxExposure Pointer to a variable receiving maximal exposure value
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiGetGainLimitsIss
(
    IsiSensorHandle_t   handle,
    float               *pMinGain,
    float               *pMaxGain
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    uint32_t RegValue = 0;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid sensor handle (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_WRONG_HANDLE );
    }

    if ( (pMinGain == NULL) || (pMaxGain == NULL) )
    {
        TRACE( S5K4EC_ERROR, "%s: NULL pointer received!!\n" );
        return ( RET_NULL_POINTER );
    }

    *pMinGain = pS5K4ECCtx->AecMinGain;
    *pMaxGain = pS5K4ECCtx->AecMaxGain;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetIntegrationTimeLimitsIss
 *
 * @brief   Returns the minimal and maximal integration time values of an
 *          S5K4EC instance
 *
 * @param   handle       S5K4EC sensor instance handle
 * @param   pMinExposure Pointer to a variable receiving minimal exposure value
 * @param   pMaxExposure Pointer to a variable receiving maximal exposure value
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiGetIntegrationTimeLimitsIss
(
    IsiSensorHandle_t   handle,
    float               *pMinIntegrationTime,
    float               *pMaxIntegrationTime
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    uint32_t RegValue = 0;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid sensor handle (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_WRONG_HANDLE );
    }

    if ( (pMinIntegrationTime == NULL) || (pMaxIntegrationTime == NULL) )
    {
        TRACE( S5K4EC_ERROR, "%s: NULL pointer received!!\n" );
        return ( RET_NULL_POINTER );
    }

    *pMinIntegrationTime = pS5K4ECCtx->AecMinIntegrationTime;
    *pMaxIntegrationTime = pS5K4ECCtx->AecMaxIntegrationTime;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    return ( result );
}


/*****************************************************************************/
/**
 *          S5K4EC_IsiGetGainIss
 *
 * @brief   Reads gain values from the image sensor module.
 *
 * @param   handle                  S5K4EC sensor instance handle
 * @param   pSetGain                set gain
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT S5K4EC_IsiGetGainIss
(
    IsiSensorHandle_t   handle,
    float               *pSetGain
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid sensor handle (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_WRONG_HANDLE );
    }

    if ( pSetGain == NULL)
    {
        return ( RET_NULL_POINTER );
    }

    *pSetGain = pS5K4ECCtx->AecCurGain;

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetGainIncrementIss
 *
 * @brief   Get smallest possible gain increment.
 *
 * @param   handle                  S5K4EC sensor instance handle
 * @param   pIncr                   increment
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT S5K4EC_IsiGetGainIncrementIss
(
    IsiSensorHandle_t   handle,
    float               *pIncr
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid sensor handle (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_WRONG_HANDLE );
    }

    if ( pIncr == NULL)
    {
        return ( RET_NULL_POINTER );
    }

    //_smallest_ increment the sensor/driver can handle (e.g. used for sliders in the application)
    *pIncr = pS5K4ECCtx->AecGainIncrement;

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiSetGainIss
 *
 * @brief   Writes gain values to the image sensor module.
 *          Updates current gain and exposure in sensor struct/state.
 *
 * @param   handle                  S5K4EC sensor instance handle
 * @param   NewGain                 gain to be set
 * @param   pSetGain                set gain
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 * @retval  RET_INVALID_PARM
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
RESULT S5K4EC_IsiSetGainIss
(
    IsiSensorHandle_t   handle,
    float               NewGain,
    float               *pSetGain
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    uint16_t usGain = 0;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid sensor handle (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_WRONG_HANDLE );
    }

    if ( pSetGain == NULL)
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid parameter (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_NULL_POINTER );
    }

  
    if( NewGain < pS5K4ECCtx->AecMinGain ) NewGain = pS5K4ECCtx->AecMinGain;
    if( NewGain > pS5K4ECCtx->AecMaxGain ) NewGain = pS5K4ECCtx->AecMaxGain;

    usGain = (uint16_t)NewGain;

    // write new gain into sensor registers, do not write if nothing has changed
    if( (usGain != pS5K4ECCtx->OldGain) )
    {

        pS5K4ECCtx->OldGain = usGain;
    }

    //calculate gain actually set
    pS5K4ECCtx->AecCurGain = NewGain;

    //return current state
    *pSetGain = pS5K4ECCtx->AecCurGain;
    TRACE( S5K4EC_DEBUG, "%s: g=%f\n", __FUNCTION__, *pSetGain );

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetIntegrationTimeIss
 *
 * @brief   Reads integration time values from the image sensor module.
 *
 * @param   handle                  S5K4EC sensor instance handle
 * @param   pSetIntegrationTime     set integration time
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT S5K4EC_IsiGetIntegrationTimeIss
(
    IsiSensorHandle_t   handle,
    float               *pSetIntegrationTime
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid sensor handle (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_WRONG_HANDLE );
    }

    if ( pSetIntegrationTime == NULL )
    {
        return ( RET_NULL_POINTER );
    }

    *pSetIntegrationTime = pS5K4ECCtx->AecCurIntegrationTime;

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetIntegrationTimeIncrementIss
 *
 * @brief   Get smallest possible integration time increment.
 *
 * @param   handle                  S5K4EC sensor instance handle
 * @param   pIncr                   increment
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT S5K4EC_IsiGetIntegrationTimeIncrementIss
(
    IsiSensorHandle_t   handle,
    float               *pIncr
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid sensor handle (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_WRONG_HANDLE );
    }

    if ( pIncr == NULL )
    {
        return ( RET_NULL_POINTER );
    }

    //_smallest_ increment the sensor/driver can handle (e.g. used for sliders in the application)
    *pIncr = pS5K4ECCtx->AecIntegrationTimeIncrement;

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiSetIntegrationTimeIss
 *
 * @brief   Writes gain and integration time values to the image sensor module.
 *          Updates current integration time and exposure in sensor
 *          struct/state.
 *
 * @param   handle                  S5K4EC sensor instance handle
 * @param   NewIntegrationTime      integration time to be set
 * @param   pSetIntegrationTime     set integration time
 * @param   pNumberOfFramesToSkip   number of frames to skip until AE is
 *                                  executed again
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 * @retval  RET_INVALID_PARM
 * @retval  RET_FAILURE
 * @retval  RET_DIVISION_BY_ZERO
 *
 *****************************************************************************/
RESULT S5K4EC_IsiSetIntegrationTimeIss
(
    IsiSensorHandle_t   handle,
    float               NewIntegrationTime,
    float               *pSetIntegrationTime,
    uint8_t             *pNumberOfFramesToSkip
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;


    TRACE( S5K4EC_DEBUG, "%s: Ti=%f\n", __FUNCTION__, *pSetIntegrationTime );
    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}




/*****************************************************************************/
/**
 *          S5K4EC_IsiExposureControlIss
 *
 * @brief   Camera hardware dependent part of the exposure control loop.
 *          Calculates appropriate register settings from the new exposure
 *          values and writes them to the image sensor module.
 *
 * @param   handle                  S5K4EC sensor instance handle
 * @param   NewGain                 newly calculated gain to be set
 * @param   NewIntegrationTime      newly calculated integration time to be set
 * @param   pNumberOfFramesToSkip   number of frames to skip until AE is
 *                                  executed again
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 * @retval  RET_INVALID_PARM
 * @retval  RET_FAILURE
 * @retval  RET_DIVISION_BY_ZERO
 *
 *****************************************************************************/
RESULT S5K4EC_IsiExposureControlIss
(
    IsiSensorHandle_t   handle,
    float               NewGain,
    float               NewIntegrationTime,
    uint8_t             *pNumberOfFramesToSkip,
    float               *pSetGain,
    float               *pSetIntegrationTime
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid sensor handle (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_WRONG_HANDLE );
    }

    if ( (pNumberOfFramesToSkip == NULL)
            || (pSetGain == NULL)
            || (pSetIntegrationTime == NULL) )
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid parameter (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_NULL_POINTER );
    }

    TRACE( S5K4EC_DEBUG, "%s: g=%f, Ti=%f\n", __FUNCTION__, NewGain, NewIntegrationTime );


    result = S5K4EC_IsiSetIntegrationTimeIss( handle, NewIntegrationTime, pSetIntegrationTime, pNumberOfFramesToSkip );
    result = S5K4EC_IsiSetGainIss( handle, NewGain, pSetGain );

    TRACE( S5K4EC_DEBUG, "%s: set: g=%f, Ti=%f, skip=%d\n", __FUNCTION__, *pSetGain, *pSetIntegrationTime, *pNumberOfFramesToSkip );
    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetCurrentExposureIss
 *
 * @brief   Returns the currently adjusted AE values
 *
 * @param   handle                  S5K4EC sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT S5K4EC_IsiGetCurrentExposureIss
(
    IsiSensorHandle_t   handle,
    float               *pSetGain,
    float               *pSetIntegrationTime
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    uint32_t RegValue = 0;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid sensor handle (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_WRONG_HANDLE );
    }

    if ( (pSetGain == NULL) || (pSetIntegrationTime == NULL) )
    {
        return ( RET_NULL_POINTER );
    }

    *pSetGain            = pS5K4ECCtx->AecCurGain;
    *pSetIntegrationTime = pS5K4ECCtx->AecCurIntegrationTime;

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetResolutionIss
 *
 * @brief   Reads integration time values from the image sensor module.
 *
 * @param   handle                  sensor instance handle
 * @param   pSettResolution         set resolution
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT S5K4EC_IsiGetResolutionIss
(
    IsiSensorHandle_t   handle,
    uint32_t            *pSetResolution
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid sensor handle (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_WRONG_HANDLE );
    }

    if ( pSetResolution == NULL )
    {
        return ( RET_NULL_POINTER );
    }

    *pSetResolution = pS5K4ECCtx->Config.Resolution;

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetAfpsInfoHelperIss
 *
 * @brief   Calc AFPS sub resolution settings for the given resolution
 *
 * @param   pS5K4ECCtx             S5K4EC sensor instance (dummy!) context
 * @param   Resolution              Any supported resolution to query AFPS params for
 * @param   pAfpsInfo               Reference of AFPS info structure to write the results to
 * @param   AfpsStageIdx            Index of current AFPS stage to use
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiGetAfpsInfoHelperIss(
    S5K4EC_Context_t   *pS5K4ECCtx,
    uint32_t            Resolution,
    IsiAfpsInfo_t*      pAfpsInfo,
    uint32_t            AfpsStageIdx
)
{
    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    DCT_ASSERT(pS5K4ECCtx != NULL);
    DCT_ASSERT(pAfpsInfo != NULL);
    DCT_ASSERT(AfpsStageIdx <= ISI_NUM_AFPS_STAGES);

    // update resolution in copy of config in context
    pS5K4ECCtx->Config.Resolution = Resolution;

    // tell sensor about that
    result = S5K4EC_SetupOutputWindow( pS5K4ECCtx, &pS5K4ECCtx->Config );
    if ( result != RET_SUCCESS )
    {
        TRACE( S5K4EC_ERROR, "%s: SetupOutputWindow failed for resolution ID %08x.\n", __FUNCTION__, Resolution);
        return ( result );
    }

    // update limits & stuff (reset current & old settings)
    result = S5K4EC_AecSetModeParameters( pS5K4ECCtx, &pS5K4ECCtx->Config );
    if ( result != RET_SUCCESS )
    {
        TRACE( S5K4EC_ERROR, "%s: AecSetModeParameters failed for resolution ID %08x.\n", __FUNCTION__, Resolution);
        return ( result );
    }

    // take over params
    pAfpsInfo->Stage[AfpsStageIdx].Resolution = Resolution;
    pAfpsInfo->Stage[AfpsStageIdx].MaxIntTime = pS5K4ECCtx->AecMaxIntegrationTime;
    pAfpsInfo->AecMinGain           = pS5K4ECCtx->AecMinGain;
    pAfpsInfo->AecMaxGain           = pS5K4ECCtx->AecMaxGain;
    pAfpsInfo->AecMinIntTime        = pS5K4ECCtx->AecMinIntegrationTime;
    pAfpsInfo->AecMaxIntTime        = pS5K4ECCtx->AecMaxIntegrationTime;
    pAfpsInfo->AecSlowestResolution = Resolution;

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}

/*****************************************************************************/
/**
 *          S5K4EC_IsiGetAfpsInfoIss
 *
 * @brief   Returns the possible AFPS sub resolution settings for the given resolution series
 *
 * @param   handle                  S5K4EC sensor instance handle
 * @param   Resolution              Any resolution within the AFPS group to query;
 *                                  0 (zero) to use the currently configured resolution
 * @param   pAfpsInfo               Reference of AFPS info structure to store the results
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 * @retval  RET_NOTSUPP
 *
 *****************************************************************************/
RESULT S5K4EC_IsiGetAfpsInfoIss(
    IsiSensorHandle_t   handle,
    uint32_t            Resolution,
    IsiAfpsInfo_t*      pAfpsInfo
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    uint32_t RegValue = 0;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        TRACE( S5K4EC_ERROR, "%s: Invalid sensor handle (NULL pointer detected)\n", __FUNCTION__ );
        return ( RET_WRONG_HANDLE );
    }

    if ( pAfpsInfo == NULL )
    {
        return ( RET_NULL_POINTER );
    }

    // use currently configured resolution?
    if (Resolution == 0)
    {
        Resolution = pS5K4ECCtx->Config.Resolution;
    }

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetCalibKFactor
 *
 * @brief   Returns the S5K4EC specific K-Factor
 *
 * @param   handle       S5K4EC sensor instance handle
 * @param   pIsiKFactor  Pointer to Pointer receiving the memory address
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiGetCalibKFactor
(
    IsiSensorHandle_t   handle,
    Isi1x1FloatMatrix_t **pIsiKFactor
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if ( pIsiKFactor == NULL )
    {
        return ( RET_NULL_POINTER );
    }

    *pIsiKFactor = NULL;

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}


/*****************************************************************************/
/**
 *          S5K4EC_IsiGetCalibPcaMatrix
 *
 * @brief   Returns the S5K4EC specific PCA-Matrix
 *
 * @param   handle          S5K4EC sensor instance handle
 * @param   pIsiPcaMatrix   Pointer to Pointer receiving the memory address
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiGetCalibPcaMatrix
(
    IsiSensorHandle_t   handle,
    Isi3x2FloatMatrix_t **pIsiPcaMatrix
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if ( pIsiPcaMatrix == NULL )
    {
        return ( RET_NULL_POINTER );
    }

    *pIsiPcaMatrix = NULL;

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetCalibSvdMeanValue
 *
 * @brief   Returns the sensor specific SvdMean-Vector
 *
 * @param   handle              S5K4EC sensor instance handle
 * @param   pIsiSvdMeanValue    Pointer to Pointer receiving the memory address
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiGetCalibSvdMeanValue
(
    IsiSensorHandle_t   handle,
    Isi3x1FloatMatrix_t **pIsiSvdMeanValue
)
{
    IsiSensorContext_t *pSensorCtx = (IsiSensorContext_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pSensorCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if ( pIsiSvdMeanValue == NULL )
    {
        return ( RET_NULL_POINTER );
    }

    *pIsiSvdMeanValue = NULL;

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetCalibSvdMeanValue
 *
 * @brief   Returns a pointer to the sensor specific centerline, a straight
 *          line in Hesse normal form in Rg/Bg colorspace
 *
 * @param   handle              S5K4EC sensor instance handle
 * @param   pIsiSvdMeanValue    Pointer to Pointer receiving the memory address
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiGetCalibCenterLine
(
    IsiSensorHandle_t   handle,
    IsiLine_t           **ptIsiCenterLine
)
{
    IsiSensorContext_t *pSensorCtx = (IsiSensorContext_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pSensorCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if ( ptIsiCenterLine == NULL )
    {
        return ( RET_NULL_POINTER );
    }

    *ptIsiCenterLine = NULL;

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetCalibClipParam
 *
 * @brief   Returns a pointer to the sensor specific arrays for Rg/Bg color
 *          space clipping
 *
 * @param   handle              S5K4EC sensor instance handle
 * @param   pIsiSvdMeanValue    Pointer to Pointer receiving the memory address
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiGetCalibClipParam
(
    IsiSensorHandle_t   handle,
    IsiAwbClipParm_t    **pIsiClipParam
)
{
    IsiSensorContext_t *pSensorCtx = (IsiSensorContext_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pSensorCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if ( pIsiClipParam == NULL )
    {
        return ( RET_NULL_POINTER );
    }

    *pIsiClipParam = NULL;

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetCalibGlobalFadeParam
 *
 * @brief   Returns a pointer to the sensor specific arrays for AWB out of
 *          range handling
 *
 * @param   handle              S5K4EC sensor instance handle
 * @param   pIsiSvdMeanValue    Pointer to Pointer receiving the memory address
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiGetCalibGlobalFadeParam
(
    IsiSensorHandle_t       handle,
    IsiAwbGlobalFadeParm_t  **ptIsiGlobalFadeParam
)
{
    IsiSensorContext_t *pSensorCtx = (IsiSensorContext_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pSensorCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if ( ptIsiGlobalFadeParam == NULL )
    {
        return ( RET_NULL_POINTER );
    }

    *ptIsiGlobalFadeParam = NULL;

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetCalibFadeParam
 *
 * @brief   Returns a pointer to the sensor specific arrays for near white
 *          pixel parameter calculations
 *
 * @param   handle              S5K4EC sensor instance handle
 * @param   pIsiSvdMeanValue    Pointer to Pointer receiving the memory address
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiGetCalibFadeParam
(
    IsiSensorHandle_t   handle,
    IsiAwbFade2Parm_t   **ptIsiFadeParam
)
{
    IsiSensorContext_t *pSensorCtx = (IsiSensorContext_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pSensorCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if ( ptIsiFadeParam == NULL )
    {
        return ( RET_NULL_POINTER );
    }

    *ptIsiFadeParam = NULL;

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}

/*****************************************************************************/
/**
 *          S5K4EC_IsiGetIlluProfile
 *
 * @brief   Returns a pointer to illumination profile idetified by CieProfile
 *          bitmask
 *
 * @param   handle              sensor instance handle
 * @param   CieProfile
 * @param   ptIsiIlluProfile    Pointer to Pointer receiving the memory address
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiGetIlluProfile
(
    IsiSensorHandle_t   handle,
    const uint32_t      CieProfile,
    IsiIlluProfile_t    **ptIsiIlluProfile
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if ( ptIsiIlluProfile == NULL )
    {
        return ( RET_NULL_POINTER );
    }
    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetLscMatrixTable
 *
 * @brief   Returns a pointer to illumination profile idetified by CieProfile
 *          bitmask
 *
 * @param   handle              sensor instance handle
 * @param   CieProfile
 * @param   ptIsiIlluProfile    Pointer to Pointer receiving the memory address
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiGetLscMatrixTable
(
    IsiSensorHandle_t   handle,
    const uint32_t      CieProfile,
    IsiLscMatrixTable_t **pLscMatrixTable
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if ( pLscMatrixTable == NULL )
    {
        return ( RET_NULL_POINTER );
    }
    else

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}


/*****************************************************************************/
/**
 *          S5K4EC_IsiMdiInitMotoDriveMds
 *
 * @brief   General initialisation tasks like I/O initialisation.
 *
 * @param   handle              S5K4EC sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiMdiInitMotoDriveMds
(
    IsiSensorHandle_t   handle
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);
#ifdef SOC_AF
    return RET_SOC_AF;
#else
    return ( result );
#endif
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiMdiSetupMotoDrive
 *
 * @brief   Setup of the MotoDrive and return possible max step.
 *
 * @param   handle          S5K4EC sensor instance handle
 *          pMaxStep        pointer to variable to receive the maximum
 *                          possible focus step
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiMdiSetupMotoDrive
(
    IsiSensorHandle_t   handle,
    uint32_t            *pMaxStep
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if ( pMaxStep == NULL )
    {
        return ( RET_NULL_POINTER );
    }

    *pMaxStep = MAX_LOG;

    //result = S5K4EC_IsiMdiFocusSet( handle, MAX_LOG );

    //return MAX_LOG;
    TRACE( S5K4EC_INFO, "%s:this is SOC camera af (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiMdiFocusSet
 *
 * @brief   Drives the lens system to a certain focus point.
 *
 * @param   handle          S5K4EC sensor instance handle
 *          AbsStep         absolute focus point to apply
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiMdiFocusSet
(
    IsiSensorHandle_t   handle,
    const uint32_t      Position
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;


    RESULT result = RET_SUCCESS;
    int cnt = 0;
    uint32_t data;
    uint8_t ack_cmd[1]={1};    
	TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    /* map 64 to 0 -> infinity */
	TRACE( S5K4EC_INFO, "%s:omy 1 (enter)\n", __FUNCTION__);
    result = HalWriteI2CMem( pS5K4ECCtx->IsiCtx.HalHandle,
                             pS5K4ECCtx->IsiCtx.I2cAfBusNum,
                             pS5K4ECCtx->IsiCtx.SlaveAfAddress,
                             0,
                             pS5K4ECCtx->IsiCtx.NrOfAfAddressBytes,
                             data,
                             2U );
    RETURN_RESULT_IF_DIFFERENT( RET_SUCCESS, result );
	TRACE( S5K4EC_INFO, "%s:omy 2 (enter)\n", __FUNCTION__);

	//osSleep(1000);
	TRACE( S5K4EC_ERROR, "%s: Position:%d\n", __FUNCTION__,Position);
#if 0
	if(Position == 1) { 
		result = HalWriteI2CMem_Rate( pS5K4ECCtx->IsiCtx.HalHandle,
                             pS5K4ECCtx->IsiCtx.I2cAfBusNum,
                             pS5K4ECCtx->IsiCtx.SlaveAfAddress,
                             AF_ACK_Address,
                             pS5K4ECCtx->IsiCtx.NrOfAfAddressBytes,
                             &ack_cmd[0],
                             1U,
							 350);
		result = IsiRegDefaultsApply( pS5K4ECCtx, S5K4EC_af_firmware_new);
		if ( result != RET_SUCCESS )
		{
			TRACE( S5K4EC_ERROR, "%s: Download S5K4EC_af_firmware failed\n", __FUNCTION__ );
			return ( result );
		}

		TRACE( S5K4EC_ERROR, "%s: Download S5K4EC_af_firmware success\n", __FUNCTION__ );
		result = IsiRegDefaultsApply( pS5K4ECCtx, S5K4EC_af_init);
		if ( result != RET_SUCCESS )
		{
			TRACE( S5K4EC_ERROR, "%s: Download S5K4EC_af_init failed\n", __FUNCTION__ );
			return ( result );
		}
	}else if (Position == 2) {

		TRACE( S5K4EC_ERROR, "%s: trigger one shot focus\n", __FUNCTION__ );
		
		result = S5K4EC_IsiRegWriteIss( pS5K4ECCtx, AF_ACK_Address, AF_ACK_VALUE);
		if ( result != RET_SUCCESS )
		{
			TRACE( S5K4EC_ERROR, "%s: trigger one shot focus failed\n", __FUNCTION__ );
			return ( result );
		}
	
		result = S5K4EC_IsiRegWriteIss( pS5K4ECCtx, AF_Address, AF_CMD);
		if ( result != RET_SUCCESS )
		{
			TRACE( S5K4EC_ERROR, "%s: trigger one shot focus failed\n", __FUNCTION__ );
			return ( result );
		}
		do {
			osSleep(10);
			if(S5K4EC_IsiRegReadIss ( pS5K4ECCtx, AF_ACK_Address, &data )){
				TRACE( S5K4EC_ERROR, "%s: zyh read tag fail \n", __FUNCTION__);
			}

			TRACE( S5K4EC_INFO, "%s: zyh read tag fail,data:%d \n", __FUNCTION__,data);
		}while(data!=0x00 && cnt++<100);

		
	}
	else {
		TRACE( S5K4EC_ERROR, "%s: download firmware and one shot focus failed\n", __FUNCTION__ );
	}	
	//osSleep( 100 );
#endif
	TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

	return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiMdiFocusGet
 *
 * @brief   Retrieves the currently applied focus point.
 *
 * @param   handle          S5K4EC sensor instance handle
 *          pAbsStep        pointer to a variable to receive the current
 *                          focus point
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiMdiFocusGet
(
    IsiSensorHandle_t   handle,
    uint32_t            *pAbsStep
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;
    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    if ( pAbsStep == NULL )
    {
        return ( RET_NULL_POINTER );
    }
   /* result = HalReadI2CMem( pS5K4ECCtx->IsiCtx.HalHandle,
                            pS5K4ECCtx->IsiCtx.I2cAfBusNum,
                            pS5K4ECCtx->IsiCtx.SlaveAfAddress,
                            0,
                            pS5K4ECCtx->IsiCtx.NrOfAfAddressBytes,
                            data,
                            2U );
    RETURN_RESULT_IF_DIFFERENT( RET_SUCCESS, result );
   */

    /* Data[0] = PD,  1, D9..D4, see AD5820 datasheet */
    /* Data[1] = D3..D0, S3..S0 */
    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiMdiFocusCalibrate
 *
 * @brief   Triggers a forced calibration of the focus hardware.
 *
 * @param   handle          S5K4EC sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
static RESULT S5K4EC_IsiMdiFocusCalibrate
(
    IsiSensorHandle_t   handle
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiActivateTestPattern
 *
 * @brief   Triggers a forced calibration of the focus hardware.
 *
 * @param   handle          S5K4EC sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 ******************************************************************************/
static RESULT S5K4EC_IsiActivateTestPattern
(
    IsiSensorHandle_t   handle,
    const bool_t        enable
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    return ( result );
}



/*****************************************************************************/
/**
 *          S5K4EC_IsiGetSensorMipiInfoIss
 *
 * @brief   Triggers a forced calibration of the focus hardware.
 *
 * @param   handle          S5K4EC sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 ******************************************************************************/
static RESULT S5K4EC_IsiGetSensorMipiInfoIss
(
    IsiSensorHandle_t   handle,
    IsiSensorMipiInfo   *ptIsiSensorMipiInfo
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
        return ( RET_WRONG_HANDLE );
    }


    if ( ptIsiSensorMipiInfo == NULL )
    {
        return ( result );
    }

    TRACE( S5K4EC_INFO, "%s: (exit)\n", __FUNCTION__);

    return ( result );
}

static RESULT S5K4EC_IsiGetSensorIsiVersion
(  IsiSensorHandle_t   handle,
   unsigned int*     pVersion
)
{
    S5K4EC_Context_t *pS5K4ECCtx = (S5K4EC_Context_t *)handle;

    RESULT result = RET_SUCCESS;


    TRACE( S5K4EC_INFO, "%s: (enter)\n", __FUNCTION__);

    if ( pS5K4ECCtx == NULL )
    {
    	TRACE( S5K4EC_ERROR, "%s: pS5K4ECCtx IS NULL\n", __FUNCTION__);
        return ( RET_WRONG_HANDLE );
    }

	if(pVersion == NULL)
	{
		TRACE( S5K4EC_ERROR, "%s: pVersion IS NULL\n", __FUNCTION__);
        return ( RET_WRONG_HANDLE );
	}

	*pVersion = CONFIG_ISI_VERSION;
	return result;
}

/*****************************************************************************/
/**
 *          S5K4EC_IsiGetSensorIss
 *
 * @brief   fills in the correct pointers for the sensor description struct
 *
 * @param   param1      pointer to sensor description struct
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT S5K4EC_IsiGetSensorIss
(
    IsiSensor_t *pIsiSensor
)
{
    RESULT result = RET_SUCCESS;

    TRACE( S5K4EC_INFO, "%s (enter)\n", __FUNCTION__);

    if ( pIsiSensor != NULL )
    {
        pIsiSensor->pszName                             = S5K4EC_g_acName;
        pIsiSensor->pRegisterTable                      = S5K4EC_g_aRegDescription;
        pIsiSensor->pIsiSensorCaps                      = &S5K4EC_g_IsiSensorDefaultConfig;
        pIsiSensor->pIsiGetSensorIsiVer					        = S5K4EC_IsiGetSensorIsiVersion;
        pIsiSensor->pIsiCreateSensorIss                 = S5K4EC_IsiCreateSensorIss;
        pIsiSensor->pIsiReleaseSensorIss                = S5K4EC_IsiReleaseSensorIss;
        pIsiSensor->pIsiGetCapsIss                      = S5K4EC_IsiGetCapsIss;
        pIsiSensor->pIsiSetupSensorIss                  = S5K4EC_IsiSetupSensorIss;
        pIsiSensor->pIsiChangeSensorResolutionIss       = S5K4EC_IsiChangeSensorResolutionIss;
        pIsiSensor->pIsiSensorSetStreamingIss           = S5K4EC_IsiSensorSetStreamingIss;
        pIsiSensor->pIsiSensorSetPowerIss               = S5K4EC_IsiSensorSetPowerIss;
        pIsiSensor->pIsiCheckSensorConnectionIss        = S5K4EC_IsiCheckSensorConnectionIss;
        pIsiSensor->pIsiGetSensorRevisionIss            = S5K4EC_IsiGetSensorRevisionIss;
        pIsiSensor->pIsiRegisterReadIss                 = S5K4EC_IsiRegReadIss;
        pIsiSensor->pIsiRegisterWriteIss                = S5K4EC_IsiRegWriteIss;

        /* AEC functions */
        pIsiSensor->pIsiExposureControlIss              = S5K4EC_IsiExposureControlIss;
        pIsiSensor->pIsiGetGainLimitsIss                = S5K4EC_IsiGetGainLimitsIss;
        pIsiSensor->pIsiGetIntegrationTimeLimitsIss     = S5K4EC_IsiGetIntegrationTimeLimitsIss;
        pIsiSensor->pIsiGetCurrentExposureIss           = S5K4EC_IsiGetCurrentExposureIss;
        pIsiSensor->pIsiGetGainIss                      = S5K4EC_IsiGetGainIss;
        pIsiSensor->pIsiGetGainIncrementIss             = S5K4EC_IsiGetGainIncrementIss;
        pIsiSensor->pIsiSetGainIss                      = S5K4EC_IsiSetGainIss;
        pIsiSensor->pIsiGetIntegrationTimeIss           = S5K4EC_IsiGetIntegrationTimeIss;
        pIsiSensor->pIsiGetIntegrationTimeIncrementIss  = S5K4EC_IsiGetIntegrationTimeIncrementIss;
        pIsiSensor->pIsiSetIntegrationTimeIss           = S5K4EC_IsiSetIntegrationTimeIss;
        pIsiSensor->pIsiGetResolutionIss                = S5K4EC_IsiGetResolutionIss;
        pIsiSensor->pIsiGetAfpsInfoIss                  = S5K4EC_IsiGetAfpsInfoIss;

        /* AWB specific functions */
        pIsiSensor->pIsiGetCalibKFactor                 = S5K4EC_IsiGetCalibKFactor;
        pIsiSensor->pIsiGetCalibPcaMatrix               = S5K4EC_IsiGetCalibPcaMatrix;
        pIsiSensor->pIsiGetCalibSvdMeanValue            = S5K4EC_IsiGetCalibSvdMeanValue;
        pIsiSensor->pIsiGetCalibCenterLine              = S5K4EC_IsiGetCalibCenterLine;
        pIsiSensor->pIsiGetCalibClipParam               = S5K4EC_IsiGetCalibClipParam;
        pIsiSensor->pIsiGetCalibGlobalFadeParam         = S5K4EC_IsiGetCalibGlobalFadeParam;
        pIsiSensor->pIsiGetCalibFadeParam               = S5K4EC_IsiGetCalibFadeParam;
        pIsiSensor->pIsiGetIlluProfile                  = S5K4EC_IsiGetIlluProfile;
        pIsiSensor->pIsiGetLscMatrixTable               = S5K4EC_IsiGetLscMatrixTable;

        /* AF functions */
        pIsiSensor->pIsiMdiInitMotoDriveMds             = S5K4EC_IsiMdiInitMotoDriveMds;
        pIsiSensor->pIsiMdiSetupMotoDrive               = S5K4EC_IsiMdiSetupMotoDrive;
        pIsiSensor->pIsiMdiFocusSet                     = S5K4EC_IsiMdiFocusSet;
        pIsiSensor->pIsiMdiFocusGet                     = S5K4EC_IsiMdiFocusGet;
        pIsiSensor->pIsiMdiFocusCalibrate               = S5K4EC_IsiMdiFocusCalibrate;

        /* MIPI */
        pIsiSensor->pIsiGetSensorMipiInfoIss            = S5K4EC_IsiGetSensorMipiInfoIss;

        /* Testpattern */
        pIsiSensor->pIsiActivateTestPattern             = S5K4EC_IsiActivateTestPattern;
    }
    else
    {
        result = RET_NULL_POINTER;
    }

    TRACE( S5K4EC_INFO, "%s (exit)\n", __FUNCTION__);

    return ( result );
}

static RESULT S5K4EC_IsiGetSensorI2cInfo(sensor_i2c_info_t** pdata)
{
    sensor_i2c_info_t* pSensorI2cInfo;

    pSensorI2cInfo = ( sensor_i2c_info_t * )malloc ( sizeof (sensor_i2c_info_t) );

    if ( pSensorI2cInfo == NULL )
    {
        TRACE( S5K4EC_ERROR,  "%s: Can't allocate ov14825 context\n",  __FUNCTION__ );
        return ( RET_OUTOFMEM );
    }
    MEMSET( pSensorI2cInfo, 0, sizeof( sensor_i2c_info_t ) );

    
    pSensorI2cInfo->i2c_addr = S5K4EC_SLAVE_ADDR;
    pSensorI2cInfo->soft_reg_addr = S5K4EC_SOFTWARE_RST;
    pSensorI2cInfo->soft_reg_value = S5K4EC_SOFTWARE_RST_VALUE;
    pSensorI2cInfo->reg_size = 2;
    pSensorI2cInfo->value_size = 2;

    {
        IsiSensorCaps_t Caps;
        sensor_caps_t *pCaps;
        uint32_t lanes,i;
        
        ListInit(&pSensorI2cInfo->lane_res[0]);
        ListInit(&pSensorI2cInfo->lane_res[1]);
        ListInit(&pSensorI2cInfo->lane_res[2]);
        
        Caps.Index = 0;            
        while(S5K4EC_IsiGetCapsIssInternal(&Caps)==RET_SUCCESS) {
            pCaps = malloc(sizeof(sensor_caps_t));
            if (pCaps != NULL) {
                memcpy(&pCaps->caps,&Caps,sizeof(IsiSensorCaps_t));
                ListPrepareItem(pCaps);
                ListAddTail(&pSensorI2cInfo->lane_res[0], pCaps);
            }
            Caps.Index++;
        }
    }
    
    ListInit(&pSensorI2cInfo->chipid_info);

    sensor_chipid_info_t* pChipIDInfo_H = (sensor_chipid_info_t *) malloc( sizeof(sensor_chipid_info_t) );
    if ( !pChipIDInfo_H )
    {
        return RET_OUTOFMEM;
    }
    MEMSET( pChipIDInfo_H, 0, sizeof(*pChipIDInfo_H) );    
    pChipIDInfo_H->chipid_reg_addr = S5K4EC_CHIP_ID_HIGH_BYTE;  
    pChipIDInfo_H->chipid_reg_value = S5K4EC_CHIP_ID_HIGH_BYTE_DEFAULT;
    ListPrepareItem( pChipIDInfo_H );
    ListAddTail( &pSensorI2cInfo->chipid_info, pChipIDInfo_H );

    sensor_chipid_info_t* pChipIDInfo_M = (sensor_chipid_info_t *) malloc( sizeof(sensor_chipid_info_t) );
    if ( !pChipIDInfo_M )
    {
        return RET_OUTOFMEM;
    }
    MEMSET( pChipIDInfo_M, 0, sizeof(*pChipIDInfo_M) ); 
    pChipIDInfo_M->chipid_reg_addr = S5K4EC_CHIP_ID_MIDDLE_BYTE;
    pChipIDInfo_M->chipid_reg_value = S5K4EC_CHIP_ID_MIDDLE_BYTE_DEFAULT;
    ListPrepareItem( pChipIDInfo_M );
    ListAddTail( &pSensorI2cInfo->chipid_info, pChipIDInfo_M );
    
    sensor_chipid_info_t* pChipIDInfo_L = (sensor_chipid_info_t *) malloc( sizeof(sensor_chipid_info_t) );
    if ( !pChipIDInfo_L )
    {
        return RET_OUTOFMEM;
    }
    MEMSET( pChipIDInfo_L, 0, sizeof(*pChipIDInfo_L) ); 
    pChipIDInfo_L->chipid_reg_addr = S5K4EC_CHIP_ID_LOW_BYTE;
    pChipIDInfo_L->chipid_reg_value = S5K4EC_CHIP_ID_LOW_BYTE_DEFAULT;
    ListPrepareItem( pChipIDInfo_L );
    ListAddTail( &pSensorI2cInfo->chipid_info, pChipIDInfo_L );

	//oyyf sensor drv version
	pSensorI2cInfo->sensor_drv_version = CONFIG_SENSOR_DRV_VERSION;
	
    *pdata = pSensorI2cInfo;
    return RET_SUCCESS;
}

/******************************************************************************
 * See header file for detailed comment.
 *****************************************************************************/

/*****************************************************************************/
/**
 */
/*****************************************************************************/
IsiCamDrvConfig_t IsiCamDrvConfig =
{
    0,
    S5K4EC_IsiGetSensorIss,
    {
        0,                      /**< IsiSensor_t.pszName */
        0,                      /**< IsiSensor_t.pRegisterTable */
        0,                      /**< IsiSensor_t.pIsiSensorCaps */
        0,											/**< IsiSensor_t.pIsiGetSensorIsiVer_t>*/   //oyyf add
        0,                      /**< IsiSensor_t.pIsiGetSensorTuningXmlVersion_t>*/   //oyyf add 
        0,                      /**< IsiSensor_t.pIsiWhiteBalanceIlluminationChk>*/   //ddl@rock-chips.com 
        0,                      /**< IsiSensor_t.pIsiWhiteBalanceIlluminationSet>*/   //ddl@rock-chips.com
        0,                      /**< IsiSensor_t.pIsiCheckOTPInfo>*/  //zyc 
        0,                      /**< IsiSensor_t.pIsiCreateSensorIss */
        0,                      /**< IsiSensor_t.pIsiReleaseSensorIss */
        0,                      /**< IsiSensor_t.pIsiGetCapsIss */
        0,                      /**< IsiSensor_t.pIsiSetupSensorIss */
        0,                      /**< IsiSensor_t.pIsiChangeSensorResolutionIss */
        0,                      /**< IsiSensor_t.pIsiSensorSetStreamingIss */
        0,                      /**< IsiSensor_t.pIsiSensorSetPowerIss */
        0,                      /**< IsiSensor_t.pIsiCheckSensorConnectionIss */
        0,                      /**< IsiSensor_t.pIsiGetSensorRevisionIss */
        0,                      /**< IsiSensor_t.pIsiRegisterReadIss */
        0,                      /**< IsiSensor_t.pIsiRegisterWriteIss */

        0,                      /**< IsiSensor_t.pIsiExposureControlIss */
        0,                      /**< IsiSensor_t.pIsiGetGainLimitsIss */
        0,                      /**< IsiSensor_t.pIsiGetIntegrationTimeLimitsIss */
        0,                      /**< IsiSensor_t.pIsiGetCurrentExposureIss */
        0,                      /**< IsiSensor_t.pIsiGetGainIss */
        0,                      /**< IsiSensor_t.pIsiGetGainIncrementIss */
        0,                      /**< IsiSensor_t.pIsiSetGainIss */
        0,                      /**< IsiSensor_t.pIsiGetIntegrationTimeIss */
        0,                      /**< IsiSensor_t.pIsiGetIntegrationTimeIncrementIss */
        0,                      /**< IsiSensor_t.pIsiSetIntegrationTimeIss */
        0,                      /**< IsiSensor_t.pIsiGetResolutionIss */
        0,                      /**< IsiSensor_t.pIsiGetAfpsInfoIss */

        0,                      /**< IsiSensor_t.pIsiGetCalibKFactor */
        0,                      /**< IsiSensor_t.pIsiGetCalibPcaMatrix */
        0,                      /**< IsiSensor_t.pIsiGetCalibSvdMeanValue */
        0,                      /**< IsiSensor_t.pIsiGetCalibCenterLine */
        0,                      /**< IsiSensor_t.pIsiGetCalibClipParam */
        0,                      /**< IsiSensor_t.pIsiGetCalibGlobalFadeParam */
        0,                      /**< IsiSensor_t.pIsiGetCalibFadeParam */
        0,                      /**< IsiSensor_t.pIsiGetIlluProfile */
        0,                      /**< IsiSensor_t.pIsiGetLscMatrixTable */

        0,                      /**< IsiSensor_t.pIsiMdiInitMotoDriveMds */
        0,                      /**< IsiSensor_t.pIsiMdiSetupMotoDrive */
        0,                      /**< IsiSensor_t.pIsiMdiFocusSet */
        0,                      /**< IsiSensor_t.pIsiMdiFocusGet */
        0,                      /**< IsiSensor_t.pIsiMdiFocusCalibrate */

        0,                      /**< IsiSensor_t.pIsiGetSensorMipiInfoIss */

        0,                      /**< IsiSensor_t.pIsiActivateTestPattern */
    },
    S5K4EC_IsiGetSensorI2cInfo,
};



