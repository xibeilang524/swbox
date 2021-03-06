/********************************************************************
	created:	2017/07/21
	created:	
	filename: 	D:\rd\src_code\ftu\inc\point_table.h
	file path:	D:\rd\src_code\ftu\inc
	file base:	point_table
	file ext:	h
	author:		
	
	purpose:	communication point table all stored in rdb
*********************************************************************/

#ifndef _POINT_TABLE_H_
#define _POINT_TABLE_H_

/*
 * 点表
 */
/* 遥信 */
enum yx_e {
	/* 起始地址0x0000 */
	device_status =0,                       //0x0001	装置状态
	TEV1_alarm,                             //0x0002	TEV1告警
	TEV2_alarm,                             //0x0003	TEV2告警
	AA_alarm,                               //0x0004	AA告警
	HFCT1_alarm,                            //0x0005	HFCT1告警
	HFCT2_alarm,                            //0x0006	HFCT2告警


/* 遥信数量 */
	num_of_yx								/* must be at last */
};

/* 遥测 */
enum yc_e {
/* 起始地址0x4000 */

	TEV1_amplitude = 0,			            //0x4001	TEV1幅值	
	TEV1_num,                               //0x4002	TEV1脉冲个数
	TEV1_severity,                          //0x4003	TEV1严重度
	TEV2_amplitude,                         //0x4004	TEV2幅值
	TEV2_num,                               //0x4005	TEV2脉冲个数
	TEV2_severity,                          //0x4006	TEV2严重度
	AA_amplitude,                           //0x4007	AA幅值
	AA_severity,                            //0x4008	AA严重度
	HFCT1_amplitude,                        //0x4009	HFCT1幅值
	HFCT1_num,                              //0x400a	HFCT1脉冲个数
	HFCT1_severity,                         //0x400b	HFCT1严重度
	HFCT2_amplitude,                        //0x400c	HFCT2幅值
	HFCT2_num,                              //0x400d	HFCT2脉冲个数
	HFCT2_severity,                         //0x400e	HFCT2严重度
	TEV1_center_biased_adv,                 //0x4020	TEV1中心偏置建议值
	TEV1_noise_biased_adv,                  //0x4021	TEV1噪声偏置建议值
	TEV2_center_biased_adv,                 //0x4022	TEV2中心偏置建议值
	TEV2_noise_biased_adv,                  //0x4023	TEV2噪声偏置建议值
	AA_biased_adv,                          //0x4024	AA偏置建议值
	HFCT1_center_biased_adv,                //0x4025	HFCT1中心偏置建议值
	HFCT1_noise_biased_adv,                 //0x4026	HFCT1噪声偏置建议值
	HFCT2_center_biased_adv,                //0x4027	HFCT2中心偏置建议值
	HFCT2_noise_biased_adv,                 //0x4028	HFCT2噪声偏置建议值
	TEV1_gain=47,                           //0x4030	TEV1增益
	TEV1_center_biased,                     //0x4031	TEV1中心偏置
	TEV1_noise_biased,                      //0x4032	TEV1噪声偏置
	TEV2_gain,                              //0x4033	TEV2增益
	TEV2_center_biased,                     //0x4034	TEV2中心偏置
	TEV2_noise_biased,                      //0x4035	TEV2噪声偏置
	AA_gain,                                //0x4036	AA增益
	AA_biased,                              //0x4037	AA偏置
	HFCT1_gain,                             //0x4038	HFCT1增益
	HFCT1_center_biased,                    //0x4039	HFCT 1中心偏置
	HFCT1_noise_biased,                     //0x403a	HFCT 1噪声偏置
	HFCT2_gain,                             //0x403b	HFCT 2增益
	HFCT2_center_biased,                    //0x403c	HFCT 2中心偏置
	HFCT2_noise_biased,                     //0x403d	HFCT 2噪声偏置
	version=95,                             //0x4060	版本


/* 遥测数量 */
	num_of_yc								/* must be at last */
};

/* 遥控 */
enum yk_e {
	/* 起始地址0x6000 */
	start=0,                                //0x6001	启动 
	stop,                                   //0x6002	停止 

/* 遥控数量 */
	num_of_yk								/* must be at last */
};

/* 设定值 */
enum sdz_e {
/* 起始地址0x6200 */
	TEV1_gain_sdz = 0,                           //0x6201	TEV1增益
	TEV1_center_biased_sdz,                      //0x6202	TEV1中心偏置
	TEV1_noise_biased_sdz,                       //0x6203	TEV1噪声偏置
	TEV2_gain_sdz,                               //0x6204	TEV2增益
	TEV2_center_biased_sdz,                      //0x6205	TEV2中心偏置
	TEV2_noise_biased_sdz,                       //0x6206	TEV2噪声偏置
	AA_gain_sdz,                                 //0x6207	AA增益
	AA_biased_sdz,                               //0x6208	AA偏置
	HFCT1_gain_sdz,                              //0x6209	HFCT1增益
	HFCT1_center_biased_sdz,                     //0x620a	HFCT 1中心偏置
	HFCT1_noise_biased_sdz,                      //0x620b	HFCT 1噪声偏置
	HFCT2_gain_sdz,                              //0x620c	HFCT 2增益
	HFCT2_center_biased_sdz,                     //0x620d	HFCT 2中心偏置
	HFCT2_noise_biased_sdz,                      //0x620e	HFCT 2噪声偏置

/* 设定值数量 */
	num_of_sdz								/* must be at last */
};

/* YX */
#define	YX_VALID_NUM 	(num_of_yx)

/* YC */
#define YC_VALID_NUM	(num_of_yc)

/* YK */
#define YK_VALID_NUM	(num_of_yk)

/* SDZ */
#define SDZ_VALID_NUM	(num_of_sdz)


/*
 * 内部保护组号、条目号
 */

#define BAY_ITEM_NUM		17
#define BUS_ITEM_NUM		2

enum group_e {
	prot_group = 1,
	alarm_group,
};

enum item_prot_e {
	ptoc_1_1 = 1,
	ptoc_1_1_a,
	ptoc_1_1_b,
	ptoc_1_1_c,
	ptoc_2_1,
	ptoc_2_1_a,
	ptoc_2_1_b,
	ptoc_2_1_c,
	ptoc_3_1,
	ptoc_3_1_a,
	ptoc_3_1_b,
	ptoc_3_1_c,
	ptoz_1_1,
	ptol_1_1,
	ptol_1_1_a,
	ptol_1_1_b,
	ptol_1_1_c,
	ptoc_1_2,
	ptoc_1_2_a,
	ptoc_1_2_b,
	ptoc_1_2_c,
	ptoc_2_2,
	ptoc_2_2_a,
	ptoc_2_2_b,
	ptoc_2_2_c,
	ptoc_3_2,
	ptoc_3_2_a,
	ptoc_3_2_b,
	ptoc_3_2_c,
	ptoz_1_2,
	ptol_1_2,
	ptol_1_2_a,
	ptol_1_2_b,
	ptol_1_2_c,
	ptoc_1_3,
	ptoc_1_3_a,
	ptoc_1_3_b,
	ptoc_1_3_c,
	ptoc_2_3,
	ptoc_2_3_a,
	ptoc_2_3_b,
	ptoc_2_3_c,
	ptoc_3_3,
	ptoc_3_3_a,
	ptoc_3_3_b,
	ptoc_3_3_c,
	ptoz_1_3,
	ptol_1_3,
	ptol_1_3_a,
	ptol_1_3_b,
	ptol_1_3_c,
	ptoc_1_4,
	ptoc_1_4_a,
	ptoc_1_4_b,
	ptoc_1_4_c,
	ptoc_2_4,
	ptoc_2_4_a,
	ptoc_2_4_b,
	ptoc_2_4_c,
	ptoc_3_4,
	ptoc_3_4_a,
	ptoc_3_4_b,
	ptoc_3_4_c,
	ptoz_1_4,
	ptol_1_4,
	ptol_1_4_a,
	ptol_1_4_b,
	ptol_1_4_c,
	ptoc_1_5,
	ptoc_1_5_a,
	ptoc_1_5_b,
	ptoc_1_5_c,
	ptoc_2_5,
	ptoc_2_5_a,
	ptoc_2_5_b,
	ptoc_2_5_c,
	ptoc_3_5,
	ptoc_3_5_a,
	ptoc_3_5_b,
	ptoc_3_5_c,
	ptoz_1_5,
	ptol_1_5,
	ptol_1_5_a,
	ptol_1_5_b,
	ptol_1_5_c,
	ptoc_1_6,
	ptoc_1_6_a,
	ptoc_1_6_b,
	ptoc_1_6_c,
	ptoc_2_6,
	ptoc_2_6_a,
	ptoc_2_6_b,
	ptoc_2_6_c,
	ptoc_3_6,
	ptoc_3_6_a,
	ptoc_3_6_b,
	ptoc_3_6_c,
	ptoz_1_6,
	ptol_1_6,
	ptol_1_6_a,
	ptol_1_6_b,
	ptol_1_6_c,
	ptoc_1_7,
	ptoc_1_7_a,
	ptoc_1_7_b,
	ptoc_1_7_c,
	ptoc_2_7,
	ptoc_2_7_a,
	ptoc_2_7_b,
	ptoc_2_7_c,
	ptoc_3_7,
	ptoc_3_7_a,
	ptoc_3_7_b,
	ptoc_3_7_c,
	ptoz_1_7,
	ptol_1_7,
	ptol_1_7_a,
	ptol_1_7_b,
	ptol_1_7_c,
	ptoc_1_8,
	ptoc_1_8_a,
	ptoc_1_8_b,
	ptoc_1_8_c,
	ptoc_2_8,
	ptoc_2_8_a,
	ptoc_2_8_b,
	ptoc_2_8_c,
	ptoc_3_8,
	ptoc_3_8_a,
	ptoc_3_8_b,
	ptoc_3_8_c,
	ptoz_1_8,
	ptol_1_8,
	ptol_1_8_a,
	ptol_1_8_b,
	ptol_1_8_c,
	avol_hi_1,
	avol_lo_1,
	avol_hi_2,
	avol_lo_2,
	avol_hi_3,
	avol_lo_3,

	item_prot_last
};
#endif /* _POINT_TABLE_H_ */

