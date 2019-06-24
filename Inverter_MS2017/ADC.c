#include "Global.h"
#include "ADC.h"
//#include <assert.h>

#define ADC_PORT_IN    P6IN                     //����
#define ADC_PORT_SEL   P6SEL                    //ѡ��
#define ADC_PORT_DIR   P6DIR                    //����

#define SampleTime0 ADC12SHT0_4
#define SampleTime1 ADC12SHT1_4

void ADC_LoadChannels(ADC_ChannelConfig Channels[16])
{
	uchar i;
	volatile unsigned char* MCTL = &ADC12MCTL0;
	for (i = 0; i < 16; i++)
	{
		*MCTL = Channels[i].Source | Channels[i].Ref << 4;
		if (Channels[i].EOS)
		{
			*MCTL |= ADC12EOS;
			break;
		}
		MCTL++;
	}
	ADC12MCTL15 |= ADC12EOS;//Ensure the last channel terminates conversion
}

void ADC_Init(ADC_InitInfo* InitInfo)
{
	assert(InitInfo->ClkDiv <= 8 && InitInfo->ClkDiv >= 1);

	REFCTL0 = REFMSTR;
	switch (InitInfo->RefVolt)
	{
	case V15:
		REFCTL0 |= REFVSEL_0;
		break;
	case V20:
		REFCTL0 |= REFVSEL_1;
		break;
	case V25:
		REFCTL0 |= REFVSEL_2;
		break;
	default:
	{
		uchar InvalidRef = 0;
		assert(InvalidRef);
	}
	}
	if (InitInfo->RefAlwaysOn)
	{
		REFCTL0 |= REFON;
	}
	if (InitInfo->RefOutput)
	{
		REFCTL0 |= REFOUT;
	}

	ADC12CTL0 &= ~ADC12ENC;//��Enable
	ADC12CTL0 = SampleTime0 | SampleTime1 | ADC12MSC | ADC12ON;//����ʱ�䣬�Զ���ʼת������AD
	if (InitInfo->Repeated)
	{
		//��ʼ��ַ0�������ź�ADC12SC����Ƶ��ACLK��ѭ������ת��
		ADC12CTL1 = ADC12CSTARTADD_0 | ADC12SHS_0 | ADC12SHP | ((InitInfo->ClkDiv - 1) * 0x20u) | ADC12SSEL_1 | ADC12CONSEQ_3;
	}
	else
	{
		//��ʼ��ַ0�������ź�ADC12SC����Ƶ��ACLK����������ת��
		ADC12CTL1 = ADC12CSTARTADD_0 | ADC12SHS_0 | ADC12SHP | ((InitInfo->ClkDiv - 1) * 0x20u) | ADC12SSEL_1 | ADC12CONSEQ_1;
	}
	if (InitInfo->SlowMode)
	{
		//Ԥ��Ƶ4��12bit��Ref������������50ksps
		ADC12CTL2 = ADC12PDIV | ADC12RES_2 | ADC12SR;
	}
	else
	{
		//12bit
		ADC12CTL2 = ADC12RES_2;
	}
	if (!InitInfo->RefAlwaysOn)
	{
		//Ref����Burst mode
		//Burst mode��REFֻ��ת���ڼ������
		ADC12CTL2 |= ADC12REFBURST;
	}

	ADC_LoadChannels(InitInfo->Channels);

	ADC12IE = 0;
	ADC12CTL0 |= ADC12ENC;//��Enable
	if (InitInfo->Repeated)
	{
		ADC12CTL0 |= ADC12SC;
	}
}

void ADC_Tigger(void)
{
	if (!(ADC12CTL1 & ADC12BUSY))//�����ظ�����,Errata ADC42
	{
		ADC12IFG = 0;
		ADC12CTL0 |= ADC12SC;
	}
}

uchar ADC_IsBusy(void)
{
	return ADC12CTL1 & ADC12BUSY ? 1 : 0;
}

ushort ADC_GetResult(uchar Index)
{
	return *((&ADC12MEM0) + Index);
}