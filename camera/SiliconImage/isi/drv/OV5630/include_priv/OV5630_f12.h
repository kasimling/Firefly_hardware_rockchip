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
#ifndef __OV5630_F12_H__
#define __OV5630_F12_H__

#ifdef __cplusplus
extern "C"
{
#endif


/*****************************************************************************/
/*!
 * CIE F12:
 *  TL83, Ultralume 30
 * This illumination is not tuned for this sensor correctly! This color profile
 * might not yield satisfying results.
 */
/*****************************************************************************/

// crosstalk matrix
const Isi3x3FloatMatrix_t  OV5630_XTalkCoeff_F12 =
{
    {
        2.20920f, -0.70371f, -0.45774f,
       -0.69063f,  1.62620f,  0.06443f,
       -0.61942f, -1.69070f,  3.37610f
    }
};

// crosstalk offset matrix
const IsiXTalkFloatOffset_t OV5630_XTalkOffset_F12 =
{
    .fCtOffsetRed      = 0.0f,
    .fCtOffsetGreen    = 0.0f,
    .fCtOffsetBlue     = 0.0f
};

// gain matrix
const IsiComponentGain_t OV5630_CompGain_F12 =
{
    .fRed      = 1.00000f,
    .fGreenR   = 1.05163f,
    .fGreenB   = 1.05163f,
    .fBlue     = 1.60514f
};

// mean value of gaussian mixture model
const Isi2x1FloatMatrix_t OV5630_GaussMeanValue_F12 =
{
    {
        0.0f,      0.0f
    }
};

// inverse covariance matrix
const Isi2x2FloatMatrix_t OV5630_CovarianceMatrix_F12 =
{
    {
        0.0f,      0.0f,
        0.0f,      0.0f
    }
};

// factor in gaussian mixture model
const IsiGaussFactor_t OV5630_GaussFactor_F12 =
{
    .fGaussFactor = 0.0f
};

// thresholds for switching between MAP classification and interpolation
const Isi2x1FloatMatrix_t OV5630_Threshold_F12 =
{
    {
        0.0f, 0.0f  // 1 = disabled
    }
};

// saturation curve
float afSaturationSensorGain_F12[AWB_SATURATION_ARRAY_SIZE] =
{
    1.0f, 2.0f, 4.0f, 8.0f
};

float afSaturation_F12[AWB_SATURATION_ARRAY_SIZE] =
{
    100.0f, 95.0f, 90.0f , 70.0f
};

const IsiSaturationCurve_t OV5630_SaturationCurve_F12 =
{
    .ArraySize      = AWB_SATURATION_ARRAY_SIZE,
    .pSensorGain    = &afSaturationSensorGain_F12[0],
    .pSaturation    = &afSaturation_F12[0]
};

// saturation depended color conversion matrices
IsiSatCcMatrix_t OV5630_SatCcMatrix_F12[AWB_COLORMATRIX_ARRAY_SIZE] =
{
    {
        .fSaturation    = 100.0f,
        .XTalkCoeff     =
        {
            {
                2.20920f, -0.70371f, -0.45774f,
               -0.69063f,  1.62620f,  0.06443f,
               -0.61942f, -1.69070f,  3.37610f
            }
        }
    },
    {
        .fSaturation    = 90.0f,
        .XTalkCoeff     =
        {
            {
                2.20920f, -0.70371f, -0.45774f,
               -0.69063f,  1.62620f,  0.06443f,
               -0.61942f, -1.69070f,  3.37610f
            }
        }
    },
    {
        .fSaturation    = 80.0f,
        .XTalkCoeff     =
        {
            {
                2.20920f, -0.70371f, -0.45774f,
               -0.69063f,  1.62620f,  0.06443f,
               -0.61942f, -1.69070f,  3.37610f
            }
        }
    },
    {
        .fSaturation    = 70.0f,
        .XTalkCoeff     =
        {
            {
                2.20920f, -0.70371f, -0.45774f,
               -0.69063f,  1.62620f,  0.06443f,
               -0.61942f, -1.69070f,  3.37610f
            }
        }
    }
};

const IsiCcMatrixTable_t OV5630_CcMatrixTable_F12 =
{
    .ArraySize          = AWB_COLORMATRIX_ARRAY_SIZE,
    .pIsiSatCcMatrix    = &OV5630_SatCcMatrix_F12[0]
};

// saturation depended color conversion offset vectors
IsiSatCcOffset_t OV5630_SatCcOffset_F12[AWB_COLORMATRIX_ARRAY_SIZE] =
{
    {
        .fSaturation    = 100.0f,
        .CcOffset       =
        {
            .fCtOffsetRed   = 0.0f,
            .fCtOffsetGreen = 0.0f,
            .fCtOffsetBlue  = 0.0f
        }
    },
    {
        .fSaturation    = 90.0f,
        .CcOffset       =
        {
            .fCtOffsetRed   = 0.0f,
            .fCtOffsetGreen = 0.0f,
            .fCtOffsetBlue  = 0.0f
        }
    },
    {
        .fSaturation    = 80.0f,
        .CcOffset       =
        {
            .fCtOffsetRed   = 0.0f,
            .fCtOffsetGreen = 0.0f,
            .fCtOffsetBlue  = 0.0f
        }
    },
    {
        .fSaturation    = 70.0f,
        .CcOffset       =
        {
            .fCtOffsetRed   = 0.0f,
            .fCtOffsetGreen = 0.0f,
            .fCtOffsetBlue  = 0.0f
        }
    }
};

const IsiCcOffsetTable_t OV5630_CcOffsetTable_F12=
{
    .ArraySize          = AWB_COLORMATRIX_ARRAY_SIZE,
    .pIsiSatCcOffset    = &OV5630_SatCcOffset_F12[0]
};

// vignetting curve
float afVignettingSensorGain_F12[AWB_VIGNETTING_ARRAY_SIZE] =
{
    1.0f, 2.0f, 4.0f, 8.0f
};

float afVignetting_F12[AWB_VIGNETTING_ARRAY_SIZE] =
{
    60.0f, 80.0f, 90.0f, 100.0f
};

const IsiVignettingCurve_t OV5630_VignettingCurve_F12 =
{
    .ArraySize      = AWB_VIGNETTING_ARRAY_SIZE,
    .pSensorGain    = &afVignettingSensorGain_F12[0],
    .pVignetting    = &afVignetting_F12[0]
};

// vignetting dependend lsc matrices
IsiVignLscMatrix_t OV5630_VignLscMatrix_F12[AWB_LSCMATRIX_ARRAY_SIZE] = 
{
    // array item 0
    {
        .fVignetting    = 80.0f,
        .LscMatrix      =
        {
            // ISI_COLOR_COMPONENT_RED
            {
                {
                    1895U, 1794U, 1699U, 1630U, 1553U, 1463U, 1390U, 1325U, 1299U, 1317U, 1381U, 1470U, 1550U, 1622U, 1713U, 1809U, 1881U,
                    1859U, 1786U, 1704U, 1623U, 1540U, 1457U, 1365U, 1302U, 1269U, 1292U, 1364U, 1451U, 1545U, 1614U, 1703U, 1791U, 1864U,
                    1808U, 1750U, 1663U, 1587U, 1493U, 1398U, 1308U, 1235U, 1208U, 1231U, 1305U, 1396U, 1498U, 1582U, 1666U, 1743U, 1834U,
                    1792U, 1704U, 1626U, 1535U, 1451U, 1349U, 1257U, 1180U, 1153U, 1179U, 1253U, 1351U, 1453U, 1546U, 1626U, 1714U, 1787U,
                    1745U, 1683U, 1587U, 1516U, 1409U, 1308U, 1212U, 1137U, 1112U, 1134U, 1206U, 1308U, 1419U, 1510U, 1602U, 1679U, 1772U,
                    1738U, 1657U, 1585U, 1490U, 1386U, 1280U, 1173U, 1104U, 1073U, 1101U, 1171U, 1275U, 1379U, 1492U, 1583U, 1663U, 1732U,
                    1705U, 1648U, 1554U, 1465U, 1357U, 1250U, 1149U, 1074U, 1046U, 1066U, 1142U, 1243U, 1357U, 1468U, 1559U, 1652U, 1716U,
                    1706U, 1632U, 1555U, 1457U, 1349U, 1236U, 1132U, 1056U, 1031U, 1051U, 1120U, 1226U, 1344U, 1456U, 1551U, 1642U, 1715U,
                    1706U, 1636U, 1552U, 1456U, 1344U, 1232U, 1130U, 1052U, 1024U, 1045U, 1122U, 1220U, 1342U, 1451U, 1548U, 1629U, 1708U,
                    1722U, 1646U, 1562U, 1465U, 1352U, 1242U, 1136U, 1060U, 1034U, 1055U, 1129U, 1232U, 1344U, 1459U, 1559U, 1644U, 1713U,
                    1731U, 1654U, 1574U, 1483U, 1373U, 1260U, 1154U, 1082U, 1051U, 1074U, 1149U, 1251U, 1367U, 1477U, 1571U, 1663U, 1720U,
                    1749U, 1686U, 1591U, 1507U, 1402U, 1289U, 1190U, 1111U, 1082U, 1108U, 1178U, 1277U, 1394U, 1504U, 1592U, 1664U, 1756U,
                    1804U, 1723U, 1652U, 1553U, 1452U, 1345U, 1242U, 1166U, 1139U, 1158U, 1228U, 1333U, 1447U, 1540U, 1632U, 1726U, 1770U,
                    1816U, 1742U, 1658U, 1577U, 1483U, 1376U, 1283U, 1207U, 1175U, 1200U, 1268U, 1362U, 1475U, 1562U, 1654U, 1737U, 1816U,
                    1858U, 1780U, 1700U, 1621U, 1532U, 1437U, 1339U, 1265U, 1236U, 1256U, 1326U, 1422U, 1518U, 1610U, 1692U, 1772U, 1841U,
                    1891U, 1823U, 1736U, 1665U, 1567U, 1488U, 1395U, 1328U, 1298U, 1315U, 1384U, 1470U, 1575U, 1648U, 1728U, 1809U, 1901U,
                    1926U, 1873U, 1774U, 1686U, 1607U, 1519U, 1436U, 1371U, 1346U, 1362U, 1434U, 1521U, 1585U, 1684U, 1745U, 1852U, 1909U
                }
            }, 

            // ISI_COLOR_COMPONENT_GREENR
            {
                {
                    1673U, 1618U, 1534U, 1476U, 1406U, 1346U, 1284U, 1243U, 1224U, 1231U, 1281U, 1344U, 1416U, 1470U, 1546U, 1627U, 1704U,
                    1681U, 1617U, 1547U, 1483U, 1418U, 1352U, 1287U, 1240U, 1214U, 1233U, 1281U, 1344U, 1419U, 1481U, 1549U, 1618U, 1685U,
                    1629U, 1580U, 1506U, 1442U, 1376U, 1303U, 1236U, 1182U, 1160U, 1181U, 1235U, 1304U, 1377U, 1448U, 1516U, 1582U, 1656U,
                    1606U, 1551U, 1482U, 1416U, 1342U, 1266U, 1192U, 1141U, 1124U, 1140U, 1194U, 1271U, 1348U, 1420U, 1486U, 1552U, 1621U,
                    1583U, 1530U, 1454U, 1391U, 1316U, 1236U, 1156U, 1113U, 1095U, 1108U, 1158U, 1239U, 1316U, 1393U, 1463U, 1527U, 1602U,
                    1562U, 1509U, 1443U, 1368U, 1292U, 1210U, 1132U, 1083U, 1063U, 1084U, 1131U, 1207U, 1294U, 1378U, 1450U, 1516U, 1580U,
                    1555U, 1492U, 1426U, 1354U, 1276U, 1187U, 1110U, 1062U, 1040U, 1057U, 1112U, 1186U, 1274U, 1357U, 1436U, 1495U, 1564U,
                    1541U, 1487U, 1418U, 1345U, 1264U, 1173U, 1103U, 1045U, 1025U, 1043U, 1097U, 1168U, 1262U, 1349U, 1419U, 1494U, 1548U,
                    1547U, 1493U, 1422U, 1348U, 1263U, 1173U, 1101U, 1042U, 1024U, 1040U, 1097U, 1169U, 1260U, 1346U, 1425U, 1500U, 1555U,
                    1554U, 1500U, 1430U, 1353U, 1270U, 1178U, 1106U, 1049U, 1031U, 1047U, 1103U, 1175U, 1268U, 1355U, 1434U, 1502U, 1568U,
                    1573U, 1511U, 1443U, 1368U, 1286U, 1198U, 1123U, 1068U, 1048U, 1067U, 1119U, 1195U, 1286U, 1372U, 1445U, 1519U, 1571U,
                    1578U, 1527U, 1460U, 1387U, 1306U, 1224U, 1144U, 1092U, 1073U, 1092U, 1141U, 1218U, 1303U, 1392U, 1462U, 1530U, 1599U,
                    1631U, 1566U, 1501U, 1429U, 1348U, 1266U, 1189U, 1131U, 1116U, 1131U, 1184U, 1263U, 1348U, 1428U, 1502U, 1570U, 1638U,
                    1632U, 1587U, 1513U, 1448U, 1373U, 1293U, 1219U, 1162U, 1139U, 1159U, 1217U, 1286U, 1368U, 1446U, 1516U, 1580U, 1652U,
                    1675U, 1615U, 1552U, 1479U, 1412U, 1338U, 1265U, 1213U, 1194U, 1210U, 1263U, 1331U, 1408U, 1477U, 1544U, 1615U, 1682U,
                    1702U, 1666U, 1582U, 1519U, 1453U, 1383U, 1315U, 1265U, 1243U, 1263U, 1312U, 1378U, 1448U, 1513U, 1580U, 1653U, 1731U,
                    1747U, 1677U, 1625U, 1541U, 1483U, 1412U, 1353U, 1303U, 1284U, 1298U, 1348U, 1413U, 1475U, 1543U, 1605U, 1682U, 1758U
                },
            },

            // ISI_COLOR_COMPONENT_GREENB
            {
                {
                    1673U, 1618U, 1534U, 1476U, 1406U, 1346U, 1284U, 1243U, 1224U, 1231U, 1281U, 1344U, 1416U, 1470U, 1546U, 1627U, 1704U,
                    1681U, 1617U, 1547U, 1483U, 1418U, 1352U, 1287U, 1240U, 1214U, 1233U, 1281U, 1344U, 1419U, 1481U, 1549U, 1618U, 1685U,
                    1629U, 1580U, 1506U, 1442U, 1376U, 1303U, 1236U, 1182U, 1160U, 1181U, 1235U, 1304U, 1377U, 1448U, 1516U, 1582U, 1656U,
                    1606U, 1551U, 1482U, 1416U, 1342U, 1266U, 1192U, 1141U, 1124U, 1140U, 1194U, 1271U, 1348U, 1420U, 1486U, 1552U, 1621U,
                    1583U, 1530U, 1454U, 1391U, 1316U, 1236U, 1156U, 1113U, 1095U, 1108U, 1158U, 1239U, 1316U, 1393U, 1463U, 1527U, 1602U,
                    1562U, 1509U, 1443U, 1368U, 1292U, 1210U, 1132U, 1083U, 1063U, 1084U, 1131U, 1207U, 1294U, 1378U, 1450U, 1516U, 1580U,
                    1555U, 1492U, 1426U, 1354U, 1276U, 1187U, 1110U, 1062U, 1040U, 1057U, 1112U, 1186U, 1274U, 1357U, 1436U, 1495U, 1564U,
                    1541U, 1487U, 1418U, 1345U, 1264U, 1173U, 1103U, 1045U, 1025U, 1043U, 1097U, 1168U, 1262U, 1349U, 1419U, 1494U, 1548U,
                    1547U, 1493U, 1422U, 1348U, 1263U, 1173U, 1101U, 1042U, 1024U, 1040U, 1097U, 1169U, 1260U, 1346U, 1425U, 1500U, 1555U,
                    1554U, 1500U, 1430U, 1353U, 1270U, 1178U, 1106U, 1049U, 1031U, 1047U, 1103U, 1175U, 1268U, 1355U, 1434U, 1502U, 1568U,
                    1573U, 1511U, 1443U, 1368U, 1286U, 1198U, 1123U, 1068U, 1048U, 1067U, 1119U, 1195U, 1286U, 1372U, 1445U, 1519U, 1571U,
                    1578U, 1527U, 1460U, 1387U, 1306U, 1224U, 1144U, 1092U, 1073U, 1092U, 1141U, 1218U, 1303U, 1392U, 1462U, 1530U, 1599U,
                    1631U, 1566U, 1501U, 1429U, 1348U, 1266U, 1189U, 1131U, 1116U, 1131U, 1184U, 1263U, 1348U, 1428U, 1502U, 1570U, 1638U,
                    1632U, 1587U, 1513U, 1448U, 1373U, 1293U, 1219U, 1162U, 1139U, 1159U, 1217U, 1286U, 1368U, 1446U, 1516U, 1580U, 1652U,
                    1675U, 1615U, 1552U, 1479U, 1412U, 1338U, 1265U, 1213U, 1194U, 1210U, 1263U, 1331U, 1408U, 1477U, 1544U, 1615U, 1682U,
                    1702U, 1666U, 1582U, 1519U, 1453U, 1383U, 1315U, 1265U, 1243U, 1263U, 1312U, 1378U, 1448U, 1513U, 1580U, 1653U, 1731U,
                    1747U, 1677U, 1625U, 1541U, 1483U, 1412U, 1353U, 1303U, 1284U, 1298U, 1348U, 1413U, 1475U, 1543U, 1605U, 1682U, 1758U
                },
            },

            // ISI_COLOR_COMPONENT_BLUE
            {
                {
                    1587U, 1506U, 1456U, 1405U, 1352U, 1307U, 1252U, 1217U, 1194U, 1205U, 1251U, 1305U, 1352U, 1406U, 1471U, 1524U, 1607U,
                    1557U, 1511U, 1459U, 1402U, 1352U, 1297U, 1246U, 1203U, 1179U, 1199U, 1238U, 1296U, 1352U, 1405U, 1469U, 1536U, 1587U,
                    1526U, 1492U, 1425U, 1375U, 1324U, 1261U, 1212U, 1156U, 1140U, 1156U, 1203U, 1261U, 1326U, 1385U, 1442U, 1498U, 1555U,
                    1504U, 1465U, 1406U, 1355U, 1292U, 1236U, 1170U, 1124U, 1104U, 1120U, 1169U, 1233U, 1300U, 1363U, 1418U, 1477U, 1544U,
                    1488U, 1444U, 1388U, 1332U, 1268U, 1203U, 1142U, 1094U, 1075U, 1095U, 1141U, 1207U, 1281U, 1336U, 1407U, 1457U, 1510U,
                    1481U, 1432U, 1376U, 1318U, 1252U, 1186U, 1121U, 1070U, 1053U, 1069U, 1117U, 1185U, 1258U, 1323U, 1387U, 1442U, 1501U,
                    1467U, 1421U, 1364U, 1301U, 1237U, 1168U, 1101U, 1054U, 1033U, 1052U, 1098U, 1167U, 1245U, 1313U, 1375U, 1436U, 1491U,
                    1465U, 1415U, 1362U, 1298U, 1232U, 1159U, 1096U, 1043U, 1024U, 1040U, 1087U, 1156U, 1234U, 1304U, 1368U, 1431U, 1486U,
                    1463U, 1424U, 1364U, 1301U, 1231U, 1159U, 1091U, 1045U, 1025U, 1040U, 1088U, 1159U, 1234U, 1306U, 1376U, 1437U, 1488U,
                    1468U, 1423U, 1365U, 1310U, 1235U, 1163U, 1097U, 1050U, 1029U, 1044U, 1094U, 1164U, 1239U, 1317U, 1383U, 1437U, 1494U,
                    1487U, 1448U, 1378U, 1327U, 1252U, 1181U, 1118U, 1065U, 1043U, 1066U, 1112U, 1180U, 1262U, 1331U, 1398U, 1459U, 1503U,
                    1503U, 1458U, 1407U, 1338U, 1276U, 1203U, 1141U, 1090U, 1072U, 1086U, 1142U, 1203U, 1284U, 1354U, 1416U, 1482U, 1512U,
                    1540U, 1496U, 1434U, 1377U, 1313U, 1246U, 1180U, 1129U, 1111U, 1129U, 1176U, 1245U, 1324U, 1386U, 1452U, 1510U, 1565U,
                    1553U, 1506U, 1455U, 1398U, 1336U, 1268U, 1212U, 1158U, 1141U, 1155U, 1207U, 1275U, 1338U, 1411U, 1468U, 1522U, 1575U,
                    1587U, 1538U, 1481U, 1430U, 1370U, 1310U, 1248U, 1204U, 1184U, 1200U, 1250U, 1313U, 1378U, 1442U, 1494U, 1555U, 1600U,
                    1613U, 1574U, 1520U, 1468U, 1410U, 1351U, 1301U, 1248U, 1235U, 1248U, 1295U, 1359U, 1425U, 1470U, 1522U, 1584U, 1641U,
                    1637U, 1605U, 1537U, 1481U, 1441U, 1380U, 1331U, 1287U, 1268U, 1282U, 1324U, 1389U, 1440U, 1488U, 1554U, 1605U, 1677U
                },
            },
        },
    },

    // array item 1
    {
       .fVignetting    = 100.0f,
       .LscMatrix      =
       {
           // ISI_COLOR_COMPONENT_RED
           {
               {
                   1895U, 1794U, 1699U, 1630U, 1553U, 1463U, 1390U, 1325U, 1299U, 1317U, 1381U, 1470U, 1550U, 1622U, 1713U, 1809U, 1881U,
                   1859U, 1786U, 1704U, 1623U, 1540U, 1457U, 1365U, 1302U, 1269U, 1292U, 1364U, 1451U, 1545U, 1614U, 1703U, 1791U, 1864U,
                   1808U, 1750U, 1663U, 1587U, 1493U, 1398U, 1308U, 1235U, 1208U, 1231U, 1305U, 1396U, 1498U, 1582U, 1666U, 1743U, 1834U,
                   1792U, 1704U, 1626U, 1535U, 1451U, 1349U, 1257U, 1180U, 1153U, 1179U, 1253U, 1351U, 1453U, 1546U, 1626U, 1714U, 1787U,
                   1745U, 1683U, 1587U, 1516U, 1409U, 1308U, 1212U, 1137U, 1112U, 1134U, 1206U, 1308U, 1419U, 1510U, 1602U, 1679U, 1772U,
                   1738U, 1657U, 1585U, 1490U, 1386U, 1280U, 1173U, 1104U, 1073U, 1101U, 1171U, 1275U, 1379U, 1492U, 1583U, 1663U, 1732U,
                   1705U, 1648U, 1554U, 1465U, 1357U, 1250U, 1149U, 1074U, 1046U, 1066U, 1142U, 1243U, 1357U, 1468U, 1559U, 1652U, 1716U,
                   1706U, 1632U, 1555U, 1457U, 1349U, 1236U, 1132U, 1056U, 1031U, 1051U, 1120U, 1226U, 1344U, 1456U, 1551U, 1642U, 1715U,
                   1706U, 1636U, 1552U, 1456U, 1344U, 1232U, 1130U, 1052U, 1024U, 1045U, 1122U, 1220U, 1342U, 1451U, 1548U, 1629U, 1708U,
                   1722U, 1646U, 1562U, 1465U, 1352U, 1242U, 1136U, 1060U, 1034U, 1055U, 1129U, 1232U, 1344U, 1459U, 1559U, 1644U, 1713U,
                   1731U, 1654U, 1574U, 1483U, 1373U, 1260U, 1154U, 1082U, 1051U, 1074U, 1149U, 1251U, 1367U, 1477U, 1571U, 1663U, 1720U,
                   1749U, 1686U, 1591U, 1507U, 1402U, 1289U, 1190U, 1111U, 1082U, 1108U, 1178U, 1277U, 1394U, 1504U, 1592U, 1664U, 1756U,
                   1804U, 1723U, 1652U, 1553U, 1452U, 1345U, 1242U, 1166U, 1139U, 1158U, 1228U, 1333U, 1447U, 1540U, 1632U, 1726U, 1770U,
                   1816U, 1742U, 1658U, 1577U, 1483U, 1376U, 1283U, 1207U, 1175U, 1200U, 1268U, 1362U, 1475U, 1562U, 1654U, 1737U, 1816U,
                   1858U, 1780U, 1700U, 1621U, 1532U, 1437U, 1339U, 1265U, 1236U, 1256U, 1326U, 1422U, 1518U, 1610U, 1692U, 1772U, 1841U,
                   1891U, 1823U, 1736U, 1665U, 1567U, 1488U, 1395U, 1328U, 1298U, 1315U, 1384U, 1470U, 1575U, 1648U, 1728U, 1809U, 1901U,
                   1926U, 1873U, 1774U, 1686U, 1607U, 1519U, 1436U, 1371U, 1346U, 1362U, 1434U, 1521U, 1585U, 1684U, 1745U, 1852U, 1909U
               }
           }, 
    
           // ISI_COLOR_COMPONENT_GREENR
           {
               {
                   1673U, 1618U, 1534U, 1476U, 1406U, 1346U, 1284U, 1243U, 1224U, 1231U, 1281U, 1344U, 1416U, 1470U, 1546U, 1627U, 1704U,
                   1681U, 1617U, 1547U, 1483U, 1418U, 1352U, 1287U, 1240U, 1214U, 1233U, 1281U, 1344U, 1419U, 1481U, 1549U, 1618U, 1685U,
                   1629U, 1580U, 1506U, 1442U, 1376U, 1303U, 1236U, 1182U, 1160U, 1181U, 1235U, 1304U, 1377U, 1448U, 1516U, 1582U, 1656U,
                   1606U, 1551U, 1482U, 1416U, 1342U, 1266U, 1192U, 1141U, 1124U, 1140U, 1194U, 1271U, 1348U, 1420U, 1486U, 1552U, 1621U,
                   1583U, 1530U, 1454U, 1391U, 1316U, 1236U, 1156U, 1113U, 1095U, 1108U, 1158U, 1239U, 1316U, 1393U, 1463U, 1527U, 1602U,
                   1562U, 1509U, 1443U, 1368U, 1292U, 1210U, 1132U, 1083U, 1063U, 1084U, 1131U, 1207U, 1294U, 1378U, 1450U, 1516U, 1580U,
                   1555U, 1492U, 1426U, 1354U, 1276U, 1187U, 1110U, 1062U, 1040U, 1057U, 1112U, 1186U, 1274U, 1357U, 1436U, 1495U, 1564U,
                   1541U, 1487U, 1418U, 1345U, 1264U, 1173U, 1103U, 1045U, 1025U, 1043U, 1097U, 1168U, 1262U, 1349U, 1419U, 1494U, 1548U,
                   1547U, 1493U, 1422U, 1348U, 1263U, 1173U, 1101U, 1042U, 1024U, 1040U, 1097U, 1169U, 1260U, 1346U, 1425U, 1500U, 1555U,
                   1554U, 1500U, 1430U, 1353U, 1270U, 1178U, 1106U, 1049U, 1031U, 1047U, 1103U, 1175U, 1268U, 1355U, 1434U, 1502U, 1568U,
                   1573U, 1511U, 1443U, 1368U, 1286U, 1198U, 1123U, 1068U, 1048U, 1067U, 1119U, 1195U, 1286U, 1372U, 1445U, 1519U, 1571U,
                   1578U, 1527U, 1460U, 1387U, 1306U, 1224U, 1144U, 1092U, 1073U, 1092U, 1141U, 1218U, 1303U, 1392U, 1462U, 1530U, 1599U,
                   1631U, 1566U, 1501U, 1429U, 1348U, 1266U, 1189U, 1131U, 1116U, 1131U, 1184U, 1263U, 1348U, 1428U, 1502U, 1570U, 1638U,
                   1632U, 1587U, 1513U, 1448U, 1373U, 1293U, 1219U, 1162U, 1139U, 1159U, 1217U, 1286U, 1368U, 1446U, 1516U, 1580U, 1652U,
                   1675U, 1615U, 1552U, 1479U, 1412U, 1338U, 1265U, 1213U, 1194U, 1210U, 1263U, 1331U, 1408U, 1477U, 1544U, 1615U, 1682U,
                   1702U, 1666U, 1582U, 1519U, 1453U, 1383U, 1315U, 1265U, 1243U, 1263U, 1312U, 1378U, 1448U, 1513U, 1580U, 1653U, 1731U,
                   1747U, 1677U, 1625U, 1541U, 1483U, 1412U, 1353U, 1303U, 1284U, 1298U, 1348U, 1413U, 1475U, 1543U, 1605U, 1682U, 1758U
               },
           },
    
           // ISI_COLOR_COMPONENT_GREENB
           {
               {
                   1673U, 1618U, 1534U, 1476U, 1406U, 1346U, 1284U, 1243U, 1224U, 1231U, 1281U, 1344U, 1416U, 1470U, 1546U, 1627U, 1704U,
                   1681U, 1617U, 1547U, 1483U, 1418U, 1352U, 1287U, 1240U, 1214U, 1233U, 1281U, 1344U, 1419U, 1481U, 1549U, 1618U, 1685U,
                   1629U, 1580U, 1506U, 1442U, 1376U, 1303U, 1236U, 1182U, 1160U, 1181U, 1235U, 1304U, 1377U, 1448U, 1516U, 1582U, 1656U,
                   1606U, 1551U, 1482U, 1416U, 1342U, 1266U, 1192U, 1141U, 1124U, 1140U, 1194U, 1271U, 1348U, 1420U, 1486U, 1552U, 1621U,
                   1583U, 1530U, 1454U, 1391U, 1316U, 1236U, 1156U, 1113U, 1095U, 1108U, 1158U, 1239U, 1316U, 1393U, 1463U, 1527U, 1602U,
                   1562U, 1509U, 1443U, 1368U, 1292U, 1210U, 1132U, 1083U, 1063U, 1084U, 1131U, 1207U, 1294U, 1378U, 1450U, 1516U, 1580U,
                   1555U, 1492U, 1426U, 1354U, 1276U, 1187U, 1110U, 1062U, 1040U, 1057U, 1112U, 1186U, 1274U, 1357U, 1436U, 1495U, 1564U,
                   1541U, 1487U, 1418U, 1345U, 1264U, 1173U, 1103U, 1045U, 1025U, 1043U, 1097U, 1168U, 1262U, 1349U, 1419U, 1494U, 1548U,
                   1547U, 1493U, 1422U, 1348U, 1263U, 1173U, 1101U, 1042U, 1024U, 1040U, 1097U, 1169U, 1260U, 1346U, 1425U, 1500U, 1555U,
                   1554U, 1500U, 1430U, 1353U, 1270U, 1178U, 1106U, 1049U, 1031U, 1047U, 1103U, 1175U, 1268U, 1355U, 1434U, 1502U, 1568U,
                   1573U, 1511U, 1443U, 1368U, 1286U, 1198U, 1123U, 1068U, 1048U, 1067U, 1119U, 1195U, 1286U, 1372U, 1445U, 1519U, 1571U,
                   1578U, 1527U, 1460U, 1387U, 1306U, 1224U, 1144U, 1092U, 1073U, 1092U, 1141U, 1218U, 1303U, 1392U, 1462U, 1530U, 1599U,
                   1631U, 1566U, 1501U, 1429U, 1348U, 1266U, 1189U, 1131U, 1116U, 1131U, 1184U, 1263U, 1348U, 1428U, 1502U, 1570U, 1638U,
                   1632U, 1587U, 1513U, 1448U, 1373U, 1293U, 1219U, 1162U, 1139U, 1159U, 1217U, 1286U, 1368U, 1446U, 1516U, 1580U, 1652U,
                   1675U, 1615U, 1552U, 1479U, 1412U, 1338U, 1265U, 1213U, 1194U, 1210U, 1263U, 1331U, 1408U, 1477U, 1544U, 1615U, 1682U,
                   1702U, 1666U, 1582U, 1519U, 1453U, 1383U, 1315U, 1265U, 1243U, 1263U, 1312U, 1378U, 1448U, 1513U, 1580U, 1653U, 1731U,
                   1747U, 1677U, 1625U, 1541U, 1483U, 1412U, 1353U, 1303U, 1284U, 1298U, 1348U, 1413U, 1475U, 1543U, 1605U, 1682U, 1758U
               },
           },
    
           // ISI_COLOR_COMPONENT_BLUE
           {
               {
                   1587U, 1506U, 1456U, 1405U, 1352U, 1307U, 1252U, 1217U, 1194U, 1205U, 1251U, 1305U, 1352U, 1406U, 1471U, 1524U, 1607U,
                   1557U, 1511U, 1459U, 1402U, 1352U, 1297U, 1246U, 1203U, 1179U, 1199U, 1238U, 1296U, 1352U, 1405U, 1469U, 1536U, 1587U,
                   1526U, 1492U, 1425U, 1375U, 1324U, 1261U, 1212U, 1156U, 1140U, 1156U, 1203U, 1261U, 1326U, 1385U, 1442U, 1498U, 1555U,
                   1504U, 1465U, 1406U, 1355U, 1292U, 1236U, 1170U, 1124U, 1104U, 1120U, 1169U, 1233U, 1300U, 1363U, 1418U, 1477U, 1544U,
                   1488U, 1444U, 1388U, 1332U, 1268U, 1203U, 1142U, 1094U, 1075U, 1095U, 1141U, 1207U, 1281U, 1336U, 1407U, 1457U, 1510U,
                   1481U, 1432U, 1376U, 1318U, 1252U, 1186U, 1121U, 1070U, 1053U, 1069U, 1117U, 1185U, 1258U, 1323U, 1387U, 1442U, 1501U,
                   1467U, 1421U, 1364U, 1301U, 1237U, 1168U, 1101U, 1054U, 1033U, 1052U, 1098U, 1167U, 1245U, 1313U, 1375U, 1436U, 1491U,
                   1465U, 1415U, 1362U, 1298U, 1232U, 1159U, 1096U, 1043U, 1024U, 1040U, 1087U, 1156U, 1234U, 1304U, 1368U, 1431U, 1486U,
                   1463U, 1424U, 1364U, 1301U, 1231U, 1159U, 1091U, 1045U, 1025U, 1040U, 1088U, 1159U, 1234U, 1306U, 1376U, 1437U, 1488U,
                   1468U, 1423U, 1365U, 1310U, 1235U, 1163U, 1097U, 1050U, 1029U, 1044U, 1094U, 1164U, 1239U, 1317U, 1383U, 1437U, 1494U,
                   1487U, 1448U, 1378U, 1327U, 1252U, 1181U, 1118U, 1065U, 1043U, 1066U, 1112U, 1180U, 1262U, 1331U, 1398U, 1459U, 1503U,
                   1503U, 1458U, 1407U, 1338U, 1276U, 1203U, 1141U, 1090U, 1072U, 1086U, 1142U, 1203U, 1284U, 1354U, 1416U, 1482U, 1512U,
                   1540U, 1496U, 1434U, 1377U, 1313U, 1246U, 1180U, 1129U, 1111U, 1129U, 1176U, 1245U, 1324U, 1386U, 1452U, 1510U, 1565U,
                   1553U, 1506U, 1455U, 1398U, 1336U, 1268U, 1212U, 1158U, 1141U, 1155U, 1207U, 1275U, 1338U, 1411U, 1468U, 1522U, 1575U,
                   1587U, 1538U, 1481U, 1430U, 1370U, 1310U, 1248U, 1204U, 1184U, 1200U, 1250U, 1313U, 1378U, 1442U, 1494U, 1555U, 1600U,
                   1613U, 1574U, 1520U, 1468U, 1410U, 1351U, 1301U, 1248U, 1235U, 1248U, 1295U, 1359U, 1425U, 1470U, 1522U, 1584U, 1641U,
                   1637U, 1605U, 1537U, 1481U, 1441U, 1380U, 1331U, 1287U, 1268U, 1282U, 1324U, 1389U, 1440U, 1488U, 1554U, 1605U, 1677U
               },
           },
       },
    },
};

const IsiLscMatrixTable_t OV5630_LscMatrixTable_F12 = 
{
    .ArraySize          = AWB_LSCMATRIX_ARRAY_SIZE,
    .psIsiVignLscMatrix = &OV5630_VignLscMatrix_F12[0]
};



#ifdef __cplusplus
}
#endif

/* @} ov5630_F12 */

#endif /* __OV5630_F12_H__ */

