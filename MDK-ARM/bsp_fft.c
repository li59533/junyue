#include "bsp.h"
#include "app.h"
#include "math.h"
#include "arm_math.h"

#define nfft 16384
float tempForenv[32768] __attribute__((at(0xC0000000))); 
float x_in_out[32768] __attribute__ ((section ("RW_IRAM2"),  zero_init));


float W[32768];//采样率最大16384   W=2*N

//#define PI    3.1415926
uint32_t acceleration(uint32_t fs,float* data,uint32_t len,float* character);
void DSPF_sp_cfftr2_dit_cn(float* x, float* w, unsigned short n);
void DSPF_sp_icfftr2_dif_cn(float* x, float* w, unsigned short n);
void backFFT(float* realIn,float* imgIn,uint32_t length,float* dataOut);
void backFFT_1(float* in, uint32_t length, float *dataOut);
void bit_rev(float x[], uint32_t N);
void gen_w_r2(float w[], uint32_t N);




float qdzb(float qd,float mrms);      //峭度指标
float qiaodu(float *dataIn, uint32_t length, float mrms);      //峭度，datain为去直流后
float ifff(float peak,float aver);            //脉冲指标
float Cp(float mrms,float aver);      //波形指标
float pr(float max, float mrms);      //峰值指标
float rms(float *dataIn, uint32_t length);      //datain为去直流后
float absMax(float* dataIn,uint32_t length);            //绝对峰值
float pp(float *dataIn,uint32_t length);            //峰峰值，datain为去直流后
float absAverage(float* dataIn,uint32_t length);       //绝对均值，datain为去直流后
float min(float *dataIn,uint32_t length);       //datain为去直流后
float max(float *dataIn,uint32_t length);           //datain为去直流后
float removeDC(float *dataIn, uint32_t length);
float sum(float *dataIn, uint32_t length);


float charter[4];

float test_in[16384];
float fft_8192 = 0;
float character[5];
int test(void)
{	

	uint32_t i;
	float yy = 0;
	fft_8192 = 1 / 8192.0;	
	gen_w_r2(W, 8192);
	bit_rev(W, 8192 >> 1);	
	acceleration(16384,test_in,16384,charter);
	while(1)
	{
//			delay_ms(1000);
		
	}
		

}

float x_in_out_back[16384];
void backFFT_1(float* in, uint32_t length, float *dataOut)
{
		uint32_t i = 0;

//    fftw_complex* out=(fftw_complex*)fftw_malloc(length*sizeof(fftw_complex));
//    fftw_plan q=fftw_plan_dft_1d(length,in,out,FFTW_BACKWARD,FFTW_ESTIMATE);
    for(i = 0;i < length; i++)
    {
        x_in_out_back[2 * i] = in[i];
        x_in_out_back[2 * i + 1] =0.0;
    }
    DSPF_sp_icfftr2_dif_cn(x_in_out_back, W, length);  //in  W根据采样率提前计算好
    bit_rev(x_in_out_back, length);//这句不知需不需要
		for(i = 0; i < length*2; i++)
		{
			x_in_out_back[i] = x_in_out_back[i]*fft_8192;//1除以采样率
		}
//    fftw_execute(q);
    
//    for(int i=0;i<length;i++)
//    {
//        dataOut[i]=out[i][0] /length;
//    }
    for(i=0;i<length*2;i+=2)
    {
        dataOut[i]=x_in_out_back[i*2] /length;
    }
//    fftw_destroy_plan(q);
//    fftw_free(out);
}

void backFFT(float* realIn,float* imgIn,uint32_t length,float* dataOut)
{
		uint32_t i = 0;
//    fftw_complex* in=(fftw_complex*)fftw_malloc(length*sizeof(fftw_complex));
//    fftw_complex* out=(fftw_complex*)fftw_malloc(length*sizeof(fftw_complex));
//    fftw_plan q=fftw_plan_dft_1d(length,in,out,FFTW_BACKWARD,FFTW_ESTIMATE);
    for(i = 0;i < length; i++)
    {
        x_in_out_back[2 * i] = realIn[i];
        x_in_out_back[2 * i + 1] =imgIn[i];
    }
    DSPF_sp_icfftr2_dif_cn(x_in_out_back, W, length);  //in  W根据采样率提前计算好
    bit_rev(x_in_out_back, length);//这句不知需不需要
		for(i = 0; i < length*2; i++)
		{
			x_in_out_back[i] = x_in_out_back[i]*fft_8192;//1除以采样率
		}
//    for(int i=0;i<length;i++)
//    {
//        in[i][0]=realIn[i];
//        in[i][1]=imgIn[i];
//    }
//    fftw_execute(q);
//    for(int i=0;i<length;i++)
//    {
//        dataOut[i]=out[i][0] /length;
//    }

    for(i=0;i<length*2;i+=2)
    {
        dataOut[i]=x_in_out_back[i*2] /length;
    }
//    fftw_destroy_plan(q);
//    fftw_free(in);
//    fftw_free(out);
}



uint32_t acceleration(uint32_t fs,float* data,uint32_t len,float* character)
{
	uint32_t i = 0;
	
	float df=((float)fs/len);
	float dw=2*PI*df;
	float wave[8192];//=new double[len];
	uint32_t indexLow=4/df;
	uint32_t indexHigh=fs/(2*df);
	float velocity[8192];
  float disp[8192]; 
  float real[8192];
  float img[8192];
  float envelopWave[8192];
	float halfband[8192]; //速度
	float realInt[8192]; //1次积分实部
	float imgInt[8192];//1次积分虚部
	float halfband2[8192]; //位移
	float realInt2[8192]; //2次积分实部
	float imgInt2[8192]; //2次积分虚部
	float halfband3[8192]; //反fft后的加速度信号

	
//	float* x_in;
	
	uint32_t outlen;
	
	      
//	           double *envelopWave=new double[len];
//	  double* real=new double[len];
//        double* img=new double[len];
//	double* velocity = new double[outlen];
//  double* disp = new double[outlen];	
//		double* halfband=new double[len];      
//	double* realInt=new double[len];       
//	double* imgInt=new double[len];         
//	double* halfband2=new double[len];     
//	double* realInt2=new double[len];      
//	double* imgInt2=new double[len];       
//	double* halfband3=new double[len];     
	
	
    if(len>0 && fs>0 )
    {
//        fftw_complex* wave1=(fftw_complex*)fftw_malloc((len*sizeof(fftw_complex)));
        memcpy(wave,data,len*sizeof(double));
//			        removeDC(wave,len);
//			     fftw_complex* out=(fftw_complex*)fftw_malloc(len*sizeof(fftw_complex));
//        fftw_complex* tempForenv=(fftw_complex*)fftw_malloc(len*sizeof(fftw_complex));
//        fftw_plan p=fftw_plan_dft_1d(len,wave1,out,FFTW_FORWARD,FFTW_ESTIMATE);
//			  for(int i=0;i<(int)len;i++)
//        {
//            wave1[i][0]=wave[i];
//            wave1[i][1]=0;
//        }
//			        fftw_execute(p);
			//去直流
		  removeDC(wave,len);
			//组合成负数
			for(i = 0; i < len; i++)
			{
			   x_in_out[i*2] = wave[i];
				 x_in_out[i*2 + 1] = 0;
			}
			
			DSPF_sp_cfftr2_dit_cn(x_in_out, W, len);
			bit_rev(x_in_out, len);//这句不知需不需要
			for(i = 0; i < len*2; i++)
		{
			x_in_out[i] = x_in_out[i]*fft_8192;//1除以采样率
		}
        for(i=0;i<len/2;i++)
        {
            halfband[i]=(i)*dw;
            halfband[len-1-i]=(i)*dw;
            halfband2[i]=pow(i*dw,2);
            halfband2[len-1-i]=pow(i*dw,2);
        }
        for(i=1;i<len/2;i++)
        {
            realInt[i]=x_in_out[i*2+1];
            imgInt[i]=-x_in_out[i*2];
            realInt[i]=realInt[i]/halfband[i];
            imgInt[i]=imgInt[i]/halfband[i];
            realInt2[i]=-x_in_out[i*2]/halfband2[i];
            imgInt2[i]=-x_in_out[i*2+1]/halfband2[i];
            if(i<indexLow || i>indexHigh )
            {
                realInt[i]=0;
                imgInt[i]=0;
                realInt2[i]=0;
                imgInt2[i]=0;
                x_in_out[i*2]=0;
                x_in_out[i*2+1]=0;
            }
            realInt[len-1-i]=0;
            imgInt[len-1-i]=0;
            realInt2[len-1-i]=0;
            imgInt2[len-1-i]=0;
            x_in_out[(len-1-i)*2]=0;    //    temptempv[N-i]=-tempdata[i];
            x_in_out[(len-1-i)*2+1]=0;
        }
        realInt[0]=0;
        imgInt[0]=0;
        realInt[len-1]=0;
        imgInt[len-1]=0;
        realInt2[0]=0;
        imgInt2[0]=0;
        realInt2[len-1]=0;
        imgInt2[len-1]=0;
        x_in_out[0]=0;
        x_in_out[1]=0;
        x_in_out[(len-1)*2]=0;
        x_in_out[(len-1)*2+1]=0;

        backFFT(realInt,imgInt,len,halfband);
        backFFT(realInt2,imgInt2,len,halfband2);
        backFFT_1(x_in_out,len,halfband3);
//        fftw_destroy_plan(p);
				
        removeDC(halfband,len);
        removeDC(halfband2,len);
        removeDC(halfband3,len);
//			for(i = 0; i < len; i++)
//			{
//			   halfband[i] = halfband[i]*0.999f;
//			   halfband2[i] = halfband2[i]*0.999f;
//				 halfband3[i] = halfband3[i]*0.999f;
//			}

        outlen=len;
//        double* velocity = new double[outlen];
//        double* disp = new double[outlen];
        for(i=0;i<outlen;i++)
        {
            //acc[i]=2*halfband3[i*fs/fs2];
            velocity[i]=2*halfband[i]*1000;
            disp[i]=2*halfband2[i]*100000;
        }
        character[0]= absMax(wave,len);
        character[1]=rms(velocity,len);
        character[2]=pp(disp+outlen/2,len/2);
        character[3]=qiaodu(wave,len,rms(wave,len));


        for(i=0;i<len/2;i++)
           {
               tempForenv[i*2]=x_in_out[i*2+1];
               tempForenv[i*2+1]=0-x_in_out[i*2];

               tempForenv[(len/2+i)*2]=0-x_in_out[(len/2+i)*2+1];
               tempForenv[(len/2+i)*2+1]=x_in_out[(len/2+i)*2];
           }
           for(i=0;i<len;i++)
           {
               real[i]=tempForenv[i*2];
               img[i]=tempForenv[i*2+1];
           }
           for(i=0;i<len;i++)
               {
                   real[i]=tempForenv[i*2];
                   img[i]=tempForenv[i*2+1];
               }

           backFFT(real,img,len,envelopWave);
           for(i=0;i<len;i++)
					 {
						 envelopWave[i]=sqrt(envelopWave[i]*envelopWave[i]+wave[i]*wave[i]);
					 }
        character[4]=absMax(envelopWave,len);
					 
					 
					 
					 
					 
//        fftw_free(wave1);
//        fftw_free(out);
//        fftw_free(tempForenv);
//        delete[] real;
//        delete[] img;
//        delete [] envelopWave;
//        delete[] wave;
//        delete[] halfband;
//        delete[] halfband2;
//        delete[] halfband3;
//        delete[] realInt;
//        delete[] imgInt;
//        delete[] realInt2;
//        delete[] imgInt2;
//        delete[] velocity;
//        delete[] disp;
        return 0;
    }
    else return -1;
}


void DSPF_sp_cfftr2_dit_cn(float* x, float* w, unsigned short n)
{
    unsigned short n2, ie, ia, i, j, k, m;
    float rtemp, itemp, c, s;

    n2 = n;
    ie = 1;

    for(k=n; k > 1; k >>= 1)
    {
        n2 >>= 1;
        ia = 0;

        for(j=0; j < ie; j++)
        {
            c = w[2*j];
            s = w[2*j+1];

            for(i=0; i < n2; i++)
            {
                m = ia + n2;
                rtemp = c * x[2*m] + s * x[2*m+1];
                itemp = c * x[2*m+1] - s * x[2*m];
                x[2*m] = x[2*ia] - rtemp;
                x[2*m+1] = x[2*ia+1] - itemp;
                x[2*ia] = x[2*ia] + rtemp;
                x[2*ia+1] = x[2*ia+1] + itemp;
                ia++;
            }

            ia += n2;
        }
        ie <<= 1;
    }
}


void gen_w_r2(float w[], uint32_t N)
{
    uint32_t i;
    float e = PI * 2.0 / N;

    for(i = 0; i < (N >> 1); i++)
    {
        w[2 * i] = cos(i * e);
        w[2 * i + 1] = sin(i * e);
    }
}

void bit_rev(float x[], uint32_t N)
{
    uint32_t i, j, k;
    float rtemp, itemp;
    j = 0;

    for(i=1; i < (N-1); i++)
    {
        k = N >> 1;

        while(k <= j)
        {
            j -= k;
            k >>= 1;
        }

        j += k;

        if(i < j)
        {
            rtemp = x[j*2];
            x[j*2] = x[i*2];
            x[i*2] = rtemp;
            itemp = x[j*2+1];
            x[j*2+1] = x[i*2+1];
            x[i*2+1] = itemp;
        }
    }
}


void DSPF_sp_icfftr2_dif_cn(float* x, float* w, unsigned short n)
{
    unsigned short n2, ie, ia, i, j, k, m;
    float rtemp, itemp, c, s;

    n2 = 1;
    ie = n;

    for(k=n; k > 1; k >>= 1)
    {
        ie >>= 1;
        ia = 0;

        for(j=0; j < ie; j++)
        {
            c = w[2*j];
            s = w[2*j+1];

            for(i=0; i < n2; i++)
            {
                m = ia + n2;
                rtemp = x[2*ia] - x[2*m];
                x[2*ia] = x[2*ia] + x[2*m];
                itemp = x[2*ia+1] - x[2*m+1];
                x[2*ia+1] = x[2*ia+1] + x[2*m+1];
                x[2*m] = c*rtemp - s*itemp;
                x[2*m+1] = c*itemp + s*rtemp;
                ia++;
            }

            ia += n2;
        }

        n2 <<= 1;
    }
}



float sum(float *dataIn, uint32_t length)
{    
	uint32_t i = 0;
	float sum=0;   
	for(i=0;i<length;i++)
	{       
		sum+=dataIn[i];   
	}    
	return sum;
}

float removeDC(float *dataIn, uint32_t length)
{   
	uint32_t i = 0; 
	float DC=0;    
	DC=sum(dataIn,length)/length;    
	for(i=0;i<length;i++)        
	{
		dataIn[i]-=DC;  
	}		
	return DC;
		
}


float max(float *dataIn,uint32_t length)           //datain为去直流后
{    
	uint32_t i = 0;
	float maxValue=dataIn[0];    
	for(i=1;i<length;i++)    
	{        
		if(dataIn[i]>maxValue)           
		{
			maxValue=dataIn[i];    
		}
	}   
	return maxValue;
}

float min(float *dataIn,uint32_t length)       //datain为去直流后
{ 
	uint32_t i = 0;
	float minValue=dataIn[0];     
	for(i=0;i<length;i++) 
	{         
		if(dataIn[i]<minValue)   
		{
			minValue=dataIn[i];   
		}
	}     
	return minValue;
}

float absAverage(float* dataIn,uint32_t length)        //绝对均值，datain为去直流后
{  
	uint32_t i = 0;
	float temp=0;    
	for(i=0;i<length;i++)        
	{
		temp+=fabs(dataIn[i]);    
	}
	return temp/length;
}

float pp(float *dataIn,uint32_t length)            //峰峰值，datain为去直流后
{
	return max(dataIn,length)-min(dataIn,length);
}

float absMax(float* dataIn,uint32_t length)            //绝对峰值
{
	uint32_t i = 0;
	float* data;
	memcpy(data,dataIn,length*sizeof(float));   
	for(int i=0;i<length;i++)       
	{
			data[i]=fabs(data[i]);  
	}
	float result= max(data,length);       
	return result;
}

float rms(float *dataIn, uint32_t length)      //datain为去直流后
{    
	uint32_t i = 0;
	float temp=0;    
	for(i=0;i<length;i++)    
	{        
		temp+=pow( dataIn[i],2);    
	}    
	return sqrt(temp/length);
}


float pr(float max, float mrms)      //峰值指标
{   
	return max/mrms;
}
float Cp(float mrms,float aver)      //波形指标
{   
	return mrms/aver;
}

float ifff(float peak,float aver)            //脉冲指标
{    
	return peak/aver;
}

float qiaodu(float *dataIn, uint32_t length, float mrms)      //峭度，datain为去直流后
{ 
	uint32_t i = 0;
	float temp=0;   
	for(i=0;i<length;i++)    
	{       
	temp+=pow( dataIn[i],4);    
	}    
	temp=temp/length;
    return temp;
}

float qdzb(float qd,float mrms)      //峭度指标
{    
	return qd/pow(mrms,4);
}

