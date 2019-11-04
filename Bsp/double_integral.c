/**
 ******************************************************************** 
 *  @filename	频域一次/二次积分
 *  @author  	L-KAYA
 *  @date    	27-July-2018
 *  @brief   	该文件主要用于实现加速度在频域的一次或二次积分
 *           	得到速度或位移，有带通滤波功能，积分后漂移较小
 *  			建议采样频率1kHz以上，效果较为理想
 ********************************************************************
 */

#include "double_integral.h"
#include "arm_const_structs.h"

//结构体指针
//dou_integ_t* integ = NULL;
struct dou_integ integ;
/** 
 *  @name	频域一次/二次积分初始化函数
 *  @brief  初始化用于存储函数的结构体，下方可更改部分根据实际情况更改
 *  @pram	len	用于二次积分的数组长度，建议大于16小于4096，2的幂次
 *  @retval	0	成功
 *  		else失败
 */
//#define DATA_LEN				1024				//数据长度，初始化时作为参数传入，2的幂次最佳

//#define SMPLE_FRQ				1000				//采样频率
//#define GRAVITY					9810.5				//单位变化系数（最后积分结果乘上该系数，根据重力加速度得到）
//#define INTEG_TIME				2					//积分次数
//#define FRQ_MIN					1					//高通滤波截止频率
//#define FRQ_MAX					200					//低通滤波截止频率（应小于采样频率）
int integ_init(uint16_t len,uint32_t sample_freq,float gravity,uint32_t integ_time,uint32_t freq_min,uint32_t freq_max)
{
//	integ = (dou_integ_t*)MALLOC(sizeof(dou_integ_t));
//	if(integ == NULL)
//		return -1;//内存申请失败
	
	// 设置频域二次积分的参数(可更改)
	integ.smple_frq  = sample_freq;								//采样频率
	integ.gravity    = gravity;								//单位变化系数（最后积分结果乘上该系数，根据重力加速度得到）
	integ.integ_time = integ_time;								//积分次数
	integ.frq_min    = freq_min;								//高通滤波截止频率
	integ.frq_max    = freq_max;								//低通滤波截止频率（应小于采样频率）
	if(integ.frq_max>=integ.smple_frq)
		return -2;
	
	// 根据输入数组长度，计算FFT数组长度
	uint8_t i;
	for(i=4; i<13; i++){										//数组长度范围为2^4=16 ~ 2^12=4096
		if(POW(2,i)>=len){
			integ.fft_len = POW(2,i);							//大于并接近n的2的幂次方的FFT长度
			break;
		}
	}
	if(i == 13)
		return -3;//数据长度太大，范围为16~4096
	
	//为fft_buf临时数组分配内存
//	integ.fft_buf = (float*)MALLOC(integ.fft_len*2*(sizeof(float)));
//	if(integ.fft_buf == NULL)
//		return -4;//内存申请失败
	
	switch(integ.fft_len)
	{
		case 16:{
			integ.S = arm_cfft_sR_f32_len16;
			break;
		}
		case 32:{
			integ.S = arm_cfft_sR_f32_len32;
			break;
		}
		case 64:{
			integ.S = arm_cfft_sR_f32_len64;
			break;
		}
		case 128:{
			integ.S = arm_cfft_sR_f32_len128;
			break;
		}
		case 256:{
			integ.S = arm_cfft_sR_f32_len256;
			break;
		}
		case 512:{
			integ.S = arm_cfft_sR_f32_len512;
			break;
		}
		case 1024:{
			integ.S = arm_cfft_sR_f32_len1024;
			break;
		}
		case 2048:{
			integ.S = arm_cfft_sR_f32_len2048;
			break;
		}
		case 4096:{
			integ.S = arm_cfft_sR_f32_len4096;
			break;
		}
		default:{
			integ.S = arm_cfft_sR_f32_len1024;
		}
	}

	float df = (float)integ.smple_frq / (float)integ.fft_len;			//计算频率间隔（Hz/s）
	integ.high_pass = round(integ.frq_min/df);						//高通频率截止点
	integ.low_pass  = round(integ.frq_max/df);  						//低通频率截止点
	
	float dw = 2*PI*df;													//圆频率间隔（rad/s）
//	integ.w_vec = (float*)MALLOC((integ.fft_len-1)*(sizeof(float)));
	for(int i=0; i<(integ.fft_len/2); i++){
		integ.w_vec[i] = dw*i;											//正离散圆频率向量
		integ.w_vec[i] = POW(integ.w_vec[i],integ.integ_time);		//以积分次数为指数，建立圆频率变量向量
	}
	for(int i=integ.fft_len/2; i<(integ.fft_len-1); i++){
		integ.w_vec[i] = -dw*(integ.fft_len/2-1) + dw*(i - integ.fft_len/2);	//负离散圆频率向量
		integ.w_vec[i] = POW(integ.w_vec[i],integ.integ_time);		//以积分次数为指数，建立圆频率变量向量
	}
	
	return 0;
}

/** 
 *  @name	频域一次/二次积分去初始化函数
 *  @brief	不需要用到频域积分后可去初始化，释放内存空间
 *  @pram	无
 *  @retval	无
 */
void integ_deinit()
{
//	free(integ.w_vec);
//	integ.w_vec = NULL;
//	free(integ.fft_buf);
//	integ.fft_buf = NULL;
//	free(integ);
//	integ = NULL;
}

/** 
 *  @name	频域一次/二次积分函数
 *  @brief	计算一次/二次积分
 *  @pram	inputbuf	输入数组
 *  		outputbuf	输出数组（长度大于等于输入数组且需为2^(4~12)）
 *  @retval	0			成功
 *  		else		失败
 */
int frq_domain_integral(float* inputbuf, float* outputbuf)
{
//	if(integ == NULL)
//		return -1;
	
	for(int i=0; i<integ.fft_len; i++){									//生成FFT数组
		integ.fft_buf[2*i]   = inputbuf[i];								//实部为加速度值
		integ.fft_buf[2*i+1] = 0;											//虚部为0
	}
	
	arm_cfft_f32(&integ.S, integ.fft_buf, 0, 1);							//计算FFT,结果保存在fft_buf中
	
	for(int i=1; i<integ.fft_len-1; i++){
		integ.fft_buf[2*i  ] /= integ.w_vec[i];
		integ.fft_buf[2*i+1] /= integ.w_vec[i];
	}
	
	if(integ.integ_time == 2){
		for(int i=0; i<integ.fft_len; i++){								//进行积分频域变换和相位变换，放在一起做了
			integ.fft_buf[2*i  ] = -integ.fft_buf[2*i  ];					//频域变换即fft数组中的实部和虚部都除以w_vec的圆频率变换向量
			integ.fft_buf[2*i+1] = -integ.fft_buf[2*i+1];					//2次积分相位变换为旋转180度，即实部虚部都加一个负号
		}
	}
	else{
		float temp;
		for(int i=0; i<integ.fft_len; i++){		
			temp = -integ.fft_buf[2*i];									//1次积分相位变换为逆时针旋转90度
			integ.fft_buf[2*i  ] = integ.fft_buf[2*i+1];					//实部等于虚部，虚部等于负实部
			integ.fft_buf[2*i+1] = temp;
		}
	}
	
	for(int i=0; i<integ.fft_len-integ.low_pass; i++){					//清除频域中的低频部分
		integ.fft_buf[2*i  ] = 0;
		integ.fft_buf[2*i+1] = 0;
	}
	for(int i=integ.fft_len-integ.high_pass; i<integ.fft_len; i++){		//清除频域中的高频部分
		integ.fft_buf[2*i  ] = 0;
		integ.fft_buf[2*i+1] = 0;
	}
	
	
	arm_cfft_f32(&integ.S, integ.fft_buf, 1, 1);							//计算IFFT,结果保存在fft_buf中
	
	for(int i=0; i<integ.fft_len; i++){
		outputbuf[i] = integ.fft_buf[2*i]*integ.gravity*2;					//乘以一个2，才是正常数据，取逆变换的实部并乘以单位变换系数为积分结果
	}
	
	return 0;
}

int frq_envelop_integral(float* inputbuf, float* outputbuf)
{
//	if(integ == NULL)
//		return -1;
	float temp;
	for(int i=0; i<integ.fft_len; i++){									//生成FFT数组
		integ.fft_buf[2*i]   =inputbuf[i];								//实部为加速度值
		integ.fft_buf[2*i+1] = 0;											//虚部为0
	}
	
	arm_cfft_f32(&integ.S, integ.fft_buf, 0, 1);							//计算FFT,结果保存在fft_buf中
		

	for(int i=0; i<integ.fft_len; i++){	
		if(i<=(integ.fft_len>>1))
		{
			temp = -integ.fft_buf[2*i];									//1次积分相位变换为逆时针旋转90度
			integ.fft_buf[2*i  ] = integ.fft_buf[2*i+1];					//实部等于虚部，虚部等于负实部
			integ.fft_buf[2*i+1] = temp;
		}else{
			temp = integ.fft_buf[2*i];									//1次积分相位变换为逆时针旋转90度
			integ.fft_buf[2*i  ] =-integ.fft_buf[2*i+1];					//实部等于虚部，虚部等于负实部
			integ.fft_buf[2*i+1] = temp;
		}
	}
	
	
//	for(int i=0; i<integ.fft_len-integ.low_pass; i++){					//清除频域中的低频部分
//		integ.fft_buf[2*i  ] = 0;
//		integ.fft_buf[2*i+1] = 0;
//	}
//	for(int i=integ.fft_len-integ.high_pass; i<integ.fft_len; i++){		//清除频域中的高频部分
//		integ.fft_buf[2*i  ] = 0;
//		integ.fft_buf[2*i+1] = 0;
//	}
	
	
	arm_cfft_f32(&integ.S, integ.fft_buf, 1, 1);							//计算IFFT,结果保存在fft_buf中
//	arm_cmplx_mag_f32(integ.fft_buf, outputbuf, 4096);
	for(int i=0; i<integ.fft_len; i++){
		outputbuf[i] =(integ.fft_buf[2*i]*integ.fft_buf[2*i])+(inputbuf[i]*inputbuf[i]);					//乘以一个2，才是正常数据，取逆变换的实部并乘以单位变换系数为积分结果
	}
	
	return 0;
}
