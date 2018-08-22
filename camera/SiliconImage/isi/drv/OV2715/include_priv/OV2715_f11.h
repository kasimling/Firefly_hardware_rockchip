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
 * @file isi_iss.h
 *
 * @brief Interface description for image sensor specific implementation (iss).
 *
 *****************************************************************************/
/**
 * @page module_name_page Module Name
 * Describe here what this module does.
 *
 * For a detailed list of functions and implementation detail refer to:
 * - @ref module_name
 *
 * @defgroup ov5630_F11   Illumination Profile F11
 * @{
 *
 */
#ifndef __OV2715_F11_H__
#define __OV2715_F11_H__

#ifdef __cplusplus
extern "C"
{
#endif


#define AWB_COLORMATRIX_ARRAY_SIZE_CIE_F11  2
#define AWB_LSCMATRIX_ARRAY_SIZE_CIE_F11    2

#define AWB_SATURATION_ARRAY_SIZE_CIE_F11   4
#define AWB_VIGNETTING_ARRAY_SIZE_CIE_F11   2

#define CC_OFFSET_SCALING_F11               14.0f

/*****************************************************************************/
/*!
 * CIE F11:
 *  TL84, Ultralume 40, SP41
 */
/*****************************************************************************/

// crosstalk matrix
const Isi3x3FloatMatrix_t  OV2715_XTalkCoeff_F11 =
{
    {
        1.74646f,  -0.46559f,  -0.28087f, 
       -0.25565f,   1.28187f,  -0.02622f, 
       -0.16931f,  -1.56516f,   2.73448f  
    }
};

// crosstalk offset matrix
const IsiXTalkFloatOffset_t OV2715_XTalkOffset_F11 =
{
    .fCtOffsetRed   = (-204.3125f / CC_OFFSET_SCALING_F11),
    .fCtOffsetGreen = (-204.0000f / CC_OFFSET_SCALING_F11),
    .fCtOffsetBlue  = (-205.8750f / CC_OFFSET_SCALING_F11)
};

// gain matrix
const IsiComponentGain_t OV2715_CompGain_F11 =
{
    .fRed      = 1.08542f,
    .fGreenR   = 1.00000f,
    .fGreenB   = 1.00000f,
    .fBlue     = 2.16275f
};

// mean value of gaussian mixture model
const Isi2x1FloatMatrix_t OV2715_GaussMeanValue_F11 =
{
    {
        -0.10206f,  0.03294f
    }
};

// inverse covariance matrix
const Isi2x2FloatMatrix_t OV2715_CovarianceMatrix_F11 =
{
    {
        1464.77763f,  -1059.82906f, 
       -1059.82906f,  2210.29275f 
    }
};

// factor in gaussian mixture model
const IsiGaussFactor_t OV2715_GaussFactor_F11 =
{
    .fGaussFactor =  231.42408f 
};

// thresholds for switching between MAP classification and interpolation
const Isi2x1FloatMatrix_t OV2715_Threshold_F11 =
{
    {
        1.00000f,  1.00000f
    }
};

// saturation curve
float afSaturationSensorGain_F11[AWB_SATURATION_ARRAY_SIZE_CIE_F11] =
{
    1.0f, 2.0f, 4.0f, 8.0f
};

float afSaturation_F11[AWB_SATURATION_ARRAY_SIZE_CIE_F11] =
{
    100.0f, 100.0f, 90.0f, 74.0f
};

const IsiSaturationCurve_t OV2715_SaturationCurve_F11 =
{
    .ArraySize      = AWB_SATURATION_ARRAY_SIZE_CIE_F11,
    .pSensorGain    = &afSaturationSensorGain_F11[0],
    .pSaturation    = &afSaturation_F11[0]
};

// saturation depended color conversion matrices
IsiSatCcMatrix_t OV2715_SatCcMatrix_F11[AWB_COLORMATRIX_ARRAY_SIZE_CIE_F11] =
{
    {
        .fSaturation    = 74.0f,
        .XTalkCoeff     =
        {
            {
                1.55014f,  -0.40779f,  -0.14235f, 
                0.12446f,   0.82988f,   0.04566f, 
                0.18461f,  -1.22552f,   2.04090f  
            }
        }
    },
    {
        .fSaturation    = 100.0f,
        .XTalkCoeff     =
        {
            {
                1.74646f,  -0.46559f,  -0.28087f, 
               -0.25565f,   1.28187f,  -0.02622f, 
               -0.16931f,  -1.56516f,   2.73448f  
            }
        }
    }
};

const IsiCcMatrixTable_t OV2715_CcMatrixTable_F11 =
{
    .ArraySize          = AWB_COLORMATRIX_ARRAY_SIZE_CIE_F11,
    .pIsiSatCcMatrix    = &OV2715_SatCcMatrix_F11[0]
};

// saturation depended color conversion offset vectors
IsiSatCcOffset_t OV2715_SatCcOffset_F11[AWB_COLORMATRIX_ARRAY_SIZE_CIE_F11] =
{
    {
        .fSaturation    = 74.0f,
        .CcOffset       =
        {
            .fCtOffsetRed   = 0.0f,
            .fCtOffsetGreen = 0.0f,
            .fCtOffsetBlue  = 0.0f
        }
    },
    {
        .fSaturation    = 100.0f,
        .CcOffset       =
        {
            .fCtOffsetRed   = (-204.3125f / CC_OFFSET_SCALING_F11),
            .fCtOffsetGreen = (-204.0000f / CC_OFFSET_SCALING_F11),
            .fCtOffsetBlue  = (-205.8750f / CC_OFFSET_SCALING_F11)
        }
    }
};

const IsiCcOffsetTable_t OV2715_CcOffsetTable_F11=
{
    .ArraySize          = AWB_COLORMATRIX_ARRAY_SIZE_CIE_F11,
    .pIsiSatCcOffset    = &OV2715_SatCcOffset_F11[0]
};

// vignetting curve
float afVignettingSensorGain_F11[AWB_VIGNETTING_ARRAY_SIZE_CIE_F11] =
{
    1.0f, 8.0f
};

float afVignetting_F11[AWB_VIGNETTING_ARRAY_SIZE_CIE_F11] =
{
    100.0f, 100.0f
};

const IsiVignettingCurve_t OV2715_VignettingCurve_F11 =
{
    .ArraySize      = AWB_VIGNETTING_ARRAY_SIZE_CIE_F11,
    .pSensorGain    = &afVignettingSensorGain_F11[0],
    .pVignetting    = &afVignetting_F11[0]
};

// vignetting dependend lsc matrices
IsiVignLscMatrix_t OV2715_VignLscMatrix_CIE_F11_1920x1080[AWB_LSCMATRIX_ARRAY_SIZE_CIE_F11] = 
{
    // array item 0
    {
       .fVignetting    = 100.0f,
       .LscMatrix      =
       {
           // ISI_COLOR_COMPONENT_RED
           {
               {
                    1116U, 1098U, 1085U, 1076U, 1056U, 1049U, 1046U, 1037U, 1036U, 1027U, 1027U, 1024U, 1026U, 1029U, 1034U, 1043U, 1048U, 
                    1120U, 1099U, 1088U, 1069U, 1060U, 1054U, 1047U, 1041U, 1035U, 1030U, 1027U, 1028U, 1026U, 1029U, 1034U, 1044U, 1046U,
                    1113U, 1101U, 1084U, 1073U, 1060U, 1059U, 1052U, 1045U, 1041U, 1033U, 1035U, 1031U, 1029U, 1034U, 1033U, 1047U, 1043U,
                    1115U, 1096U, 1085U, 1070U, 1062U, 1060U, 1051U, 1047U, 1041U, 1038U, 1035U, 1032U, 1031U, 1031U, 1036U, 1044U, 1042U,
                    1117U, 1103U, 1088U, 1076U, 1063U, 1059U, 1057U, 1048U, 1041U, 1039U, 1037U, 1036U, 1033U, 1034U, 1035U, 1043U, 1042U,
                    1121U, 1106U, 1090U, 1077U, 1070U, 1060U, 1053U, 1045U, 1043U, 1041U, 1038U, 1039U, 1037U, 1037U, 1038U, 1046U, 1044U,
                    1130U, 1110U, 1095U, 1084U, 1071U, 1062U, 1056U, 1048U, 1041U, 1039U, 1041U, 1043U, 1040U, 1042U, 1040U, 1047U, 1045U,
                    1129U, 1113U, 1100U, 1085U, 1074U, 1067U, 1058U, 1046U, 1039U, 1038U, 1045U, 1041U, 1040U, 1044U, 1045U, 1049U, 1050U,
                    1133U, 1123U, 1104U, 1091U, 1080U, 1069U, 1060U, 1050U, 1039U, 1044U, 1042U, 1046U, 1046U, 1045U, 1045U, 1048U, 1050U,
                    1144U, 1126U, 1110U, 1096U, 1083U, 1075U, 1063U, 1055U, 1044U, 1044U, 1046U, 1049U, 1048U, 1050U, 1046U, 1055U, 1052U,
                    1145U, 1131U, 1112U, 1102U, 1085U, 1077U, 1068U, 1056U, 1050U, 1046U, 1049U, 1049U, 1051U, 1048U, 1048U, 1057U, 1057U,
                    1155U, 1140U, 1121U, 1105U, 1093U, 1081U, 1071U, 1060U, 1056U, 1051U, 1054U, 1055U, 1053U, 1051U, 1052U, 1061U, 1061U,
                    1161U, 1147U, 1128U, 1109U, 1097U, 1084U, 1076U, 1067U, 1059U, 1054U, 1055U, 1056U, 1055U, 1055U, 1057U, 1065U, 1066U,
                    1170U, 1153U, 1130U, 1119U, 1098U, 1089U, 1082U, 1071U, 1063U, 1059U, 1059U, 1060U, 1058U, 1055U, 1060U, 1067U, 1071U,
                    1176U, 1156U, 1140U, 1117U, 1103U, 1093U, 1083U, 1074U, 1068U, 1063U, 1059U, 1063U, 1061U, 1060U, 1065U, 1075U, 1076U,
                    1187U, 1165U, 1144U, 1126U, 1109U, 1095U, 1088U, 1077U, 1070U, 1065U, 1064U, 1065U, 1061U, 1064U, 1070U, 1077U, 1081U,
                    1195U, 1166U, 1149U, 1130U, 1115U, 1103U, 1088U, 1080U, 1077U, 1072U, 1071U, 1068U, 1072U, 1071U, 1078U, 1085U, 1093U
               }
           }, 
    
           // ISI_COLOR_COMPONENT_GREENR
           {
               {
                    1123U, 1102U, 1091U, 1080U, 1072U, 1064U, 1056U, 1052U, 1048U, 1035U, 1028U, 1025U, 1024U, 1031U, 1037U, 1045U, 1055U, 
                    1137U, 1118U, 1103U, 1092U, 1081U, 1084U, 1078U, 1075U, 1065U, 1054U, 1046U, 1040U, 1035U, 1039U, 1044U, 1058U, 1065U,
                    1140U, 1121U, 1105U, 1092U, 1087U, 1088U, 1084U, 1083U, 1074U, 1064U, 1052U, 1046U, 1042U, 1036U, 1045U, 1057U, 1058U,
                    1141U, 1119U, 1103U, 1096U, 1088U, 1089U, 1089U, 1087U, 1082U, 1071U, 1060U, 1052U, 1043U, 1046U, 1043U, 1052U, 1053U,
                    1145U, 1127U, 1108U, 1100U, 1090U, 1091U, 1094U, 1091U, 1087U, 1079U, 1070U, 1062U, 1051U, 1046U, 1041U, 1051U, 1051U,
                    1150U, 1129U, 1111U, 1099U, 1095U, 1095U, 1089U, 1092U, 1088U, 1085U, 1075U, 1068U, 1057U, 1047U, 1047U, 1048U, 1049U,
                    1157U, 1135U, 1116U, 1105U, 1098U, 1095U, 1090U, 1093U, 1089U, 1089U, 1082U, 1071U, 1059U, 1056U, 1048U, 1050U, 1050U,
                    1156U, 1141U, 1122U, 1113U, 1102U, 1098U, 1093U, 1088U, 1089U, 1089U, 1082U, 1075U, 1065U, 1057U, 1050U, 1057U, 1056U,
                    1169U, 1146U, 1129U, 1116U, 1108U, 1101U, 1097U, 1094U, 1085U, 1088U, 1084U, 1079U, 1069U, 1063U, 1056U, 1059U, 1056U,
                    1176U, 1155U, 1135U, 1123U, 1109U, 1108U, 1101U, 1094U, 1090U, 1088U, 1087U, 1086U, 1078U, 1066U, 1058U, 1059U, 1057U,
                    1178U, 1164U, 1140U, 1130U, 1118U, 1111U, 1100U, 1093U, 1092U, 1090U, 1092U, 1089U, 1082U, 1069U, 1059U, 1063U, 1061U,
                    1191U, 1170U, 1149U, 1137U, 1123U, 1113U, 1103U, 1096U, 1094U, 1097U, 1093U, 1091U, 1080U, 1071U, 1064U, 1069U, 1069U,
                    1205U, 1180U, 1160U, 1141U, 1126U, 1116U, 1106U, 1100U, 1102U, 1098U, 1093U, 1091U, 1086U, 1075U, 1068U, 1073U, 1074U,
                    1214U, 1189U, 1167U, 1148U, 1132U, 1118U, 1112U, 1108U, 1101U, 1100U, 1089U, 1093U, 1088U, 1082U, 1077U, 1081U, 1077U,
                    1222U, 1199U, 1174U, 1150U, 1139U, 1128U, 1119U, 1108U, 1102U, 1096U, 1097U, 1095U, 1091U, 1084U, 1084U, 1090U, 1083U,
                    1237U, 1206U, 1181U, 1165U, 1151U, 1135U, 1121U, 1113U, 1104U, 1099U, 1101U, 1098U, 1092U, 1091U, 1093U, 1093U, 1093U,
                    1238U, 1210U, 1197U, 1181U, 1156U, 1139U, 1125U, 1117U, 1114U, 1109U, 1102U, 1098U, 1096U, 1097U, 1095U, 1101U, 1109U
               },
           },
    
           // ISI_COLOR_COMPONENT_GREENB
           {
               {
                    1131U, 1107U, 1094U, 1084U, 1072U, 1069U, 1059U, 1053U, 1048U, 1037U, 1029U, 1028U, 1024U, 1033U, 1036U, 1047U, 1057U,
                    1145U, 1123U, 1107U, 1096U, 1085U, 1086U, 1079U, 1077U, 1066U, 1056U, 1047U, 1040U, 1036U, 1041U, 1044U, 1059U, 1066U,
                    1145U, 1126U, 1109U, 1096U, 1092U, 1091U, 1087U, 1085U, 1077U, 1066U, 1053U, 1049U, 1042U, 1037U, 1047U, 1058U, 1059U,
                    1148U, 1125U, 1108U, 1099U, 1091U, 1093U, 1092U, 1088U, 1083U, 1073U, 1061U, 1054U, 1045U, 1047U, 1045U, 1053U, 1055U,
                    1148U, 1131U, 1113U, 1104U, 1094U, 1094U, 1098U, 1093U, 1089U, 1081U, 1072U, 1063U, 1052U, 1047U, 1044U, 1052U, 1054U,
                    1155U, 1136U, 1115U, 1105U, 1099U, 1099U, 1092U, 1093U, 1090U, 1087U, 1077U, 1070U, 1058U, 1050U, 1049U, 1052U, 1050U,
                    1162U, 1140U, 1121U, 1109U, 1101U, 1098U, 1094U, 1095U, 1091U, 1092U, 1084U, 1072U, 1061U, 1056U, 1050U, 1051U, 1054U,
                    1162U, 1146U, 1127U, 1118U, 1105U, 1101U, 1094U, 1092U, 1090U, 1092U, 1084U, 1077U, 1067U, 1060U, 1052U, 1058U, 1058U,
                    1175U, 1153U, 1134U, 1121U, 1111U, 1104U, 1100U, 1096U, 1089U, 1089U, 1087U, 1079U, 1072U, 1064U, 1059U, 1062U, 1058U,
                    1186U, 1159U, 1140U, 1127U, 1114U, 1111U, 1104U, 1096U, 1092U, 1090U, 1089U, 1088U, 1079U, 1068U, 1060U, 1060U, 1059U,
                    1183U, 1169U, 1145U, 1136U, 1121U, 1114U, 1103U, 1096U, 1097U, 1091U, 1095U, 1090U, 1084U, 1071U, 1061U, 1065U, 1064U,
                    1197U, 1175U, 1154U, 1141U, 1126U, 1118U, 1105U, 1099U, 1095U, 1100U, 1095U, 1092U, 1086U, 1072U, 1066U, 1072U, 1070U,
                    1211U, 1186U, 1165U, 1145U, 1130U, 1120U, 1108U, 1104U, 1103U, 1100U, 1096U, 1093U, 1088U, 1077U, 1073U, 1076U, 1077U,
                    1222U, 1194U, 1171U, 1152U, 1136U, 1122U, 1115U, 1109U, 1103U, 1102U, 1092U, 1093U, 1092U, 1083U, 1078U, 1084U, 1078U,
                    1228U, 1204U, 1180U, 1155U, 1143U, 1130U, 1120U, 1112U, 1106U, 1098U, 1098U, 1098U, 1093U, 1086U, 1088U, 1091U, 1086U,
                    1243U, 1210U, 1186U, 1170U, 1153U, 1138U, 1125U, 1115U, 1107U, 1102U, 1102U, 1101U, 1094U, 1093U, 1095U, 1094U, 1095U,
                    1243U, 1216U, 1201U, 1183U, 1160U, 1143U, 1126U, 1118U, 1117U, 1110U, 1106U, 1100U, 1099U, 1099U, 1100U, 1104U, 1112U
               },
           },
    
           // ISI_COLOR_COMPONENT_BLUE
           {
               {
                    1063U, 1052U, 1045U, 1039U, 1037U, 1031U, 1040U, 1041U, 1037U, 1035U, 1033U, 1030U, 1024U, 1034U, 1035U, 1041U, 1043U,
                    1081U, 1066U, 1058U, 1052U, 1048U, 1060U, 1055U, 1062U, 1055U, 1050U, 1045U, 1044U, 1040U, 1041U, 1040U, 1048U, 1051U,
                    1089U, 1072U, 1063U, 1056U, 1062U, 1055U, 1068U, 1063U, 1066U, 1060U, 1052U, 1047U, 1042U, 1043U, 1042U, 1050U, 1051U,
                    1085U, 1071U, 1062U, 1062U, 1058U, 1064U, 1070U, 1073U, 1067U, 1065U, 1059U, 1055U, 1048U, 1046U, 1043U, 1046U, 1046U,
                    1091U, 1075U, 1066U, 1063U, 1063U, 1072U, 1076U, 1076U, 1077U, 1071U, 1068U, 1060U, 1052U, 1047U, 1044U, 1044U, 1048U,
                    1094U, 1079U, 1071U, 1069U, 1068U, 1077U, 1083U, 1080U, 1075U, 1076U, 1072U, 1062U, 1056U, 1045U, 1046U, 1051U, 1044U,
                    1099U, 1085U, 1073U, 1070U, 1080U, 1080U, 1075U, 1081U, 1078U, 1083U, 1077U, 1070U, 1061U, 1052U, 1043U, 1048U, 1053U,
                    1102U, 1093U, 1079U, 1081U, 1075U, 1084U, 1084U, 1084U, 1080U, 1081U, 1078U, 1073U, 1060U, 1054U, 1050U, 1059U, 1051U,
                    1109U, 1095U, 1085U, 1083U, 1083U, 1087U, 1091U, 1083U, 1075U, 1082U, 1078U, 1073U, 1065U, 1057U, 1055U, 1059U, 1058U,
                    1111U, 1105U, 1088U, 1087U, 1085U, 1089U, 1093U, 1085U, 1080U, 1080U, 1080U, 1078U, 1074U, 1062U, 1053U, 1060U, 1061U,
                    1111U, 1106U, 1094U, 1092U, 1091U, 1092U, 1092U, 1087U, 1080U, 1086U, 1085U, 1088U, 1076U, 1065U, 1056U, 1063U, 1063U,
                    1123U, 1112U, 1103U, 1096U, 1096U, 1089U, 1094U, 1082U, 1088U, 1089U, 1092U, 1088U, 1083U, 1066U, 1061U, 1068U, 1067U,
                    1121U, 1126U, 1104U, 1095U, 1093U, 1088U, 1085U, 1087U, 1092U, 1092U, 1087U, 1087U, 1078U, 1072U, 1065U, 1074U, 1062U,
                    1140U, 1120U, 1106U, 1095U, 1084U, 1087U, 1095U, 1096U, 1091U, 1085U, 1088U, 1090U, 1084U, 1077U, 1071U, 1079U, 1070U,
                    1130U, 1118U, 1104U, 1088U, 1095U, 1091U, 1094U, 1092U, 1090U, 1088U, 1087U, 1090U, 1087U, 1079U, 1077U, 1082U, 1078U,
                    1148U, 1107U, 1102U, 1106U, 1103U, 1098U, 1097U, 1096U, 1092U, 1091U, 1095U, 1093U, 1090U, 1089U, 1090U, 1090U, 1090U,
                    1112U, 1098U, 1116U, 1102U, 1094U, 1097U, 1089U, 1094U, 1096U, 1089U, 1091U, 1094U, 1098U, 1090U, 1094U, 1094U, 1093U
               },
           },
       },
    },
};

IsiLscMatrixTable_t OV2715_LscMatrixTable_CIE_F11_1920x1080 = 
{
    .ArraySize          = AWB_LSCMATRIX_ARRAY_SIZE_CIE_F11,
    .psIsiVignLscMatrix = &OV2715_VignLscMatrix_CIE_F11_1920x1080[0],
    .LscXGradTbl        = { 285U, 280U, 278U, 266U, 273U, 266U, 271U, 266U },
    .LscYGradTbl        = { 482U, 475U, 496U, 475U, 489U, 482U, 496U, 489U },
    .LscXSizeTbl        = { 115U, 117U, 118U, 123U, 120U, 123U, 121U, 123U },
    .LscYSizeTbl        = {  68U,  69U,  66U,  69U,  67U,  68U,  66U,  67U }
};



#ifdef __cplusplus
}
#endif

/* @} ov5630_F11 */

#endif /* __OV2715_F11_H__ */

