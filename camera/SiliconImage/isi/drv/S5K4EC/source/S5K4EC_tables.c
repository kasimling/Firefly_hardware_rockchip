#include <ebase/types.h>
#include <ebase/trace.h>
#include <ebase/builtins.h>

#include <common/return_codes.h>

#include "isi.h"
#include "isi_iss.h"
#include "isi_priv.h"
#include "S5K4EC_priv.h"


/*****************************************************************************
 * DEFINES
 *****************************************************************************/


/*****************************************************************************
 * GLOBALS
 *****************************************************************************/

// Image sensor register settings default values taken from data sheet OV8810_DS_1.1_SiliconImage.pdf.
// The settings may be altered by the code in IsiSetupSensor.
const IsiRegDescription_t S5K4EC_g_aRegDescription[] =
{
    {0xfcfc, 0xd000,"",eReadWrite_16,},
	{0x0010, 0x0001,"",eReadWrite_16,},//S/W Reset                                              
	{0x1030, 0x0000,"",eReadWrite_16,},//contint_host_int                                       
	{0x0014, 0x0001,"",eReadWrite_16,},//sw_load_complete - Release CORE (Arm) from reset state 
	{0xffff, 0x000a,"",eDelay,},//Delay 10ms
    //==================================================================================
    //02.ETC Setting
    //==================================================================================
	{0x0028, 0xd000,"",eReadWrite_16,},
	{0x002a, 0x1082,"",eReadWrite_16,},//cregs_d0_d4_cd10 //D4[9:8], D3[7:6], D2[5:4], D1[3:2], D0[1:0]                     
	{0x0f12, 0x03ff,"",eReadWrite_16,},//cregs_d5_d9_cd10 //D9[9:8], D8[7:6], D7[5:4], D6[3:2], D5[1:0]                     
	{0x002a, 0x1084,"",eReadWrite_16,},                                                                                     
	{0x0f12, 0x03ff,"",eReadWrite_16,},//cregs_clks_output_cd10 //SDA[11:10], SCL[9:8], PCLK[7:6], VSYNC[3:2], HSYNC[1:0]   
	{0x0f12, 0x03ff,"",eReadWrite_16,},
	{0x0f12, 0x0faf,"",eReadWrite_16,},
    //==================================================================================
    // 03.Analog Setting & ASP Control
    //==================================================================================
	{0x0028, 0xd000,"",eReadWrite_16,},
	{0x002a, 0x007a,"",eReadWrite_16,},                                                                                                                                                                                  
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                                                                                                                                  
	{0x002a, 0xe406,"",eReadWrite_16,},                                                                                                                                                                                  
	{0x0f12, 0x0092,"",eReadWrite_16,},                                                                                                                                                                                  
	{0x002a, 0xe410,"",eReadWrite_16,},                                                                                                                                                                                  
	{0x0f12, 0x3804,"",eReadWrite_16,}, //[15:8]fadlc_filter_co_b, [7:0]fadlc_filter_co_a                                                                                                                                
	{0x002a, 0xe41a,"",eReadWrite_16,},                                                                                                                                                                                  
	{0x0f12, 0x0010,"",eReadWrite_16,},                                                                                                                                                                                  
	{0x002a, 0xe420,"",eReadWrite_16,},                                                                                                                                                                                  
	{0x0f12, 0x0003,"",eReadWrite_16,}, //adlc_fadlc_filter_refresh                                                                                                                                                      
	{0x0f12, 0x0060,"",eReadWrite_16,}, //adlc_filter_level_diff_threshold                                                                                                                                               
	{0x002a, 0xe42e,"",eReadWrite_16,},                                                                                                                                                                                  
	{0x0f12, 0x0004,"",eReadWrite_16,}, //dithered l-ADLC(4bit)                                                                                                                                                          
	{0x002a, 0xf400,"",eReadWrite_16,},                                                                                                                                                                                  
	{0x0f12, 0x5a3c,"",eReadWrite_16,}, //[15:8]stx_width, [7:0]dstx_width                                                                                                                                               
	{0x0f12, 0x0023,"",eReadWrite_16,}, //[14]binning_test [13]gain_mode [11:12]row_id [10]cfpn_test [9]pd_pix [8]teg_en, [7]adc_res, [6]smp_en, [5]ldb_en, [4]ld_en, [3]clp_en [2]srx_en, [1]dshut_en, [0]dcds_en       
	{0x0f12, 0x8080,"",eReadWrite_16,}, //CDS option                                                                                                                                                                     
	{0x0f12, 0x03af,"",eReadWrite_16,}, //[11:6]rst_mx, [5:0]sig_mx                                                                                                                                                      
	{0x0f12, 0x000a,"",eReadWrite_16,}, //Avg mode                                                                                                                                                                       
	{0x0f12, 0xaa54,"",eReadWrite_16,}, //x1~x1.49:No MS, x1.5~x3.99:MS2, x4~x16:MS4                                                                                                                                     
	{0x0f12, 0x0040,"",eReadWrite_16,}, //RMP option [6]1: RES gain                                                                                                                                                      
	{0x0f12, 0x464e,"",eReadWrite_16,}, //[14]msoff_en, [13:8]off_rst, [7:0]adc_sat                                                                                                                                      
	{0x0f12, 0x0240,"",eReadWrite_16,}, //bist_sig_width_e                                                                                                                                                               
	{0x0f12, 0x0240,"",eReadWrite_16,}, //bist_sig_width_o                                                                                                                                                               
	{0x0f12, 0x0040,"",eReadWrite_16,}, //[9]dbs_bist_en, [8:0]bist_rst_width                                                                                                                                            
	{0x0f12, 0x1000,"",eReadWrite_16,}, //[15]aac_en, [14]GCLK_DIV2_EN, [13:10]dl_cont [9:8]dbs_mode, [7:0]dbs_option                                                                                                    
	{0x0f12, 0x55ff,"",eReadWrite_16,}, //bias [15:12]pix, [11:8]pix_bst [7:4]comp2, [3:0]comp1                                                                                                                          
	{0x0f12, 0xd000,"",eReadWrite_16,}, //[15:8]clp_lvl, [7:0]ref_option, [5]pix_bst_en                                                                                                                                  
	{0x0f12, 0x0010,"",eReadWrite_16,}, //[7:0]monit                                                                                                                                                                     
	{0x0f12, 0x0202,"",eReadWrite_16,}, //[15:8]dbr_tune_tgsl, [7:0]dbr_tune_pix                                                                                                                                         
	{0x0f12, 0x0401,"",eReadWrite_16,}, //[15:8]dbr_tune_ntg, [7:0]dbr_tune_rg                                                                                                                                           
	{0x0f12, 0x0022,"",eReadWrite_16,}, //[15:8]reg_option, [7:4]rosc_tune_ncp, [3:0]rosc_tune_cp                                                                                                                        
	{0x0f12, 0x0088,"",eReadWrite_16,}, //PD [8]inrush_ctrl, [7]fblv, [6]reg_ntg, [5]reg_tgsl, [4]reg_rg, [3]reg_pix, [2]ncp_rosc, [1]cp_rosc, [0]cp                                                                     
	{0x0f12, 0x009f,"",eReadWrite_16,}, //[9]capa_ctrl_en, [8:7]fb_lv, [6:5]dbr_clk_sel, [4:0]cp_capa                                                                                                                    
	{0x0f12, 0x0000,"",eReadWrite_16,}, //[15:0]blst_en_cintr                                                                                                                                                            
	{0x0f12, 0x1800,"",eReadWrite_16,}, //[11]blst_en, [10]rfpn_test, [9]sl_off, [8]tx_off, [7:0]rdv_option                                                                                                              
	{0x0f12, 0x0088,"",eReadWrite_16,}, //[15:0]pmg_reg_tune                                                                                                                                                             
	{0x0f12, 0x0000,"",eReadWrite_16,}, //[15:1]analog_dummy, [0]pd_reg_test                                                                                                                                             
	{0x0f12, 0x2428,"",eReadWrite_16,}, //[13:11]srx_gap1, [10:8]srx_gap0, [7:0]stx_gap                                                                                                                                  
	{0x0f12, 0x0000,"",eReadWrite_16,}, //[0]atx_option                                                                                                                                                                  
	{0x0f12, 0x03ee,"",eReadWrite_16,}, //aig_avg_half                                                                                                                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,}, //[0]hvs_test_reg                                                                                                                                                                
	{0x0f12, 0x0000,"",eReadWrite_16,}, //[0]dbus_bist_auto                                                                                                                                                              
	{0x0f12, 0x0000,"",eReadWrite_16,}, //[7:0]dbr_option                                                                                                                                                                
	{0x002a, 0xf552,"",eReadWrite_16,},                                                                                                                                                                                  
	{0x0f12, 0x0708,"",eReadWrite_16,}, //[7:0]lat_st, [15:8]lat_width                                                                                                                                                   
	{0x0f12, 0x080c,"",eReadWrite_16,}, //[7:0]hold_st, [15:8]hold_width    
    //=================================================================================
    // 04.Trap and Patch  Driver IC DW9714  //update by Chris 20130326
    //=================================================================================
// Start of Patch data                 
/*	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x3af8,"",eReadWrite_16,},
	{0x0f12, 0xb5f8,"",eReadWrite_16,},
	{0x0f12, 0x4b44,"",eReadWrite_16,},
	{0x0f12, 0x4944,"",eReadWrite_16,},
	{0x0f12, 0x4845,"",eReadWrite_16,},
	{0x0f12, 0x2200,"",eReadWrite_16,},
	{0x0f12, 0xc008,"",eReadWrite_16,},
	{0x0f12, 0x6001,"",eReadWrite_16,},
	{0x0f12, 0x4944,"",eReadWrite_16,},
	{0x0f12, 0x4844,"",eReadWrite_16,},
	{0x0f12, 0x2401,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfca4,"",eReadWrite_16,},
	{0x0f12, 0x4943,"",eReadWrite_16,},
	{0x0f12, 0x4844,"",eReadWrite_16,},
	{0x0f12, 0x2702,"",eReadWrite_16,},
	{0x0f12, 0x0022,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfc9e,"",eReadWrite_16,},
	{0x0f12, 0x0260,"",eReadWrite_16,},
	{0x0f12, 0x4c42,"",eReadWrite_16,},
	{0x0f12, 0x8020,"",eReadWrite_16,},
	{0x0f12, 0x2600,"",eReadWrite_16,},
	{0x0f12, 0x8066,"",eReadWrite_16,},
	{0x0f12, 0x4941,"",eReadWrite_16,},
	{0x0f12, 0x4841,"",eReadWrite_16,},
	{0x0f12, 0x6041,"",eReadWrite_16,},
	{0x0f12, 0x4941,"",eReadWrite_16,},
	{0x0f12, 0x4842,"",eReadWrite_16,},
	{0x0f12, 0x003a,"",eReadWrite_16,},
	{0x0f12, 0x2503,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfc90,"",eReadWrite_16,},
	{0x0f12, 0x483d,"",eReadWrite_16,},
	{0x0f12, 0x4940,"",eReadWrite_16,},
	{0x0f12, 0x30c0,"",eReadWrite_16,},
	{0x0f12, 0x63c1,"",eReadWrite_16,},
	{0x0f12, 0x4f3b,"",eReadWrite_16,},
	{0x0f12, 0x483f,"",eReadWrite_16,},
	{0x0f12, 0x3f80,"",eReadWrite_16,},
	{0x0f12, 0x6438,"",eReadWrite_16,},
	{0x0f12, 0x483e,"",eReadWrite_16,},
	{0x0f12, 0x493f,"",eReadWrite_16,},
	{0x0f12, 0x6388,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0x493e,"",eReadWrite_16,},
	{0x0f12, 0x483f,"",eReadWrite_16,},
	{0x0f12, 0x2504,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfc7f,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0x493d,"",eReadWrite_16,},
	{0x0f12, 0x483e,"",eReadWrite_16,},
	{0x0f12, 0x2505,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf8a7,"",eReadWrite_16,},
	{0x0f12, 0x483c,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0x493c,"",eReadWrite_16,},
	{0x0f12, 0x2506,"",eReadWrite_16,},
	{0x0f12, 0x1d80,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf8a0,"",eReadWrite_16,},
	{0x0f12, 0x4838,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0x4939,"",eReadWrite_16,},
	{0x0f12, 0x2507,"",eReadWrite_16,},
	{0x0f12, 0x300c,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf899,"",eReadWrite_16,},
	{0x0f12, 0x4835,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0x4937,"",eReadWrite_16,},
	{0x0f12, 0x2508,"",eReadWrite_16,},
	{0x0f12, 0x3010,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf892,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0x4935,"",eReadWrite_16,},
	{0x0f12, 0x4835,"",eReadWrite_16,},
	{0x0f12, 0x2509,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfc5e,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0x4934,"",eReadWrite_16,},
	{0x0f12, 0x4834,"",eReadWrite_16,},
	{0x0f12, 0x250a,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfc58,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0x4933,"",eReadWrite_16,},
	{0x0f12, 0x4833,"",eReadWrite_16,},
	{0x0f12, 0x250b,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfc52,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0x4932,"",eReadWrite_16,},
	{0x0f12, 0x4832,"",eReadWrite_16,},
	{0x0f12, 0x250c,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfc4c,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0x4931,"",eReadWrite_16,},
	{0x0f12, 0x4831,"",eReadWrite_16,},
	{0x0f12, 0x250d,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfc46,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0x4930,"",eReadWrite_16,},
	{0x0f12, 0x4830,"",eReadWrite_16,},
	{0x0f12, 0x250e,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfc40,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0x492f,"",eReadWrite_16,},
	{0x0f12, 0x482f,"",eReadWrite_16,},
	{0x0f12, 0x250f,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfc3a,"",eReadWrite_16,},
	{0x0f12, 0x8626,"",eReadWrite_16,},
	{0x0f12, 0x20ff,"",eReadWrite_16,},
	{0x0f12, 0x1c40,"",eReadWrite_16,},
	{0x0f12, 0x8660,"",eReadWrite_16,},
	{0x0f12, 0x482c,"",eReadWrite_16,},
	{0x0f12, 0x64f8,"",eReadWrite_16,},
	{0x0f12, 0x492c,"",eReadWrite_16,},
	{0x0f12, 0x482d,"",eReadWrite_16,},
	{0x0f12, 0x2410,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfc2e,"",eReadWrite_16,},
	{0x0f12, 0x492b,"",eReadWrite_16,},
	{0x0f12, 0x482c,"",eReadWrite_16,},
	{0x0f12, 0x0022,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfc29,"",eReadWrite_16,},
	{0x0f12, 0xbcf8,"",eReadWrite_16,},
	{0x0f12, 0xbc08,"",eReadWrite_16,},
	{0x0f12, 0x4718,"",eReadWrite_16,},
	{0x0f12, 0x019c,"",eReadWrite_16,},
	{0x0f12, 0x4ec2,"",eReadWrite_16,},
	{0x0f12, 0x73ff,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x1f90,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x3ccd,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0xe38b,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x3d05,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0xc3b1,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4780,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x3d63,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x0080,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x3d9f,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0xb49d,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x3e4b,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x3dff,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0xffff,"",eReadWrite_16,},
	{0x0f12, 0x00ff,"",eReadWrite_16,},
	{0x0f12, 0x17e0,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x3fc7,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x053d,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0a89,"",eReadWrite_16,},
	{0x0f12, 0x6cd2,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x02c9,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0a9a,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x02d2,"",eReadWrite_16,},
	{0x0f12, 0x4015,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x9e65,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4089,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x7c49,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x40fd,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x7c63,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4119,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x8f01,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x41bb,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x7f3f,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4249,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x98c5,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x43b5,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x6099,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x430f,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x4365,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0xa70b,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4387,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x400d,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0xb570,"",eReadWrite_16,},
	{0x0f12, 0x000c,"",eReadWrite_16,},
	{0x0f12, 0x0015,"",eReadWrite_16,},
	{0x0f12, 0x0029,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfbd4,"",eReadWrite_16,},
	{0x0f12, 0x49f8,"",eReadWrite_16,},
	{0x0f12, 0x00a8,"",eReadWrite_16,},
	{0x0f12, 0x500c,"",eReadWrite_16,},
	{0x0f12, 0xbc70,"",eReadWrite_16,},
	{0x0f12, 0xbc08,"",eReadWrite_16,},
	{0x0f12, 0x4718,"",eReadWrite_16,},
	{0x0f12, 0x6808,"",eReadWrite_16,},
	{0x0f12, 0x0400,"",eReadWrite_16,},
	{0x0f12, 0x0c00,"",eReadWrite_16,},
	{0x0f12, 0x6849,"",eReadWrite_16,},
	{0x0f12, 0x0409,"",eReadWrite_16,},
	{0x0f12, 0x0c09,"",eReadWrite_16,},
	{0x0f12, 0x4af3,"",eReadWrite_16,},
	{0x0f12, 0x8992,"",eReadWrite_16,},
	{0x0f12, 0x2a00,"",eReadWrite_16,},
	{0x0f12, 0xd00d,"",eReadWrite_16,},
	{0x0f12, 0x2300,"",eReadWrite_16,},
	{0x0f12, 0x1a89,"",eReadWrite_16,},
	{0x0f12, 0xd400,"",eReadWrite_16,},
	{0x0f12, 0x000b,"",eReadWrite_16,},
	{0x0f12, 0x0419,"",eReadWrite_16,},
	{0x0f12, 0x0c09,"",eReadWrite_16,},
	{0x0f12, 0x23ff,"",eReadWrite_16,},
	{0x0f12, 0x33c1,"",eReadWrite_16,},
	{0x0f12, 0x1810,"",eReadWrite_16,},
	{0x0f12, 0x4298,"",eReadWrite_16,},
	{0x0f12, 0xd800,"",eReadWrite_16,},
	{0x0f12, 0x0003,"",eReadWrite_16,},
	{0x0f12, 0x0418,"",eReadWrite_16,},
	{0x0f12, 0x0c00,"",eReadWrite_16,},
	{0x0f12, 0x4aeb,"",eReadWrite_16,},
	{0x0f12, 0x8150,"",eReadWrite_16,},
	{0x0f12, 0x8191,"",eReadWrite_16,},
	{0x0f12, 0x4770,"",eReadWrite_16,},
	{0x0f12, 0xb5f3,"",eReadWrite_16,},
	{0x0f12, 0x0004,"",eReadWrite_16,},
	{0x0f12, 0xb081,"",eReadWrite_16,},
	{0x0f12, 0x9802,"",eReadWrite_16,},
	{0x0f12, 0x6800,"",eReadWrite_16,},
	{0x0f12, 0x0600,"",eReadWrite_16,},
	{0x0f12, 0x0e00,"",eReadWrite_16,},
	{0x0f12, 0x2201,"",eReadWrite_16,},
	{0x0f12, 0x0015,"",eReadWrite_16,},
	{0x0f12, 0x0021,"",eReadWrite_16,},
	{0x0f12, 0x3910,"",eReadWrite_16,},
	{0x0f12, 0x408a,"",eReadWrite_16,},
	{0x0f12, 0x40a5,"",eReadWrite_16,},
	{0x0f12, 0x4fe4,"",eReadWrite_16,},
	{0x0f12, 0x0016,"",eReadWrite_16,},
	{0x0f12, 0x2c10,"",eReadWrite_16,},
	{0x0f12, 0xda03,"",eReadWrite_16,},
	{0x0f12, 0x8839,"",eReadWrite_16,},
	{0x0f12, 0x43a9,"",eReadWrite_16,},
	{0x0f12, 0x8039,"",eReadWrite_16,},
	{0x0f12, 0xe002,"",eReadWrite_16,},
	{0x0f12, 0x8879,"",eReadWrite_16,},
	{0x0f12, 0x43b1,"",eReadWrite_16,},
	{0x0f12, 0x8079,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfba0,"",eReadWrite_16,},
	{0x0f12, 0x2c10,"",eReadWrite_16,},
	{0x0f12, 0xda03,"",eReadWrite_16,},
	{0x0f12, 0x8839,"",eReadWrite_16,},
	{0x0f12, 0x4329,"",eReadWrite_16,},
	{0x0f12, 0x8039,"",eReadWrite_16,},
	{0x0f12, 0xe002,"",eReadWrite_16,},
	{0x0f12, 0x8879,"",eReadWrite_16,},
	{0x0f12, 0x4331,"",eReadWrite_16,},
	{0x0f12, 0x8079,"",eReadWrite_16,},
	{0x0f12, 0x49da,"",eReadWrite_16,},
	{0x0f12, 0x8809,"",eReadWrite_16,},
	{0x0f12, 0x2900,"",eReadWrite_16,},
	{0x0f12, 0xd102,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfb99,"",eReadWrite_16,},
	{0x0f12, 0x2000,"",eReadWrite_16,},
	{0x0f12, 0x9902,"",eReadWrite_16,},
	{0x0f12, 0x6008,"",eReadWrite_16,},
	{0x0f12, 0xbcfe,"",eReadWrite_16,},
	{0x0f12, 0xbc08,"",eReadWrite_16,},
	{0x0f12, 0x4718,"",eReadWrite_16,},
	{0x0f12, 0xb538,"",eReadWrite_16,},
	{0x0f12, 0x9c04,"",eReadWrite_16,},
	{0x0f12, 0x0015,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0x9400,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfb94,"",eReadWrite_16,},
	{0x0f12, 0x4ad1,"",eReadWrite_16,},
	{0x0f12, 0x8811,"",eReadWrite_16,},
	{0x0f12, 0x2900,"",eReadWrite_16,},
	{0x0f12, 0xd00f,"",eReadWrite_16,},
	{0x0f12, 0x8820,"",eReadWrite_16,},
	{0x0f12, 0x4281,"",eReadWrite_16,},
	{0x0f12, 0xd20c,"",eReadWrite_16,},
	{0x0f12, 0x8861,"",eReadWrite_16,},
	{0x0f12, 0x8853,"",eReadWrite_16,},
	{0x0f12, 0x4299,"",eReadWrite_16,},
	{0x0f12, 0xd200,"",eReadWrite_16,},
	{0x0f12, 0x1e40,"",eReadWrite_16,},
	{0x0f12, 0x0400,"",eReadWrite_16,},
	{0x0f12, 0x0c00,"",eReadWrite_16,},
	{0x0f12, 0x8020,"",eReadWrite_16,},
	{0x0f12, 0x8851,"",eReadWrite_16,},
	{0x0f12, 0x8061,"",eReadWrite_16,},
	{0x0f12, 0x4368,"",eReadWrite_16,},
	{0x0f12, 0x1840,"",eReadWrite_16,},
	{0x0f12, 0x6060,"",eReadWrite_16,},
	{0x0f12, 0xbc38,"",eReadWrite_16,},
	{0x0f12, 0xbc08,"",eReadWrite_16,},
	{0x0f12, 0x4718,"",eReadWrite_16,},
	{0x0f12, 0xb5f8,"",eReadWrite_16,},
	{0x0f12, 0x0004,"",eReadWrite_16,},
	{0x0f12, 0x6808,"",eReadWrite_16,},
	{0x0f12, 0x0400,"",eReadWrite_16,},
	{0x0f12, 0x0c00,"",eReadWrite_16,},
	{0x0f12, 0x2201,"",eReadWrite_16,},
	{0x0f12, 0x0015,"",eReadWrite_16,},
	{0x0f12, 0x0021,"",eReadWrite_16,},
	{0x0f12, 0x3910,"",eReadWrite_16,},
	{0x0f12, 0x408a,"",eReadWrite_16,},
	{0x0f12, 0x40a5,"",eReadWrite_16,},
	{0x0f12, 0x4fbe,"",eReadWrite_16,},
	{0x0f12, 0x0016,"",eReadWrite_16,},
	{0x0f12, 0x2c10,"",eReadWrite_16,},
	{0x0f12, 0xda03,"",eReadWrite_16,},
	{0x0f12, 0x8839,"",eReadWrite_16,},
	{0x0f12, 0x43a9,"",eReadWrite_16,},
	{0x0f12, 0x8039,"",eReadWrite_16,},
	{0x0f12, 0xe002,"",eReadWrite_16,},
	{0x0f12, 0x8879,"",eReadWrite_16,},
	{0x0f12, 0x43b1,"",eReadWrite_16,},
	{0x0f12, 0x8079,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfb6d,"",eReadWrite_16,},
	{0x0f12, 0x2c10,"",eReadWrite_16,},
	{0x0f12, 0xda03,"",eReadWrite_16,},
	{0x0f12, 0x8838,"",eReadWrite_16,},
	{0x0f12, 0x4328,"",eReadWrite_16,},
	{0x0f12, 0x8038,"",eReadWrite_16,},
	{0x0f12, 0xe002,"",eReadWrite_16,},
	{0x0f12, 0x8878,"",eReadWrite_16,},
	{0x0f12, 0x4330,"",eReadWrite_16,},
	{0x0f12, 0x8078,"",eReadWrite_16,},
	{0x0f12, 0x48b6,"",eReadWrite_16,},
	{0x0f12, 0x8800,"",eReadWrite_16,},
	{0x0f12, 0x0400,"",eReadWrite_16,},
	{0x0f12, 0xd507,"",eReadWrite_16,},
	{0x0f12, 0x4bb5,"",eReadWrite_16,},
	{0x0f12, 0x7819,"",eReadWrite_16,},
	{0x0f12, 0x4ab5,"",eReadWrite_16,},
	{0x0f12, 0x7810,"",eReadWrite_16,},
	{0x0f12, 0x7018,"",eReadWrite_16,},
	{0x0f12, 0x7011,"",eReadWrite_16,},
	{0x0f12, 0x49b4,"",eReadWrite_16,},
	{0x0f12, 0x8188,"",eReadWrite_16,},
	{0x0f12, 0xbcf8,"",eReadWrite_16,},
	{0x0f12, 0xbc08,"",eReadWrite_16,},
	{0x0f12, 0x4718,"",eReadWrite_16,},
	{0x0f12, 0xb538,"",eReadWrite_16,},
	{0x0f12, 0x48b2,"",eReadWrite_16,},
	{0x0f12, 0x4669,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfb58,"",eReadWrite_16,},
	{0x0f12, 0x48b1,"",eReadWrite_16,},
	{0x0f12, 0x49b0,"",eReadWrite_16,},
	{0x0f12, 0x69c2,"",eReadWrite_16,},
	{0x0f12, 0x2400,"",eReadWrite_16,},
	{0x0f12, 0x31a8,"",eReadWrite_16,},
	{0x0f12, 0x2a00,"",eReadWrite_16,},
	{0x0f12, 0xd008,"",eReadWrite_16,},
	{0x0f12, 0x61c4,"",eReadWrite_16,},
	{0x0f12, 0x684a,"",eReadWrite_16,},
	{0x0f12, 0x6242,"",eReadWrite_16,},
	{0x0f12, 0x6282,"",eReadWrite_16,},
	{0x0f12, 0x466b,"",eReadWrite_16,},
	{0x0f12, 0x881a,"",eReadWrite_16,},
	{0x0f12, 0x6302,"",eReadWrite_16,},
	{0x0f12, 0x885a,"",eReadWrite_16,},
	{0x0f12, 0x6342,"",eReadWrite_16,},
	{0x0f12, 0x6a02,"",eReadWrite_16,},
	{0x0f12, 0x2a00,"",eReadWrite_16,},
	{0x0f12, 0xd00a,"",eReadWrite_16,},
	{0x0f12, 0x6204,"",eReadWrite_16,},
	{0x0f12, 0x6849,"",eReadWrite_16,},
	{0x0f12, 0x6281,"",eReadWrite_16,},
	{0x0f12, 0x466b,"",eReadWrite_16,},
	{0x0f12, 0x8819,"",eReadWrite_16,},
	{0x0f12, 0x6301,"",eReadWrite_16,},
	{0x0f12, 0x8859,"",eReadWrite_16,},
	{0x0f12, 0x6341,"",eReadWrite_16,},
	{0x0f12, 0x49a5,"",eReadWrite_16,},
	{0x0f12, 0x88c9,"",eReadWrite_16,},
	{0x0f12, 0x63c1,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfb40,"",eReadWrite_16,},
	{0x0f12, 0xe7a6,"",eReadWrite_16,},
	{0x0f12, 0xb5f0,"",eReadWrite_16,},
	{0x0f12, 0xb08b,"",eReadWrite_16,},
	{0x0f12, 0x20ff,"",eReadWrite_16,},
	{0x0f12, 0x1c40,"",eReadWrite_16,},
	{0x0f12, 0x49a1,"",eReadWrite_16,},
	{0x0f12, 0x89cc,"",eReadWrite_16,},
	{0x0f12, 0x4e9e,"",eReadWrite_16,},
	{0x0f12, 0x6ab1,"",eReadWrite_16,},
	{0x0f12, 0x4284,"",eReadWrite_16,},
	{0x0f12, 0xd101,"",eReadWrite_16,},
	{0x0f12, 0x489f,"",eReadWrite_16,},
	{0x0f12, 0x6081,"",eReadWrite_16,},
	{0x0f12, 0x6a70,"",eReadWrite_16,},
	{0x0f12, 0x0200,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfb37,"",eReadWrite_16,},
	{0x0f12, 0x0400,"",eReadWrite_16,},
	{0x0f12, 0x0c00,"",eReadWrite_16,},
	{0x0f12, 0x4a96,"",eReadWrite_16,},
	{0x0f12, 0x8a11,"",eReadWrite_16,},
	{0x0f12, 0x9109,"",eReadWrite_16,},
	{0x0f12, 0x2101,"",eReadWrite_16,},
	{0x0f12, 0x0349,"",eReadWrite_16,},
	{0x0f12, 0x4288,"",eReadWrite_16,},
	{0x0f12, 0xd200,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x4a92,"",eReadWrite_16,},
	{0x0f12, 0x8211,"",eReadWrite_16,},
	{0x0f12, 0x4d97,"",eReadWrite_16,},
	{0x0f12, 0x8829,"",eReadWrite_16,},
	{0x0f12, 0x9108,"",eReadWrite_16,},
	{0x0f12, 0x4a8b,"",eReadWrite_16,},
	{0x0f12, 0x2303,"",eReadWrite_16,},
	{0x0f12, 0x3222,"",eReadWrite_16,},
	{0x0f12, 0x1f91,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfb28,"",eReadWrite_16,},
	{0x0f12, 0x8028,"",eReadWrite_16,},
	{0x0f12, 0x488e,"",eReadWrite_16,},
	{0x0f12, 0x4987,"",eReadWrite_16,},
	{0x0f12, 0x6bc2,"",eReadWrite_16,},
	{0x0f12, 0x6ac0,"",eReadWrite_16,},
	{0x0f12, 0x4282,"",eReadWrite_16,},
	{0x0f12, 0xd201,"",eReadWrite_16,},
	{0x0f12, 0x8cc8,"",eReadWrite_16,},
	{0x0f12, 0x8028,"",eReadWrite_16,},
	{0x0f12, 0x88e8,"",eReadWrite_16,},
	{0x0f12, 0x9007,"",eReadWrite_16,},
	{0x0f12, 0x2240,"",eReadWrite_16,},
	{0x0f12, 0x4310,"",eReadWrite_16,},
	{0x0f12, 0x80e8,"",eReadWrite_16,},
	{0x0f12, 0x2000,"",eReadWrite_16,},
	{0x0f12, 0x0041,"",eReadWrite_16,},
	{0x0f12, 0x194b,"",eReadWrite_16,},
	{0x0f12, 0x001e,"",eReadWrite_16,},
	{0x0f12, 0x3680,"",eReadWrite_16,},
	{0x0f12, 0x8bb2,"",eReadWrite_16,},
	{0x0f12, 0xaf04,"",eReadWrite_16,},
	{0x0f12, 0x527a,"",eReadWrite_16,},
	{0x0f12, 0x4a7d,"",eReadWrite_16,},
	{0x0f12, 0x188a,"",eReadWrite_16,},
	{0x0f12, 0x8897,"",eReadWrite_16,},
	{0x0f12, 0x83b7,"",eReadWrite_16,},
	{0x0f12, 0x33a0,"",eReadWrite_16,},
	{0x0f12, 0x891f,"",eReadWrite_16,},
	{0x0f12, 0xae01,"",eReadWrite_16,},
	{0x0f12, 0x5277,"",eReadWrite_16,},
	{0x0f12, 0x8a11,"",eReadWrite_16,},
	{0x0f12, 0x8119,"",eReadWrite_16,},
	{0x0f12, 0x1c40,"",eReadWrite_16,},
	{0x0f12, 0x0400,"",eReadWrite_16,},
	{0x0f12, 0x0c00,"",eReadWrite_16,},
	{0x0f12, 0x2806,"",eReadWrite_16,},
	{0x0f12, 0xd3e9,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfb09,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfb0f,"",eReadWrite_16,},
	{0x0f12, 0x4f79,"",eReadWrite_16,},
	{0x0f12, 0x37a8,"",eReadWrite_16,},
	{0x0f12, 0x2800,"",eReadWrite_16,},
	{0x0f12, 0xd10a,"",eReadWrite_16,},
	{0x0f12, 0x1fe0,"",eReadWrite_16,},
	{0x0f12, 0x38fd,"",eReadWrite_16,},
	{0x0f12, 0xd001,"",eReadWrite_16,},
	{0x0f12, 0x1cc0,"",eReadWrite_16,},
	{0x0f12, 0xd105,"",eReadWrite_16,},
	{0x0f12, 0x4874,"",eReadWrite_16,},
	{0x0f12, 0x8829,"",eReadWrite_16,},
	{0x0f12, 0x3818,"",eReadWrite_16,},
	{0x0f12, 0x6840,"",eReadWrite_16,},
	{0x0f12, 0x4348,"",eReadWrite_16,},
	{0x0f12, 0x6078,"",eReadWrite_16,},
	{0x0f12, 0x4972,"",eReadWrite_16,},
	{0x0f12, 0x6878,"",eReadWrite_16,},
	{0x0f12, 0x6b89,"",eReadWrite_16,},
	{0x0f12, 0x4288,"",eReadWrite_16,},
	{0x0f12, 0xd300,"",eReadWrite_16,},
	{0x0f12, 0x0008,"",eReadWrite_16,},
	{0x0f12, 0x6078,"",eReadWrite_16,},
	{0x0f12, 0x2000,"",eReadWrite_16,},
	{0x0f12, 0x0041,"",eReadWrite_16,},
	{0x0f12, 0xaa04,"",eReadWrite_16,},
	{0x0f12, 0x5a53,"",eReadWrite_16,},
	{0x0f12, 0x194a,"",eReadWrite_16,},
	{0x0f12, 0x269c,"",eReadWrite_16,},
	{0x0f12, 0x52b3,"",eReadWrite_16,},
	{0x0f12, 0xab01,"",eReadWrite_16,},
	{0x0f12, 0x5a59,"",eReadWrite_16,},
	{0x0f12, 0x32a0,"",eReadWrite_16,},
	{0x0f12, 0x8111,"",eReadWrite_16,},
	{0x0f12, 0x1c40,"",eReadWrite_16,},
	{0x0f12, 0x0400,"",eReadWrite_16,},
	{0x0f12, 0x0c00,"",eReadWrite_16,},
	{0x0f12, 0x2806,"",eReadWrite_16,},
	{0x0f12, 0xd3f0,"",eReadWrite_16,},
	{0x0f12, 0x4965,"",eReadWrite_16,},
	{0x0f12, 0x9809,"",eReadWrite_16,},
	{0x0f12, 0x8208,"",eReadWrite_16,},
	{0x0f12, 0x9808,"",eReadWrite_16,},
	{0x0f12, 0x8028,"",eReadWrite_16,},
	{0x0f12, 0x9807,"",eReadWrite_16,},
	{0x0f12, 0x80e8,"",eReadWrite_16,},
	{0x0f12, 0x1fe0,"",eReadWrite_16,},
	{0x0f12, 0x38fd,"",eReadWrite_16,},
	{0x0f12, 0xd13b,"",eReadWrite_16,},
	{0x0f12, 0x4d64,"",eReadWrite_16,},
	{0x0f12, 0x89e8,"",eReadWrite_16,},
	{0x0f12, 0x1fc1,"",eReadWrite_16,},
	{0x0f12, 0x39ff,"",eReadWrite_16,},
	{0x0f12, 0xd136,"",eReadWrite_16,},
	{0x0f12, 0x4c5f,"",eReadWrite_16,},
	{0x0f12, 0x8ae0,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfade,"",eReadWrite_16,},
	{0x0f12, 0x0006,"",eReadWrite_16,},
	{0x0f12, 0x8b20,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfae2,"",eReadWrite_16,},
	{0x0f12, 0x9000,"",eReadWrite_16,},
	{0x0f12, 0x6aa1,"",eReadWrite_16,},
	{0x0f12, 0x6878,"",eReadWrite_16,},
	{0x0f12, 0x1809,"",eReadWrite_16,},
	{0x0f12, 0x0200,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfab5,"",eReadWrite_16,},
	{0x0f12, 0x0400,"",eReadWrite_16,},
	{0x0f12, 0x0c00,"",eReadWrite_16,},
	{0x0f12, 0x0022,"",eReadWrite_16,},
	{0x0f12, 0x3246,"",eReadWrite_16,},
	{0x0f12, 0x0011,"",eReadWrite_16,},
	{0x0f12, 0x310a,"",eReadWrite_16,},
	{0x0f12, 0x2305,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfab2,"",eReadWrite_16,},
	{0x0f12, 0x66e8,"",eReadWrite_16,},
	{0x0f12, 0x6b23,"",eReadWrite_16,},
	{0x0f12, 0x0002,"",eReadWrite_16,},
	{0x0f12, 0x0031,"",eReadWrite_16,},
	{0x0f12, 0x0018,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfad3,"",eReadWrite_16,},
	{0x0f12, 0x466b,"",eReadWrite_16,},
	{0x0f12, 0x8518,"",eReadWrite_16,},
	{0x0f12, 0x6eea,"",eReadWrite_16,},
	{0x0f12, 0x6b60,"",eReadWrite_16,},
	{0x0f12, 0x9900,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfacc,"",eReadWrite_16,},
	{0x0f12, 0x466b,"",eReadWrite_16,},
	{0x0f12, 0x8558,"",eReadWrite_16,},
	{0x0f12, 0x0029,"",eReadWrite_16,},
	{0x0f12, 0x980a,"",eReadWrite_16,},
	{0x0f12, 0x3170,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfacd,"",eReadWrite_16,},
	{0x0f12, 0x0028,"",eReadWrite_16,},
	{0x0f12, 0x3060,"",eReadWrite_16,},
	{0x0f12, 0x8a02,"",eReadWrite_16,},
	{0x0f12, 0x4946,"",eReadWrite_16,},
	{0x0f12, 0x3128,"",eReadWrite_16,},
	{0x0f12, 0x808a,"",eReadWrite_16,},
	{0x0f12, 0x8a42,"",eReadWrite_16,},
	{0x0f12, 0x80ca,"",eReadWrite_16,},
	{0x0f12, 0x8a80,"",eReadWrite_16,},
	{0x0f12, 0x8108,"",eReadWrite_16,},
	{0x0f12, 0xb00b,"",eReadWrite_16,},
	{0x0f12, 0xbcf0,"",eReadWrite_16,},
	{0x0f12, 0xbc08,"",eReadWrite_16,},
	{0x0f12, 0x4718,"",eReadWrite_16,},
	{0x0f12, 0xb570,"",eReadWrite_16,},
	{0x0f12, 0x2400,"",eReadWrite_16,},
	{0x0f12, 0x4d46,"",eReadWrite_16,},
	{0x0f12, 0x4846,"",eReadWrite_16,},
	{0x0f12, 0x8881,"",eReadWrite_16,},
	{0x0f12, 0x4846,"",eReadWrite_16,},
	{0x0f12, 0x8041,"",eReadWrite_16,},
	{0x0f12, 0x2101,"",eReadWrite_16,},
	{0x0f12, 0x8001,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfabc,"",eReadWrite_16,},
	{0x0f12, 0x4842,"",eReadWrite_16,},
	{0x0f12, 0x3820,"",eReadWrite_16,},
	{0x0f12, 0x8bc0,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfabf,"",eReadWrite_16,},
	{0x0f12, 0x4b42,"",eReadWrite_16,},
	{0x0f12, 0x220d,"",eReadWrite_16,},
	{0x0f12, 0x0712,"",eReadWrite_16,},
	{0x0f12, 0x18a8,"",eReadWrite_16,},
	{0x0f12, 0x8806,"",eReadWrite_16,},
	{0x0f12, 0x00e1,"",eReadWrite_16,},
	{0x0f12, 0x18c9,"",eReadWrite_16,},
	{0x0f12, 0x81ce,"",eReadWrite_16,},
	{0x0f12, 0x8846,"",eReadWrite_16,},
	{0x0f12, 0x818e,"",eReadWrite_16,},
	{0x0f12, 0x8886,"",eReadWrite_16,},
	{0x0f12, 0x824e,"",eReadWrite_16,},
	{0x0f12, 0x88c0,"",eReadWrite_16,},
	{0x0f12, 0x8208,"",eReadWrite_16,},
	{0x0f12, 0x3508,"",eReadWrite_16,},
	{0x0f12, 0x042d,"",eReadWrite_16,},
	{0x0f12, 0x0c2d,"",eReadWrite_16,},
	{0x0f12, 0x1c64,"",eReadWrite_16,},
	{0x0f12, 0x0424,"",eReadWrite_16,},
	{0x0f12, 0x0c24,"",eReadWrite_16,},
	{0x0f12, 0x2c07,"",eReadWrite_16,},
	{0x0f12, 0xd3ec,"",eReadWrite_16,},
	{0x0f12, 0xe658,"",eReadWrite_16,},
	{0x0f12, 0xb510,"",eReadWrite_16,},
	{0x0f12, 0x4834,"",eReadWrite_16,},
	{0x0f12, 0x4c34,"",eReadWrite_16,},
	{0x0f12, 0x88c0,"",eReadWrite_16,},
	{0x0f12, 0x8060,"",eReadWrite_16,},
	{0x0f12, 0x2001,"",eReadWrite_16,},
	{0x0f12, 0x8020,"",eReadWrite_16,},
	{0x0f12, 0x4831,"",eReadWrite_16,},
	{0x0f12, 0x3820,"",eReadWrite_16,},
	{0x0f12, 0x8bc0,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfa9c,"",eReadWrite_16,},
	{0x0f12, 0x88e0,"",eReadWrite_16,},
	{0x0f12, 0x4a31,"",eReadWrite_16,},
	{0x0f12, 0x2800,"",eReadWrite_16,},
	{0x0f12, 0xd003,"",eReadWrite_16,},
	{0x0f12, 0x4930,"",eReadWrite_16,},
	{0x0f12, 0x8849,"",eReadWrite_16,},
	{0x0f12, 0x2900,"",eReadWrite_16,},
	{0x0f12, 0xd009,"",eReadWrite_16,},
	{0x0f12, 0x2001,"",eReadWrite_16,},
	{0x0f12, 0x03c0,"",eReadWrite_16,},
	{0x0f12, 0x8050,"",eReadWrite_16,},
	{0x0f12, 0x80d0,"",eReadWrite_16,},
	{0x0f12, 0x2000,"",eReadWrite_16,},
	{0x0f12, 0x8090,"",eReadWrite_16,},
	{0x0f12, 0x8110,"",eReadWrite_16,},
	{0x0f12, 0xbc10,"",eReadWrite_16,},
	{0x0f12, 0xbc08,"",eReadWrite_16,},
	{0x0f12, 0x4718,"",eReadWrite_16,},
	{0x0f12, 0x8050,"",eReadWrite_16,},
	{0x0f12, 0x8920,"",eReadWrite_16,},
	{0x0f12, 0x80d0,"",eReadWrite_16,},
	{0x0f12, 0x8960,"",eReadWrite_16,},
	{0x0f12, 0x0400,"",eReadWrite_16,},
	{0x0f12, 0x1400,"",eReadWrite_16,},
	{0x0f12, 0x8090,"",eReadWrite_16,},
	{0x0f12, 0x89a1,"",eReadWrite_16,},
	{0x0f12, 0x0409,"",eReadWrite_16,},
	{0x0f12, 0x1409,"",eReadWrite_16,},
	{0x0f12, 0x8111,"",eReadWrite_16,},
	{0x0f12, 0x89e3,"",eReadWrite_16,},
	{0x0f12, 0x8a24,"",eReadWrite_16,},
	{0x0f12, 0x2b00,"",eReadWrite_16,},
	{0x0f12, 0xd104,"",eReadWrite_16,},
	{0x0f12, 0x17c3,"",eReadWrite_16,},
	{0x0f12, 0x0f5b,"",eReadWrite_16,},
	{0x0f12, 0x1818,"",eReadWrite_16,},
	{0x0f12, 0x10c0,"",eReadWrite_16,},
	{0x0f12, 0x8090,"",eReadWrite_16,},
	{0x0f12, 0x2c00,"",eReadWrite_16,},
	{0x0f12, 0xd1e6,"",eReadWrite_16,},
	{0x0f12, 0x17c8,"",eReadWrite_16,},
	{0x0f12, 0x0f40,"",eReadWrite_16,},
	{0x0f12, 0x1840,"",eReadWrite_16,},
	{0x0f12, 0x10c0,"",eReadWrite_16,},
	{0x0f12, 0x8110,"",eReadWrite_16,},
	{0x0f12, 0xe7e0,"",eReadWrite_16,},
	{0x0f12, 0xb510,"",eReadWrite_16,},
	{0x0f12, 0x000c,"",eReadWrite_16,},
	{0x0f12, 0x4919,"",eReadWrite_16,},
	{0x0f12, 0x2204,"",eReadWrite_16,},
	{0x0f12, 0x6820,"",eReadWrite_16,},
	{0x0f12, 0x5e8a,"",eReadWrite_16,},
	{0x0f12, 0x0140,"",eReadWrite_16,},
	{0x0f12, 0x1a80,"",eReadWrite_16,},
	{0x0f12, 0x0280,"",eReadWrite_16,},
	{0x0f12, 0x8849,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfa6a,"",eReadWrite_16,},
	{0x0f12, 0x6020,"",eReadWrite_16,},
	{0x0f12, 0xe7d2,"",eReadWrite_16,},
	{0x0f12, 0x38d4,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x17d0,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x5000,"",eReadWrite_16,},
	{0x0f12, 0xd000,"",eReadWrite_16,},
	{0x0f12, 0x1100,"",eReadWrite_16,},
	{0x0f12, 0xd000,"",eReadWrite_16,},
	{0x0f12, 0x171a,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x4780,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x2fca,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x2fc5,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x2fc6,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x2ed8,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x2bd0,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x17e0,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x2de8,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x37e0,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x210c,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x1484,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0xa006,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0724,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0xa000,"",eReadWrite_16,},
	{0x0f12, 0xd000,"",eReadWrite_16,},
	{0x0f12, 0x2270,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x2558,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x146c,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0xb510,"",eReadWrite_16,},
	{0x0f12, 0x000c,"",eReadWrite_16,},
	{0x0f12, 0x49c7,"",eReadWrite_16,},
	{0x0f12, 0x2208,"",eReadWrite_16,},
	{0x0f12, 0x6820,"",eReadWrite_16,},
	{0x0f12, 0x5e8a,"",eReadWrite_16,},
	{0x0f12, 0x0140,"",eReadWrite_16,},
	{0x0f12, 0x1a80,"",eReadWrite_16,},
	{0x0f12, 0x0280,"",eReadWrite_16,},
	{0x0f12, 0x88c9,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfa30,"",eReadWrite_16,},
	{0x0f12, 0x6020,"",eReadWrite_16,},
	{0x0f12, 0xe798,"",eReadWrite_16,},
	{0x0f12, 0xb5fe,"",eReadWrite_16,},
	{0x0f12, 0x000c,"",eReadWrite_16,},
	{0x0f12, 0x6825,"",eReadWrite_16,},
	{0x0f12, 0x6866,"",eReadWrite_16,},
	{0x0f12, 0x68a0,"",eReadWrite_16,},
	{0x0f12, 0x9001,"",eReadWrite_16,},
	{0x0f12, 0x68e7,"",eReadWrite_16,},
	{0x0f12, 0x1ba8,"",eReadWrite_16,},
	{0x0f12, 0x42b5,"",eReadWrite_16,},
	{0x0f12, 0xda00,"",eReadWrite_16,},
	{0x0f12, 0x1b70,"",eReadWrite_16,},
	{0x0f12, 0x9000,"",eReadWrite_16,},
	{0x0f12, 0x49bb,"",eReadWrite_16,},
	{0x0f12, 0x48bc,"",eReadWrite_16,},
	{0x0f12, 0x884a,"",eReadWrite_16,},
	{0x0f12, 0x8843,"",eReadWrite_16,},
	{0x0f12, 0x435a,"",eReadWrite_16,},
	{0x0f12, 0x2304,"",eReadWrite_16,},
	{0x0f12, 0x5ecb,"",eReadWrite_16,},
	{0x0f12, 0x0a92,"",eReadWrite_16,},
	{0x0f12, 0x18d2,"",eReadWrite_16,},
	{0x0f12, 0x02d2,"",eReadWrite_16,},
	{0x0f12, 0x0c12,"",eReadWrite_16,},
	{0x0f12, 0x88cb,"",eReadWrite_16,},
	{0x0f12, 0x8880,"",eReadWrite_16,},
	{0x0f12, 0x4343,"",eReadWrite_16,},
	{0x0f12, 0x0a98,"",eReadWrite_16,},
	{0x0f12, 0x2308,"",eReadWrite_16,},
	{0x0f12, 0x5ecb,"",eReadWrite_16,},
	{0x0f12, 0x18c0,"",eReadWrite_16,},
	{0x0f12, 0x02c0,"",eReadWrite_16,},
	{0x0f12, 0x0c00,"",eReadWrite_16,},
	{0x0f12, 0x0411,"",eReadWrite_16,},
	{0x0f12, 0x0400,"",eReadWrite_16,},
	{0x0f12, 0x1409,"",eReadWrite_16,},
	{0x0f12, 0x1400,"",eReadWrite_16,},
	{0x0f12, 0x1a08,"",eReadWrite_16,},
	{0x0f12, 0x49b0,"",eReadWrite_16,},
	{0x0f12, 0x39e0,"",eReadWrite_16,},
	{0x0f12, 0x6148,"",eReadWrite_16,},
	{0x0f12, 0x9801,"",eReadWrite_16,},
	{0x0f12, 0x3040,"",eReadWrite_16,},
	{0x0f12, 0x7880,"",eReadWrite_16,},
	{0x0f12, 0x2800,"",eReadWrite_16,},
	{0x0f12, 0xd103,"",eReadWrite_16,},
	{0x0f12, 0x9801,"",eReadWrite_16,},
	{0x0f12, 0x0029,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xfa03,"",eReadWrite_16,},
	{0x0f12, 0x8839,"",eReadWrite_16,},
	{0x0f12, 0x9800,"",eReadWrite_16,},
	{0x0f12, 0x4281,"",eReadWrite_16,},
	{0x0f12, 0xd814,"",eReadWrite_16,},
	{0x0f12, 0x8879,"",eReadWrite_16,},
	{0x0f12, 0x9800,"",eReadWrite_16,},
	{0x0f12, 0x4281,"",eReadWrite_16,},
	{0x0f12, 0xd20c,"",eReadWrite_16,},
	{0x0f12, 0x9801,"",eReadWrite_16,},
	{0x0f12, 0x0029,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf9ff,"",eReadWrite_16,},
	{0x0f12, 0x9801,"",eReadWrite_16,},
	{0x0f12, 0x0029,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf9fb,"",eReadWrite_16,},
	{0x0f12, 0x9801,"",eReadWrite_16,},
	{0x0f12, 0x0029,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf9f7,"",eReadWrite_16,},
	{0x0f12, 0xe003,"",eReadWrite_16,},
	{0x0f12, 0x9801,"",eReadWrite_16,},
	{0x0f12, 0x0029,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf9f2,"",eReadWrite_16,},
	{0x0f12, 0x9801,"",eReadWrite_16,},
	{0x0f12, 0x0032,"",eReadWrite_16,},
	{0x0f12, 0x0039,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf9f5,"",eReadWrite_16,},
	{0x0f12, 0x6020,"",eReadWrite_16,},
	{0x0f12, 0xe5d0,"",eReadWrite_16,},
	{0x0f12, 0xb57c,"",eReadWrite_16,},
	{0x0f12, 0x489a,"",eReadWrite_16,},
	{0x0f12, 0xa901,"",eReadWrite_16,},
	{0x0f12, 0x0004,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf979,"",eReadWrite_16,},
	{0x0f12, 0x466b,"",eReadWrite_16,},
	{0x0f12, 0x88d9,"",eReadWrite_16,},
	{0x0f12, 0x8898,"",eReadWrite_16,},
	{0x0f12, 0x4b95,"",eReadWrite_16,},
	{0x0f12, 0x3346,"",eReadWrite_16,},
	{0x0f12, 0x1e9a,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf9ed,"",eReadWrite_16,},
	{0x0f12, 0x4894,"",eReadWrite_16,},
	{0x0f12, 0x4992,"",eReadWrite_16,},
	{0x0f12, 0x3812,"",eReadWrite_16,},
	{0x0f12, 0x3140,"",eReadWrite_16,},
	{0x0f12, 0x8a42,"",eReadWrite_16,},
	{0x0f12, 0x888b,"",eReadWrite_16,},
	{0x0f12, 0x18d2,"",eReadWrite_16,},
	{0x0f12, 0x8242,"",eReadWrite_16,},
	{0x0f12, 0x8ac2,"",eReadWrite_16,},
	{0x0f12, 0x88c9,"",eReadWrite_16,},
	{0x0f12, 0x1851,"",eReadWrite_16,},
	{0x0f12, 0x82c1,"",eReadWrite_16,},
	{0x0f12, 0x0020,"",eReadWrite_16,},
	{0x0f12, 0x4669,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf961,"",eReadWrite_16,},
	{0x0f12, 0x488d,"",eReadWrite_16,},
	{0x0f12, 0x214d,"",eReadWrite_16,},
	{0x0f12, 0x8301,"",eReadWrite_16,},
	{0x0f12, 0x2196,"",eReadWrite_16,},
	{0x0f12, 0x8381,"",eReadWrite_16,},
	{0x0f12, 0x211d,"",eReadWrite_16,},
	{0x0f12, 0x3020,"",eReadWrite_16,},
	{0x0f12, 0x8001,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf9db,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf9e1,"",eReadWrite_16,},
	{0x0f12, 0x4888,"",eReadWrite_16,},
	{0x0f12, 0x4c88,"",eReadWrite_16,},
	{0x0f12, 0x6e00,"",eReadWrite_16,},
	{0x0f12, 0x60e0,"",eReadWrite_16,},
	{0x0f12, 0x466b,"",eReadWrite_16,},
	{0x0f12, 0x8818,"",eReadWrite_16,},
	{0x0f12, 0x8859,"",eReadWrite_16,},
	{0x0f12, 0x0025,"",eReadWrite_16,},
	{0x0f12, 0x1a40,"",eReadWrite_16,},
	{0x0f12, 0x3540,"",eReadWrite_16,},
	{0x0f12, 0x61a8,"",eReadWrite_16,},
	{0x0f12, 0x487f,"",eReadWrite_16,},
	{0x0f12, 0x9900,"",eReadWrite_16,},
	{0x0f12, 0x3060,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf9d9,"",eReadWrite_16,},
	{0x0f12, 0x466b,"",eReadWrite_16,},
	{0x0f12, 0x8819,"",eReadWrite_16,},
	{0x0f12, 0x1de0,"",eReadWrite_16,},
	{0x0f12, 0x30f9,"",eReadWrite_16,},
	{0x0f12, 0x8741,"",eReadWrite_16,},
	{0x0f12, 0x8859,"",eReadWrite_16,},
	{0x0f12, 0x8781,"",eReadWrite_16,},
	{0x0f12, 0x2000,"",eReadWrite_16,},
	{0x0f12, 0x71a0,"",eReadWrite_16,},
	{0x0f12, 0x74a8,"",eReadWrite_16,},
	{0x0f12, 0xbc7c,"",eReadWrite_16,},
	{0x0f12, 0xbc08,"",eReadWrite_16,},
	{0x0f12, 0x4718,"",eReadWrite_16,},
	{0x0f12, 0xb5f8,"",eReadWrite_16,},
	{0x0f12, 0x0005,"",eReadWrite_16,},
	{0x0f12, 0x6808,"",eReadWrite_16,},
	{0x0f12, 0x0400,"",eReadWrite_16,},
	{0x0f12, 0x0c00,"",eReadWrite_16,},
	{0x0f12, 0x684a,"",eReadWrite_16,},
	{0x0f12, 0x0412,"",eReadWrite_16,},
	{0x0f12, 0x0c12,"",eReadWrite_16,},
	{0x0f12, 0x688e,"",eReadWrite_16,},
	{0x0f12, 0x68cc,"",eReadWrite_16,},
	{0x0f12, 0x4970,"",eReadWrite_16,},
	{0x0f12, 0x884b,"",eReadWrite_16,},
	{0x0f12, 0x4343,"",eReadWrite_16,},
	{0x0f12, 0x0a98,"",eReadWrite_16,},
	{0x0f12, 0x2304,"",eReadWrite_16,},
	{0x0f12, 0x5ecb,"",eReadWrite_16,},
	{0x0f12, 0x18c0,"",eReadWrite_16,},
	{0x0f12, 0x02c0,"",eReadWrite_16,},
	{0x0f12, 0x0c00,"",eReadWrite_16,},
	{0x0f12, 0x88cb,"",eReadWrite_16,},
	{0x0f12, 0x4353,"",eReadWrite_16,},
	{0x0f12, 0x0a9a,"",eReadWrite_16,},
	{0x0f12, 0x2308,"",eReadWrite_16,},
	{0x0f12, 0x5ecb,"",eReadWrite_16,},
	{0x0f12, 0x18d1,"",eReadWrite_16,},
	{0x0f12, 0x02c9,"",eReadWrite_16,},
	{0x0f12, 0x0c09,"",eReadWrite_16,},
	{0x0f12, 0x2701,"",eReadWrite_16,},
	{0x0f12, 0x003a,"",eReadWrite_16,},
	{0x0f12, 0x40aa,"",eReadWrite_16,},
	{0x0f12, 0x9200,"",eReadWrite_16,},
	{0x0f12, 0x002a,"",eReadWrite_16,},
	{0x0f12, 0x3a10,"",eReadWrite_16,},
	{0x0f12, 0x4097,"",eReadWrite_16,},
	{0x0f12, 0x2d10,"",eReadWrite_16,},
	{0x0f12, 0xda06,"",eReadWrite_16,},
	{0x0f12, 0x4a69,"",eReadWrite_16,},
	{0x0f12, 0x9b00,"",eReadWrite_16,},
	{0x0f12, 0x8812,"",eReadWrite_16,},
	{0x0f12, 0x439a,"",eReadWrite_16,},
	{0x0f12, 0x4b67,"",eReadWrite_16,},
	{0x0f12, 0x801a,"",eReadWrite_16,},
	{0x0f12, 0xe003,"",eReadWrite_16,},
	{0x0f12, 0x4b66,"",eReadWrite_16,},
	{0x0f12, 0x885a,"",eReadWrite_16,},
	{0x0f12, 0x43ba,"",eReadWrite_16,},
	{0x0f12, 0x805a,"",eReadWrite_16,},
	{0x0f12, 0x0023,"",eReadWrite_16,},
	{0x0f12, 0x0032,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf981,"",eReadWrite_16,},
	{0x0f12, 0x2d10,"",eReadWrite_16,},
	{0x0f12, 0xda05,"",eReadWrite_16,},
	{0x0f12, 0x4961,"",eReadWrite_16,},
	{0x0f12, 0x9a00,"",eReadWrite_16,},
	{0x0f12, 0x8808,"",eReadWrite_16,},
	{0x0f12, 0x4310,"",eReadWrite_16,},
	{0x0f12, 0x8008,"",eReadWrite_16,},
	{0x0f12, 0xe003,"",eReadWrite_16,},
	{0x0f12, 0x485e,"",eReadWrite_16,},
	{0x0f12, 0x8841,"",eReadWrite_16,},
	{0x0f12, 0x4339,"",eReadWrite_16,},
	{0x0f12, 0x8041,"",eReadWrite_16,},
	{0x0f12, 0x4d5b,"",eReadWrite_16,},
	{0x0f12, 0x2000,"",eReadWrite_16,},
	{0x0f12, 0x3580,"",eReadWrite_16,},
	{0x0f12, 0x88aa,"",eReadWrite_16,},
	{0x0f12, 0x5e30,"",eReadWrite_16,},
	{0x0f12, 0x2100,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf98d,"",eReadWrite_16,},
	{0x0f12, 0x8030,"",eReadWrite_16,},
	{0x0f12, 0x2000,"",eReadWrite_16,},
	{0x0f12, 0x88aa,"",eReadWrite_16,},
	{0x0f12, 0x5e20,"",eReadWrite_16,},
	{0x0f12, 0x2100,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf986,"",eReadWrite_16,},
	{0x0f12, 0x8020,"",eReadWrite_16,},
	{0x0f12, 0xe587,"",eReadWrite_16,},
	{0x0f12, 0xb510,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf989,"",eReadWrite_16,},
	{0x0f12, 0x4a53,"",eReadWrite_16,},
	{0x0f12, 0x8d50,"",eReadWrite_16,},
	{0x0f12, 0x2800,"",eReadWrite_16,},
	{0x0f12, 0xd007,"",eReadWrite_16,},
	{0x0f12, 0x494e,"",eReadWrite_16,},
	{0x0f12, 0x31c0,"",eReadWrite_16,},
	{0x0f12, 0x684b,"",eReadWrite_16,},
	{0x0f12, 0x4950,"",eReadWrite_16,},
	{0x0f12, 0x4283,"",eReadWrite_16,},
	{0x0f12, 0xd202,"",eReadWrite_16,},
	{0x0f12, 0x8d90,"",eReadWrite_16,},
	{0x0f12, 0x81c8,"",eReadWrite_16,},
	{0x0f12, 0xe6a0,"",eReadWrite_16,},
	{0x0f12, 0x8dd0,"",eReadWrite_16,},
	{0x0f12, 0x81c8,"",eReadWrite_16,},
	{0x0f12, 0xe69d,"",eReadWrite_16,},
	{0x0f12, 0xb5f8,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf97e,"",eReadWrite_16,},
	{0x0f12, 0x4d49,"",eReadWrite_16,},
	{0x0f12, 0x8e28,"",eReadWrite_16,},
	{0x0f12, 0x2800,"",eReadWrite_16,},
	{0x0f12, 0xd01f,"",eReadWrite_16,},
	{0x0f12, 0x4e49,"",eReadWrite_16,},
	{0x0f12, 0x4844,"",eReadWrite_16,},
	{0x0f12, 0x68b4,"",eReadWrite_16,},
	{0x0f12, 0x6800,"",eReadWrite_16,},
	{0x0f12, 0x4284,"",eReadWrite_16,},
	{0x0f12, 0xd903,"",eReadWrite_16,},
	{0x0f12, 0x1a21,"",eReadWrite_16,},
	{0x0f12, 0x0849,"",eReadWrite_16,},
	{0x0f12, 0x1847,"",eReadWrite_16,},
	{0x0f12, 0xe006,"",eReadWrite_16,},
	{0x0f12, 0x4284,"",eReadWrite_16,},
	{0x0f12, 0xd203,"",eReadWrite_16,},
	{0x0f12, 0x1b01,"",eReadWrite_16,},
	{0x0f12, 0x0849,"",eReadWrite_16,},
	{0x0f12, 0x1a47,"",eReadWrite_16,},
	{0x0f12, 0xe000,"",eReadWrite_16,},
	{0x0f12, 0x0027,"",eReadWrite_16,},
	{0x0f12, 0x0020,"",eReadWrite_16,},
	{0x0f12, 0x493b,"",eReadWrite_16,},
	{0x0f12, 0x3120,"",eReadWrite_16,},
	{0x0f12, 0x7a0c,"",eReadWrite_16,},
	{0x0f12, 0x2c00,"",eReadWrite_16,},
	{0x0f12, 0xd004,"",eReadWrite_16,},
	{0x0f12, 0x0200,"",eReadWrite_16,},
	{0x0f12, 0x0039,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf8c3,"",eReadWrite_16,},
	{0x0f12, 0x8668,"",eReadWrite_16,},
	{0x0f12, 0x2c00,"",eReadWrite_16,},
	{0x0f12, 0xd000,"",eReadWrite_16,},
	{0x0f12, 0x60b7,"",eReadWrite_16,},
	{0x0f12, 0xe54d,"",eReadWrite_16,},
	{0x0f12, 0x20ff,"",eReadWrite_16,},
	{0x0f12, 0x1c40,"",eReadWrite_16,},
	{0x0f12, 0x8668,"",eReadWrite_16,},
	{0x0f12, 0xe549,"",eReadWrite_16,},
	{0x0f12, 0xb510,"",eReadWrite_16,},
	{0x0f12, 0x000c,"",eReadWrite_16,},
	{0x0f12, 0x6820,"",eReadWrite_16,},
	{0x0f12, 0x0400,"",eReadWrite_16,},
	{0x0f12, 0x0c00,"",eReadWrite_16,},
	{0x0f12, 0x4933,"",eReadWrite_16,},
	{0x0f12, 0x8e0a,"",eReadWrite_16,},
	{0x0f12, 0x2a00,"",eReadWrite_16,},
	{0x0f12, 0xd003,"",eReadWrite_16,},
	{0x0f12, 0x8e49,"",eReadWrite_16,},
	{0x0f12, 0x0200,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf8ad,"",eReadWrite_16,},
	{0x0f12, 0x6020,"",eReadWrite_16,},
	{0x0f12, 0x0400,"",eReadWrite_16,},
	{0x0f12, 0x0c00,"",eReadWrite_16,},
	{0x0f12, 0xe661,"",eReadWrite_16,},
	{0x0f12, 0xb570,"",eReadWrite_16,},
	{0x0f12, 0x680c,"",eReadWrite_16,},
	{0x0f12, 0x4d2f,"",eReadWrite_16,},
	{0x0f12, 0x0020,"",eReadWrite_16,},
	{0x0f12, 0x6f29,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf946,"",eReadWrite_16,},
	{0x0f12, 0x6f69,"",eReadWrite_16,},
	{0x0f12, 0x1d20,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf942,"",eReadWrite_16,},
	{0x0f12, 0x4827,"",eReadWrite_16,},
	{0x0f12, 0x8e00,"",eReadWrite_16,},
	{0x0f12, 0x2800,"",eReadWrite_16,},
	{0x0f12, 0xd006,"",eReadWrite_16,},
	{0x0f12, 0x4922,"",eReadWrite_16,},
	{0x0f12, 0x2214,"",eReadWrite_16,},
	{0x0f12, 0x3168,"",eReadWrite_16,},
	{0x0f12, 0x0008,"",eReadWrite_16,},
	{0x0f12, 0x383c,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf93f,"",eReadWrite_16,},
	{0x0f12, 0xe488,"",eReadWrite_16,},
	{0x0f12, 0xb5f8,"",eReadWrite_16,},
	{0x0f12, 0x0004,"",eReadWrite_16,},
	{0x0f12, 0x4d24,"",eReadWrite_16,},
	{0x0f12, 0x8b68,"",eReadWrite_16,},
	{0x0f12, 0x2800,"",eReadWrite_16,},
	{0x0f12, 0xd012,"",eReadWrite_16,},
	{0x0f12, 0x4823,"",eReadWrite_16,},
	{0x0f12, 0x8a00,"",eReadWrite_16,},
	{0x0f12, 0x06c0,"",eReadWrite_16,},
	{0x0f12, 0xd50e,"",eReadWrite_16,},
	{0x0f12, 0x4822,"",eReadWrite_16,},
	{0x0f12, 0x7800,"",eReadWrite_16,},
	{0x0f12, 0x2800,"",eReadWrite_16,},
	{0x0f12, 0xd00a,"",eReadWrite_16,},
	{0x0f12, 0x481d,"",eReadWrite_16,},
	{0x0f12, 0x6fc1,"",eReadWrite_16,},
	{0x0f12, 0x2000,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf923,"",eReadWrite_16,},
	{0x0f12, 0x8b28,"",eReadWrite_16,},
	{0x0f12, 0x2201,"",eReadWrite_16,},
	{0x0f12, 0x2180,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf92c,"",eReadWrite_16,},
	{0x0f12, 0x8328,"",eReadWrite_16,},
	{0x0f12, 0x2101,"",eReadWrite_16,},
	{0x0f12, 0x000d,"",eReadWrite_16,},
	{0x0f12, 0x0020,"",eReadWrite_16,},
	{0x0f12, 0x3810,"",eReadWrite_16,},
	{0x0f12, 0x4081,"",eReadWrite_16,},
	{0x0f12, 0x40a5,"",eReadWrite_16,},
	{0x0f12, 0x4f11,"",eReadWrite_16,},
	{0x0f12, 0x000e,"",eReadWrite_16,},
	{0x0f12, 0x2c10,"",eReadWrite_16,},
	{0x0f12, 0xda03,"",eReadWrite_16,},
	{0x0f12, 0x8838,"",eReadWrite_16,},
	{0x0f12, 0x43a8,"",eReadWrite_16,},
	{0x0f12, 0x8038,"",eReadWrite_16,},
	{0x0f12, 0xe002,"",eReadWrite_16,},
	{0x0f12, 0x8878,"",eReadWrite_16,},
	{0x0f12, 0x43b0,"",eReadWrite_16,},
	{0x0f12, 0x8078,"",eReadWrite_16,},
	{0x0f12, 0xf000,"",eReadWrite_16,},
	{0x0f12, 0xf920,"",eReadWrite_16,},
	{0x0f12, 0x2c10,"",eReadWrite_16,},
	{0x0f12, 0xda03,"",eReadWrite_16,},
	{0x0f12, 0x8838,"",eReadWrite_16,},
	{0x0f12, 0x4328,"",eReadWrite_16,},
	{0x0f12, 0x8038,"",eReadWrite_16,},
	{0x0f12, 0xe4ef,"",eReadWrite_16,},
	{0x0f12, 0x8878,"",eReadWrite_16,},
	{0x0f12, 0x4330,"",eReadWrite_16,},
	{0x0f12, 0x8078,"",eReadWrite_16,},
	{0x0f12, 0xe4eb,"",eReadWrite_16,},
	{0x0f12, 0x2558,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x2ab8,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x145e,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x2698,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x2bb8,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x2998,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x1100,"",eReadWrite_16,},
	{0x0f12, 0xd000,"",eReadWrite_16,},
	{0x0f12, 0x4780,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0xe200,"",eReadWrite_16,},
	{0x0f12, 0xd000,"",eReadWrite_16,},
	{0x0f12, 0x210c,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x308c,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0xb040,"",eReadWrite_16,},
	{0x0f12, 0xd000,"",eReadWrite_16,},
	{0x0f12, 0x3858,"",eReadWrite_16,},
	{0x0f12, 0x7000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x1789,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x16f1,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0xc3b1,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0xc36d,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0xf6d7,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0xb49d,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x7edf,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x448d,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xf004,"",eReadWrite_16,},
	{0x0f12, 0xe51f,"",eReadWrite_16,},
	{0x0f12, 0x29ec,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x2ef1,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0xee03,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0xa58b,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x7c49,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x7c63,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x2db7,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0xeb3d,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0xf061,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0xf0ef,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xf004,"",eReadWrite_16,},
	{0x0f12, 0xe51f,"",eReadWrite_16,},
	{0x0f12, 0x2824,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x8edd,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x8dcb,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x8e17,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x98c5,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x7c7d,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x7e31,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x7eab,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x7501,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0xf63f,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x3d0b,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x29bf,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xf004,"",eReadWrite_16,},
	{0x0f12, 0xe51f,"",eReadWrite_16,},
	{0x0f12, 0x26d8,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x306b,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x4778,"",eReadWrite_16,},
	{0x0f12, 0x46c0,"",eReadWrite_16,},
	{0x0f12, 0xc000,"",eReadWrite_16,},
	{0x0f12, 0xe59f,"",eReadWrite_16,},
	{0x0f12, 0xff1c,"",eReadWrite_16,},
	{0x0f12, 0xe12f,"",eReadWrite_16,},
	{0x0f12, 0x6099,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
*/
	
	//*************************************************************************///
// 05.  OTP setting																																//
//*************************************************************************///
{0x0028,0x7000,"",eReadWrite_16,},
{0x002A,0x0722,"",eReadWrite_16,},
{0x0F12,0x0100,"",eReadWrite_16,},//#skl_OTP_usWaitTimeThisregshouldbeinforntofD0001000//
{0x002A,0x0726,"",eReadWrite_16,},
{0x0F12,0x0000,"",eReadWrite_16,},//#skl_bUseOTPfuncOTPshadingisused,thisregshouldbe1//
{0x002A,0x08D6,"",eReadWrite_16,},
{0x0F12,0x0000,"",eReadWrite_16,},//#ash_bUseOTPDataOTPshadingisused,thisregshouldbe1//
{0x002A,0x146E,"",eReadWrite_16,},
{0x0F12,0x0001,"",eReadWrite_16,},//#awbb_otp_disableOTPAWB(0:useAWBCal.)//
{0x002A,0x08DC,"",eReadWrite_16,},
{0x0F12,0x0000,"",eReadWrite_16,},//#ash_bUseGasAlphaOTPOTPalphaisused,thisregshouldbe1//

{0x0028,0xD000,"",eReadWrite_16,},
{0x002A,0x1000,"",eReadWrite_16,},
{0x0F12,0x0001,"",eReadWrite_16,},


	
    //================================================================================== 
    // 06.Gas_Anti Shading                                                               	
    //================================================================================== 	

/*
// Refer Mon_AWB_RotGain  
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x08b4,"",eReadWrite_16,},                                                           
	{0x0f12, 0x0001,"",eReadWrite_16,}, //wbt_bUseOutdoorASH                                      
	{0x002a, 0x08bc,"",eReadWrite_16,},                                                           
	{0x0f12, 0x00c0,"",eReadWrite_16,}, //TVAR_ash_AwbAshCord_0_ 2300K                            
	{0x0f12, 0x00df,"",eReadWrite_16,}, //TVAR_ash_AwbAshCord_1_ 2750K                            
	{0x0f12, 0x0100,"",eReadWrite_16,}, //TVAR_ash_AwbAshCord_2_ 3300K                            
	{0x0f12, 0x0125,"",eReadWrite_16,}, //TVAR_ash_AwbAshCord_3_ 4150K                            
	{0x0f12, 0x015f,"",eReadWrite_16,}, //TVAR_ash_AwbAshCord_4_ 5250K                            
	{0x0f12, 0x017c,"",eReadWrite_16,}, //TVAR_ash_AwbAshCord_5_ 6400K                            
	{0x0f12, 0x0194,"",eReadWrite_16,}, //TVAR_ash_AwbAshCord_6_ 7500K                            
	{0x002a, 0x08f6,"",eReadWrite_16,},                                                           
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_0__0_ R  // 2300K                     
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_0__1_ GR                              
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_0__2_ GB                              
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_0__3_ B                               
	{0x0f12, 0x4000,"",eReadWrite_16,}, //20130513, 4000, //TVAR_ash_GASAlpha_1__0_ R  // 2750K   
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_1__1_ GR                              
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_1__2_ GB                              
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_1__3_ B                               
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_2__0_ R  // 3300K                     
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_2__1_ GR                              
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_2__2_ GB                              
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_2__3_ B                               
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_3__0_ R  // 4150K                     
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_3__1_ GR                              
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_3__2_ GB                              
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_3__3_ B                               
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_4__0_ R  // 5250K                     
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_4__1_ GR                              
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_4__2_ GB                              
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_4__3_ B                               
	{0x0f12, 0x4300,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_5__0_ R  // 6400K                     
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_5__1_ GR                              
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_5__2_ GB                              
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_5__3_ B                               
	{0x0f12, 0x4300,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_6__0_ R  // 7500K                     
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_6__1_ GR                              
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_6__2_ GB                              
	{0x0f12, 0x4000,"",eReadWrite_16,}, //TVAR_ash_GASAlpha_6__3_ B   
	//Outdoor GAS Alpha   
	{0x0f12, 0x4500,"",eReadWrite_16,},
	{0x0f12, 0x4000,"",eReadWrite_16,},
	{0x0f12, 0x4000,"",eReadWrite_16,},
	{0x0f12, 0x4000,"",eReadWrite_16,},
	
		{0x002A, 0x08F4,"",eReadWrite_16,},
		{0x0F12, 0x0001,"",eReadWrite_16,},//ash_bUseGasAlpha
	*/
	


    //==================================================================================
    // 07. Analog Setting 2
    //==================================================================================
    
        //This register is for FACTORY ONLY.
        //If you change it without prior notification
        //YOU are RESPONSIBLE for the FAILURE that will happen in the future
        //For subsampling Size
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x18bc,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0004,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x05b6,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0001,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x05ba,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0007,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x05ba,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x01f4,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x024e,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x01f4,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x05b6,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x01f4,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x05ba,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x01f4,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x024f,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0075,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x00cf,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0075,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x00d6,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0004,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x01f4,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x00f0,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x01f4,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x029e,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x05b2,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x01f8,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0228,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0208,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0238,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0218,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0238,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0001,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0009,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x00de,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x05c0,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x00df,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x00e4,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x01f8,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x01fd,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x05b6,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x05bb,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x01f8,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0077,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x007e,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x024f,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x025e,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0004,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x09d1,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0001,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x09d5,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0008,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x09d5,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02aa,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0326,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02aa,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x09d1,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02aa,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x09d5,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02aa,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0327,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0008,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0084,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0008,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x008d,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0008,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02aa,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x00aa,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02aa,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x03ad,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x09cd,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02ae,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02de,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02be,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02ee,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02ce,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02ee,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0001,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0009,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0095,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x09db,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0096,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x009b,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02ae,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02b3,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x09d1,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x09d6,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x02ae,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0009,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0010,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0327,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0336,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x002a, 0x1af8,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x5a3c,"",eReadWrite_16,}, //senHal_TuneStr_AngTuneData1_2_D000F400 register at subsampling  
	{0x002a, 0x1896,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0002,"",eReadWrite_16,}, //senHal_SamplingType 0002 03EE: PLA setting                      
	{0x0f12, 0x0000,"",eReadWrite_16,}, //senHal_SamplingMode 0 : 2 PLA / 1 : 4PLA                        
	{0x0f12, 0x0003,"",eReadWrite_16,}, //senHal_PLAOption  [0] VPLA enable  [1] HPLA enable              
	{0x002a, 0x1b00,"",eReadWrite_16,},                                                                   
	{0x0f12, 0xf428,"",eReadWrite_16,},                                                                   
	{0x0f12, 0xffff,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},                                                                   
	{0x002a, 0x189e,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0fb0,"",eReadWrite_16,}, //senHal_ExpMinPixels                                             
	{0x002a, 0x18ac,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0060,"",eReadWrite_16,},   //senHal_uAddColsBin                                            
	{0x0f12, 0x0060,"",eReadWrite_16,}, //senHal_uAddColsNoBin                                            
	{0x0f12, 0x05c0,"",eReadWrite_16,}, //senHal_uMinColsBin                                              
	{0x0f12, 0x05c0,"",eReadWrite_16,}, //senHal_uMinColsNoBin                                            
	{0x002a, 0x1aea,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x8080,"",eReadWrite_16,}, //senHal_SubF404Tune                                              
	{0x0f12, 0x0080,"",eReadWrite_16,}, //senHal_FullF404Tune                                             
	{0x002a, 0x1ae0,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,}, //senHal_bSenAAC                                                  
	{0x002a, 0x1a72,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,}, //senHal_bSRX SRX off                                             
	{0x002a, 0x18a2,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0004,"",eReadWrite_16,}, //senHal_NExpLinesCheckFine extend Forbidden area line            
	{0x002a, 0x1a6a,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x009a,"",eReadWrite_16,}, //senHal_usForbiddenRightOfs extend right Forbidden area line     
	{0x002a, 0x385e,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x024c,"",eReadWrite_16,}, //Mon_Sen_uExpPixelsOfs                                           
	{0x002a, 0x0ee6,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,}, //setot_bUseDigitalHbin                                           
	{0x002a, 0x1b2a,"",eReadWrite_16,},                                                                   
	{0x0f12, 0x0300,"",eReadWrite_16,}, //70001B2A //senHal_TuneStr2_usAngTuneGainTh                      
	{0x0f12, 0x00d6,"",eReadWrite_16,}, //70001B2C //senHal_TuneStr2_AngTuneF4CA_0_                       
	{0x0f12, 0x008d,"",eReadWrite_16,}, //70001B2E //senHal_TuneStr2_AngTuneF4CA_1_                       
	{0x0f12, 0x00cf,"",eReadWrite_16,}, //70001B30 //senHal_TuneStr2_AngTuneF4C2_0_                       
	{0x0f12, 0x0084,"",eReadWrite_16,}, //70001B32 //senHal_TuneStr2_AngTuneF4C2_1_                       
	

    //==================================================================================
    // 08.AF Setting
    //==================================================================================
    
//AF interface setting
  {0x002A, 0x01FC,"",eReadWrite_16,},
  {0x0F12, 0x0001,"",eReadWrite_16,}, //REG_TC_IPRM_LedGpio, for Flash control
//s002A1720
//s0F120100 //afd_usFlags, Low voltage AF enable
  {0x0F12, 0x0003,"",eReadWrite_16,}, //REG_TC_IPRM_CM_Init_AfModeType, VCM IIC
  {0x0F12, 0x0000,"",eReadWrite_16,}, //REG_TC_IPRM_CM_Init_PwmConfig1
  {0x002A, 0x0204,"",eReadWrite_16,},
  {0x0F12, 0x0061,"",eReadWrite_16,}, //REG_TC_IPRM_CM_Init_GpioConfig1, AF Enable GPIO 6
  {0x002A, 0x020C,"",eReadWrite_16,},
  {0x0F12, 0x2F0C,"",eReadWrite_16,}, //REG_TC_IPRM_CM_Init_Mi2cBit
  {0x0F12, 0x0190,"",eReadWrite_16,}, //REG_TC_IPRM_CM_Init_Mi2cRateKhz, IIC Speed

//AF Window Settings
  {0x002A, 0x0294,"",eReadWrite_16,},
  {0x0F12, 0x0100,"",eReadWrite_16,}, //REG_TC_AF_FstWinStartX
  {0x0F12, 0x00E3,"",eReadWrite_16,}, //REG_TC_AF_FstWinStartY
  {0x0F12, 0x0200,"",eReadWrite_16,}, //REG_TC_AF_FstWinSizeX
  {0x0F12, 0x0238,"",eReadWrite_16,}, //REG_TC_AF_FstWinSizeY
  {0x0F12, 0x018C,"",eReadWrite_16,}, //REG_TC_AF_ScndWinStartX
  {0x0F12, 0x0166,"",eReadWrite_16,}, //REG_TC_AF_ScndWinStartY
  {0x0F12, 0x00E6,"",eReadWrite_16,}, //REG_TC_AF_ScndWinSizeX
  {0x0F12, 0x0132,"",eReadWrite_16,}, //REG_TC_AF_ScndWinSizeY
  {0x0F12, 0x0001,"",eReadWrite_16,}, //REG_TC_AF_WinSizesUpdated
//2nd search setting
  {0x002A, 0x070E,"",eReadWrite_16,},
  {0x0F12, 0x00C0,"",eReadWrite_16,}, //skl_af_StatOvlpExpFactor
  {0x002A, 0x071E,"",eReadWrite_16,},
  {0x0F12, 0x0000,"",eReadWrite_16,}, //skl_af_bAfStatOff
  {0x002A, 0x163C,"",eReadWrite_16,},
  {0x0F12, 0x0000,"",eReadWrite_16,}, //af_search_usAeStable
  {0x002A, 0x1648,"",eReadWrite_16,},
  {0x0F12, 0x9002,"",eReadWrite_16,}, //af_search_usSingleAfFlags
  {0x002A, 0x1652,"",eReadWrite_16,},
  {0x0F12, 0x0002,"",eReadWrite_16,}, //af_search_usFinePeakCount
  {0x0F12, 0x0000,"",eReadWrite_16,}, //af_search_usFineMaxScale
  {0x002A, 0x15E0,"",eReadWrite_16,},
  {0x0F12, 0x0403,"",eReadWrite_16,}, //af_pos_usFineStepNumSize
  {0x002A, 0x1656,"",eReadWrite_16,},
  {0x0F12, 0x0000,"",eReadWrite_16,}, //af_search_usCapturePolicy
//Peak Threshold
  {0x002A, 0x164C,"",eReadWrite_16,},
  {0x0F12, 0x0003,"",eReadWrite_16,}, //af_search_usMinPeakSamples
  {0x002A, 0x163E,"",eReadWrite_16,},
  {0x0F12, 0x00C0,"",eReadWrite_16,}, //af_search_usPeakThr
  {0x0F12, 0x0080,"",eReadWrite_16,}, //af_search_usPeakThrLow
  {0x002A, 0x47A8,"",eReadWrite_16,},
  {0x0F12, 0x0080,"",eReadWrite_16,}, //TNP, Macro Threshold register
//Home Pos
  {0x002A, 0x15D4,"",eReadWrite_16,},
  {0x0F12, 0x0000,"",eReadWrite_16,}, //af_pos_usHomePos
  {0x0F12, 0xD000,"",eReadWrite_16,}, //af_pos_usLowConfPos
//AF statistics
  {0x002A, 0x169A,"",eReadWrite_16,},
  {0x0F12, 0xFF95,"",eReadWrite_16,}, //af_search_usConfCheckOrder_1_
  {0x002A, 0x166A,"",eReadWrite_16,},
  {0x0F12, 0x0280,"",eReadWrite_16,}, //af_search_usConfThr_4_
  {0x002A, 0x1676,"",eReadWrite_16,},
  {0x0F12, 0x03A0,"",eReadWrite_16,}, //af_search_usConfThr_10_
  {0x0F12, 0x0320,"",eReadWrite_16,}, //af_search_usConfThr_11_
  {0x002A, 0x16BC,"",eReadWrite_16,},
  {0x0F12, 0x0030,"",eReadWrite_16,}, //af_stat_usMinStatVal
  {0x002A, 0x16E0,"",eReadWrite_16,},
  {0x0F12, 0x0060,"",eReadWrite_16,}, //af_scene_usSceneLowNormBrThr
  {0x002A, 0x16D4,"",eReadWrite_16,},
  {0x0F12, 0x0010,"",eReadWrite_16,}, //af_stat_usBpfThresh
//night mode
{0x002A, 0x0638,"",eReadWrite_16,},
{0x0F12, 0x0001,"",eReadWrite_16,},
{0x0F12, 0x0000,"",eReadWrite_16,},
{0x0F12, 0x1478,"",eReadWrite_16,},
{0x0F12, 0x0000,"",eReadWrite_16,},
{0x0F12, 0x1A0A,"",eReadWrite_16,},
{0x0F12, 0x0000,"",eReadWrite_16,},
{0x0F12, 0x6810,"",eReadWrite_16,},
{0x0F12, 0x0000,"",eReadWrite_16,},
{0x0F12, 0x6810,"",eReadWrite_16,},
{0x0F12, 0x0000,"",eReadWrite_16,},
{0x0F12, 0xD020,"",eReadWrite_16,},
{0x0F12, 0x0000,"",eReadWrite_16,},
{0x0F12, 0x0428,"",eReadWrite_16,},
{0x0F12, 0x0001,"",eReadWrite_16,},
{0x0F12, 0x1A80,"",eReadWrite_16,},
{0x0F12, 0x0006,"",eReadWrite_16,},
{0x0F12, 0x1A80,"",eReadWrite_16,},
{0x0F12, 0x0006,"",eReadWrite_16,},
{0x0F12, 0x1A80,"",eReadWrite_16,},
{0x0F12, 0x0006,"",eReadWrite_16,},

//AF Lens Position Table Settings
  {0x002A, 0x15E8,"",eReadWrite_16,},
  {0x0F12, 0x0010,"",eReadWrite_16,}, //af_pos_usTableLastInd
  {0x0F12, 0x0018,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0020,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0028,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0030,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0038,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0040,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0048,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0050,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0058,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0060,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0068,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0070,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0078,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0080,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0088,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0090,"",eReadWrite_16,}, //af_pos_usTable
  {0x0F12, 0x0098,"",eReadWrite_16,}, //af_pos_usTable


//VCM AF driver with PWM/I2C
  {0x002A, 0x1722,"",eReadWrite_16,},
  {0x0F12, 0x8000,"",eReadWrite_16,}, //afd_usParam[0] I2C power down command
  {0x0F12, 0x0006,"",eReadWrite_16,}, //afd_usParam[1] Position Right Shift
  {0x0F12, 0x3FF0,"",eReadWrite_16,}, //afd_usParam[2] I2C Data Mask
  {0x0F12, 0x03E8,"",eReadWrite_16,}, //afd_usParam[3] PWM Period
  {0x0F12, 0x0000,"",eReadWrite_16,}, //afd_usParam[4] PWM Divider
  {0x0F12, 0x0020,"",eReadWrite_16,}, //afd_usParam[5] SlowMotion Delay 4. reduce lens collision noise.
  {0x0F12, 0x0010,"",eReadWrite_16,}, //afd_usParam[6] SlowMotion Threshold
  {0x0F12, 0x0008,"",eReadWrite_16,}, //afd_usParam[7] Signal Shaping
  {0x0F12, 0x0040,"",eReadWrite_16,}, //afd_usParam[8] Signal Shaping level
  {0x0F12, 0x0080,"",eReadWrite_16,}, //afd_usParam[9] Signal Shaping level
  {0x0F12, 0x00C0,"",eReadWrite_16,}, //afd_usParam[10] Signal Shaping level
  {0x0F12, 0x00E0,"",eReadWrite_16,}, //afd_usParam[11] Signal Shaping level
  {0x002A, 0x028C,"",eReadWrite_16,},
  {0x0F12, 0x0003,"",eReadWrite_16,}, //REG_TC_AF_AfCmd
             
//20131208
  {0x002A, 0x1720,"",eReadWrite_16,},
  {0x0F12, 0x0000,"",eReadWrite_16,}, 
	
	
	
	
	
	

	    //==================================================================================
    // 09.AWB-BASIC setting
    //==================================================================================

// AWB init Start point
  {0x002A, 0x145E,"",eReadWrite_16,},
  {0x0F12, 0x0580,"",eReadWrite_16,}, //awbb_GainsInit_0_
  {0x0F12, 0x0428,"",eReadWrite_16,}, //awbb_GainsInit_1_
  {0x0F12, 0x07B0,"",eReadWrite_16,}, //awbb_GainsInit_2_
// AWB Convergence Speed
  {0x0F12, 0x0008,"",eReadWrite_16,}, //awbb_WpFilterMinThr
  {0x0F12, 0x0190,"",eReadWrite_16,}, //awbb_WpFilterMaxThr
  {0x0F12, 0x00A0,"",eReadWrite_16,}, //awbb_WpFilterCoef
  {0x0F12, 0x0004,"",eReadWrite_16,}, //awbb_WpFilterSize
  {0x0F12, 0x0002,"",eReadWrite_16,}, //awbb_GridEnable
  {0x002A, 0x144E,"",eReadWrite_16,},
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_RGainOff
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_BGainOff
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GGainOff
  {0x0F12, 0x00C2,"",eReadWrite_16,}, //awbb_Alpha_Comp_Mode
  {0x0F12, 0x0002,"",eReadWrite_16,}, //awbb_Rpl_InvalidOutDoor
  {0x0F12, 0x0001,"",eReadWrite_16,}, //awbb_UseGrThrCorr
  {0x0F12, 0x0074,"",eReadWrite_16,}, //awbb_Use_Filters
  {0x0F12, 0x0001,"",eReadWrite_16,}, //awbb_CorrectMinNumPatches
// White Locus
  {0x002A, 0x11F0,"",eReadWrite_16,},
  {0x0F12, 0x012C,"",eReadWrite_16,}, //awbb_IntcR
  {0x0F12, 0x0121,"",eReadWrite_16,}, //awbb_IntcB
  {0x0F12, 0x02DF,"",eReadWrite_16,}, //awbb_GLocusR
  {0x0F12, 0x0314,"",eReadWrite_16,}, //awbb_GLocusB
  {0x002A, 0x120E,"",eReadWrite_16,},
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_MovingScale10
  {0x0F12, 0x05FD,"",eReadWrite_16,}, //awbb_GamutWidthThr1
  {0x0F12, 0x036B,"",eReadWrite_16,}, //awbb_GamutHeightThr1
  {0x0F12, 0x0020,"",eReadWrite_16,}, //awbb_GamutWidthThr2
  {0x0F12, 0x001A,"",eReadWrite_16,}, //awbb_GamutHeightThr2
  {0x002A, 0x1278,"",eReadWrite_16,},
  {0x0F12, 0xFEF7,"",eReadWrite_16,}, //awbb_SCDetectionMap_SEC_StartR_B
  {0x0F12, 0x0021,"",eReadWrite_16,}, //awbb_SCDetectionMap_SEC_StepR_B
  {0x0F12, 0x07D0,"",eReadWrite_16,}, //awbb_SCDetectionMap_SEC_SunnyNB
  {0x0F12, 0x07D0,"",eReadWrite_16,}, //awbb_SCDetectionMap_SEC_StepNB
  {0x0F12, 0x01C8,"",eReadWrite_16,}, //awbb_SCDetectionMap_SEC_LowTempR_B
  {0x0F12, 0x0096,"",eReadWrite_16,}, //awbb_SCDetectionMap_SEC_SunnyNBZone
  {0x0F12, 0x0004,"",eReadWrite_16,}, //awbb_SCDetectionMap_SEC_LowTempR_BZone
  {0x002A, 0x1224,"",eReadWrite_16,},
  {0x0F12, 0x0032,"",eReadWrite_16,}, //awbb_LowBr
  {0x0F12, 0x001E,"",eReadWrite_16,}, //awbb_LowBr_NBzone
  {0x0F12, 0x00E2,"",eReadWrite_16,}, //awbb_YThreshHigh
  {0x0F12, 0x0010,"",eReadWrite_16,}, //awbb_YThreshLow_Norm
  {0x0F12, 0x0002,"",eReadWrite_16,}, //awbb_YThreshLow_Low
  {0x002A, 0x2BA4,"",eReadWrite_16,},
  {0x0F12, 0x0004,"",eReadWrite_16,}, //Mon_AWB_ByPassMode
  {0x002A, 0x11FC,"",eReadWrite_16,},
  {0x0F12, 0x000C,"",eReadWrite_16,}, //awbb_MinNumOfFinalPatches
  {0x002A, 0x1208,"",eReadWrite_16,},
  {0x0F12, 0x0020,"",eReadWrite_16,}, //awbb_MinNumOfChromaclassifpatches
// Indoor Zone
  {0x002A, 0x101C,"",eReadWrite_16,},
  {0x0F12, 0x0360,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_0__m_left
  {0x0F12, 0x036C,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_0__m_right
  {0x0F12, 0x0320,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_1__m_left
  {0x0F12, 0x038A,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_1__m_right
  {0x0F12, 0x02E8,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_2__m_left
  {0x0F12, 0x0380,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_2__m_right
  {0x0F12, 0x02BE,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_3__m_left
  {0x0F12, 0x035A,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_3__m_right
  {0x0F12, 0x0298,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_4__m_left
  {0x0F12, 0x0334,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_4__m_right
  {0x0F12, 0x0272,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_5__m_left
  {0x0F12, 0x030E,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_5__m_right
  {0x0F12, 0x024C,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_6__m_left
  {0x0F12, 0x02EA,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_6__m_right
  {0x0F12, 0x0230,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_7__m_left
  {0x0F12, 0x02CC,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_7__m_right
  {0x0F12, 0x0214,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_8__m_left
  {0x0F12, 0x02B0,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_8__m_right
  {0x0F12, 0x01F8,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_9__m_left
  {0x0F12, 0x0294,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_9__m_right
  {0x0F12, 0x01DC,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_10__m_left
  {0x0F12, 0x0278,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_10__m_right
  {0x0F12, 0x01C0,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_11__m_left
  {0x0F12, 0x0264,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_11__m_right
  {0x0F12, 0x01AA,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_12__m_left
  {0x0F12, 0x0250,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_12__m_right
  {0x0F12, 0x0196,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_13__m_left
  {0x0F12, 0x023C,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_13__m_right
  {0x0F12, 0x0180,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_14__m_left
  {0x0F12, 0x0228,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_14__m_right
  {0x0F12, 0x016C,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_15__m_left
  {0x0F12, 0x0214,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_15__m_right
  {0x0F12, 0x0168,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_16__m_left
  {0x0F12, 0x0200,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_16__m_right
  {0x0F12, 0x0172,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_17__m_left
  {0x0F12, 0x01EC,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_17__m_right
  {0x0F12, 0x019A,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_18__m_left
  {0x0F12, 0x01D8,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_18__m_right
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_19__m_left
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_BGrid_19__m_right
  {0x0F12, 0x0005,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_GridStep
  {0x002A, 0x1070,"",eReadWrite_16,},
  {0x0F12, 0x0013,"",eReadWrite_16,}, //awbb_IndoorGrZones_ZInfo_m_GridSz
  {0x002A, 0x1074,"",eReadWrite_16,},
  {0x0F12, 0x00EC,"",eReadWrite_16,}, //awbb_IndoorGrZones_m_Boffs
// Outdoor Zone
  {0x002A, 0x1078,"",eReadWrite_16,},
  {0x0F12, 0x0232,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_0__m_left
  {0x0F12, 0x025A,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_0__m_right
  {0x0F12, 0x021E,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_1__m_left
  {0x0F12, 0x0274,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_1__m_right
  {0x0F12, 0x020E,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_2__m_left
  {0x0F12, 0x028E,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_2__m_right
  {0x0F12, 0x0200,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_3__m_left
  {0x0F12, 0x0290,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_3__m_right
  {0x0F12, 0x01F4,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_4__m_left
  {0x0F12, 0x0286,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_4__m_right
  {0x0F12, 0x01E8,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_5__m_left
  {0x0F12, 0x027E,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_5__m_right
  {0x0F12, 0x01DE,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_6__m_left
  {0x0F12, 0x0274,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_6__m_right
  {0x0F12, 0x01D2,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_7__m_left
  {0x0F12, 0x0268,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_7__m_right
  {0x0F12, 0x01D0,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_8__m_left
  {0x0F12, 0x025E,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_8__m_right
  {0x0F12, 0x01D6,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_9__m_left
  {0x0F12, 0x0252,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_9__m_right
  {0x0F12, 0x01E2,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_10__m_left
  {0x0F12, 0x0248,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_10__m_right
  {0x0F12, 0x01F4,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_11__m_left
  {0x0F12, 0x021A,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_BGrid_11__m_right
  {0x0F12, 0x0004,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_GridStep
  {0x002A, 0x10AC,"",eReadWrite_16,},
  {0x0F12, 0x000C,"",eReadWrite_16,}, //awbb_OutdoorGrZones_ZInfo_m_GridSz
  {0x002A, 0x10B0,"",eReadWrite_16,},
  {0x0F12, 0x01DA,"",eReadWrite_16,}, //awbb_OutdoorGrZones_m_Boffs
// Low Brightness Zone
  {0x002A, 0x10B4,"",eReadWrite_16,},
  {0x0F12, 0x0348,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_0__m_left
  {0x0F12, 0x03B6,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_0__m_right
  {0x0F12, 0x02B8,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_1__m_left
  {0x0F12, 0x03B6,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_1__m_right
  {0x0F12, 0x0258,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_2__m_left
  {0x0F12, 0x038E,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_2__m_right
  {0x0F12, 0x0212,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_3__m_left
  {0x0F12, 0x0348,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_3__m_right
  {0x0F12, 0x01CC,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_4__m_left
  {0x0F12, 0x030C,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_4__m_right
  {0x0F12, 0x01A2,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_5__m_left
  {0x0F12, 0x02D2,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_5__m_right
  {0x0F12, 0x0170,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_6__m_left
  {0x0F12, 0x02A6,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_6__m_right
  {0x0F12, 0x014C,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_7__m_left
  {0x0F12, 0x0280,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_7__m_right
  {0x0F12, 0x0128,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_8__m_left
  {0x0F12, 0x025C,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_8__m_right
  {0x0F12, 0x0146,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_9__m_left
  {0x0F12, 0x0236,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_9__m_right
  {0x0F12, 0x0164,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_10__m_left
  {0x0F12, 0x0212,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_10__m_right
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_11__m_left
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_BGrid_11__m_right
  {0x0F12, 0x0006,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_GridStep
  {0x002A, 0x10E8,"",eReadWrite_16,},
  {0x0F12, 0x000B,"",eReadWrite_16,}, //awbb_LowBrGrZones_ZInfo_m_GridSz
  {0x002A, 0x10EC,"",eReadWrite_16,},
  {0x0F12, 0x00D2,"",eReadWrite_16,}, //awbb_LowBrGrZones_m_Boffs

// Low Temp. Zone
  {0x002A, 0x10F0,"",eReadWrite_16,},
  {0x0F12, 0x039A,"",eReadWrite_16,},
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_CrclLowT_R_c
  {0x0F12, 0x00FE,"",eReadWrite_16,},
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_CrclLowT_B_c
  {0x0F12, 0x2284,"",eReadWrite_16,},
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_CrclLowT_Rad_c

//AWB - GridCorrection
  {0x002A, 0x1434,"",eReadWrite_16,},
  {0x0F12, 0x02C1,"",eReadWrite_16,}, //awbb_GridConst_1_0_
  {0x0F12, 0x033A,"",eReadWrite_16,}, //awbb_GridConst_1_1_
  {0x0F12, 0x038A,"",eReadWrite_16,}, //awbb_GridConst_1_2_
  {0x0F12, 0x101A,"",eReadWrite_16,}, //awbb_GridConst_2_0_
  {0x0F12, 0x1075,"",eReadWrite_16,}, //awbb_GridConst_2_1_
  {0x0F12, 0x113D,"",eReadWrite_16,}, //awbb_GridConst_2_2_
  {0x0F12, 0x113F,"",eReadWrite_16,}, //awbb_GridConst_2_3_
  {0x0F12, 0x11AF,"",eReadWrite_16,}, //awbb_GridConst_2_4_
  {0x0F12, 0x11F0,"",eReadWrite_16,}, //awbb_GridConst_2_5_
  {0x0F12, 0x00B2,"",eReadWrite_16,}, //awbb_GridCoeff_R_1
  {0x0F12, 0x00B8,"",eReadWrite_16,}, //awbb_GridCoeff_B_1
  {0x0F12, 0x00CA,"",eReadWrite_16,}, //awbb_GridCoeff_R_2
  {0x0F12, 0x009D,"",eReadWrite_16,}, //awbb_GridCoeff_B_2

// Indoor Grid Offset
  {0x002A, 0x13A4,"",eReadWrite_16,},
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_0__0_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_0__1_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_0__2_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_0__3_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_0__4_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_0__5_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_1__0_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_1__1_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_1__2_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_1__3_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_1__4_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_1__5_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_2__0_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_2__1_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_2__2_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_2__3_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_2__4_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_2__5_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_0__0_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_0__1_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_0__2_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_0__3_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_0__4_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_0__5_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_1__0_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_1__1_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_1__2_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_1__3_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_1__4_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_1__5_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_2__0_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_2__1_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_2__2_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_2__3_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_2__4_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_2__5_

// Outdoor Grid Offset
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_0__0_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_0__1_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_0__2_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_0__3_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_0__4_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_0__5_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_1__0_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_1__1_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_1__2_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_1__3_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_1__4_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_1__5_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_2__0_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_2__1_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_2__2_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_2__3_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_2__4_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_R_Out_2__5_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_0__0_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_0__1_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_0__2_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_0__3_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_0__4_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_0__5_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_1__0_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_1__1_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_1__2_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_1__3_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_1__4_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_1__5_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_2__0_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_2__1_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_2__2_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_2__3_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_2__4_
  {0x0F12, 0x0000,"",eReadWrite_16,}, //awbb_GridCorr_B_Out_2__5_

	
	
    //==================================================================================
    // 10.Clock Setting
    //==================================================================================
    //For MCLK=24MHz, PCLK=5DC0
	{0x002a, 0x01f8,"",eReadWrite_16,},      //System Setting  
	{0x0f12, 0x5dc0,"",eReadWrite_16,},
	{0x002a, 0x0212,"",eReadWrite_16,},//
	{0x0f12, 0x0002,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},//0x0216  PVI
	{0x002a, 0x021a,"",eReadWrite_16,},
	{0x0f12, 0x34bc,"",eReadWrite_16,},
	{0x0f12, 0x4f1a,"",eReadWrite_16,},
	{0x0f12, 0x4f1a,"",eReadWrite_16,},
	{0x0f12, 0x4f1a,"",eReadWrite_16,},
	{0x0f12, 0x4f1a,"",eReadWrite_16,},
	{0x0f12, 0x4f1a,"",eReadWrite_16,},
	{0x002a, 0x0f30,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x0f2a,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},//AFC_Default60Hz 0001:60Hz 0000h:50Hz
	{0x002a, 0x04e6,"",eReadWrite_16,},//REG_TC_DBG_AutoAlgEnBits
	{0x0f12, 0x077f,"",eReadWrite_16,},//0x075f



	
	{0x002a, 0x04d6,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x1484,"",eReadWrite_16,},
	{0x0f12, 0x0038,"",eReadWrite_16,},//TVAR_ae_BrAve
 //ae_StatMode bit[3] BLC has to be bypassed to prevent AE weight change especially backlight scene
   {0x002A, 0x148A,"",eReadWrite_16,},
   {0x0F12, 0x000F,"",eReadWrite_16,},   //ae_StatMode
   {0x002A, 0x058C,"",eReadWrite_16,},
   {0x0F12, 0x3520,"",eReadWrite_16,},
   {0x0F12, 0x0000,"",eReadWrite_16,},   //lt_uMaxExp1
   {0x0F12, 0xD4C0,"",eReadWrite_16,},
   {0x0F12, 0x0001,"",eReadWrite_16,},   //lt_uMaxExp2
   {0x0F12, 0x3520,"",eReadWrite_16,},
   {0x0F12, 0x0000,"",eReadWrite_16,},   //lt_uCapMaxExp1
   {0x0F12, 0xD4C0,"",eReadWrite_16,},
   {0x0F12, 0x0001,"",eReadWrite_16,},   //lt_uCapMaxExp2
   {0x002A, 0x059C,"",eReadWrite_16,},
   {0x0F12, 0x0470,"",eReadWrite_16,},   //lt_uMaxAnGain1
   {0x0F12, 0x0C00,"",eReadWrite_16,},   //lt_uMaxAnGain2
   {0x0F12, 0x0100,"",eReadWrite_16,},   //lt_uMaxDigGain
   {0x0F12, 0x1000,"",eReadWrite_16,},   //lt_uMaxTotGain
   {0x002A, 0x0544,"",eReadWrite_16,},
   {0x0F12, 0x0111,"",eReadWrite_16,},   //lt_uLimitHigh
   {0x0F12, 0x00EF,"",eReadWrite_16,},   //lt_uLimitLow
   {0x002A, 0x0608,"",eReadWrite_16,},
   {0x0F12, 0x0001,"",eReadWrite_16,},   //lt_ExpGain_uSubsamplingmode
   {0x0F12, 0x0001,"",eReadWrite_16,},   //lt_ExpGain_uNonSubsampling
   {0x0F12, 0x0800,"",eReadWrite_16,},   //lt_ExpGain_ExpCurveGainMaxStr
   {0x0F12, 0x0100,"",eReadWrite_16,},   //0100   //lt_ExpGain_ExpCurveGainMaxStr_0__uMaxDigGain
   {0x0F12, 0x0001,"",eReadWrite_16,},   //0001
   {0x0F12, 0x0000,"",eReadWrite_16,},   //0000   //lt_ExpGain_ExpCurveGainMaxStr_0__ulExpIn_0_
   {0x0F12, 0x0A3C,"",eReadWrite_16,},   //0A3C
   {0x0F12, 0x0000,"",eReadWrite_16,},   //0000
   {0x0F12, 0x0D05,"",eReadWrite_16,},   //0D05
   {0x0F12, 0x0000,"",eReadWrite_16,},   //0000
   {0x0F12, 0x4008,"",eReadWrite_16,},   //4008
   {0x0F12, 0x0000,"",eReadWrite_16,},   //0000
   {0x0F12, 0x7000,"",eReadWrite_16,},   //7400  //?? //700Lux
   {0x0F12, 0x0000,"",eReadWrite_16,},   //0000
   {0x0F12, 0x9C00,"",eReadWrite_16,},   //C000  //?? //9C00->9F->A5 //400Lux
   {0x0F12, 0x0000,"",eReadWrite_16,},   //0000
   {0x0F12, 0xAD00,"",eReadWrite_16,},   //AD00
   {0x0F12, 0x0001,"",eReadWrite_16,},   //0001
   {0x0F12, 0xF1D4,"",eReadWrite_16,},   //F1D4
   {0x0F12, 0x0002,"",eReadWrite_16,},   //0002
   {0x0F12, 0xDC00,"",eReadWrite_16,},   //DC00
   {0x0F12, 0x0005,"",eReadWrite_16,},   //0005
   {0x0F12, 0xDC00,"",eReadWrite_16,},   //DC00
   {0x0F12, 0x0005,"",eReadWrite_16,},   //0005         //
   {0x002A, 0x0638,"",eReadWrite_16,},   //0638
   {0x0F12, 0x0001,"",eReadWrite_16,},   //0001
   {0x0F12, 0x0000,"",eReadWrite_16,},   //0000   //lt_ExpGain_ExpCurveGainMaxStr_0__ulExpOut_0_
   {0x0F12, 0x0A3C,"",eReadWrite_16,},   //0A3C
   {0x0F12, 0x0000,"",eReadWrite_16,},   //0000
   {0x0F12, 0x0D05,"",eReadWrite_16,},   //0D05
   {0x0F12, 0x0000,"",eReadWrite_16,},   //0000
   {0x0F12, 0x3408,"",eReadWrite_16,},   //3408
   {0x0F12, 0x0000,"",eReadWrite_16,},   //0000
   {0x0F12, 0x3408,"",eReadWrite_16,},   //3408
   {0x0F12, 0x0000,"",eReadWrite_16,},   //0000
   {0x0F12, 0x6810,"",eReadWrite_16,},   //6810
   {0x0F12, 0x0000,"",eReadWrite_16,},   //0000
   {0x0F12, 0x8214,"",eReadWrite_16,},   //8214
   {0x0F12, 0x0000,"",eReadWrite_16,},   //0000
   {0x0F12, 0xC350,"",eReadWrite_16,},   //C350
   {0x0F12, 0x0000,"",eReadWrite_16,},   //0000
   {0x0F12, 0xD4C0,"",eReadWrite_16,},   //C350
   {0x0F12, 0x0001,"",eReadWrite_16,},   //0000
   {0x0F12, 0xD4C0,"",eReadWrite_16,},   //C350
   {0x0F12, 0x0001,"",eReadWrite_16,},   //0000
   {0x002A, 0x0660,"",eReadWrite_16,},
   {0x0F12, 0x0650,"",eReadWrite_16,},   //lt_ExpGain_ExpCurveGainMaxStr_1_
   {0x0F12, 0x0100,"",eReadWrite_16,},   //lt_ExpGain_ExpCurveGainMaxStr_1__uMaxDigGain
   {0x002A, 0x06B8,"",eReadWrite_16,},
   {0x0F12, 0x452C,"",eReadWrite_16,},
   {0x0F12, 0x000A,"",eReadWrite_16,},   //0005   //lt_uMaxLei
   {0x002A, 0x05D0,"",eReadWrite_16,},
   {0x0F12, 0x0000,"",eReadWrite_16,},   //lt_mbr_Peak_behind 
	
	


	
    //================================================================================== 
    // 13.AE Weight (Normal)                                                             
    //================================================================================== 
	{0x002a, 0x1492,"",eReadWrite_16,},
	{0x0f12, 0x0100,"",eReadWrite_16,},
	{0x0f12, 0x0101,"",eReadWrite_16,},
	{0x0f12, 0x0101,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0101,"",eReadWrite_16,},
	{0x0f12, 0x0201,"",eReadWrite_16,},
	{0x0f12, 0x0102,"",eReadWrite_16,},
	{0x0f12, 0x0101,"",eReadWrite_16,},
	{0x0f12, 0x0101,"",eReadWrite_16,},
	{0x0f12, 0x0202,"",eReadWrite_16,},
	{0x0f12, 0x0202,"",eReadWrite_16,},
	{0x0f12, 0x0101,"",eReadWrite_16,},
	{0x0f12, 0x0201,"",eReadWrite_16,},
	{0x0f12, 0x0302,"",eReadWrite_16,},
	{0x0f12, 0x0203,"",eReadWrite_16,},
	{0x0f12, 0x0102,"",eReadWrite_16,},
	{0x0f12, 0x0201,"",eReadWrite_16,},
	{0x0f12, 0x0302,"",eReadWrite_16,},
	{0x0f12, 0x0203,"",eReadWrite_16,},
	{0x0f12, 0x0102,"",eReadWrite_16,},
	{0x0f12, 0x0201,"",eReadWrite_16,},
	{0x0f12, 0x0202,"",eReadWrite_16,},
	{0x0f12, 0x0202,"",eReadWrite_16,},
	{0x0f12, 0x0102,"",eReadWrite_16,},
	{0x0f12, 0x0101,"",eReadWrite_16,},
	{0x0f12, 0x0202,"",eReadWrite_16,},
	{0x0f12, 0x0202,"",eReadWrite_16,},
	{0x0f12, 0x0101,"",eReadWrite_16,},
	{0x0f12, 0x0101,"",eReadWrite_16,},
	{0x0f12, 0x0101,"",eReadWrite_16,},
	{0x0f12, 0x0101,"",eReadWrite_16,},
	{0x0f12, 0x0101,"",eReadWrite_16,},
	{0x002a, 0x0484,"",eReadWrite_16,},
	{0x0f12, 0x0002,"",eReadWrite_16,},
	{0x002a, 0x183a,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x17f6,"",eReadWrite_16,},
	{0x0f12, 0x023c,"",eReadWrite_16,},
	{0x0f12, 0x0248,"",eReadWrite_16,},
	{0x002a, 0x1840,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0100,"",eReadWrite_16,},
	{0x0f12, 0x0120,"",eReadWrite_16,},
	{0x0f12, 0x0180,"",eReadWrite_16,},
	{0x0f12, 0x0200,"",eReadWrite_16,},
	{0x0f12, 0x0400,"",eReadWrite_16,},
	{0x0f12, 0x0800,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x1000,"",eReadWrite_16,},
	{0x0f12, 0x0100,"",eReadWrite_16,},
	{0x0f12, 0x00a0,"",eReadWrite_16,},
	{0x0f12, 0x0090,"",eReadWrite_16,},
	{0x0f12, 0x0080,"",eReadWrite_16,},
	{0x0f12, 0x0070,"",eReadWrite_16,},
	{0x0f12, 0x0045,"",eReadWrite_16,},
	{0x0f12, 0x0030,"",eReadWrite_16,},
	{0x0f12, 0x0010,"",eReadWrite_16,},
	{0x002a, 0x1884,"",eReadWrite_16,},
	{0x0f12, 0x0100,"",eReadWrite_16,},
	{0x0f12, 0x0100,"",eReadWrite_16,},
	{0x0f12, 0x0100,"",eReadWrite_16,},
	{0x0f12, 0x0100,"",eReadWrite_16,},
	{0x0f12, 0x0100,"",eReadWrite_16,},
	{0x0f12, 0x0100,"",eReadWrite_16,},
	{0x0f12, 0x0100,"",eReadWrite_16,},
	{0x0f12, 0x0100,"",eReadWrite_16,},
	{0x002a, 0x1826,"",eReadWrite_16,},
	{0x0f12, 0x0100,"",eReadWrite_16,},
	{0x0f12, 0x00c0,"",eReadWrite_16,},
	{0x0f12, 0x0080,"",eReadWrite_16,},
	{0x0f12, 0x000a,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0030,"",eReadWrite_16,},
	{0x0f12, 0x0040,"",eReadWrite_16,},
	{0x0f12, 0x0048,"",eReadWrite_16,},
	{0x0f12, 0x0050,"",eReadWrite_16,},
	{0x0f12, 0x0060,"",eReadWrite_16,},
	{0x002a, 0x4784,"",eReadWrite_16,},
	{0x0f12, 0x00a0,"",eReadWrite_16,},
	{0x0f12, 0x00c0,"",eReadWrite_16,},
	{0x0f12, 0x00d0,"",eReadWrite_16,},
	{0x0f12, 0x0100,"",eReadWrite_16,},
	{0x0f12, 0x0200,"",eReadWrite_16,},
	{0x0f12, 0x0300,"",eReadWrite_16,},
	{0x0f12, 0x0088,"",eReadWrite_16,},
	{0x0f12, 0x00b0,"",eReadWrite_16,},
	{0x0f12, 0x00c0,"",eReadWrite_16,},
	{0x0f12, 0x0100,"",eReadWrite_16,},
	{0x0f12, 0x0200,"",eReadWrite_16,},
	{0x0f12, 0x0300,"",eReadWrite_16,},
	{0x002a, 0x479c,"",eReadWrite_16,},
	{0x0f12, 0x0120,"",eReadWrite_16,},
	{0x0f12, 0x0150,"",eReadWrite_16,},
	{0x0f12, 0x0200,"",eReadWrite_16,},
	{0x0f12, 0x003c,"",eReadWrite_16,},
	{0x0f12, 0x003b,"",eReadWrite_16,},
	{0x0f12, 0x0026,"",eReadWrite_16,},
	

	

    //==================================================================================
    // 15.CCM Setting
    //==================================================================================
 {0x002A, 0x08A6,"",eReadWrite_16,},
 {0x0F12, 0x00C0,"",eReadWrite_16,}, //SARR_AwbCcmCord[0]
 {0x0F12, 0x0100,"",eReadWrite_16,}, //SARR_AwbCcmCord[1]
 {0x0F12, 0x0125,"",eReadWrite_16,}, //SARR_AwbCcmCord[2]
 {0x0F12, 0x015F,"",eReadWrite_16,}, //SARR_AwbCcmCord[3]
 {0x0F12, 0x017C,"",eReadWrite_16,}, //SARR_AwbCcmCord[4]
 {0x0F12, 0x0194,"",eReadWrite_16,}, //SARR_AwbCcmCord[5]
 {0x002A, 0x0898,"",eReadWrite_16,},
 {0x0F12, 0x4800,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms
 {0x0F12, 0x7000,"",eReadWrite_16,},
 {0x002A, 0x08A0,"",eReadWrite_16,},
 {0x0F12, 0x48D8,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm
 {0x0F12, 0x7000,"",eReadWrite_16,},
//Horizon
 {0x002A, 0x4800,"",eReadWrite_16,},
 {0x0F12, 0x0208,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[0]
 {0x0F12, 0xFFB5,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[1]
 {0x0F12, 0xFFE8,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[2]
 {0x0F12, 0xFF20,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[3]
 {0x0F12, 0x01BF,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[4]
 {0x0F12, 0xFF53,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[5]
 {0x0F12, 0x0022,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[6]
 {0x0F12, 0xFFEA,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[7]
 {0x0F12, 0x01C2,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[8]
 {0x0F12, 0x00C6,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[9]
 {0x0F12, 0x0095,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[10]
 {0x0F12, 0xFEFD,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[11]
 {0x0F12, 0x0206,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[12]
 {0x0F12, 0xFF7F,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[13]
 {0x0F12, 0x0191,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[14]
 {0x0F12, 0xFF06,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[15]
 {0x0F12, 0x01BA,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[16]
 {0x0F12, 0x0108,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[17]

// INCA A
 {0x0F12, 0x0208,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[18]
 {0x0F12, 0xFFB5,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[19]
 {0x0F12, 0xFFE8,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[20]
 {0x0F12, 0xFF20,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[21]
 {0x0F12, 0x01BF,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[22]
 {0x0F12, 0xFF53,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[23]
 {0x0F12, 0x0022,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[24]
 {0x0F12, 0xFFEA,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[25]
 {0x0F12, 0x01C2,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[26]
 {0x0F12, 0x00C6,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[27]
 {0x0F12, 0x0095,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[28]
 {0x0F12, 0xFEFD,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[29]
 {0x0F12, 0x0206,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[30]
 {0x0F12, 0xFF7F,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[31]
 {0x0F12, 0x0191,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[32]
 {0x0F12, 0xFF06,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[33]
 {0x0F12, 0x01BA,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[34]
 {0x0F12, 0x0108,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[35]
//Warm White
 {0x0F12, 0x0208,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[36]
 {0x0F12, 0xFFB5,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[37]
 {0x0F12, 0xFFE8,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[38]
 {0x0F12, 0xFF20,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[39]
 {0x0F12, 0x01BF,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[40]
 {0x0F12, 0xFF53,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[41]
 {0x0F12, 0x0022,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[42]
 {0x0F12, 0xFFEA,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[43]
 {0x0F12, 0x01C2,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[44]
 {0x0F12, 0x00C6,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[45]
 {0x0F12, 0x0095,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[46]
 {0x0F12, 0xFEFD,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[47]
 {0x0F12, 0x0206,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[48]
 {0x0F12, 0xFF7F,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[49]
 {0x0F12, 0x0191,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[50]
 {0x0F12, 0xFF06,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[51]
 {0x0F12, 0x01BA,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[52]
 {0x0F12, 0x0108,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[53]
//Cool White
 {0x0F12, 0x0204,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[54]
 {0x0F12, 0xFFB2,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[55]
 {0x0F12, 0xFFF5,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[56]
 {0x0F12, 0xFEF1,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[57]
 {0x0F12, 0x014E,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[58]
 {0x0F12, 0xFF18,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[59]
 {0x0F12, 0xFFE6,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[60]
 {0x0F12, 0xFFDD,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[61]
 {0x0F12, 0x01B2,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[62]
 {0x0F12, 0x00F2,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[63]
 {0x0F12, 0x00CA,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[64]
 {0x0F12, 0xFF48,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[65]
 {0x0F12, 0x0151,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[66]
 {0x0F12, 0xFF50,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[67]
 {0x0F12, 0x0147,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[68]
 {0x0F12, 0xFF75,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[69]
 {0x0F12, 0x0187,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[70]
 {0x0F12, 0x01BF,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[71]
//D50
 {0x0F12, 0x0204,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[72]
 {0x0F12, 0xFFB2,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[73]
 {0x0F12, 0xFFF5,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[74]
 {0x0F12, 0xFEF1,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[75]
 {0x0F12, 0x014E,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[76]
 {0x0F12, 0xFF18,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[77]
 {0x0F12, 0xFFE6,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[78]
 {0x0F12, 0xFFDD,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[79]
 {0x0F12, 0x01B2,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[80]
 {0x0F12, 0x00F2,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[81]
 {0x0F12, 0x00CA,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[82]
 {0x0F12, 0xFF48,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[83]
 {0x0F12, 0x0151,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[84]
 {0x0F12, 0xFF50,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[85]
 {0x0F12, 0x0147,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[86]
 {0x0F12, 0xFF75,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[87]
 {0x0F12, 0x0187,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[88]
 {0x0F12, 0x01BF,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[89]
//D65
 {0x0F12, 0x0204,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[90]
 {0x0F12, 0xFFB2,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[91]
 {0x0F12, 0xFFF5,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[92]
 {0x0F12, 0xFEF1,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[93]
 {0x0F12, 0x014E,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[94]
 {0x0F12, 0xFF18,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[95]
 {0x0F12, 0xFFE6,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[96]
 {0x0F12, 0xFFDD,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[97]
 {0x0F12, 0x01B2,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[98]
 {0x0F12, 0x00F2,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[99]
 {0x0F12, 0x00CA,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[100]
 {0x0F12, 0xFF48,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[101]
 {0x0F12, 0x0151,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[102]
 {0x0F12, 0xFF50,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[103]
 {0x0F12, 0x0147,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[104]
 {0x0F12, 0xFF75,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[105]
 {0x0F12, 0x0187,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[106]
 {0x0F12, 0x01BF,"",eReadWrite_16,}, //TVAR_wbt_pBaseCcms[107]
//Outdoor
 {0x0F12, 0x01E5,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[0]
 {0x0F12, 0xFFA4,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[1]
 {0x0F12, 0xFFDC,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[2]
 {0x0F12, 0xFE90,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[3]
 {0x0F12, 0x013F,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[4]
 {0x0F12, 0xFF1B,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[5]
 {0x0F12, 0xFFD2,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[6]
 {0x0F12, 0xFFDF,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[7]
 {0x0F12, 0x0236,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[8]
 {0x0F12, 0x00EC,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[9]
 {0x0F12, 0x00F8,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[10]
 {0x0F12, 0xFF34,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[11]
 {0x0F12, 0x01CE,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[12]
 {0x0F12, 0xFF83,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[13]
 {0x0F12, 0x0195,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[14]
 {0x0F12, 0xFEF3,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[15]
 {0x0F12, 0x0126,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[16]
 {0x0F12, 0x0162,"",eReadWrite_16,}, //TVAR_wbt_pOutdoorCcm[17]


	{0x002a, 0x48d8,"",eReadWrite_16,},
	{0x0f12, 0x01dc,"",eReadWrite_16,},
	{0x0f12, 0xff91,"",eReadWrite_16,},
	{0x0f12, 0xffe9,"",eReadWrite_16,},
	{0x0f12, 0xfeb5,"",eReadWrite_16,},
	{0x0f12, 0x013a,"",eReadWrite_16,},
	{0x0f12, 0xfeff,"",eReadWrite_16,},
	{0x0f12, 0xffb7,"",eReadWrite_16,},
	{0x0f12, 0xfff5,"",eReadWrite_16,},
	{0x0f12, 0x0237,"",eReadWrite_16,},
	{0x0f12, 0x00d5,"",eReadWrite_16,},
	{0x0f12, 0x0101,"",eReadWrite_16,},
	{0x0f12, 0xff39,"",eReadWrite_16,},
	{0x0f12, 0x01ce,"",eReadWrite_16,},
	{0x0f12, 0xff83,"",eReadWrite_16,},
	{0x0f12, 0x0195,"",eReadWrite_16,},
	{0x0f12, 0xfef3,"",eReadWrite_16,},
	{0x0f12, 0x014f,"",eReadWrite_16,},
	{0x0f12, 0x0137,"",eReadWrite_16,},
	//16.gamma
			{0x002A, 0x0734,"",eReadWrite_16,},
		{0x0F12, 0x0000,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][0]
		{0x0F12, 0x0004,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][1]
		{0x0F12, 0x000B,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][2]
		{0x0F12, 0x001B,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][3]
		{0x0F12, 0x0046,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][4]
		{0x0F12, 0x00AE,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][5]
		{0x0F12, 0x011E,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][6]
		{0x0F12, 0x0154,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][7]
		{0x0F12, 0x0184,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][8]
		{0x0F12, 0x01C6,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][9]
		{0x0F12, 0x01F8,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][10]
		{0x0F12, 0x0222,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][11]
		{0x0F12, 0x0247,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][12]
		{0x0F12, 0x0282,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][13]
		{0x0F12, 0x02B5,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][14]
		{0x0F12, 0x030F,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][15]
		{0x0F12, 0x035F,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][16]
		{0x0F12, 0x03A2,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][17]
		{0x0F12, 0x03D8,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][18]
		{0x0F12, 0x03FF,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][19]
		{0x0F12, 0x0000,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][0]
		{0x0F12, 0x0004,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][1]
		{0x0F12, 0x000B,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][2]
		{0x0F12, 0x001B,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][3]
		{0x0F12, 0x0046,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][4]
		{0x0F12, 0x00AE,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][5]
		{0x0F12, 0x011E,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][6]
		{0x0F12, 0x0154,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][7]
		{0x0F12, 0x0184,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][8]
		{0x0F12, 0x01C6,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][9]
		{0x0F12, 0x01F8,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][10]
		{0x0F12, 0x0222,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][11]
		{0x0F12, 0x0247,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][12]
		{0x0F12, 0x0282,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][13]
		{0x0F12, 0x02B5,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][14]
		{0x0F12, 0x030F,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][15]
		{0x0F12, 0x035F,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][16]
		{0x0F12, 0x03A2,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][17]
		{0x0F12, 0x03D8,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][18]
		{0x0F12, 0x03FF,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][19]
		{0x0F12, 0x0000,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][0]
		{0x0F12, 0x0004,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][1]
		{0x0F12, 0x000B,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][2]
		{0x0F12, 0x001B,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][3]
		{0x0F12, 0x0046,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][4]
		{0x0F12, 0x00AE,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][5]
		{0x0F12, 0x011E,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][6]
		{0x0F12, 0x0154,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][7]
		{0x0F12, 0x0184,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][8]
		{0x0F12, 0x01C6,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][9]
		{0x0F12, 0x01F8,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][10]
		{0x0F12, 0x0222,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][11]
		{0x0F12, 0x0247,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][12]
		{0x0F12, 0x0282,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][13]
		{0x0F12, 0x02B5,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][14]
		{0x0F12, 0x030F,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][15]
		{0x0F12, 0x035F,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][16]
		{0x0F12, 0x03A2,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][17]
		{0x0F12, 0x03D8,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][18]
		{0x0F12, 0x03FF,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][19]
	{0x0f12, 0x0000,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][0]                
	{0x0f12, 0x000b,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][1]                
	{0x0f12, 0x0019,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][2]                
	{0x0f12, 0x0036,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][3]                
	{0x0f12, 0x006f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][4]                
	{0x0f12, 0x00d8,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][5]                
	{0x0f12, 0x0135,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][6]                
	{0x0f12, 0x015f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][7]                
	{0x0f12, 0x0185,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][8]                
	{0x0f12, 0x01c1,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][9]                
	{0x0f12, 0x01f3,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][10]               
	{0x0f12, 0x0220,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][11]               
	{0x0f12, 0x024a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][12]               
	{0x0f12, 0x0291,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][13]               
	{0x0f12, 0x02d0,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][14]               
	{0x0f12, 0x032a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][15]               
	{0x0f12, 0x036a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][16]               
	{0x0f12, 0x039f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][17]               
	{0x0f12, 0x03cc,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][18]               
	{0x0f12, 0x03f9,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][19]               
	{0x0f12, 0x0000,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][0]                
	{0x0f12, 0x000b,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][1]                
	{0x0f12, 0x0019,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][2]                
	{0x0f12, 0x0036,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][3]                
	{0x0f12, 0x006f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][4]                
	{0x0f12, 0x00d8,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][5]                
	{0x0f12, 0x0135,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][6]                
	{0x0f12, 0x015f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][7]                
	{0x0f12, 0x0185,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][8]                
	{0x0f12, 0x01c1,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][9]                
	{0x0f12, 0x01f3,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][10]               
	{0x0f12, 0x0220,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][11]               
	{0x0f12, 0x024a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][12]               
	{0x0f12, 0x0291,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][13]               
	{0x0f12, 0x02d0,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][14]               
	{0x0f12, 0x032a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][15]               
	{0x0f12, 0x036a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][16]               
	{0x0f12, 0x039f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][17]               
	{0x0f12, 0x03cc,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][18]               
	{0x0f12, 0x03f9,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][19]               
	{0x0f12, 0x0000,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][0]                
	{0x0f12, 0x000b,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][1]                
	{0x0f12, 0x0019,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][2]                
	{0x0f12, 0x0036,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][3]                
	{0x0f12, 0x006f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][4]                
	{0x0f12, 0x00d8,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][5]                
	{0x0f12, 0x0135,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][6]                
	{0x0f12, 0x015f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][7]                
	{0x0f12, 0x0185,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][8]                
	{0x0f12, 0x01c1,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][9]                
	{0x0f12, 0x01f3,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][10]               
	{0x0f12, 0x0220,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][11]               
	{0x0f12, 0x024a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][12]               
	{0x0f12, 0x0291,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][13]               
	{0x0f12, 0x02d0,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][14]               
	{0x0f12, 0x032a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][15]               
	{0x0f12, 0x036a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][16]               
	{0x0f12, 0x039f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][17]               
	{0x0f12, 0x03cc,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][18]               
	{0x0f12, 0x03f9,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][19] 


/*
		{0x0F12, 0x0000,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][0]
		{0x0F12, 0x0004,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][1]
		{0x0F12, 0x000B,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][2]
		{0x0F12, 0x001B,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][3]
		{0x0F12, 0x0046,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][4]
		{0x0F12, 0x00AE,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][5]
		{0x0F12, 0x011E,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][6]
		{0x0F12, 0x0154,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][7]
		{0x0F12, 0x0184,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][8]
		{0x0F12, 0x01C6,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][9]
		{0x0F12, 0x01F8,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][10]
		{0x0F12, 0x0222,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][11]
		{0x0F12, 0x0247,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][12]
		{0x0F12, 0x0282,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][13]
		{0x0F12, 0x02B5,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][14]
		{0x0F12, 0x030F,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][15]
		{0x0F12, 0x035F,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][16]
		{0x0F12, 0x03A2,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][17]
		{0x0F12, 0x03D8,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][18]
		{0x0F12, 0x03FF,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][19]
		{0x0F12, 0x0000,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][0]
		{0x0F12, 0x0004,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][1]
		{0x0F12, 0x000B,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][2]
		{0x0F12, 0x001B,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][3]
		{0x0F12, 0x0046,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][4]
		{0x0F12, 0x00AE,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][5]
		{0x0F12, 0x011E,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][6]
		{0x0F12, 0x0154,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][7]
		{0x0F12, 0x0184,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][8]
		{0x0F12, 0x01C6,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][9]
		{0x0F12, 0x01F8,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][10]
		{0x0F12, 0x0222,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][11]
		{0x0F12, 0x0247,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][12]
		{0x0F12, 0x0282,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][13]
		{0x0F12, 0x02B5,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][14]
		{0x0F12, 0x030F,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][15]
		{0x0F12, 0x035F,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][16]
		{0x0F12, 0x03A2,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][17]
		{0x0F12, 0x03D8,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][18]
		{0x0F12, 0x03FF,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][19]
		{0x0F12, 0x0000,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][0]
		{0x0F12, 0x0004,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][1]
		{0x0F12, 0x000B,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][2]
		{0x0F12, 0x001B,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][3]
		{0x0F12, 0x0046,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][4]
		{0x0F12, 0x00AE,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][5]
		{0x0F12, 0x011E,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][6]
		{0x0F12, 0x0154,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][7]
		{0x0F12, 0x0184,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][8]
		{0x0F12, 0x01C6,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][9]
		{0x0F12, 0x01F8,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][10]
		{0x0F12, 0x0222,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][11]
		{0x0F12, 0x0247,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][12]
		{0x0F12, 0x0282,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][13]
		{0x0F12, 0x02B5,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][14]
		{0x0F12, 0x030F,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][15]
		{0x0F12, 0x035F,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][16]
		{0x0F12, 0x03A2,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][17]
		{0x0F12, 0x03D8,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][18]
		{0x0F12, 0x03FF,"",eReadWrite_16,},//saRR_usDualGammaLutRGBIndoor[0][19]
	*/	
	/*
	{0x002a, 0x0734,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},   //0000  //saRR_usDualGammaLutRGBIndoor[0][0]        
	{0x0f12, 0x000A,"",eReadWrite_16,},   //000A  //saRR_usDualGammaLutRGBIndoor[0][1]        
	{0x0f12, 0x0016,"",eReadWrite_16,},   //0016  //saRR_usDualGammaLutRGBIndoor[0][2]        
	{0x0f12, 0x0030,"",eReadWrite_16,},   //0030  //saRR_usDualGammaLutRGBIndoor[0][3]        
	{0x0f12, 0x0066,"",eReadWrite_16,},   //0066  //saRR_usDualGammaLutRGBIndoor[0][4]        
	{0x0f12, 0x00C3,"",eReadWrite_16,},   //00D5  //saRR_usDualGammaLutRGBIndoor[0][5]        
	{0x0f12, 0x0112,"",eReadWrite_16,},   //0138  //saRR_usDualGammaLutRGBIndoor[0][6]        
	{0x0f12, 0x0136,"",eReadWrite_16,},   //0163  //saRR_usDualGammaLutRGBIndoor[0][7]        
	{0x0f12, 0x0152,"",eReadWrite_16,},   //0189  //saRR_usDualGammaLutRGBIndoor[0][8]        
	{0x0f12, 0x0186,"",eReadWrite_16,},   //01C6  //saRR_usDualGammaLutRGBIndoor[0][9]        
	{0x0f12, 0x01AE,"",eReadWrite_16,},   //01F8  //saRR_usDualGammaLutRGBIndoor[0][10]       
	{0x0f12, 0x01D6,"",eReadWrite_16,},   //0222  //saRR_usDualGammaLutRGBIndoor[0][11]       
	{0x0f12, 0x01FA,"",eReadWrite_16,},   //0247  //saRR_usDualGammaLutRGBIndoor[0][12]       
	{0x0f12, 0x024D,"",eReadWrite_16,},   //0282  //saRR_usDualGammaLutRGBIndoor[0][13]       
	{0x0f12, 0x0289,"",eReadWrite_16,},   //02B5  //saRR_usDualGammaLutRGBIndoor[0][14]       
	{0x0f12, 0x02E5,"",eReadWrite_16,},   //030F  //saRR_usDualGammaLutRGBIndoor[0][15]       
	{0x0f12, 0x0338,"",eReadWrite_16,},   //035F  //saRR_usDualGammaLutRGBIndoor[0][16]       
	{0x0f12, 0x038C,"",eReadWrite_16,},   //03A2  //saRR_usDualGammaLutRGBIndoor[0][17]       
	{0x0f12, 0x03D0,"",eReadWrite_16,},   //03D8  //saRR_usDualGammaLutRGBIndoor[0][18]       
	{0x0f12, 0x03FF,"",eReadWrite_16,},   //03FF  //saRR_usDualGammaLutRGBIndoor[0][19]       
	{0x0f12, 0x0000,"",eReadWrite_16,},   //0000  //saRR_usDualGammaLutRGBIndoor[1][0]        
	{0x0f12, 0x000A,"",eReadWrite_16,},   //000A  //saRR_usDualGammaLutRGBIndoor[1][1]        
	{0x0f12, 0x0016,"",eReadWrite_16,},   //0016  //saRR_usDualGammaLutRGBIndoor[1][2]        
	{0x0f12, 0x0030,"",eReadWrite_16,},   //0030  //saRR_usDualGammaLutRGBIndoor[1][3]        
	{0x0f12, 0x0066,"",eReadWrite_16,},   //0066  //saRR_usDualGammaLutRGBIndoor[1][4]        
	{0x0f12, 0x00D5,"",eReadWrite_16,},   //00D5  //saRR_usDualGammaLutRGBIndoor[1][5]        
	{0x0f12, 0x0138,"",eReadWrite_16,},   //0138  //saRR_usDualGammaLutRGBIndoor[1][6]        
	{0x0f12, 0x0163,"",eReadWrite_16,},   //0163  //saRR_usDualGammaLutRGBIndoor[1][7]        
	{0x0f12, 0x0189,"",eReadWrite_16,},   //0189  //saRR_usDualGammaLutRGBIndoor[1][8]        
	{0x0f12, 0x01C6,"",eReadWrite_16,},   //01C6  //saRR_usDualGammaLutRGBIndoor[1][9]        
	{0x0f12, 0x01F8,"",eReadWrite_16,},   //01F8  //saRR_usDualGammaLutRGBIndoor[1][10]       
	{0x0f12, 0x0222,"",eReadWrite_16,},   //0222  //saRR_usDualGammaLutRGBIndoor[1][11]       
	{0x0f12, 0x0247,"",eReadWrite_16,},   //0247  //saRR_usDualGammaLutRGBIndoor[1][12]       
	{0x0f12, 0x0282,"",eReadWrite_16,},   //0282  //saRR_usDualGammaLutRGBIndoor[1][13]       
	{0x0f12, 0x02B5,"",eReadWrite_16,},   //02B5  //saRR_usDualGammaLutRGBIndoor[1][14]       
	{0x0f12, 0x030F,"",eReadWrite_16,},   //030F  //saRR_usDualGammaLutRGBIndoor[1][15]       
	{0x0f12, 0x035F,"",eReadWrite_16,},   //035F  //saRR_usDualGammaLutRGBIndoor[1][16]       
	{0x0f12, 0x03A2,"",eReadWrite_16,},   //03A2  //saRR_usDualGammaLutRGBIndoor[1][17]       
	{0x0f12, 0x03D8,"",eReadWrite_16,},   //03D8  //saRR_usDualGammaLutRGBIndoor[1][18]       
	{0x0f12, 0x03FF,"",eReadWrite_16,},   //03FF  //saRR_usDualGammaLutRGBIndoor[1][19]       
	{0x0f12, 0x0000,"",eReadWrite_16,},   //0000  //saRR_usDualGammaLutRGBIndoor[2][0]        
	{0x0f12, 0x000A,"",eReadWrite_16,},   //000A  //saRR_usDualGammaLutRGBIndoor[2][1]        
	{0x0f12, 0x0016,"",eReadWrite_16,},   //0016  //saRR_usDualGammaLutRGBIndoor[2][2]        
	{0x0f12, 0x0030,"",eReadWrite_16,},   //0030  //saRR_usDualGammaLutRGBIndoor[2][3]        
	{0x0f12, 0x0066,"",eReadWrite_16,},   //0066  //saRR_usDualGammaLutRGBIndoor[2][4]        
	{0x0f12, 0x00D5,"",eReadWrite_16,},   //00D5  //saRR_usDualGammaLutRGBIndoor[2][5]        
	{0x0f12, 0x011E,"",eReadWrite_16,},   //0138  //saRR_usDualGammaLutRGBIndoor[2][6]        
	{0x0f12, 0x013A,"",eReadWrite_16,},   //0163  //saRR_usDualGammaLutRGBIndoor[2][7]        
	{0x0f12, 0x0156,"",eReadWrite_16,},   //0189  //saRR_usDualGammaLutRGBIndoor[2][8]        
	{0x0f12, 0x0182,"",eReadWrite_16,},   //01C6  //saRR_usDualGammaLutRGBIndoor[2][9]        
	{0x0f12, 0x01AE,"",eReadWrite_16,},   //01F8  //saRR_usDualGammaLutRGBIndoor[2][10]       
	{0x0f12, 0x01D6,"",eReadWrite_16,},   //0222  //saRR_usDualGammaLutRGBIndoor[2][11]       
	{0x0f12, 0x0202,"",eReadWrite_16,},   //0247  //saRR_usDualGammaLutRGBIndoor[2][12]       
	{0x0f12, 0x0251,"",eReadWrite_16,},   //0282  //saRR_usDualGammaLutRGBIndoor[2][13]       
	{0x0f12, 0x028D,"",eReadWrite_16,},   //02B5  //saRR_usDualGammaLutRGBIndoor[2][14]       
	{0x0f12, 0x02D9,"",eReadWrite_16,},   //030F  //saRR_usDualGammaLutRGBIndoor[2][15]       
	{0x0f12, 0x0340,"",eReadWrite_16,},   //035F  //saRR_usDualGammaLutRGBIndoor[2][16]       
	{0x0f12, 0x0388,"",eReadWrite_16,},   //03A2  //saRR_usDualGammaLutRGBIndoor[2][17]       
	{0x0f12, 0x03C8,"",eReadWrite_16,},   //03D8  //saRR_usDualGammaLutRGBIndoor[2][18]       
	{0x0f12, 0x03FF,"",eReadWrite_16,},   //03FF  //saRR_usDualGammaLutRGBIndoor[2][19]   
	{0x0f12, 0x0000,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][0]                
	{0x0f12, 0x000b,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][1]                
	{0x0f12, 0x0019,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][2]                
	{0x0f12, 0x0036,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][3]                
	{0x0f12, 0x006f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][4]                
	{0x0f12, 0x00d8,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][5]                
	{0x0f12, 0x0135,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][6]                
	{0x0f12, 0x015f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][7]                
	{0x0f12, 0x0185,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][8]                
	{0x0f12, 0x01c1,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][9]                
	{0x0f12, 0x01f3,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][10]               
	{0x0f12, 0x0220,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][11]               
	{0x0f12, 0x024a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][12]               
	{0x0f12, 0x0291,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][13]               
	{0x0f12, 0x02d0,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][14]               
	{0x0f12, 0x032a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][15]               
	{0x0f12, 0x036a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][16]               
	{0x0f12, 0x039f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][17]               
	{0x0f12, 0x03cc,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][18]               
	{0x0f12, 0x03f9,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[0][19]               
	{0x0f12, 0x0000,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][0]                
	{0x0f12, 0x000b,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][1]                
	{0x0f12, 0x0019,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][2]                
	{0x0f12, 0x0036,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][3]                
	{0x0f12, 0x006f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][4]                
	{0x0f12, 0x00d8,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][5]                
	{0x0f12, 0x0135,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][6]                
	{0x0f12, 0x015f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][7]                
	{0x0f12, 0x0185,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][8]                
	{0x0f12, 0x01c1,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][9]                
	{0x0f12, 0x01f3,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][10]               
	{0x0f12, 0x0220,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][11]               
	{0x0f12, 0x024a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][12]               
	{0x0f12, 0x0291,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][13]               
	{0x0f12, 0x02d0,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][14]               
	{0x0f12, 0x032a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][15]               
	{0x0f12, 0x036a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][16]               
	{0x0f12, 0x039f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][17]               
	{0x0f12, 0x03cc,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][18]               
	{0x0f12, 0x03f9,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[1][19]               
	{0x0f12, 0x0000,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][0]                
	{0x0f12, 0x000b,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][1]                
	{0x0f12, 0x0019,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][2]                
	{0x0f12, 0x0036,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][3]                
	{0x0f12, 0x006f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][4]                
	{0x0f12, 0x00d8,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][5]                
	{0x0f12, 0x0135,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][6]                
	{0x0f12, 0x015f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][7]                
	{0x0f12, 0x0185,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][8]                
	{0x0f12, 0x01c1,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][9]                
	{0x0f12, 0x01f3,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][10]               
	{0x0f12, 0x0220,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][11]               
	{0x0f12, 0x024a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][12]               
	{0x0f12, 0x0291,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][13]               
	{0x0f12, 0x02d0,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][14]               
	{0x0f12, 0x032a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][15]               
	{0x0f12, 0x036a,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][16]               
	{0x0f12, 0x039f,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][17]               
	{0x0f12, 0x03cc,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][18]               
	{0x0f12, 0x03f9,"",eReadWrite_16,},  //saRR_usDualGammaLutRGBOutdoor[2][19]  
	*/
	{0x002A, 0x0230,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0020,"",eReadWrite_16,},
	{0x0f12, 0x0018,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002A, 0x023A,"",eReadWrite_16,},
	{0x0f12, 0x0120,"",eReadWrite_16,},

//================================================================================== 
// 17.AFIT                                                                           
//==================================================================================            
 {0x002a, 0x0944,"",eReadWrite_16,},
  {0x0f12, 0x0050,"",eReadWrite_16,}, //afit_uNoiseIndInDoor 0x0050                                                                              
	{0x0f12, 0x00b0,"",eReadWrite_16,}, //afit_uNoiseIndInDoor 0x00b0                                                                              
	{0x0f12, 0x0196,"",eReadWrite_16,}, //afit_uNoiseIndInDoor    0x0196                                                                           
	{0x0f12, 0x0245,"",eReadWrite_16,}, //afit_uNoiseIndInDoor                                                                              
	{0x0f12, 0x0300,"",eReadWrite_16,}, //afit_uNoiseIndInDoor                                                                              
	{0x002a, 0x0938,"",eReadWrite_16,},                                                                                                     
	{0x0f12, 0x0000,"",eReadWrite_16,}, // on/off AFIT by NB option                                                                         
	{0x0f12, 0x0014,"",eReadWrite_16,}, //SARR_uNormBrInDoor                                                                                
	{0x0f12, 0x00d2,"",eReadWrite_16,}, //SARR_uNormBrInDoor                                                                                
	{0x0f12, 0x0384,"",eReadWrite_16,}, //SARR_uNormBrInDoor                                                                                
	{0x0f12, 0x07d0,"",eReadWrite_16,}, //SARR_uNormBrInDoor                                                                                
	{0x0f12, 0x1388,"",eReadWrite_16,}, //SARR_uNormBrInDoor                                                                                
	{0x002a, 0x0976,"",eReadWrite_16,},                                                                                                     
	{0x0f12, 0x0070,"",eReadWrite_16,}, //afit_usGamutTh                                                                                    
	{0x0f12, 0x0005,"",eReadWrite_16,}, //afit_usNeargrayOffset                                                                             
	{0x0f12, 0x0000,"",eReadWrite_16,}, //afit_bUseSenBpr                                                                                   
	{0x0f12, 0x01cc,"",eReadWrite_16,}, //afit_usBprThr_0_                                                                                  
	{0x0f12, 0x01cc,"",eReadWrite_16,}, //afit_usBprThr_1_                                                                                  
	{0x0f12, 0x01cc,"",eReadWrite_16,}, //afit_usBprThr_2_                                                                                  
	{0x0f12, 0x01cc,"",eReadWrite_16,}, //afit_usBprThr_3_                                                                                  
	{0x0f12, 0x01cc,"",eReadWrite_16,}, //afit_usBprThr_4_                                                                                  
	{0x0f12, 0x0180,"",eReadWrite_16,}, //afit_NIContrastAFITValue                                                                          
	{0x0f12, 0x0196,"",eReadWrite_16,}, //afit_NIContrastTh                                                                                 
	{0x002a, 0x098c,"",eReadWrite_16,},                                                                                                     
	{0x0f12, 0x0000,"",eReadWrite_16,}, //7000098C//AFIT16_BRIGHTNESS                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //7000098E//AFIT16_CONTRAST                                                                         
	{0x0f12, 0xfff0,"",eReadWrite_16,}, //70000990//AFIT16_SATURATION                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000992//AFIT16_SHARP_BLUR                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000994//AFIT16_GLAMOUR                                                                          
	{0x0f12, 0x00c0,"",eReadWrite_16,}, //70000996//AFIT16_bnr_edge_high                                                                    
	{0x0f12, 0x0064,"",eReadWrite_16,}, //70000998//AFIT16_postdmsc_iLowBright                                                              
	{0x0f12, 0x0384,"",eReadWrite_16,}, //7000099A//AFIT16_postdmsc_iHighBright                                                             
	{0x0f12, 0x005f,"",eReadWrite_16,}, //7000099C//AFIT16_postdmsc_iLowSat                                                                 
	{0x0f12, 0x01f4,"",eReadWrite_16,}, //7000099E//AFIT16_postdmsc_iHighSat                                                                
	{0x0f12, 0x0070,"",eReadWrite_16,}, //700009A0//AFIT16_postdmsc_iTune                                                                   
	{0x0f12, 0x0040,"",eReadWrite_16,}, //700009A2//AFIT16_yuvemix_mNegRanges_0                                                             
	{0x0f12, 0x00a0,"",eReadWrite_16,}, //700009A4//AFIT16_yuvemix_mNegRanges_1                                                             
	{0x0f12, 0x0100,"",eReadWrite_16,}, //700009A6//AFIT16_yuvemix_mNegRanges_2                                                             
	{0x0f12, 0x0010,"",eReadWrite_16,}, //700009A8//AFIT16_yuvemix_mPosRanges_0                                                             
	{0x0f12, 0x0040,"",eReadWrite_16,}, //700009AA//AFIT16_yuvemix_mPosRanges_1                                                             
	{0x0f12, 0x00a0,"",eReadWrite_16,}, //700009AC//AFIT16_yuvemix_mPosRanges_2                                                             
	{0x0f12, 0x1430,"",eReadWrite_16,}, //700009AE//AFIT8_bnr_edge_low  [7:0] AFIT8_bnr_repl_thresh                                         
	{0x0f12, 0x0201,"",eReadWrite_16,}, //700009B0//AFIT8_bnr_repl_force  [7:0] AFIT8_bnr_iHotThreshHigh                                    
	{0x0f12, 0x0204,"",eReadWrite_16,}, //700009B2//AFIT8_bnr_iHotThreshLow   [7:0] AFIT8_bnr_iColdThreshHigh                               
	{0x0f12, 0x3604,"",eReadWrite_16,}, //700009B4//AFIT8_bnr_iColdThreshLow   [7:0] AFIT8_bnr_DispTH_Low                                   
	{0x0f12, 0x032a,"",eReadWrite_16,}, //700009B6//AFIT8_bnr_DispTH_High   [7:0] AFIT8_bnr_DISP_Limit_Low                                  
	{0x0f12, 0x0403,"",eReadWrite_16,}, //700009B8//AFIT8_bnr_DISP_Limit_High   [7:0] AFIT8_bnr_iDistSigmaMin                               
	{0x0f12, 0x1b06,"",eReadWrite_16,}, //700009BA//AFIT8_bnr_iDistSigmaMax   [7:0] AFIT8_bnr_iDiffSigmaLow                                 
	{0x0f12, 0x6015,"",eReadWrite_16,}, //700009BC//AFIT8_bnr_iDiffSigmaHigh   [7:0] AFIT8_bnr_iNormalizedSTD_TH                            
	{0x0f12, 0x00c0,"",eReadWrite_16,}, //700009BE//AFIT8_bnr_iNormalizedSTD_Limit [7:0] AFIT8_bnr_iDirNRTune                               
	{0x0f12, 0x6080,"",eReadWrite_16,}, //700009C0//AFIT8_bnr_iDirMinThres [7:0] AFIT8_bnr_iDirFltDiffThresHigh                             
	{0x0f12, 0x4080,"",eReadWrite_16,}, //700009C2//AFIT8_bnr_iDirFltDiffThresLow   [7:0] AFIT8_bnr_iDirSmoothPowerHigh                     
	{0x0f12, 0x0640,"",eReadWrite_16,}, //700009C4//AFIT8_bnr_iDirSmoothPowerLow   [7:0] AFIT8_bnr_iLowMaxSlopeAllowed                      
	{0x0f12, 0x0306,"",eReadWrite_16,}, //700009C6//AFIT8_bnr_iHighMaxSlopeAllowed [7:0] AFIT8_bnr_iLowSlopeThresh                          
	{0x0f12, 0x2003,"",eReadWrite_16,}, //700009C8//AFIT8_bnr_iHighSlopeThresh [7:0] AFIT8_bnr_iSlopenessTH                                 
	{0x0f12, 0xff01,"",eReadWrite_16,}, //700009CA//AFIT8_bnr_iSlopeBlurStrength   [7:0] AFIT8_bnr_iSlopenessLimit                          
	{0x0f12, 0x0000,"",eReadWrite_16,}, //700009CC//AFIT8_bnr_AddNoisePower1   [7:0] AFIT8_bnr_AddNoisePower2                               
	{0x0f12, 0x0400,"",eReadWrite_16,}, //700009CE//AFIT8_bnr_iRadialTune   [7:0] AFIT8_bnr_iRadialPower                                    
	{0x0f12, 0x365a,"",eReadWrite_16,}, //700009D0//AFIT8_bnr_iRadialLimit [7:0] AFIT8_ee_iFSMagThLow                                       
	{0x0f12, 0x102a,"",eReadWrite_16,}, //700009D2//AFIT8_ee_iFSMagThHigh   [7:0] AFIT8_ee_iFSVarThLow                                      
	{0x0f12, 0x000b,"",eReadWrite_16,}, //700009D4//AFIT8_ee_iFSVarThHigh   [7:0] AFIT8_ee_iFSThLow                                         
	{0x0f12, 0x0600,"",eReadWrite_16,}, //700009D6//AFIT8_ee_iFSThHigh [7:0] AFIT8_ee_iFSmagPower                                           
	{0x0f12, 0x5a0f,"",eReadWrite_16,}, //700009D8//AFIT8_ee_iFSVarCountTh [7:0] AFIT8_ee_iRadialLimit                                      
	{0x0f12, 0x0505,"",eReadWrite_16,}, //700009DA//AFIT8_ee_iRadialPower   [7:0] AFIT8_ee_iSmoothEdgeSlope                                 
	{0x0f12, 0x1802,"",eReadWrite_16,}, //700009DC//AFIT8_ee_iROADThres   [7:0] AFIT8_ee_iROADMaxNR                                         
	{0x0f12, 0x0000,"",eReadWrite_16,}, //700009DE//AFIT8_ee_iROADSubMaxNR [7:0] AFIT8_ee_iROADSubThres                                     
	{0x0f12, 0x2006,"",eReadWrite_16,}, //700009E0//AFIT8_ee_iROADNeiThres [7:0] AFIT8_ee_iROADNeiMaxNR                                     
	{0x0f12, 0x3028,"",eReadWrite_16,}, //700009E2//AFIT8_ee_iSmoothEdgeThres   [7:0] AFIT8_ee_iMSharpen                                    
	{0x0f12, 0x0468,"",eReadWrite_16,}, //700009E4//AFIT8_ee_iWSharpen [7:0] AFIT8_ee_iMShThresh 0x0418                                           
	{0x0f12, 0x0101,"",eReadWrite_16,}, //700009E6//AFIT8_ee_iWShThresh   [7:0] AFIT8_ee_iReduceNegative                                    
	{0x0f12, 0x0800,"",eReadWrite_16,}, //700009E8//AFIT8_ee_iEmbossCentAdd   [7:0] AFIT8_ee_iShDespeckle                                   
	{0x0f12, 0x1804,"",eReadWrite_16,}, //700009EA//AFIT8_ee_iReduceEdgeThresh [7:0] AFIT8_dmsc_iEnhThresh                                  
	{0x0f12, 0x4008,"",eReadWrite_16,}, //700009EC//AFIT8_dmsc_iDesatThresh   [7:0] AFIT8_dmsc_iDemBlurHigh                                 
	{0x0f12, 0x0540,"",eReadWrite_16,}, //700009EE//AFIT8_dmsc_iDemBlurLow [7:0] AFIT8_dmsc_iDemBlurRange                                   
	{0x0f12, 0x8006,"",eReadWrite_16,}, //700009F0//AFIT8_dmsc_iDecisionThresh [7:0] AFIT8_dmsc_iCentGrad                                   
	{0x0f12, 0x0020,"",eReadWrite_16,}, //700009F2//AFIT8_dmsc_iMonochrom   [7:0] AFIT8_dmsc_iGBDenoiseVal                                  
	{0x0f12, 0x0000,"",eReadWrite_16,}, //700009F4//AFIT8_dmsc_iGRDenoiseVal   [7:0] AFIT8_dmsc_iEdgeDesatThrHigh                           
	{0x0f12, 0x1800,"",eReadWrite_16,}, //700009F6//AFIT8_dmsc_iEdgeDesatThrLow   [7:0] AFIT8_dmsc_iEdgeDesat                               
	{0x0f12, 0x0000,"",eReadWrite_16,}, //700009F8//AFIT8_dmsc_iNearGrayDesat   [7:0] AFIT8_dmsc_iEdgeDesatLimit                            
	{0x0f12, 0x1e10,"",eReadWrite_16,}, //700009FA//AFIT8_postdmsc_iBCoeff [7:0] AFIT8_postdmsc_iGCoeff                                     
	{0x0f12, 0x000b,"",eReadWrite_16,}, //700009FC//AFIT8_postdmsc_iWideMult   [7:0] AFIT8_yuvemix_mNegSlopes_0                             
	{0x0f12, 0x0607,"",eReadWrite_16,}, //700009FE//AFIT8_yuvemix_mNegSlopes_1 [7:0] AFIT8_yuvemix_mNegSlopes_2                             
	{0x0f12, 0x0005,"",eReadWrite_16,}, //70000A00//AFIT8_yuvemix_mNegSlopes_3 [7:0] AFIT8_yuvemix_mPosSlopes_0                             
	{0x0f12, 0x0607,"",eReadWrite_16,}, //70000A02//AFIT8_yuvemix_mPosSlopes_1 [7:0] AFIT8_yuvemix_mPosSlopes_2                             
	{0x0f12, 0x0405,"",eReadWrite_16,}, //70000A04//AFIT8_yuvemix_mPosSlopes_3 [7:0] AFIT8_yuviirnr_iXSupportY                              
	{0x0f12, 0x0205,"",eReadWrite_16,}, //70000A06//AFIT8_yuviirnr_iXSupportUV [7:0] AFIT8_yuviirnr_iLowYNorm                               
	{0x0f12, 0x0304,"",eReadWrite_16,}, //70000A08//AFIT8_yuviirnr_iHighYNorm   [7:0] AFIT8_yuviirnr_iLowUVNorm                             
	{0x0f12, 0x0409,"",eReadWrite_16,}, //70000A0A//AFIT8_yuviirnr_iHighUVNorm [7:0] AFIT8_yuviirnr_iYNormShift                             
	{0x0f12, 0x0306,"",eReadWrite_16,}, //70000A0C//AFIT8_yuviirnr_iUVNormShift   [7:0] AFIT8_yuviirnr_iVertLength_Y                        
	{0x0f12, 0x0407,"",eReadWrite_16,}, //70000A0E//AFIT8_yuviirnr_iVertLength_UV   [7:0] AFIT8_yuviirnr_iDiffThreshL_Y                     
	{0x0f12, 0x1c04,"",eReadWrite_16,}, //70000A10//AFIT8_yuviirnr_iDiffThreshH_Y   [7:0] AFIT8_yuviirnr_iDiffThreshL_UV                    
	{0x0f12, 0x0214,"",eReadWrite_16,}, //70000A12//AFIT8_yuviirnr_iDiffThreshH_UV [7:0] AFIT8_yuviirnr_iMaxThreshL_Y                       
	{0x0f12, 0x1002,"",eReadWrite_16,}, //70000A14//AFIT8_yuviirnr_iMaxThreshH_Y   [7:0] AFIT8_yuviirnr_iMaxThreshL_UV                      
	{0x0f12, 0x0610,"",eReadWrite_16,}, //70000A16//AFIT8_yuviirnr_iMaxThreshH_UV   [7:0] AFIT8_yuviirnr_iYNRStrengthL                      
	{0x0f12, 0x1a02,"",eReadWrite_16,}, //70000A18//AFIT8_yuviirnr_iYNRStrengthH   [7:0] AFIT8_yuviirnr_iUVNRStrengthL                      
	{0x0f12, 0x4a18,"",eReadWrite_16,}, //70000A1A//AFIT8_yuviirnr_iUVNRStrengthH   [7:0] AFIT8_byr_gras_iShadingPower                      
	{0x0f12, 0x0080,"",eReadWrite_16,}, //70000A1C//AFIT8_RGBGamma2_iLinearity [7:0] AFIT8_RGBGamma2_iDarkReduce                            
	{0x0f12, 0x0348,"",eReadWrite_16,}, //70000A1E//AFIT8_ccm_oscar_iSaturation   [7:0] AFIT8_RGB2YUV_iYOffset                              
	{0x0f12, 0x0180,"",eReadWrite_16,}, //70000A20//AFIT8_RGB2YUV_iRGBGain [7:0] AFIT8_bnr_nClustLevel_H                                    
	{0x0f12, 0x0a0a,"",eReadWrite_16,}, //70000A22//AFIT8_bnr_iClustMulT_H [7:0] AFIT8_bnr_iClustMulT_C                                     
	{0x0f12, 0x0101,"",eReadWrite_16,}, //70000A24//AFIT8_bnr_iClustThresh_H   [7:0] AFIT8_bnr_iClustThresh_C                               
	{0x0f12, 0x0505,"",eReadWrite_16,}, //70000A26//AFIT8_bnr_iDenThreshLow   [7:0] AFIT8_bnr_iDenThreshHigh                                
	{0x0f12, 0xc080,"",eReadWrite_16,}, //70000A28//AFIT8_ee_iLowSharpPower   [7:0] AFIT8_ee_iHighSharpPower                                
	{0x0f12, 0x2a36,"",eReadWrite_16,}, //70000A2A//AFIT8_ee_iLowShDenoise [7:0] AFIT8_ee_iHighShDenoise                                    
	{0x0f12, 0xffff,"",eReadWrite_16,}, //70000A2C//AFIT8_ee_iLowSharpClamp   [7:0] AFIT8_ee_iHighSharpClamp                                
	{0x0f12, 0x0808,"",eReadWrite_16,}, //70000A2E//AFIT8_ee_iReduceEdgeMinMult   [7:0] AFIT8_ee_iReduceEdgeSlope                           
	{0x0f12, 0x0a01,"",eReadWrite_16,}, //70000A30//AFIT8_bnr_nClustLevel_H_Bin   [7:0] AFIT8_bnr_iClustMulT_H_Bin                          
	{0x0f12, 0x010a,"",eReadWrite_16,}, //70000A32//AFIT8_bnr_iClustMulT_C_Bin [7:0] AFIT8_bnr_iClustThresh_H_Bin                           
	{0x0f12, 0x3601,"",eReadWrite_16,}, //70000A34//AFIT8_bnr_iClustThresh_C_Bin   [7:0] AFIT8_bnr_iDenThreshLow_Bin                        
	{0x0f12, 0x242a,"",eReadWrite_16,}, //70000A36//AFIT8_bnr_iDenThreshHigh_Bin   [7:0] AFIT8_ee_iLowSharpPower_Bin                        
	{0x0f12, 0x3660,"",eReadWrite_16,}, //70000A38//AFIT8_ee_iHighSharpPower_Bin   [7:0] AFIT8_ee_iLowShDenoise_Bin                         
	{0x0f12, 0xff2a,"",eReadWrite_16,}, //70000A3A//AFIT8_ee_iHighShDenoise_Bin   [7:0] AFIT8_ee_iLowSharpClamp_Bin                         
	{0x0f12, 0x08ff,"",eReadWrite_16,}, //70000A3C//AFIT8_ee_iHighSharpClamp_Bin   [7:0] AFIT8_ee_iReduceEdgeMinMult_Bin                    
	{0x0f12, 0x0008,"",eReadWrite_16,}, //70000A3E//AFIT8_ee_iReduceEdgeSlope_Bin [7:0]                                                     
	{0x0f12, 0x0001,"",eReadWrite_16,}, //70000A40//AFITB_bnr_nClustLevel_C    [0]                                                          
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000A42//AFIT16_BRIGHTNESS                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000A44//AFIT16_CONTRAST                                                                         
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000A46//AFIT16_SATURATION                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000A48//AFIT16_SHARP_BLUR                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000A4A//AFIT16_GLAMOUR                                                                          
	{0x0f12, 0x00c0,"",eReadWrite_16,}, //70000A4C//AFIT16_bnr_edge_high                                                                    
	{0x0f12, 0x0064,"",eReadWrite_16,}, //70000A4E//AFIT16_postdmsc_iLowBright                                                              
	{0x0f12, 0x0384,"",eReadWrite_16,}, //70000A50//AFIT16_postdmsc_iHighBright                                                             
	{0x0f12, 0x0051,"",eReadWrite_16,}, //70000A52//AFIT16_postdmsc_iLowSat                                                                 
	{0x0f12, 0x01f4,"",eReadWrite_16,}, //70000A54//AFIT16_postdmsc_iHighSat                                                                
	{0x0f12, 0x0070,"",eReadWrite_16,}, //70000A56//AFIT16_postdmsc_iTune                                                                   
	{0x0f12, 0x0040,"",eReadWrite_16,}, //70000A58//AFIT16_yuvemix_mNegRanges_0                                                             
	{0x0f12, 0x00a0,"",eReadWrite_16,}, //70000A5A//AFIT16_yuvemix_mNegRanges_1                                                             
	{0x0f12, 0x0100,"",eReadWrite_16,}, //70000A5C//AFIT16_yuvemix_mNegRanges_2                                                             
	{0x0f12, 0x0010,"",eReadWrite_16,}, //70000A5E//AFIT16_yuvemix_mPosRanges_0                                                             
	{0x0f12, 0x0060,"",eReadWrite_16,}, //70000A60//AFIT16_yuvemix_mPosRanges_1                                                             
	{0x0f12, 0x0100,"",eReadWrite_16,}, //70000A62//AFIT16_yuvemix_mPosRanges_2                                                             
	{0x0f12, 0x1430,"",eReadWrite_16,}, //70000A64//AFIT8_bnr_edge_low  [7:0] AFIT8_bnr_repl_thresh                                         
	{0x0f12, 0x0201,"",eReadWrite_16,}, //70000A66//AFIT8_bnr_repl_force  [7:0] AFIT8_bnr_iHotThreshHigh                                    
	{0x0f12, 0x0204,"",eReadWrite_16,}, //70000A68//AFIT8_bnr_iHotThreshLow   [7:0] AFIT8_bnr_iColdThreshHigh                               
	{0x0f12, 0x2404,"",eReadWrite_16,}, //70000A6A//AFIT8_bnr_iColdThreshLow   [7:0] AFIT8_bnr_DispTH_Low                                   
	{0x0f12, 0x031b,"",eReadWrite_16,}, //70000A6C//AFIT8_bnr_DispTH_High   [7:0] AFIT8_bnr_DISP_Limit_Low                                  
	{0x0f12, 0x0103,"",eReadWrite_16,}, //70000A6E//AFIT8_bnr_DISP_Limit_High   [7:0] AFIT8_bnr_iDistSigmaMin                               
	{0x0f12, 0x1205,"",eReadWrite_16,}, //70000A70//AFIT8_bnr_iDistSigmaMax   [7:0] AFIT8_bnr_iDiffSigmaLow                                 
	{0x0f12, 0x400d,"",eReadWrite_16,}, //70000A72//AFIT8_bnr_iDiffSigmaHigh   [7:0] AFIT8_bnr_iNormalizedSTD_TH                            
	{0x0f12, 0x0080,"",eReadWrite_16,}, //70000A74//AFIT8_bnr_iNormalizedSTD_Limit [7:0] AFIT8_bnr_iDirNRTune                               
	{0x0f12, 0x2080,"",eReadWrite_16,}, //70000A76//AFIT8_bnr_iDirMinThres [7:0] AFIT8_bnr_iDirFltDiffThresHigh                             
	{0x0f12, 0x3040,"",eReadWrite_16,}, //70000A78//AFIT8_bnr_iDirFltDiffThresLow   [7:0] AFIT8_bnr_iDirSmoothPowerHigh                     
	{0x0f12, 0x0630,"",eReadWrite_16,}, //70000A7A//AFIT8_bnr_iDirSmoothPowerLow   [7:0] AFIT8_bnr_iLowMaxSlopeAllowed                      
	{0x0f12, 0x0306,"",eReadWrite_16,}, //70000A7C//AFIT8_bnr_iHighMaxSlopeAllowed [7:0] AFIT8_bnr_iLowSlopeThresh                          
	{0x0f12, 0x2003,"",eReadWrite_16,}, //70000A7E//AFIT8_bnr_iHighSlopeThresh [7:0] AFIT8_bnr_iSlopenessTH                                 
	{0x0f12, 0xff01,"",eReadWrite_16,}, //70000A80//AFIT8_bnr_iSlopeBlurStrength   [7:0] AFIT8_bnr_iSlopenessLimit                          
	{0x0f12, 0x0404,"",eReadWrite_16,}, //70000A82//AFIT8_bnr_AddNoisePower1   [7:0] AFIT8_bnr_AddNoisePower2                               
	{0x0f12, 0x0300,"",eReadWrite_16,}, //70000A84//AFIT8_bnr_iRadialTune   [7:0] AFIT8_bnr_iRadialPower                                    
	{0x0f12, 0x245a,"",eReadWrite_16,}, //70000A86//AFIT8_bnr_iRadialLimit [7:0] AFIT8_ee_iFSMagThLow                                       
	{0x0f12, 0x1018,"",eReadWrite_16,}, //70000A88//AFIT8_ee_iFSMagThHigh   [7:0] AFIT8_ee_iFSVarThLow                                      
	{0x0f12, 0x000b,"",eReadWrite_16,}, //70000A8A//AFIT8_ee_iFSVarThHigh   [7:0] AFIT8_ee_iFSThLow                                         
	{0x0f12, 0x0b00,"",eReadWrite_16,}, //70000A8C//AFIT8_ee_iFSThHigh [7:0] AFIT8_ee_iFSmagPower                                           
	{0x0f12, 0x5a0f,"",eReadWrite_16,}, //70000A8E//AFIT8_ee_iFSVarCountTh [7:0] AFIT8_ee_iRadialLimit                                      
	{0x0f12, 0x0505,"",eReadWrite_16,}, //70000A90//AFIT8_ee_iRadialPower   [7:0] AFIT8_ee_iSmoothEdgeSlope                                 
	{0x0f12, 0x1802,"",eReadWrite_16,}, //70000A92//AFIT8_ee_iROADThres   [7:0] AFIT8_ee_iROADMaxNR                                         
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000A94//AFIT8_ee_iROADSubMaxNR [7:0] AFIT8_ee_iROADSubThres                                     
	{0x0f12, 0x2006,"",eReadWrite_16,}, //70000A96//AFIT8_ee_iROADNeiThres [7:0] AFIT8_ee_iROADNeiMaxNR                                     
	{0x0f12, 0x3428,"",eReadWrite_16,}, //70000A98//AFIT8_ee_iSmoothEdgeThres   [7:0] AFIT8_ee_iMSharpen                                    
	{0x0f12, 0x046c,"",eReadWrite_16,}, //70000A9A//AFIT8_ee_iWSharpen [7:0] AFIT8_ee_iMShThresh   0x041c                                         
	{0x0f12, 0x0101,"",eReadWrite_16,}, //70000A9C//AFIT8_ee_iWShThresh   [7:0] AFIT8_ee_iReduceNegative                                    
	{0x0f12, 0x0800,"",eReadWrite_16,}, //70000A9E//AFIT8_ee_iEmbossCentAdd   [7:0] AFIT8_ee_iShDespeckle                                   
	{0x0f12, 0x1004,"",eReadWrite_16,}, //70000AA0//AFIT8_ee_iReduceEdgeThresh [7:0] AFIT8_dmsc_iEnhThresh                                  
	{0x0f12, 0x4008,"",eReadWrite_16,}, //70000AA2//AFIT8_dmsc_iDesatThresh   [7:0] AFIT8_dmsc_iDemBlurHigh                                 
	{0x0f12, 0x0540,"",eReadWrite_16,}, //70000AA4//AFIT8_dmsc_iDemBlurLow [7:0] AFIT8_dmsc_iDemBlurRange                                   
	{0x0f12, 0x8006,"",eReadWrite_16,}, //70000AA6//AFIT8_dmsc_iDecisionThresh [7:0] AFIT8_dmsc_iCentGrad                                   
	{0x0f12, 0x0020,"",eReadWrite_16,}, //70000AA8//AFIT8_dmsc_iMonochrom   [7:0] AFIT8_dmsc_iGBDenoiseVal                                  
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000AAA//AFIT8_dmsc_iGRDenoiseVal   [7:0] AFIT8_dmsc_iEdgeDesatThrHigh                           
	{0x0f12, 0x1800,"",eReadWrite_16,}, //70000AAC//AFIT8_dmsc_iEdgeDesatThrLow   [7:0] AFIT8_dmsc_iEdgeDesat                               
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000AAE//AFIT8_dmsc_iNearGrayDesat   [7:0] AFIT8_dmsc_iEdgeDesatLimit                            
	{0x0f12, 0x1e10,"",eReadWrite_16,}, //70000AB0//AFIT8_postdmsc_iBCoeff [7:0] AFIT8_postdmsc_iGCoeff                                     
	{0x0f12, 0x000b,"",eReadWrite_16,}, //70000AB2//AFIT8_postdmsc_iWideMult   [7:0] AFIT8_yuvemix_mNegSlopes_0                             
	{0x0f12, 0x0607,"",eReadWrite_16,}, //70000AB4//AFIT8_yuvemix_mNegSlopes_1 [7:0] AFIT8_yuvemix_mNegSlopes_2                             
	{0x0f12, 0x0005,"",eReadWrite_16,}, //70000AB6//AFIT8_yuvemix_mNegSlopes_3 [7:0] AFIT8_yuvemix_mPosSlopes_0                             
	{0x0f12, 0x0607,"",eReadWrite_16,}, //70000AB8//AFIT8_yuvemix_mPosSlopes_1 [7:0] AFIT8_yuvemix_mPosSlopes_2                             
	{0x0f12, 0x0405,"",eReadWrite_16,}, //70000ABA//AFIT8_yuvemix_mPosSlopes_3 [7:0] AFIT8_yuviirnr_iXSupportY                              
	{0x0f12, 0x0205,"",eReadWrite_16,}, //70000ABC//AFIT8_yuviirnr_iXSupportUV [7:0] AFIT8_yuviirnr_iLowYNorm                               
	{0x0f12, 0x0304,"",eReadWrite_16,}, //70000ABE//AFIT8_yuviirnr_iHighYNorm   [7:0] AFIT8_yuviirnr_iLowUVNorm                             
	{0x0f12, 0x0409,"",eReadWrite_16,}, //70000AC0//AFIT8_yuviirnr_iHighUVNorm [7:0] AFIT8_yuviirnr_iYNormShift                             
	{0x0f12, 0x0306,"",eReadWrite_16,}, //70000AC2//AFIT8_yuviirnr_iUVNormShift   [7:0] AFIT8_yuviirnr_iVertLength_Y                        
	{0x0f12, 0x0407,"",eReadWrite_16,}, //70000AC4//AFIT8_yuviirnr_iVertLength_UV   [7:0] AFIT8_yuviirnr_iDiffThreshL_Y                     
	{0x0f12, 0x1f04,"",eReadWrite_16,}, //70000AC6//AFIT8_yuviirnr_iDiffThreshH_Y   [7:0] AFIT8_yuviirnr_iDiffThreshL_UV                    
	{0x0f12, 0x0218,"",eReadWrite_16,}, //70000AC8//AFIT8_yuviirnr_iDiffThreshH_UV [7:0] AFIT8_yuviirnr_iMaxThreshL_Y                       
	{0x0f12, 0x1102,"",eReadWrite_16,}, //70000ACA//AFIT8_yuviirnr_iMaxThreshH_Y   [7:0] AFIT8_yuviirnr_iMaxThreshL_UV                      
	{0x0f12, 0x0611,"",eReadWrite_16,}, //70000ACC//AFIT8_yuviirnr_iMaxThreshH_UV   [7:0] AFIT8_yuviirnr_iYNRStrengthL                      
	{0x0f12, 0x1a02,"",eReadWrite_16,}, //70000ACE//AFIT8_yuviirnr_iYNRStrengthH   [7:0] AFIT8_yuviirnr_iUVNRStrengthL                      
	{0x0f12, 0x8018,"",eReadWrite_16,}, //70000AD0//AFIT8_yuviirnr_iUVNRStrengthH   [7:0] AFIT8_byr_gras_iShadingPower                      
	{0x0f12, 0x0080,"",eReadWrite_16,}, //70000AD2//AFIT8_RGBGamma2_iLinearity [7:0] AFIT8_RGBGamma2_iDarkReduce                            
	{0x0f12, 0x0380,"",eReadWrite_16,}, //70000AD4//AFIT8_ccm_oscar_iSaturation   [7:0] AFIT8_RGB2YUV_iYOffset                              
	{0x0f12, 0x0180,"",eReadWrite_16,}, //70000AD6//AFIT8_RGB2YUV_iRGBGain [7:0] AFIT8_bnr_nClustLevel_H                                    
	{0x0f12, 0x0a0a,"",eReadWrite_16,}, //70000AD8//AFIT8_bnr_iClustMulT_H [7:0] AFIT8_bnr_iClustMulT_C                                     
	{0x0f12, 0x0101,"",eReadWrite_16,}, //70000ADA//AFIT8_bnr_iClustThresh_H   [7:0] AFIT8_bnr_iClustThresh_C                               
	{0x0f12, 0x0505,"",eReadWrite_16,}, //70000ADC//AFIT8_bnr_iDenThreshLow   [7:0] AFIT8_bnr_iDenThreshHigh 0x1b24                               
	{0x0f12, 0xc080,"",eReadWrite_16,}, //70000ADE//AFIT8_ee_iLowSharpPower   [7:0] AFIT8_ee_iHighSharpPower                                
	{0x0f12, 0x1d22,"",eReadWrite_16,}, //70000AE0//AFIT8_ee_iLowShDenoise [7:0] AFIT8_ee_iHighShDenoise                                    
	{0x0f12, 0xffff,"",eReadWrite_16,}, //70000AE2//AFIT8_ee_iLowSharpClamp   [7:0] AFIT8_ee_iHighSharpClamp                                
	{0x0f12, 0x0808,"",eReadWrite_16,}, //70000AE4//AFIT8_ee_iReduceEdgeMinMult   [7:0] AFIT8_ee_iReduceEdgeSlope                           
	{0x0f12, 0x0a01,"",eReadWrite_16,}, //70000AE6//AFIT8_bnr_nClustLevel_H_Bin   [7:0] AFIT8_bnr_iClustMulT_H_Bin                          
	{0x0f12, 0x010a,"",eReadWrite_16,}, //70000AE8//AFIT8_bnr_iClustMulT_C_Bin [7:0] AFIT8_bnr_iClustThresh_H_Bin                           
	{0x0f12, 0x2401,"",eReadWrite_16,}, //70000AEA//AFIT8_bnr_iClustThresh_C_Bin   [7:0] AFIT8_bnr_iDenThreshLow_Bin                        
	{0x0f12, 0x241b,"",eReadWrite_16,}, //70000AEC//AFIT8_bnr_iDenThreshHigh_Bin   [7:0] AFIT8_ee_iLowSharpPower_Bin                        
	{0x0f12, 0x1e60,"",eReadWrite_16,}, //70000AEE//AFIT8_ee_iHighSharpPower_Bin   [7:0] AFIT8_ee_iLowShDenoise_Bin                         
	{0x0f12, 0xff18,"",eReadWrite_16,}, //70000AF0//AFIT8_ee_iHighShDenoise_Bin   [7:0] AFIT8_ee_iLowSharpClamp_Bin                         
	{0x0f12, 0x08ff,"",eReadWrite_16,}, //70000AF2//AFIT8_ee_iHighSharpClamp_Bin   [7:0] AFIT8_ee_iReduceEdgeMinMult_Bin                    
	{0x0f12, 0x0008,"",eReadWrite_16,}, //70000AF4//AFIT8_ee_iReduceEdgeSlope_Bin [7:0]                                                     
	{0x0f12, 0x0001,"",eReadWrite_16,}, //70000AF6//AFITB_bnr_nClustLevel_C    [0]                                                          
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000AF8//AFIT16_BRIGHTNESS                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000AFA//AFIT16_CONTRAST                                                                         
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000AFC//AFIT16_SATURATION                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000AFE//AFIT16_SHARP_BLUR                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000B00//AFIT16_GLAMOUR                                                                          
	{0x0f12, 0x00c0,"",eReadWrite_16,}, //70000B02//AFIT16_bnr_edge_high                                                                    
	{0x0f12, 0x0064,"",eReadWrite_16,}, //70000B04//AFIT16_postdmsc_iLowBright                                                              
	{0x0f12, 0x0384,"",eReadWrite_16,}, //70000B06//AFIT16_postdmsc_iHighBright                                                             
	{0x0f12, 0x0043,"",eReadWrite_16,}, //70000B08//AFIT16_postdmsc_iLowSat                                                                 
	{0x0f12, 0x01f4,"",eReadWrite_16,}, //70000B0A//AFIT16_postdmsc_iHighSat                                                                
	{0x0f12, 0x0070,"",eReadWrite_16,}, //70000B0C//AFIT16_postdmsc_iTune                                                                   
	{0x0f12, 0x0040,"",eReadWrite_16,}, //70000B0E//AFIT16_yuvemix_mNegRanges_0                                                             
	{0x0f12, 0x00a0,"",eReadWrite_16,}, //70000B10//AFIT16_yuvemix_mNegRanges_1                                                             
	{0x0f12, 0x0100,"",eReadWrite_16,}, //70000B12//AFIT16_yuvemix_mNegRanges_2                                                             
	{0x0f12, 0x0010,"",eReadWrite_16,}, //70000B14//AFIT16_yuvemix_mPosRanges_0                                                             
	{0x0f12, 0x0060,"",eReadWrite_16,}, //70000B16//AFIT16_yuvemix_mPosRanges_1                                                             
	{0x0f12, 0x0100,"",eReadWrite_16,}, //70000B18//AFIT16_yuvemix_mPosRanges_2                                                             
	{0x0f12, 0x1430,"",eReadWrite_16,}, //70000B1A//AFIT8_bnr_edge_low  [7:0] AFIT8_bnr_repl_thresh                                         
	{0x0f12, 0x0201,"",eReadWrite_16,}, //70000B1C//AFIT8_bnr_repl_force  [7:0] AFIT8_bnr_iHotThreshHigh                                    
	{0x0f12, 0x0204,"",eReadWrite_16,}, //70000B1E//AFIT8_bnr_iHotThreshLow   [7:0] AFIT8_bnr_iColdThreshHigh                               
	{0x0f12, 0x1b04,"",eReadWrite_16,}, //70000B20//AFIT8_bnr_iColdThreshLow   [7:0] AFIT8_bnr_DispTH_Low                                   
	{0x0f12, 0x0312,"",eReadWrite_16,}, //70000B22//AFIT8_bnr_DispTH_High   [7:0] AFIT8_bnr_DISP_Limit_Low                                  
	{0x0f12, 0x0003,"",eReadWrite_16,}, //70000B24//AFIT8_bnr_DISP_Limit_High   [7:0] AFIT8_bnr_iDistSigmaMin                               
	{0x0f12, 0x0c03,"",eReadWrite_16,}, //70000B26//AFIT8_bnr_iDistSigmaMax   [7:0] AFIT8_bnr_iDiffSigmaLow                                 
	{0x0f12, 0x2806,"",eReadWrite_16,}, //70000B28//AFIT8_bnr_iDiffSigmaHigh   [7:0] AFIT8_bnr_iNormalizedSTD_TH                            
	{0x0f12, 0x0060,"",eReadWrite_16,}, //70000B2A//AFIT8_bnr_iNormalizedSTD_Limit [7:0] AFIT8_bnr_iDirNRTune                               
	{0x0f12, 0x1580,"",eReadWrite_16,}, //70000B2C//AFIT8_bnr_iDirMinThres [7:0] AFIT8_bnr_iDirFltDiffThresHigh                             
	{0x0f12, 0x2020,"",eReadWrite_16,}, //70000B2E//AFIT8_bnr_iDirFltDiffThresLow   [7:0] AFIT8_bnr_iDirSmoothPowerHigh                     
	{0x0f12, 0x0620,"",eReadWrite_16,}, //70000B30//AFIT8_bnr_iDirSmoothPowerLow   [7:0] AFIT8_bnr_iLowMaxSlopeAllowed                      
	{0x0f12, 0x0306,"",eReadWrite_16,}, //70000B32//AFIT8_bnr_iHighMaxSlopeAllowed [7:0] AFIT8_bnr_iLowSlopeThresh                          
	{0x0f12, 0x2003,"",eReadWrite_16,}, //70000B34//AFIT8_bnr_iHighSlopeThresh [7:0] AFIT8_bnr_iSlopenessTH                                 
	{0x0f12, 0xff01,"",eReadWrite_16,}, //70000B36//AFIT8_bnr_iSlopeBlurStrength   [7:0] AFIT8_bnr_iSlopenessLimit                          
	{0x0f12, 0x0404,"",eReadWrite_16,}, //70000B38//AFIT8_bnr_AddNoisePower1   [7:0] AFIT8_bnr_AddNoisePower2                               
	{0x0f12, 0x0300,"",eReadWrite_16,}, //70000B3A//AFIT8_bnr_iRadialTune   [7:0] AFIT8_bnr_iRadialPower                                    
	{0x0f12, 0x145a,"",eReadWrite_16,}, //70000B3C//AFIT8_bnr_iRadialLimit [7:0] AFIT8_ee_iFSMagThLow                                       
	{0x0f12, 0x1010,"",eReadWrite_16,}, //70000B3E//AFIT8_ee_iFSMagThHigh   [7:0] AFIT8_ee_iFSVarThLow                                      
	{0x0f12, 0x000b,"",eReadWrite_16,}, //70000B40//AFIT8_ee_iFSVarThHigh   [7:0] AFIT8_ee_iFSThLow                                         
	{0x0f12, 0x0e00,"",eReadWrite_16,}, //70000B42//AFIT8_ee_iFSThHigh [7:0] AFIT8_ee_iFSmagPower                                           
	{0x0f12, 0x5a0f,"",eReadWrite_16,}, //70000B44//AFIT8_ee_iFSVarCountTh [7:0] AFIT8_ee_iRadialLimit                                      
	{0x0f12, 0x0504,"",eReadWrite_16,}, //70000B46//AFIT8_ee_iRadialPower   [7:0] AFIT8_ee_iSmoothEdgeSlope                                 
	{0x0f12, 0x1802,"",eReadWrite_16,}, //70000B48//AFIT8_ee_iROADThres   [7:0] AFIT8_ee_iROADMaxNR                                         
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000B4A//AFIT8_ee_iROADSubMaxNR [7:0] AFIT8_ee_iROADSubThres                                     
	{0x0f12, 0x2006,"",eReadWrite_16,}, //70000B4C//AFIT8_ee_iROADNeiThres [7:0] AFIT8_ee_iROADNeiMaxNR                                     
	{0x0f12, 0x3828,"",eReadWrite_16,}, //70000B4E//AFIT8_ee_iSmoothEdgeThres   [7:0] AFIT8_ee_iMSharpen                                    
	{0x0f12, 0x0478,"",eReadWrite_16,}, //70000B50//AFIT8_ee_iWSharpen [7:0] AFIT8_ee_iMShThresh  0x0428                                          
	{0x0f12, 0x0101,"",eReadWrite_16,}, //70000B52//AFIT8_ee_iWShThresh   [7:0] AFIT8_ee_iReduceNegative                                    
	{0x0f12, 0x8000,"",eReadWrite_16,}, //70000B54//AFIT8_ee_iEmbossCentAdd   [7:0] AFIT8_ee_iShDespeckle                                   
	{0x0f12, 0x0a04,"",eReadWrite_16,}, //70000B56//AFIT8_ee_iReduceEdgeThresh [7:0] AFIT8_dmsc_iEnhThresh                                  
	{0x0f12, 0x4008,"",eReadWrite_16,}, //70000B58//AFIT8_dmsc_iDesatThresh   [7:0] AFIT8_dmsc_iDemBlurHigh                                 
	{0x0f12, 0x0540,"",eReadWrite_16,}, //70000B5A//AFIT8_dmsc_iDemBlurLow [7:0] AFIT8_dmsc_iDemBlurRange                                   
	{0x0f12, 0x8006,"",eReadWrite_16,}, //70000B5C//AFIT8_dmsc_iDecisionThresh [7:0] AFIT8_dmsc_iCentGrad                                   
	{0x0f12, 0x0020,"",eReadWrite_16,}, //70000B5E//AFIT8_dmsc_iMonochrom   [7:0] AFIT8_dmsc_iGBDenoiseVal                                  
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000B60//AFIT8_dmsc_iGRDenoiseVal   [7:0] AFIT8_dmsc_iEdgeDesatThrHigh                           
	{0x0f12, 0x1800,"",eReadWrite_16,}, //70000B62//AFIT8_dmsc_iEdgeDesatThrLow   [7:0] AFIT8_dmsc_iEdgeDesat                               
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000B64//AFIT8_dmsc_iNearGrayDesat   [7:0] AFIT8_dmsc_iEdgeDesatLimit                            
	{0x0f12, 0x1e10,"",eReadWrite_16,}, //70000B66//AFIT8_postdmsc_iBCoeff [7:0] AFIT8_postdmsc_iGCoeff                                     
	{0x0f12, 0x000b,"",eReadWrite_16,}, //70000B68//AFIT8_postdmsc_iWideMult   [7:0] AFIT8_yuvemix_mNegSlopes_0                             
	{0x0f12, 0x0607,"",eReadWrite_16,}, //70000B6A//AFIT8_yuvemix_mNegSlopes_1 [7:0] AFIT8_yuvemix_mNegSlopes_2                             
	{0x0f12, 0x0005,"",eReadWrite_16,}, //70000B6C//AFIT8_yuvemix_mNegSlopes_3 [7:0] AFIT8_yuvemix_mPosSlopes_0                             
	{0x0f12, 0x0607,"",eReadWrite_16,}, //70000B6E//AFIT8_yuvemix_mPosSlopes_1 [7:0] AFIT8_yuvemix_mPosSlopes_2                             
	{0x0f12, 0x0405,"",eReadWrite_16,}, //70000B70//AFIT8_yuvemix_mPosSlopes_3 [7:0] AFIT8_yuviirnr_iXSupportY                              
	{0x0f12, 0x0207,"",eReadWrite_16,}, //70000B72//AFIT8_yuviirnr_iXSupportUV [7:0] AFIT8_yuviirnr_iLowYNorm                               
	{0x0f12, 0x0304,"",eReadWrite_16,}, //70000B74//AFIT8_yuviirnr_iHighYNorm   [7:0] AFIT8_yuviirnr_iLowUVNorm                             
	{0x0f12, 0x0409,"",eReadWrite_16,}, //70000B76//AFIT8_yuviirnr_iHighUVNorm [7:0] AFIT8_yuviirnr_iYNormShift                             
	{0x0f12, 0x0306,"",eReadWrite_16,}, //70000B78//AFIT8_yuviirnr_iUVNormShift   [7:0] AFIT8_yuviirnr_iVertLength_Y                        
	{0x0f12, 0x0407,"",eReadWrite_16,}, //70000B7A//AFIT8_yuviirnr_iVertLength_UV   [7:0] AFIT8_yuviirnr_iDiffThreshL_Y                     
	{0x0f12, 0x2404,"",eReadWrite_16,}, //70000B7C//AFIT8_yuviirnr_iDiffThreshH_Y   [7:0] AFIT8_yuviirnr_iDiffThreshL_UV                    
	{0x0f12, 0x0221,"",eReadWrite_16,}, //70000B7E//AFIT8_yuviirnr_iDiffThreshH_UV [7:0] AFIT8_yuviirnr_iMaxThreshL_Y                       
	{0x0f12, 0x1202,"",eReadWrite_16,}, //70000B80//AFIT8_yuviirnr_iMaxThreshH_Y   [7:0] AFIT8_yuviirnr_iMaxThreshL_UV                      
	{0x0f12, 0x0613,"",eReadWrite_16,}, //70000B82//AFIT8_yuviirnr_iMaxThreshH_UV   [7:0] AFIT8_yuviirnr_iYNRStrengthL                      
	{0x0f12, 0x1a02,"",eReadWrite_16,}, //70000B84//AFIT8_yuviirnr_iYNRStrengthH   [7:0] AFIT8_yuviirnr_iUVNRStrengthL                      
	{0x0f12, 0x8018,"",eReadWrite_16,}, //70000B86//AFIT8_yuviirnr_iUVNRStrengthH   [7:0] AFIT8_byr_gras_iShadingPower                      
	{0x0f12, 0x0080,"",eReadWrite_16,}, //70000B88//AFIT8_RGBGamma2_iLinearity [7:0] AFIT8_RGBGamma2_iDarkReduce                            
	{0x0f12, 0x0080,"",eReadWrite_16,}, //70000B8A//AFIT8_ccm_oscar_iSaturation   [7:0] AFIT8_RGB2YUV_iYOffset                              
	{0x0f12, 0x0180,"",eReadWrite_16,}, //70000B8C//AFIT8_RGB2YUV_iRGBGain [7:0] AFIT8_bnr_nClustLevel_H                                    
	{0x0f12, 0x0a0a,"",eReadWrite_16,}, //70000B8E//AFIT8_bnr_iClustMulT_H [7:0] AFIT8_bnr_iClustMulT_C                                     
	{0x0f12, 0x0101,"",eReadWrite_16,}, //70000B90//AFIT8_bnr_iClustThresh_H   [7:0] AFIT8_bnr_iClustThresh_C                               
	{0x0f12, 0x0404,"",eReadWrite_16,}, //70000B92//AFIT8_bnr_iDenThreshLow   [7:0] AFIT8_bnr_iDenThreshHigh  0x141d                              
	{0x0f12, 0xc090,"",eReadWrite_16,}, //70000B94//AFIT8_ee_iLowSharpPower   [7:0] AFIT8_ee_iHighSharpPower                                
	{0x0f12, 0x0c0c,"",eReadWrite_16,}, //70000B96//AFIT8_ee_iLowShDenoise [7:0] AFIT8_ee_iHighShDenoise                                    
	{0x0f12, 0xffff,"",eReadWrite_16,}, //70000B98//AFIT8_ee_iLowSharpClamp   [7:0] AFIT8_ee_iHighSharpClamp                                
	{0x0f12, 0x0808,"",eReadWrite_16,}, //70000B9A//AFIT8_ee_iReduceEdgeMinMult   [7:0] AFIT8_ee_iReduceEdgeSlope                           
	{0x0f12, 0x0a01,"",eReadWrite_16,}, //70000B9C//AFIT8_bnr_nClustLevel_H_Bin   [7:0] AFIT8_bnr_iClustMulT_H_Bin                          
	{0x0f12, 0x010a,"",eReadWrite_16,}, //70000B9E//AFIT8_bnr_iClustMulT_C_Bin [7:0] AFIT8_bnr_iClustThresh_H_Bin                           
	{0x0f12, 0x1b01,"",eReadWrite_16,}, //70000BA0//AFIT8_bnr_iClustThresh_C_Bin   [7:0] AFIT8_bnr_iDenThreshLow_Bin                        
	{0x0f12, 0x2412,"",eReadWrite_16,}, //70000BA2//AFIT8_bnr_iDenThreshHigh_Bin   [7:0] AFIT8_ee_iLowSharpPower_Bin                        
	{0x0f12, 0x0c60,"",eReadWrite_16,}, //70000BA4//AFIT8_ee_iHighSharpPower_Bin   [7:0] AFIT8_ee_iLowShDenoise_Bin                         
	{0x0f12, 0xff0c,"",eReadWrite_16,}, //70000BA6//AFIT8_ee_iHighShDenoise_Bin   [7:0] AFIT8_ee_iLowSharpClamp_Bin                         
	{0x0f12, 0x08ff,"",eReadWrite_16,}, //70000BA8//AFIT8_ee_iHighSharpClamp_Bin   [7:0] AFIT8_ee_iReduceEdgeMinMult_Bin                    
	{0x0f12, 0x0008,"",eReadWrite_16,}, //70000BAA//AFIT8_ee_iReduceEdgeSlope_Bin [7:0]                                                     
	{0x0f12, 0x0001,"",eReadWrite_16,}, //70000BAC//AFITB_bnr_nClustLevel_C    [0]                                                          
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000BAE//AFIT16_BRIGHTNESS                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000BB0//AFIT16_CONTRAST                                                                         
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000BB2//AFIT16_SATURATION                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000BB4//AFIT16_SHARP_BLUR                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000BB6//AFIT16_GLAMOUR                                                                          
	{0x0f12, 0x00c0,"",eReadWrite_16,}, //70000BB8//AFIT16_bnr_edge_high                                                                    
	{0x0f12, 0x0064,"",eReadWrite_16,}, //70000BBA//AFIT16_postdmsc_iLowBright                                                              
	{0x0f12, 0x0384,"",eReadWrite_16,}, //70000BBC//AFIT16_postdmsc_iHighBright                                                             
	{0x0f12, 0x0032,"",eReadWrite_16,}, //70000BBE//AFIT16_postdmsc_iLowSat                                                                 
	{0x0f12, 0x01f4,"",eReadWrite_16,}, //70000BC0//AFIT16_postdmsc_iHighSat                                                                
	{0x0f12, 0x0070,"",eReadWrite_16,}, //70000BC2//AFIT16_postdmsc_iTune                                                                   
	{0x0f12, 0x0040,"",eReadWrite_16,}, //70000BC4//AFIT16_yuvemix_mNegRanges_0                                                             
	{0x0f12, 0x00a0,"",eReadWrite_16,}, //70000BC6//AFIT16_yuvemix_mNegRanges_1                                                             
	{0x0f12, 0x0100,"",eReadWrite_16,}, //70000BC8//AFIT16_yuvemix_mNegRanges_2                                                             
	{0x0f12, 0x0010,"",eReadWrite_16,}, //70000BCA//AFIT16_yuvemix_mPosRanges_0                                                             
	{0x0f12, 0x0060,"",eReadWrite_16,}, //70000BCC//AFIT16_yuvemix_mPosRanges_1                                                             
	{0x0f12, 0x0100,"",eReadWrite_16,}, //70000BCE//AFIT16_yuvemix_mPosRanges_2                                                             
	{0x0f12, 0x1430,"",eReadWrite_16,}, //70000BD0//AFIT8_bnr_edge_low  [7:0] AFIT8_bnr_repl_thresh                                         
	{0x0f12, 0x0201,"",eReadWrite_16,}, //70000BD2//AFIT8_bnr_repl_force  [7:0] AFIT8_bnr_iHotThreshHigh                                    
	{0x0f12, 0x0204,"",eReadWrite_16,}, //70000BD4//AFIT8_bnr_iHotThreshLow   [7:0] AFIT8_bnr_iColdThreshHigh                               
	{0x0f12, 0x1504,"",eReadWrite_16,}, //70000BD6//AFIT8_bnr_iColdThreshLow   [7:0] AFIT8_bnr_DispTH_Low                                   
	{0x0f12, 0x030f,"",eReadWrite_16,}, //70000BD8//AFIT8_bnr_DispTH_High   [7:0] AFIT8_bnr_DISP_Limit_Low                                  
	{0x0f12, 0x0003,"",eReadWrite_16,}, //70000BDA//AFIT8_bnr_DISP_Limit_High   [7:0] AFIT8_bnr_iDistSigmaMin                               
	{0x0f12, 0x0902,"",eReadWrite_16,}, //70000BDC//AFIT8_bnr_iDistSigmaMax   [7:0] AFIT8_bnr_iDiffSigmaLow                                 
	{0x0f12, 0x2004,"",eReadWrite_16,}, //70000BDE//AFIT8_bnr_iDiffSigmaHigh   [7:0] AFIT8_bnr_iNormalizedSTD_TH                            
	{0x0f12, 0x0050,"",eReadWrite_16,}, //70000BE0//AFIT8_bnr_iNormalizedSTD_Limit [7:0] AFIT8_bnr_iDirNRTune                               
	{0x0f12, 0x1140,"",eReadWrite_16,}, //70000BE2//AFIT8_bnr_iDirMinThres [7:0] AFIT8_bnr_iDirFltDiffThresHigh                             
	{0x0f12, 0x201c,"",eReadWrite_16,}, //70000BE4//AFIT8_bnr_iDirFltDiffThresLow   [7:0] AFIT8_bnr_iDirSmoothPowerHigh                     
	{0x0f12, 0x0620,"",eReadWrite_16,}, //70000BE6//AFIT8_bnr_iDirSmoothPowerLow   [7:0] AFIT8_bnr_iLowMaxSlopeAllowed                      
	{0x0f12, 0x0306,"",eReadWrite_16,}, //70000BE8//AFIT8_bnr_iHighMaxSlopeAllowed [7:0] AFIT8_bnr_iLowSlopeThresh                          
	{0x0f12, 0x2003,"",eReadWrite_16,}, //70000BEA//AFIT8_bnr_iHighSlopeThresh [7:0] AFIT8_bnr_iSlopenessTH                                 
	{0x0f12, 0xff01,"",eReadWrite_16,}, //70000BEC//AFIT8_bnr_iSlopeBlurStrength   [7:0] AFIT8_bnr_iSlopenessLimit                          
	{0x0f12, 0x0404,"",eReadWrite_16,}, //70000BEE//AFIT8_bnr_AddNoisePower1   [7:0] AFIT8_bnr_AddNoisePower2                               
	{0x0f12, 0x0300,"",eReadWrite_16,}, //70000BF0//AFIT8_bnr_iRadialTune   [7:0] AFIT8_bnr_iRadialPower                                    
	{0x0f12, 0x145a,"",eReadWrite_16,}, //70000BF2//AFIT8_bnr_iRadialLimit [7:0] AFIT8_ee_iFSMagThLow                                       
	{0x0f12, 0x1010,"",eReadWrite_16,}, //70000BF4//AFIT8_ee_iFSMagThHigh   [7:0] AFIT8_ee_iFSVarThLow                                      
	{0x0f12, 0x000b,"",eReadWrite_16,}, //70000BF6//AFIT8_ee_iFSVarThHigh   [7:0] AFIT8_ee_iFSThLow                                         
	{0x0f12, 0x1000,"",eReadWrite_16,}, //70000BF8//AFIT8_ee_iFSThHigh [7:0] AFIT8_ee_iFSmagPower                                           
	{0x0f12, 0x5a0f,"",eReadWrite_16,}, //70000BFA//AFIT8_ee_iFSVarCountTh [7:0] AFIT8_ee_iRadialLimit                                      
	{0x0f12, 0x0503,"",eReadWrite_16,}, //70000BFC//AFIT8_ee_iRadialPower   [7:0] AFIT8_ee_iSmoothEdgeSlope                                 
	{0x0f12, 0x1802,"",eReadWrite_16,}, //70000BFE//AFIT8_ee_iROADThres   [7:0] AFIT8_ee_iROADMaxNR                                         
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000C00//AFIT8_ee_iROADSubMaxNR [7:0] AFIT8_ee_iROADSubThres                                     
	{0x0f12, 0x2006,"",eReadWrite_16,}, //70000C02//AFIT8_ee_iROADNeiThres [7:0] AFIT8_ee_iROADNeiMaxNR                                     
	{0x0f12, 0x3c28,"",eReadWrite_16,}, //70000C04//AFIT8_ee_iSmoothEdgeThres   [7:0] AFIT8_ee_iMSharpen                                    
	{0x0f12, 0x047c,"",eReadWrite_16,}, //70000C06//AFIT8_ee_iWSharpen [7:0] AFIT8_ee_iMShThresh  0x042c                                          
	{0x0f12, 0x0101,"",eReadWrite_16,}, //70000C08//AFIT8_ee_iWShThresh   [7:0] AFIT8_ee_iReduceNegative                                    
	{0x0f12, 0xff00,"",eReadWrite_16,}, //70000C0A//AFIT8_ee_iEmbossCentAdd   [7:0] AFIT8_ee_iShDespeckle                                   
	{0x0f12, 0x0904,"",eReadWrite_16,}, //70000C0C//AFIT8_ee_iReduceEdgeThresh [7:0] AFIT8_dmsc_iEnhThresh                                  
	{0x0f12, 0x4008,"",eReadWrite_16,}, //70000C0E//AFIT8_dmsc_iDesatThresh   [7:0] AFIT8_dmsc_iDemBlurHigh                                 
	{0x0f12, 0x0540,"",eReadWrite_16,}, //70000C10//AFIT8_dmsc_iDemBlurLow [7:0] AFIT8_dmsc_iDemBlurRange                                   
	{0x0f12, 0x8006,"",eReadWrite_16,}, //70000C12//AFIT8_dmsc_iDecisionThresh [7:0] AFIT8_dmsc_iCentGrad                                   
	{0x0f12, 0x0020,"",eReadWrite_16,}, //70000C14//AFIT8_dmsc_iMonochrom   [7:0] AFIT8_dmsc_iGBDenoiseVal                                  
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000C16//AFIT8_dmsc_iGRDenoiseVal   [7:0] AFIT8_dmsc_iEdgeDesatThrHigh                           
	{0x0f12, 0x1800,"",eReadWrite_16,}, //70000C18//AFIT8_dmsc_iEdgeDesatThrLow   [7:0] AFIT8_dmsc_iEdgeDesat                               
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000C1A//AFIT8_dmsc_iNearGrayDesat   [7:0] AFIT8_dmsc_iEdgeDesatLimit                            
	{0x0f12, 0x1e10,"",eReadWrite_16,}, //70000C1C//AFIT8_postdmsc_iBCoeff [7:0] AFIT8_postdmsc_iGCoeff                                     
	{0x0f12, 0x000b,"",eReadWrite_16,}, //70000C1E//AFIT8_postdmsc_iWideMult   [7:0] AFIT8_yuvemix_mNegSlopes_0                             
	{0x0f12, 0x0607,"",eReadWrite_16,}, //70000C20//AFIT8_yuvemix_mNegSlopes_1 [7:0] AFIT8_yuvemix_mNegSlopes_2                             
	{0x0f12, 0x0005,"",eReadWrite_16,}, //70000C22//AFIT8_yuvemix_mNegSlopes_3 [7:0] AFIT8_yuvemix_mPosSlopes_0                             
	{0x0f12, 0x0607,"",eReadWrite_16,}, //70000C24//AFIT8_yuvemix_mPosSlopes_1 [7:0] AFIT8_yuvemix_mPosSlopes_2                             
	{0x0f12, 0x0405,"",eReadWrite_16,}, //70000C26//AFIT8_yuvemix_mPosSlopes_3 [7:0] AFIT8_yuviirnr_iXSupportY                              
	{0x0f12, 0x0206,"",eReadWrite_16,}, //70000C28//AFIT8_yuviirnr_iXSupportUV [7:0] AFIT8_yuviirnr_iLowYNorm                               
	{0x0f12, 0x0304,"",eReadWrite_16,}, //70000C2A//AFIT8_yuviirnr_iHighYNorm   [7:0] AFIT8_yuviirnr_iLowUVNorm                             
	{0x0f12, 0x0409,"",eReadWrite_16,}, //70000C2C//AFIT8_yuviirnr_iHighUVNorm [7:0] AFIT8_yuviirnr_iYNormShift                             
	{0x0f12, 0x0305,"",eReadWrite_16,}, //70000C2E//AFIT8_yuviirnr_iUVNormShift   [7:0] AFIT8_yuviirnr_iVertLength_Y                        
	{0x0f12, 0x0406,"",eReadWrite_16,}, //70000C30//AFIT8_yuviirnr_iVertLength_UV   [7:0] AFIT8_yuviirnr_iDiffThreshL_Y                     
	{0x0f12, 0x2804,"",eReadWrite_16,}, //70000C32//AFIT8_yuviirnr_iDiffThreshH_Y   [7:0] AFIT8_yuviirnr_iDiffThreshL_UV                    
	{0x0f12, 0x0228,"",eReadWrite_16,}, //70000C34//AFIT8_yuviirnr_iDiffThreshH_UV [7:0] AFIT8_yuviirnr_iMaxThreshL_Y                       
	{0x0f12, 0x1402,"",eReadWrite_16,}, //70000C36//AFIT8_yuviirnr_iMaxThreshH_Y   [7:0] AFIT8_yuviirnr_iMaxThreshL_UV                      
	{0x0f12, 0x0618,"",eReadWrite_16,}, //70000C38//AFIT8_yuviirnr_iMaxThreshH_UV   [7:0] AFIT8_yuviirnr_iYNRStrengthL                      
	{0x0f12, 0x1a02,"",eReadWrite_16,}, //70000C3A//AFIT8_yuviirnr_iYNRStrengthH   [7:0] AFIT8_yuviirnr_iUVNRStrengthL                      
	{0x0f12, 0x8018,"",eReadWrite_16,}, //70000C3C//AFIT8_yuviirnr_iUVNRStrengthH   [7:0] AFIT8_byr_gras_iShadingPower                      
	{0x0f12, 0x0080,"",eReadWrite_16,}, //70000C3E//AFIT8_RGBGamma2_iLinearity [7:0] AFIT8_RGBGamma2_iDarkReduce                            
	{0x0f12, 0x0080,"",eReadWrite_16,}, //70000C40//AFIT8_ccm_oscar_iSaturation   [7:0] AFIT8_RGB2YUV_iYOffset                              
	{0x0f12, 0x0180,"",eReadWrite_16,}, //70000C42//AFIT8_RGB2YUV_iRGBGain [7:0] AFIT8_bnr_nClustLevel_H                                    
	{0x0f12, 0x0a0a,"",eReadWrite_16,}, //70000C44//AFIT8_bnr_iClustMulT_H [7:0] AFIT8_bnr_iClustMulT_C                                     
	{0x0f12, 0x0101,"",eReadWrite_16,}, //70000C46//AFIT8_bnr_iClustThresh_H   [7:0] AFIT8_bnr_iClustThresh_C                               
	{0x0f12, 0x0404,"",eReadWrite_16,}, //70000C48//AFIT8_bnr_iDenThreshLow   [7:0] AFIT8_bnr_iDenThreshHigh 0x1117                               
	{0x0f12, 0xE0a0,"",eReadWrite_16,}, //70000C4A//AFIT8_ee_iLowSharpPower   [7:0] AFIT8_ee_iHighSharpPower                                
	{0x0f12, 0x0a0a,"",eReadWrite_16,}, //70000C4C//AFIT8_ee_iLowShDenoise [7:0] AFIT8_ee_iHighShDenoise                                    
	{0x0f12, 0xffff,"",eReadWrite_16,}, //70000C4E//AFIT8_ee_iLowSharpClamp   [7:0] AFIT8_ee_iHighSharpClamp                                
	{0x0f12, 0x0808,"",eReadWrite_16,}, //70000C50//AFIT8_ee_iReduceEdgeMinMult   [7:0] AFIT8_ee_iReduceEdgeSlope                           
	{0x0f12, 0x0a01,"",eReadWrite_16,}, //70000C52//AFIT8_bnr_nClustLevel_H_Bin   [7:0] AFIT8_bnr_iClustMulT_H_Bin                          
	{0x0f12, 0x010a,"",eReadWrite_16,}, //70000C54//AFIT8_bnr_iClustMulT_C_Bin [7:0] AFIT8_bnr_iClustThresh_H_Bin                           
	{0x0f12, 0x1501,"",eReadWrite_16,}, //70000C56//AFIT8_bnr_iClustThresh_C_Bin   [7:0] AFIT8_bnr_iDenThreshLow_Bin                        
	{0x0f12, 0x240f,"",eReadWrite_16,}, //70000C58//AFIT8_bnr_iDenThreshHigh_Bin   [7:0] AFIT8_ee_iLowSharpPower_Bin                        
	{0x0f12, 0x0a60,"",eReadWrite_16,}, //70000C5A//AFIT8_ee_iHighSharpPower_Bin   [7:0] AFIT8_ee_iLowShDenoise_Bin                         
	{0x0f12, 0xff0a,"",eReadWrite_16,}, //70000C5C//AFIT8_ee_iHighShDenoise_Bin   [7:0] AFIT8_ee_iLowSharpClamp_Bin                         
	{0x0f12, 0x08ff,"",eReadWrite_16,}, //70000C5E//AFIT8_ee_iHighSharpClamp_Bin   [7:0] AFIT8_ee_iReduceEdgeMinMult_Bin                    
	{0x0f12, 0x0008,"",eReadWrite_16,}, //70000C60//AFIT8_ee_iReduceEdgeSlope_Bin [7:0]                                                     
	{0x0f12, 0x0001,"",eReadWrite_16,}, //70000C62//AFITB_bnr_nClustLevel_C    [0]                                                          
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000C64//AFIT16_BRIGHTNESS                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000C66//AFIT16_CONTRAST                                                                         
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000C68//AFIT16_SATURATION                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000C6A//AFIT16_SHARP_BLUR                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000C6C//AFIT16_GLAMOUR                                                                          
	{0x0f12, 0x00c0,"",eReadWrite_16,}, //70000C6E//AFIT16_bnr_edge_high                                                                    
	{0x0f12, 0x0064,"",eReadWrite_16,}, //70000C70//AFIT16_postdmsc_iLowBright                                                              
	{0x0f12, 0x0384,"",eReadWrite_16,}, //70000C72//AFIT16_postdmsc_iHighBright                                                             
	{0x0f12, 0x0032,"",eReadWrite_16,}, //70000C74//AFIT16_postdmsc_iLowSat                                                                 
	{0x0f12, 0x01f4,"",eReadWrite_16,}, //70000C76//AFIT16_postdmsc_iHighSat                                                                
	{0x0f12, 0x0070,"",eReadWrite_16,}, //70000C78//AFIT16_postdmsc_iTune                                                                   
	{0x0f12, 0x0040,"",eReadWrite_16,}, //70000C7A//AFIT16_yuvemix_mNegRanges_0                                                             
	{0x0f12, 0x00a0,"",eReadWrite_16,}, //70000C7C//AFIT16_yuvemix_mNegRanges_1                                                             
	{0x0f12, 0x0100,"",eReadWrite_16,}, //70000C7E//AFIT16_yuvemix_mNegRanges_2                                                             
	{0x0f12, 0x0010,"",eReadWrite_16,}, //70000C80//AFIT16_yuvemix_mPosRanges_0                                                             
	{0x0f12, 0x0060,"",eReadWrite_16,}, //70000C82//AFIT16_yuvemix_mPosRanges_1                                                             
	{0x0f12, 0x0100,"",eReadWrite_16,}, //70000C84//AFIT16_yuvemix_mPosRanges_2                                                             
	{0x0f12, 0x1430,"",eReadWrite_16,}, //70000C86//AFIT8_bnr_edge_low  [7:0] AFIT8_bnr_repl_thresh                                         
	{0x0f12, 0x0201,"",eReadWrite_16,}, //70000C88//AFIT8_bnr_repl_force  [7:0] AFIT8_bnr_iHotThreshHigh                                    
	{0x0f12, 0x0204,"",eReadWrite_16,}, //70000C8A//AFIT8_bnr_iHotThreshLow   [7:0] AFIT8_bnr_iColdThreshHigh                               
	{0x0f12, 0x0f04,"",eReadWrite_16,}, //70000C8C//AFIT8_bnr_iColdThreshLow   [7:0] AFIT8_bnr_DispTH_Low                                   
	{0x0f12, 0x030c,"",eReadWrite_16,}, //70000C8E//AFIT8_bnr_DispTH_High   [7:0] AFIT8_bnr_DISP_Limit_Low                                  
	{0x0f12, 0x0003,"",eReadWrite_16,}, //70000C90//AFIT8_bnr_DISP_Limit_High   [7:0] AFIT8_bnr_iDistSigmaMin                               
	{0x0f12, 0x0602,"",eReadWrite_16,}, //70000C92//AFIT8_bnr_iDistSigmaMax   [7:0] AFIT8_bnr_iDiffSigmaLow                                 
	{0x0f12, 0x1803,"",eReadWrite_16,}, //70000C94//AFIT8_bnr_iDiffSigmaHigh   [7:0] AFIT8_bnr_iNormalizedSTD_TH                            
	{0x0f12, 0x0040,"",eReadWrite_16,}, //70000C96//AFIT8_bnr_iNormalizedSTD_Limit [7:0] AFIT8_bnr_iDirNRTune                               
	{0x0f12, 0x0e20,"",eReadWrite_16,}, //70000C98//AFIT8_bnr_iDirMinThres [7:0] AFIT8_bnr_iDirFltDiffThresHigh                             
	{0x0f12, 0x2018,"",eReadWrite_16,}, //70000C9A//AFIT8_bnr_iDirFltDiffThresLow   [7:0] AFIT8_bnr_iDirSmoothPowerHigh                     
	{0x0f12, 0x0620,"",eReadWrite_16,}, //70000C9C//AFIT8_bnr_iDirSmoothPowerLow   [7:0] AFIT8_bnr_iLowMaxSlopeAllowed                      
	{0x0f12, 0x0306,"",eReadWrite_16,}, //70000C9E//AFIT8_bnr_iHighMaxSlopeAllowed [7:0] AFIT8_bnr_iLowSlopeThresh                          
	{0x0f12, 0x2003,"",eReadWrite_16,}, //70000CA0//AFIT8_bnr_iHighSlopeThresh [7:0] AFIT8_bnr_iSlopenessTH                                 
	{0x0f12, 0xff01,"",eReadWrite_16,}, //70000CA2//AFIT8_bnr_iSlopeBlurStrength   [7:0] AFIT8_bnr_iSlopenessLimit                          
	{0x0f12, 0x0404,"",eReadWrite_16,}, //70000CA4//AFIT8_bnr_AddNoisePower1   [7:0] AFIT8_bnr_AddNoisePower2                               
	{0x0f12, 0x0200,"",eReadWrite_16,}, //70000CA6//AFIT8_bnr_iRadialTune   [7:0] AFIT8_bnr_iRadialPower                                    
	{0x0f12, 0x145a,"",eReadWrite_16,}, //70000CA8//AFIT8_bnr_iRadialLimit [7:0] AFIT8_ee_iFSMagThLow                                       
	{0x0f12, 0x1010,"",eReadWrite_16,}, //70000CAA//AFIT8_ee_iFSMagThHigh   [7:0] AFIT8_ee_iFSVarThLow                                      
	{0x0f12, 0x000b,"",eReadWrite_16,}, //70000CAC//AFIT8_ee_iFSVarThHigh   [7:0] AFIT8_ee_iFSThLow                                         
	{0x0f12, 0x1200,"",eReadWrite_16,}, //70000CAE//AFIT8_ee_iFSThHigh [7:0] AFIT8_ee_iFSmagPower                                           
	{0x0f12, 0x5a0f,"",eReadWrite_16,}, //70000CB0//AFIT8_ee_iFSVarCountTh [7:0] AFIT8_ee_iRadialLimit                                      
	{0x0f12, 0x0502,"",eReadWrite_16,}, //70000CB2//AFIT8_ee_iRadialPower   [7:0] AFIT8_ee_iSmoothEdgeSlope                                 
	{0x0f12, 0x1802,"",eReadWrite_16,}, //70000CB4//AFIT8_ee_iROADThres   [7:0] AFIT8_ee_iROADMaxNR                                         
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000CB6//AFIT8_ee_iROADSubMaxNR [7:0] AFIT8_ee_iROADSubThres                                     
	{0x0f12, 0x2006,"",eReadWrite_16,}, //70000CB8//AFIT8_ee_iROADNeiThres [7:0] AFIT8_ee_iROADNeiMaxNR                                     
	{0x0f12, 0x4028,"",eReadWrite_16,}, //70000CBA//AFIT8_ee_iSmoothEdgeThres   [7:0] AFIT8_ee_iMSharpen                                    
	{0x0f12, 0x0480,"",eReadWrite_16,}, //70000CBC//AFIT8_ee_iWSharpen [7:0] AFIT8_ee_iMShThresh 0x0430                                            
	{0x0f12, 0x0101,"",eReadWrite_16,}, //70000CBE//AFIT8_ee_iWShThresh   [7:0] AFIT8_ee_iReduceNegative                                    
	{0x0f12, 0xff00,"",eReadWrite_16,}, //70000CC0//AFIT8_ee_iEmbossCentAdd   [7:0] AFIT8_ee_iShDespeckle                                   
	{0x0f12, 0x0804,"",eReadWrite_16,}, //70000CC2//AFIT8_ee_iReduceEdgeThresh [7:0] AFIT8_dmsc_iEnhThresh                                  
	{0x0f12, 0x4008,"",eReadWrite_16,}, //70000CC4//AFIT8_dmsc_iDesatThresh   [7:0] AFIT8_dmsc_iDemBlurHigh                                 
	{0x0f12, 0x0540,"",eReadWrite_16,}, //70000CC6//AFIT8_dmsc_iDemBlurLow [7:0] AFIT8_dmsc_iDemBlurRange                                   
	{0x0f12, 0x8006,"",eReadWrite_16,}, //70000CC8//AFIT8_dmsc_iDecisionThresh [7:0] AFIT8_dmsc_iCentGrad                                   
	{0x0f12, 0x0020,"",eReadWrite_16,}, //70000CCA//AFIT8_dmsc_iMonochrom   [7:0] AFIT8_dmsc_iGBDenoiseVal                                  
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000CCC//AFIT8_dmsc_iGRDenoiseVal   [7:0] AFIT8_dmsc_iEdgeDesatThrHigh                           
	{0x0f12, 0x1800,"",eReadWrite_16,}, //70000CCE//AFIT8_dmsc_iEdgeDesatThrLow   [7:0] AFIT8_dmsc_iEdgeDesat                               
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000CD0//AFIT8_dmsc_iNearGrayDesat   [7:0] AFIT8_dmsc_iEdgeDesatLimit                            
	{0x0f12, 0x1e10,"",eReadWrite_16,}, //70000CD2//AFIT8_postdmsc_iBCoeff [7:0] AFIT8_postdmsc_iGCoeff                                     
	{0x0f12, 0x000b,"",eReadWrite_16,}, //70000CD4//AFIT8_postdmsc_iWideMult   [7:0] AFIT8_yuvemix_mNegSlopes_0                             
	{0x0f12, 0x0607,"",eReadWrite_16,}, //70000CD6//AFIT8_yuvemix_mNegSlopes_1 [7:0] AFIT8_yuvemix_mNegSlopes_2                             
	{0x0f12, 0x0005,"",eReadWrite_16,}, //70000CD8//AFIT8_yuvemix_mNegSlopes_3 [7:0] AFIT8_yuvemix_mPosSlopes_0                             
	{0x0f12, 0x0607,"",eReadWrite_16,}, //70000CDA//AFIT8_yuvemix_mPosSlopes_1 [7:0] AFIT8_yuvemix_mPosSlopes_2                             
	{0x0f12, 0x0405,"",eReadWrite_16,}, //70000CDC//AFIT8_yuvemix_mPosSlopes_3 [7:0] AFIT8_yuviirnr_iXSupportY                              
	{0x0f12, 0x0205,"",eReadWrite_16,}, //70000CDE//AFIT8_yuviirnr_iXSupportUV [7:0] AFIT8_yuviirnr_iLowYNorm                               
	{0x0f12, 0x0304,"",eReadWrite_16,}, //70000CE0//AFIT8_yuviirnr_iHighYNorm   [7:0] AFIT8_yuviirnr_iLowUVNorm                             
	{0x0f12, 0x0409,"",eReadWrite_16,}, //70000CE2//AFIT8_yuviirnr_iHighUVNorm [7:0] AFIT8_yuviirnr_iYNormShift                             
	{0x0f12, 0x0306,"",eReadWrite_16,}, //70000CE4//AFIT8_yuviirnr_iUVNormShift   [7:0] AFIT8_yuviirnr_iVertLength_Y                        
	{0x0f12, 0x0407,"",eReadWrite_16,}, //70000CE6//AFIT8_yuviirnr_iVertLength_UV   [7:0] AFIT8_yuviirnr_iDiffThreshL_Y                     
	{0x0f12, 0x2c04,"",eReadWrite_16,}, //70000CE8//AFIT8_yuviirnr_iDiffThreshH_Y   [7:0] AFIT8_yuviirnr_iDiffThreshL_UV                    
	{0x0f12, 0x022c,"",eReadWrite_16,}, //70000CEA//AFIT8_yuviirnr_iDiffThreshH_UV [7:0] AFIT8_yuviirnr_iMaxThreshL_Y                       
	{0x0f12, 0x1402,"",eReadWrite_16,}, //70000CEC//AFIT8_yuviirnr_iMaxThreshH_Y   [7:0] AFIT8_yuviirnr_iMaxThreshL_UV                      
	{0x0f12, 0x0618,"",eReadWrite_16,}, //70000CEE//AFIT8_yuviirnr_iMaxThreshH_UV   [7:0] AFIT8_yuviirnr_iYNRStrengthL                      
	{0x0f12, 0x1a02,"",eReadWrite_16,}, //70000CF0//AFIT8_yuviirnr_iYNRStrengthH   [7:0] AFIT8_yuviirnr_iUVNRStrengthL                      
	{0x0f12, 0x8018,"",eReadWrite_16,}, //70000CF2//AFIT8_yuviirnr_iUVNRStrengthH   [7:0] AFIT8_byr_gras_iShadingPower                      
	{0x0f12, 0x0080,"",eReadWrite_16,}, //70000CF4//AFIT8_RGBGamma2_iLinearity [7:0] AFIT8_RGBGamma2_iDarkReduce                            
	{0x0f12, 0x0080,"",eReadWrite_16,}, //70000CF6//AFIT8_ccm_oscar_iSaturation   [7:0] AFIT8_RGB2YUV_iYOffset                              
	{0x0f12, 0x0180,"",eReadWrite_16,}, //70000CF8//AFIT8_RGB2YUV_iRGBGain [7:0] AFIT8_bnr_nClustLevel_H                                    
	{0x0f12, 0x0a0a,"",eReadWrite_16,}, //70000CFA//AFIT8_bnr_iClustMulT_H [7:0] AFIT8_bnr_iClustMulT_C                                     
	{0x0f12, 0x0101,"",eReadWrite_16,}, //70000CFC//AFIT8_bnr_iClustThresh_H   [7:0] AFIT8_bnr_iClustThresh_C                               
	{0x0f12, 0x0404,"",eReadWrite_16,}, //70000CFE//AFIT8_bnr_iDenThreshLow   [7:0] AFIT8_bnr_iDenThreshHigh 0x0c0f                               
	{0x0f12, 0xE0a0,"",eReadWrite_16,}, //70000D00//AFIT8_ee_iLowSharpPower   [7:0] AFIT8_ee_iHighSharpPower                                
	{0x0f12, 0x0808,"",eReadWrite_16,}, //70000D02//AFIT8_ee_iLowShDenoise [7:0] AFIT8_ee_iHighShDenoise                                    
	{0x0f12, 0xffff,"",eReadWrite_16,}, //70000D04//AFIT8_ee_iLowSharpClamp   [7:0] AFIT8_ee_iHighSharpClamp                                
	{0x0f12, 0x0808,"",eReadWrite_16,}, //70000D06//AFIT8_ee_iReduceEdgeMinMult   [7:0] AFIT8_ee_iReduceEdgeSlope                           
	{0x0f12, 0x0a01,"",eReadWrite_16,}, //70000D08//AFIT8_bnr_nClustLevel_H_Bin   [7:0] AFIT8_bnr_iClustMulT_H_Bin                          
	{0x0f12, 0x010a,"",eReadWrite_16,}, //70000D0A//AFIT8_bnr_iClustMulT_C_Bin [7:0] AFIT8_bnr_iClustThresh_H_Bin                           
	{0x0f12, 0x0f01,"",eReadWrite_16,}, //70000D0C//AFIT8_bnr_iClustThresh_C_Bin   [7:0] AFIT8_bnr_iDenThreshLow_Bin                        
	{0x0f12, 0x240c,"",eReadWrite_16,}, //70000D0E//AFIT8_bnr_iDenThreshHigh_Bin   [7:0] AFIT8_ee_iLowSharpPower_Bin                        
	{0x0f12, 0x0860,"",eReadWrite_16,}, //70000D10//AFIT8_ee_iHighSharpPower_Bin   [7:0] AFIT8_ee_iLowShDenoise_Bin                         
	{0x0f12, 0xff08,"",eReadWrite_16,}, //70000D12//AFIT8_ee_iHighShDenoise_Bin   [7:0] AFIT8_ee_iLowSharpClamp_Bin                         
	{0x0f12, 0x08ff,"",eReadWrite_16,}, //70000D14//AFIT8_ee_iHighSharpClamp_Bin   [7:0] AFIT8_ee_iReduceEdgeMinMult_Bin                    
	{0x0f12, 0x0008,"",eReadWrite_16,}, //70000D16//AFIT8_ee_iReduceEdgeSlope_Bin [7:0]                                                     
	{0x0f12, 0x0001,"",eReadWrite_16,},   //70000D18 AFITB_bnr_nClustLevel_C    [0]   bWideWide[1]                                          
	{0x0f12, 0x23ce,"",eReadWrite_16,}, //70000D19//ConstAfitBaseVals                                                                       
	{0x0f12, 0xfdc8,"",eReadWrite_16,}, //70000D1A//ConstAfitBaseVals                                                                       
	{0x0f12, 0x112e,"",eReadWrite_16,}, //70000D1B//ConstAfitBaseVals                                                                       
	{0x0f12, 0x93a5,"",eReadWrite_16,}, //70000D1C//ConstAfitBaseVals                                                                       
	{0x0f12, 0xfe67,"",eReadWrite_16,}, //70000D1D//ConstAfitBaseVals                                                                       
	{0x0f12, 0x0000,"",eReadWrite_16,}, //70000D1E//ConstAfitBaseVals   
	                                                                    
	{0x002a, 0x0250,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0010,"",eReadWrite_16,},
	{0x0f12, 0x000c,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0010,"",eReadWrite_16,},
	{0x0f12, 0x000c,"",eReadWrite_16,},
	{0x002a, 0x0494,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x0262,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x02a6,"",eReadWrite_16,},
	{0x0f12, 0x0320,"",eReadWrite_16,},//#REG_0TC_PCFG_usWidth//Hsize:800//        //PCFG                                   
	{0x0f12, 0x0258,"",eReadWrite_16,},//#REG_0TC_PCFG_usHeight//Vsize:600//       //output image width:800                 
	{0x0f12, 0x0005,"",eReadWrite_16,},//#REG_0TC_PCFG_Format//5:YUV7:Raw9:JPG//   //output image height:600                
	{0x0f12, 0x4f1a,"",eReadWrite_16,},//#REG_0TC_PCFG_usMaxOut4KHzRate//          //yuv                                    
	{0x0f12, 0x4f1a,"",eReadWrite_16,},//#REG_0TC_PCFG_usMinOut4KHzRate//                                                   
	{0x0f12, 0x0100,"",eReadWrite_16,},//#REG_0TC_PCFG_OutClkPerPix88//                                                     
	{0x0f12, 0x0300,"",eReadWrite_16,},//#REG_0TC_PCFG_uBpp88//                                                             
	{0x0f12, 0x0052,"",eReadWrite_16,},//#REG_0TC_PCFG_PVIMask//                                                            
	{0x0f12, 0x0000,"",eReadWrite_16,},//#REG_0TC_PCFG_OIFMask//                                                            
	{0x0f12, 0x01e0,"",eReadWrite_16,},//#REG_0TC_PCFG_usJpegPacketSize//                                                   
	{0x0f12, 0x0000,"",eReadWrite_16,},//#REG_0TC_PCFG_usJpegTotalPackets//                                                 
	{0x0f12, 0x0000,"",eReadWrite_16,},//#REG_0TC_PCFG_uClockInd//                                                          
	{0x0f12, 0x0000,"",eReadWrite_16,},//#REG_0TC_PCFG_usFrTimeType//                                                       
	{0x0f12, 0x0001,"",eReadWrite_16,},//#REG_0TC_PCFG_FrRateQualityType//    0x0001   0x0002                                           
	{0x0f12, 0x0535,"",eReadWrite_16,},//29A//#REG_0TC_PCFG_usMaxFrTimeMsecMult10//                                         
	{0x0f12, 0x014d,"",eReadWrite_16,},//14D//#REG_0TC_PCFG_usMinFrTimeMsecMult10//                                         
	{0x002a, 0x02d0,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	
	{0x002a, 0x0396,"",eReadWrite_16,},//CGFG
	{0x0f12, 0x0001,"",eReadWrite_16,},//#REG_0TC_CCFG_uCaptureMode//         
	{0x0f12, 0x0a00,"",eReadWrite_16,},//#REG_0TC_CCFG_usWidth//              //
	{0x0f12, 0x0780,"",eReadWrite_16,},//#REG_0TC_CCFG_usHeight//             //
	{0x0f12, 0x0005,"",eReadWrite_16,},//#REG_0TC_CCFG_Format//               //
	{0x0f12, 0x4f1a,"",eReadWrite_16,},//#REG_0TC_CCFG_usMaxOut4KHzRate//     
	{0x0f12, 0x4f1a,"",eReadWrite_16,},//#REG_0TC_CCFG_usMinOut4KHzRate//     
	{0x0f12, 0x0100,"",eReadWrite_16,},//#REG_0TC_CCFG_OutClkPerPix88//       
	{0x0f12, 0x0300,"",eReadWrite_16,},//#REG_0TC_CCFG_uBpp88//               
	{0x0f12, 0x0050,"",eReadWrite_16,},//#REG_0TC_CCFG_PVIMask//              
	{0x0f12, 0x0000,"",eReadWrite_16,},//#REG_0TC_CCFG_OIFMask//              
	{0x0f12, 0x01e0,"",eReadWrite_16,},//#REG_0TC_CCFG_usJpegPacketSize//     
	{0x0f12, 0x0000,"",eReadWrite_16,},//#REG_0TC_CCFG_usJpegTotalPackets//   
	{0x0f12, 0x0000,"",eReadWrite_16,},//#REG_0TC_CCFG_uClockInd//            
	{0x0f12, 0x0000,"",eReadWrite_16,},//#REG_0TC_CCFG_usFrTimeType//         
	{0x0f12, 0x0002,"",eReadWrite_16,},//#REG_0TC_CCFG_FrRateQualityType//    
	{0x0f12, 0x0535,"",eReadWrite_16,},//#REG_0TC_CCFG_usMaxFrTimeMsecMult10//
	{0x0f12, 0x0535,"",eReadWrite_16,},//#REG_0TC_CCFG_usMinFrTimeMsecMult10//
	
	{0x002a, 0x022c,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x0266,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x026a,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x0268,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x026e,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x026a,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x0270,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x024e,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x023e,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x01a8,"",eReadWrite_16,},
	{0x0f12, 0xaaaa,"",eReadWrite_16,},
	{0x0028, 0x147c,"",eReadWrite_16,},
	{0x0f12, 0x0180,"",eReadWrite_16,},
	{0x0028, 0x1482,"",eReadWrite_16,},
	{0x0f12, 0x0180,"",eReadWrite_16,},
	/* set 422 uyvy */
//	{0xffff, 0x0000,"",eReadWrite_16,},
//	{0xfcfc, 0xd000,"",eReadWrite_16,},
//	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x02b4,"",eReadWrite_16,},
	{0x0f12, 0x0050,"",eReadWrite_16,},
	{0x002a, 0x03a6,"",eReadWrite_16,},
	{0x0f12, 0x0050,"",eReadWrite_16,},

#if 0
	{0x002a, 0x18ac,"",eReadWrite_16,},
	{0x0f12, 0x0060,"",eReadWrite_16,},
	{0x0f12, 0x0060,"",eReadWrite_16,},
	{0x0f12, 0x05c0,"",eReadWrite_16,},
	{0x0f12, 0x05c0,"",eReadWrite_16,},
	{0x002a, 0x0250,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0010,"",eReadWrite_16,},
	{0x0f12, 0x000c,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0010,"",eReadWrite_16,},
	{0x0f12, 0x000c,"",eReadWrite_16,},
	{0x002a, 0x0494,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x0262,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0xfcfc, 0xd000,"",eReadWrite_16,},
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x02a6,"",eReadWrite_16,},//PCFG
	{0x0f12, 0x0320,"",eReadWrite_16,},//width:800
	{0x0f12, 0x0258,"",eReadWrite_16,},//height:600
	{0x0f12, 0x0005,"",eReadWrite_16,},//5:yuv
	{0x002a, 0x02bc,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0320,"",eReadWrite_16,},  //0x0535
	{0x0f12, 0x014D,"",eReadWrite_16,},
	
	{0xfcfc, 0xd000,"",eReadWrite_16,},
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x028c,"",eReadWrite_16,},
	{0x0f12, 0x0005,"",eReadWrite_16,},
	
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x0266,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x026a,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x0268,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x026e,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x026a,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x0270,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x024e,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x023e,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	//framerate set,"",eReadWrite_16,ting
	{0x002a, 0x03b4,"",eReadWrite_16,},
	{0x0f12, 0x0535,"",eReadWrite_16,},//framerate:15 //0x29a
	{0x0f12, 0x029a,"",eReadWrite_16,},
	//set pvi mode
	{0x002a, 0x0216,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x02b4,"",eReadWrite_16,},//REG_0TC_PCFG_PVIMask
	{0x0f12, 0x0052,"",eReadWrite_16,},
	{0x002a, 0x03a6,"",eReadWrite_16,},//REG_0TC_CCFG_PVIMask
	{0x0f12, 0x0052,"",eReadWrite_16,},
#endif
	
    {0x0000 ,0x00,"eTableEnd",eTableEnd}

};

// 800 x 600
const IsiRegDescription_t S5K4EC_g_svga[] =
{
	{0x002a, 0x18ac,"",eReadWrite_16,},
	{0x0f12, 0x0060,"",eReadWrite_16,},
	{0x0f12, 0x0060,"",eReadWrite_16,},
	{0x0f12, 0x05c0,"",eReadWrite_16,},
	{0x0f12, 0x05c0,"",eReadWrite_16,},
	{0x002a, 0x0250,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0010,"",eReadWrite_16,},
	{0x0f12, 0x000c,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0010,"",eReadWrite_16,},
	{0x0f12, 0x000c,"",eReadWrite_16,},
	{0x002a, 0x0494,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x0262,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0xfcfc, 0xd000,"",eReadWrite_16,},
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x02a6,"",eReadWrite_16,},//PCFG
	{0x0f12, 0x0320,"",eReadWrite_16,},//width:800
	{0x0f12, 0x0258,"",eReadWrite_16,},//height:600
	{0x0f12, 0x0005,"",eReadWrite_16,},//5:yuv
	{0x002a, 0x02bc,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0320,"",eReadWrite_16,},  //0x0535
	{0x0f12, 0x014D,"",eReadWrite_16,},
	
	{0xfcfc, 0xd000,"",eReadWrite_16,},
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x028c,"",eReadWrite_16,},
	{0x0f12, 0x0005,"",eReadWrite_16,},
	
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x0266,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x026a,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x0268,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x026e,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x026a,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x0270,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x024e,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x023e,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	//framerate setting
	{0x002a, 0x03b4,"",eReadWrite_16,},
	{0x0f12, 0x0535,"",eReadWrite_16,},//framerate:15 //0x29a
	{0x0f12, 0x029a,"",eReadWrite_16,},
	//set pvi mode
	{0x002a, 0x0216,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x02b4,"",eReadWrite_16,},//REG_0TC_PCFG_PVIMask
	{0x0f12, 0x0052,"",eReadWrite_16,},
	{0x002a, 0x03a6,"",eReadWrite_16,},//REG_0TC_CCFG_PVIMask
	{0x0f12, 0x0052,"",eReadWrite_16,},

    {0x0000 ,0x00,"eTableEnd",eTableEnd}
};



//640 x 480
const IsiRegDescription_t S5K4EC_g_vga[] =
{
	{0x002a, 0x18ac,"",eReadWrite_16,},
	{0x0f12, 0x0060,"",eReadWrite_16,},
	{0x0f12, 0x0060,"",eReadWrite_16,},
	{0x0f12, 0x05c0,"",eReadWrite_16,},
	{0x0f12, 0x05c0,"",eReadWrite_16,},
	{0x002a, 0x0250,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0010,"",eReadWrite_16,},
	{0x0f12, 0x000c,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0010,"",eReadWrite_16,},
	{0x0f12, 0x000c,"",eReadWrite_16,},
	{0x002a, 0x0494,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x0262,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0xfcfc, 0xd000,"",eReadWrite_16,},
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x02a6,"",eReadWrite_16,},//PCFG
	{0x0f12, 0x0280,"",eReadWrite_16,},//width:640
	{0x0f12, 0x01e0,"",eReadWrite_16,},//height:480
	{0x0f12, 0x0005,"",eReadWrite_16,},//5:yuv
	{0x002a, 0x02bc,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0320,"",eReadWrite_16,},  //0x0535
	{0x0f12, 0x014D,"",eReadWrite_16,},
	
	{0xfcfc, 0xd000,"",eReadWrite_16,},
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x028c,"",eReadWrite_16,},
	{0x0f12, 0x0005,"",eReadWrite_16,},
	
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x0266,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x026a,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x0268,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x026e,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x026a,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x0270,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x024e,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x023e,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	//framerate setting
	{0x002a, 0x03b4,"",eReadWrite_16,},
	{0x0f12, 0x0535,"",eReadWrite_16,},//framerate:15 //0x29a
	{0x0f12, 0x029a,"",eReadWrite_16,},
	//set pvi mode
	{0x002a, 0x0216,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x02b4,"",eReadWrite_16,},//REG_0TC_PCFG_PVIMask
	{0x0f12, 0x0052,"",eReadWrite_16,},
	{0x002a, 0x03a6,"",eReadWrite_16,},//REG_0TC_CCFG_PVIMask
	{0x0f12, 0x0052,"",eReadWrite_16,},
	
    {0x0000 ,0x00,"eTableEnd",eTableEnd}
};

// 1280 x 720
const IsiRegDescription_t S5K4EC_g_video_720p[] =
{
	{0x002a, 0x18ac,"",eReadWrite_16,},
	{0x0f12, 0x0060,"",eReadWrite_16,},
	{0x0f12, 0x0060,"",eReadWrite_16,},
	{0x0f12, 0x05c0,"",eReadWrite_16,},
	{0x0f12, 0x0a96,"",eReadWrite_16,},
	{0x002a, 0x0250,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x05a0,"",eReadWrite_16,},
	{0x0f12, 0x0010,"",eReadWrite_16,},
	{0x0f12, 0x00fc,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0010,"",eReadWrite_16,},
	{0x0f12, 0x000c,"",eReadWrite_16,},
	{0x002a, 0x0494,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x05a0,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0a00,"",eReadWrite_16,},
	{0x0f12, 0x0780,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x0262,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0xfcfc, 0xd000,"",eReadWrite_16,},
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x02a6,"",eReadWrite_16,},//PCFG
	{0x0f12, 0x0500,"",eReadWrite_16,},//width
	{0x0f12, 0x02d0,"",eReadWrite_16,},//height
	{0x002a, 0x02bc,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x014d,"",eReadWrite_16,},
	{0x0f12, 0x014d,"",eReadWrite_16,},
	{0x002a, 0x022c,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	
		//for AF
	{0xfcfc, 0xd000,"",eReadWrite_16,},
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x028c,"",eReadWrite_16,},
	{0x0f12, 0x0005,"",eReadWrite_16,},
	
	{0xfcfc, 0xd000,"",eReadWrite_16,},
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x0266,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x026a,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x0268,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x026e,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x026a,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x0270,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x024e,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x023e,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0xffff, 0x002c,"",eDelay,},//delay

    {0x0000 ,0x00,"eTableEnd",eTableEnd}
};

//1920 x 1080
const IsiRegDescription_t S5K4EC_g_1080p[] =
{
        {0x002a, 0x18ac,"",eReadWrite_16,},
        {0x0f12, 0x0060,"",eReadWrite_16,},
        {0x0f12, 0x0060,"",eReadWrite_16,},
        {0x0f12, 0x05c0,"",eReadWrite_16,},
        {0x0f12, 0x0a96,"",eReadWrite_16,},
        {0x002a, 0x0250,"",eReadWrite_16,},
        {0x0f12, 0x0780,"",eReadWrite_16,},
        {0x0f12, 0x0438,"",eReadWrite_16,},
        {0x0f12, 0x014e,"",eReadWrite_16,},
        {0x0f12, 0x01b0,"",eReadWrite_16,},
        {0x0f12, 0x0a00,"",eReadWrite_16,},
        {0x0f12, 0x0780,"",eReadWrite_16,},
        {0x0f12, 0x0010,"",eReadWrite_16,},
        {0x0f12, 0x000c,"",eReadWrite_16,},
        {0x002a, 0x0494,"",eReadWrite_16,},
        {0x0f12, 0x0780,"",eReadWrite_16,},
        {0x0f12, 0x0438,"",eReadWrite_16,},
        {0x0f12, 0x0000,"",eReadWrite_16,},
        {0x0f12, 0x0000,"",eReadWrite_16,},
        {0x0f12, 0x0a00,"",eReadWrite_16,},
        {0x0f12, 0x0780,"",eReadWrite_16,},
        {0x0f12, 0x0000,"",eReadWrite_16,},
        {0x0f12, 0x0000,"",eReadWrite_16,},
        {0x002a, 0x0262,"",eReadWrite_16,},
        {0x0f12, 0x0001,"",eReadWrite_16,},
        {0x0f12, 0x0001,"",eReadWrite_16,},
        {0xfcfc, 0xd000,"",eReadWrite_16,},
        {0x0028, 0x7000,"",eReadWrite_16,},
        {0x002a, 0x02a6,"",eReadWrite_16,},//PCFG
        {0x0f12, 0x0780,"",eReadWrite_16,},//WIDTH
        {0x0f12, 0x0438,"",eReadWrite_16,},//HEIGHT
        {0x002a, 0x02bc,"",eReadWrite_16,},
        {0x0f12, 0x0001,"",eReadWrite_16,},
        {0x0f12, 0x0001,"",eReadWrite_16,},
        {0x0f12, 0x0000,"",eReadWrite_16,},
        {0x0f12, 0x029a,"",eReadWrite_16,}, //framerate 15 
        {0x0f12, 0x029a,"",eReadWrite_16,}, //framerate 15
        {0x002a, 0x022c,"",eReadWrite_16,},
        {0x0f12, 0x0001,"",eReadWrite_16,},
                //for AF
        {0xfcfc, 0xd000,"",eReadWrite_16,},
        {0x0028, 0x7000,"",eReadWrite_16,},
        {0x002a, 0x028c,"",eReadWrite_16,},
        {0x0f12, 0x0005,"",eReadWrite_16,},
        
        {0xfcfc, 0xd000,"",eReadWrite_16,},
        {0x0028, 0x7000,"",eReadWrite_16,},
        {0x002a, 0x0266,"",eReadWrite_16,},
        {0x0f12, 0x0000,"",eReadWrite_16,},
        {0x002a, 0x026a,"",eReadWrite_16,},
        {0x0f12, 0x0001,"",eReadWrite_16,},
        {0x002a, 0x0268,"",eReadWrite_16,},
        {0x0f12, 0x0001,"",eReadWrite_16,},
        {0x002a, 0x026e,"",eReadWrite_16,},
        {0x0f12, 0x0000,"",eReadWrite_16,},
        {0x002a, 0x026a,"",eReadWrite_16,},
        {0x0f12, 0x0001,"",eReadWrite_16,},
        {0x002a, 0x0270,"",eReadWrite_16,},
        {0x0f12, 0x0001,"",eReadWrite_16,},
        {0x002a, 0x024e,"",eReadWrite_16,},
        {0x0f12, 0x0001,"",eReadWrite_16,},
        {0x002a, 0x023e,"",eReadWrite_16,},
        {0x0f12, 0x0001,"",eReadWrite_16,},
        {0x0f12, 0x0001,"",eReadWrite_16,},
        {0xffff, 0x002c,"",eDelay,},//delay
    {0x0000 ,0x00,"eTableEnd",eTableEnd}
};


const IsiRegDescription_t S5K4EC_g_2592x1944[] =
{
	{0x002a, 0x0396,"",eReadWrite_16,},//CCFG
	{0x0f12, 0x0001,"",eReadWrite_16,},//capture mode
	{0x0f12, 0x0a00,"",eReadWrite_16,},//2590
	{0x0f12, 0x0780,"",eReadWrite_16,},//1920
//	{0x0f12, 0x0a00,"",eReadWrite_16,},//2590
//	{0x0f12, 0x0780,"",eReadWrite_16,},//1920
	{0x0f12, 0x0005,"",eReadWrite_16,},//yuv
	{0x002a, 0x03ae,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x002a, 0x03b4,"",eReadWrite_16,},//
	{0x0f12, 0x0535,"",eReadWrite_16,},//framerate:5   0x7d0
	{0x0f12, 0x0535,"",eReadWrite_16,},    // 0x7d0
	{0xfcfc, 0xd000,"",eReadWrite_16,},
	{0x0028, 0x7000,"",eReadWrite_16,},
	//set pvi mode
	{0x002a, 0x0216,"",eReadWrite_16,},//0216
	{0x0f12, 0x0000,"",eReadWrite_16,},    //0x0000
	/* set 422 uyvy */
//	{0xffff, 0x0000,"",eReadWrite_16,},
//	{0xfcfc, 0xd000,"",eReadWrite_16,},
//	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x02b4,"",eReadWrite_16,},	//REG_0TC_PCFG_PVIMask
	{0x0f12, 0x0052,"",eReadWrite_16,},
	
	{0x002a, 0x03a6,"",eReadWrite_16,},   //REG_0TC_CCFG_PVIMask
	{0x0f12, 0x0052,"",eReadWrite_16,},
	/*//for AF
	{0xfcfc, 0xd000,"",eReadWrite_16,},
	{0x0028, 0x7000,"",eReadWrite_16,},
	{0x002a, 0x028c,"",eReadWrite_16,},
	{0x0f12, 0x0005,"",eReadWrite_16,},
	*/
	{0x002a, 0x026e,"",eReadWrite_16,},
	{0x0f12, 0x0000,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x0242,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x024e,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0x002a, 0x0244,"",eReadWrite_16,},
	{0x0f12, 0x0001,"",eReadWrite_16,},
	{0xffff, 0x00f4,"",eDelay,},//delay

    {0x0000 ,0x00,"eTableEnd",eTableEnd}

};

const IsiRegDescription_t S5K4EC_af_init[] =
{
    {0x0000 ,0x00,"eTableEnd",eTableEnd}
};
const IsiRegDescription_t S5K4EC_af_firmware[] =
{
    {0x0000 ,0x00,"eTableEnd",eTableEnd}
};


