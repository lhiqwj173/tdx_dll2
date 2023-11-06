#include "stdafx.h"
#include "TCalcFuncSets.h"

writer w;

// 生成的dll及相关依赖dll请拷贝到通达信安装目录的T0002/dlls/下面,再在公式管理器进行绑定

void TestPlugin1(int DataLen, float *pfOUT, float *pfINa, float *pfINb, float *pfINc)
{
	for (int i = 0; i < DataLen; i++)
		pfOUT[i] = i;
}

void TestPlugin2(int DataLen, float *pfOUT, float *pfINa, float *pfINb, float *pfINc)
{
	for (int i = 0; i < DataLen; i++)
	{
		pfOUT[i] = pfINa[i] + pfINb[i] + pfINc[i];
		pfOUT[i] = pfOUT[i] / 3;
	}
}

// 切换标的
void switch_code(int DataLen, float *pfOUT, float *pfINa, float *pfINb, float *pfINc)
{
	w.switch_code(static_cast<int>(pfINa[0]), static_cast<int>(pfINb[0]), static_cast<int>(pfINc[0]));
}

// 即时行情保存到csv中
void save_2_csv(int DataLen, float *pfOUT, float *pfINa, float *pfINb, float *pfINc)
{
	// 只获取第一个即可
	w.write(pfINa[0]);
	w.write(pfINb[0]);
	w.write(pfINc[0]);
}

// 加载的函数
PluginTCalcFuncInfo g_CalcFuncSets[] =
	{
		{1, (pPluginFUNC)&switch_code},
		{2, (pPluginFUNC)&save_2_csv},
		{0, NULL},
};

// 导出给TCalc的注册函数
BOOL RegisterTdxFunc(PluginTCalcFuncInfo **pFun)
{
	if (*pFun == NULL)
	{
		(*pFun) = g_CalcFuncSets;
		return TRUE;
	}
	return FALSE;
}
