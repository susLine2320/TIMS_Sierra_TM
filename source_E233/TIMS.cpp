/* >>EXPORT
◆ファイル名
TIMS.dll: TIMS表示器モジュール

◆ファイルの説明
このファイルはTIMS表示器モジュール。
DetailManager.dllとの併用専用。

ドア継電器が別途必要。
ビルド時には必ずPanelを電車仕様か列車使用か選択すること。
*/
#include "stdafx.h"
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "atsplugin.h"
#include "TIMSini.h"    // #include "Ats.h"の上に書く
#include "TIMS.h"

CTims g_tims; // TIMS装置
CSpp g_spp; // 停車駅通過防止装置
CDate g_date; // 日付

#ifdef __cplusplus
extern "C" {
#endif

BOOL APIENTRY DllMain(HINSTANCE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch(ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_tims.BrakeNotch = &g_brakeNotch;
		g_tims.PowerNotch = &g_powerNotch;
		g_tims.Reverser = &g_reverser;
		g_tims.Time = &g_time;
		g_tims.TrainSpeed = &g_speed;
		g_tims.DeltaT = &g_deltaT;
		g_spp.TrainSpeed = &g_speed;
		g_spp.DeltaT = &g_deltaT;
		g_spp.BrakeNotch = &g_brakeNotch;
		/*
		g_sub.BrakeNotch = &g_brakeNotch;
		g_sub.PowerNotch = &g_powerNotch;
		g_sub.Reverser = &g_reverser;
		g_sub.TrainSpeed = &g_speed;
		g_accel.Time = &g_time;
		g_accel.TrainSpeed = &g_speed;
		g_accel.DeltaT = &g_deltaT;
		*/
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	//ファイルパス格納
	char filePath[_MAX_PATH + 1] = _T("");
	//検索文字列へのポインタ
	char* posIni;
	bool loadCheck; // INIファイルのロードに成功したかどうか
	//Ats.dllのファイルパスを取得
	::GetModuleFileName((HMODULE)hModule, filePath, _MAX_PATH);
	//パスから.dllの位置を検索
	posIni = strstr(filePath, ".dll");
	//.dllを.iniに置換
	memmove(posIni, ".ini", 4);
	// INIファイルをロードして結果を取得
	loadCheck = ini.load(filePath);

	return true;
}

#ifdef __cplusplus
};
#endif

ATS_API void WINAPI Load(void)
{
	g_tims.load();
}

ATS_API void WINAPI Dispose(void)
{
}

ATS_API int WINAPI GetPluginVersion(void)
{
	return ATS_VERSION;
}

ATS_API void WINAPI SetVehicleSpec(ATS_VEHICLESPEC vehicleSpec)
{
	g_svcBrake = vehicleSpec.BrakeNotches;
	g_emgBrake = g_svcBrake + 1;

	g_tims.EmergencyNotch = g_emgBrake;
	g_spp.ServiceNotch = vehicleSpec.AtsNotch;
	// g_sub.EmergencyNotch = g_emgBrake;
}

ATS_API void WINAPI Initialize(int brake)
{
	g_tims.event_lbInit = ini.Emulate.lbInit; // 初回起動時力行遅延
	g_tims.ini_cutPressTime = ini.Emulate.ebCutPressure; // ブレーキ圧を減圧する時間
	ini_UseJRTIMS = ini.Disp.UseJRTIMS; //JRTIMS使用可否
	ini_UseUnit = ini.Disp.UseUnit; //ユニット灯使用可否
	ini_UseDistance = ini.Disp.UseDistance; //走行距離使用可否
	ini_Line = ini.Disp.Line; //路線定義

	g_speed = 0;
	g_tims.initialize();
	g_spp.InitSpp();
	g_date.initialize();
	// g_sub.initialize();
	// g_accel.initialize();
}

ATS_API ATS_HANDLES WINAPI Elapse(ATS_VEHICLESTATE vehicleState, int *panel, int *sound)
{
	g_deltaT = vehicleState.Time - g_time;
	g_time = vehicleState.Time;
	g_speed = vehicleState.Speed;

	g_tims.Location = vehicleState.Location; // 列車位置[m]
	g_tims.BcPressure = vehicleState.BcPressure; // ブレーキシリンダ圧力[kPa]
	g_tims.MrPressure = vehicleState.MrPressure; // 元空気ダメ圧力[kPa]
	g_tims.Current = vehicleState.Current; // 電流[A]

	McKey = panel[160];

	g_tims.execute(); // TIMS装置
	g_spp.RunSpp(); // 停車駅通過防止装置

	g_date.update(); // 日時
	// g_sub.execute(); // その他

	// ハンドル出力
	if(g_tims.BcPressCut == 1)
	{
		g_output.Brake = 1; // EB緩解時にブレーキを減圧する
	}
	else
	{
		g_output.Brake = g_brakeNotch;
	}

	g_output.Reverser = g_reverser;

	if(g_time > g_tims.AccelCutting && g_tims.VCB_ON == 1) // 力行遅延とVCB
	{
		g_output.Power = g_powerNotch;
	}
	else
	{
		g_output.Power = 0;
	}

	g_output.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;

	// パネル出力
	//panel[226] = g_tims.AmmeterC; // 電流計[符号]
	//panel[227] = g_tims.Ammeter1; // 電流計[1000位]
	//panel[228] = g_tims.Ammeter2; // 電流計[100位]
	//panel[229] = g_tims.Ammeter3; // 電流計[10位]
	//panel[230] = g_tims.Ammeter4; // 電流計[1位]
	panel[252] = g_tims.Ammeter0; // 電流計[A]

	// panel[29] = g_sub.Key; // マスコンキー
	panel[30] = g_tims.AccelDelay; // 力行表示
	panel[41] = g_tims.BrakeDelay; // ブレーキ段
	panel[42] = g_tims.BrakeDelay == g_emgBrake ? 1 : 0; // 非常ブレーキ
	/* うさプラ併用
	panel[58] = (g_time / 3600000) % 24; // デジタル時
	panel[59] = g_time / 60000 % 60; // デジタル分
	panel[60] = g_time / 1000 % 60; // デジタル秒
	*//* うさプラ併用でオミット
	panel[41] = g_tims.UnitState[0]; // ユニット表示灯1
	panel[42] = g_tims.UnitState[1]; // ユニット表示灯2
	panel[43] = g_tims.UnitState[2]; // ユニット表示灯3
	panel[44] = g_tims.UnitState[3]; // ユニット表示灯4
	*/

	panel[17] = g_tims.Speed100; // 速度(100km/hの桁)
	panel[18] = g_tims.Speed10; // 速度(10km/hの桁)
	panel[20] = g_tims.Speed1; // 速度(1km/hの桁)
	panel[21] = g_tims.Speed; // 速度計

	// panel[46] = (g_speed != 0) && (g_accel.accel_kmhs > 2.2F) && (vehicleState.Current > 0) ? 1 + ((g_time % 1200) / 600) : 0; // 空転(点滅)
	// panel[47] = (g_speed != 0) && (g_accel.accel_kmhs < -5.2F) ? 1 + ((g_time % 1200) / 600) : 0; // 滑走(点滅)

//ブレーキ圧ここから
	/*
	panel[87] = g_tims.BcPress0; // ブレーキシリンダ指針(0-180kPa)
	panel[88] = g_tims.BcPress1; // ブレーキシリンダ指針(200-380kPa)
	panel[89] = g_tims.BcPress2; // ブレーキシリンダ指針(400-580kPa)
	panel[90] = g_tims.BcPress3; // ブレーキシリンダ指針(600-780kPa)
	panel[97] = g_tims.MrPress0; // 元空気ダメ指針(750-795kPa)
	panel[98] = g_tims.MrPress1; // 元空気ダメ指針(800-845kPa)
	panel[99] = g_tims.MrPress2; // 元空気ダメ指針(850-895kPa)*/
	panel[48] = g_tims.BcCaution ? ((g_time % 1000) / 500) : 0; // 200kPa警告
	/*
	//追加分、デジタル表示の場合はこれを使用
	panel[87] = g_tims.BcPress4; // ブレーキシリンダ指針(100位)
	panel[88] = g_tims.BcPress5; // ブレーキシリンダ指針(10位)
	panel[89] = g_tims.BcPress6; // ブレーキシリンダ指針(1位)
	panel[90] = g_tims.BcPress7; // ブレーキシリンダ指針(int)
	panel[97] = g_tims.MrPress4; // 元空気ダメ指針(100位)
	panel[98] = g_tims.MrPress5; // 元空気ダメ指針(10位)
	panel[99] = g_tims.MrPress6; // 元空気ダメ指針(1位)
	panel[100] = g_tims.MrPress7; // 元空気ダメ指針(int)
	*/
//ブレーキ圧終り

//TIMS全般表示ここから
	panel[239] = g_tims.TimsSpeed100; // TIMS速度計(100km/hの桁)
	panel[240] = g_tims.TimsSpeed10; // TIMS速度計(10km/hの桁)
	panel[241] = g_tims.TimsSpeed1; // TIMS速度計(1km/hの桁)

	if (ini_UseDistance != 0)
	{
		panel[13] = g_tims.Distance1000; // 走行距離(kmの桁)
		panel[14] = g_tims.Distance100; // 走行距離(100mの桁)
		panel[15] = g_tims.Distance10; // 走行距離(10mの桁)
	}
	if (ini_UseUnit != 0) //ユニット灯使用時
	{
		panel[83] = g_tims.UnitTims[0]; // TIMSユニット表示1
		panel[84] = g_tims.UnitTims[1]; // TIMSユニット表示2
		panel[85] = g_tims.UnitTims[2]; // TIMSユニット表示3
		//panel[145] = g_tims.UnitTims[3]; // TIMSユニット表示4
	}
	panel[94] = g_tims.ArrowDirection; // 進行方向矢印
	panel[95] = g_tims.TrainArrow; // 行路表矢印

	panel[69] = McKey != 6 ? 0 : g_tims.Kind; // 列車種別
	panel[74] = McKey != 6 ? 0 : g_tims.Number[0]; // 列車番号(千の桁)
	panel[75] = McKey != 6 ? 0 : g_tims.Number[1]; // 列車番号(百の桁)
	panel[76] = McKey != 6 ? 0 : g_tims.Number[2]; // 列車番号(十の桁)
	panel[77] = McKey != 6 ? 0 : g_tims.Number[3]; // 列車番号(一の桁)
	panel[70] = McKey != 6 ? 0 : g_tims.Charactor; // 列車番号(記号)

	panel[97] = McKey == 6 && g_tims.Number[3] != 0 ? 1 : 0; // 設定完了
	panel[165] = McKey != 6 ? 0 : g_tims.PassMode; // 通過設定
	//うさプラ併用 panel[119] = g_tims.NextBlinkLamp; // 次駅停車表示灯
	panel[49] = McKey == 6 ? 0 : g_tims.From; // 運行パターン発駅
	panel[50] = McKey == 6 ? 0 : g_tims.Destination; // 運行パターン着駅
	panel[234] = McKey == 6 ? 0 : g_tims.RelayName; // 降車駅駅名表示
//TIMS全般表示終り

//TIMS電圧類ここから
	//うさプラ併用でオミット panel[217] = g_tims.AC; // 交流
	panel[25] = g_tims.DC; // 直流
	panel[28] = g_tims.CVacc; // 制御電圧異常
	panel[78] = g_tims.CVacc10; // 制御電圧(10位)
	panel[79] = g_tims.CVacc1; // 制御電圧(1位)
	/* うさプラ併用でオミット
	panel[222] = g_tims.ACacc; // 交流電圧異常
	panel[223] = g_tims.ACacc10000; // 交流電圧(10000位)
	panel[224] = g_tims.ACacc1000; // 交流電圧(1000位)
	panel[225] = g_tims.ACacc100; // 交流電圧(100位)
	*/
	panel[28] = g_tims.DCacc; // 直流電圧異常
	panel[32] = g_tims.DCacc1000; // 直流電圧(1000位)
	panel[33] = g_tims.DCacc100; // 直流電圧(100位)
	panel[34] = g_tims.DCacc10; // 直流電圧(10位)

	panel[253] = g_tims.Cvmeter; // 制御指針
	//うさプラ併用でオミット panel[231] = g_tims.Acmeter; // 交流指針
	panel[254] = g_tims.Dcmeter; // 直流指針
	
	panel[245] = g_tims.Accident; // 事故
	panel[246] = g_tims.Tp; // 三相
	panel[247] = g_tims.VCB; // VCB(DigitalNumber、0無表示～1入～2切)
	//うさプラ併用でオミット panel[235] = g_tims.alert_ACDC > 0 ? g_tims.alert_ACDC + ((g_time % 800) / 400) : 0; // 交直切替(点滅、無表示0、点滅1-2、3-4でペア)
//TIMS電圧類終り

//電列共通部分ここから
	panel[43] = g_tims.HiddenLine[0] ? 0 : McKey != 6 ? g_tims.SESta[0] : g_tims.Station[0]; // 駅名表示1
	panel[44] = g_tims.HiddenLine[1] ? 0 : McKey != 6 ? g_tims.SESta[1] : g_tims.Station[1]; // 駅名表示2
	panel[45] = g_tims.HiddenLine[2] ? 0 : McKey != 6 ? g_tims.SESta[2] : g_tims.Station[2]; // 駅名表示3
	panel[46] = g_tims.HiddenLine[3] ? 0 : McKey != 6 ? g_tims.SESta[3] : g_tims.Station[3]; // 駅名表示4
	panel[47] = g_tims.HiddenLine[4] ? 0 : McKey != 6 ? g_tims.SESta[4] : g_tims.Station[4]; // 駅名表示5

	panel[4] = ini_Line != 5 && McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Arrive[0][1]; // 到着時刻1M
	panel[10] = ini_Line != 5 && McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Arrive[1][1]; // 到着時刻2M
	panel[11] = ini_Line != 5 && McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Arrive[2][1]; // 到着時刻3M
	panel[12] = ini_Line != 5 && McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Arrive[3][1]; // 到着時刻4M
	panel[16] = ini_Line != 5 && McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Arrive[4][1]; // 到着時刻5M

	if (ini_UseJRTIMS != 0)
	{
		panel[147] = McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Arrive[0][0]; // 到着時刻1H
		panel[51] = McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Arrive[0][2]; // 到着時刻1S
		panel[148] = McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Arrive[1][0]; // 到着時刻2H
		panel[52] = McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Arrive[1][2]; // 到着時刻2S
		panel[158] = McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Arrive[2][0]; // 到着時刻3H
		panel[53] = McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Arrive[2][2]; // 到着時刻3S

		panel[159] = McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Arrive[3][0]; // 到着時刻4H
		panel[54] = McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Arrive[3][2]; // 到着時刻4S
		panel[162] = McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Arrive[4][0]; // 到着時刻5H
		panel[57] = McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Arrive[4][2]; // 到着時刻5S

	// うさプラ併用でオミット
	//panel[150] = g_tims.HiddenLine[0] ? 0 : g_tims.Leave[0][0]; // 出発時刻1H
		panel[194] = McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Leave[0][1]; // 出発時刻1M
		panel[195] = McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Leave[0][2]; // 出発時刻1S
		//panel[153] = g_tims.HiddenLine[1] ? 0 : g_tims.Leave[1][0]; // 出発時刻2H
		panel[196] = McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Leave[1][1]; // 出発時刻2M
		panel[197] = McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Leave[1][2]; // 出発時刻2S
		//panel[156] = g_tims.HiddenLine[2] ? 0 : g_tims.Leave[2][0]; // 出発時刻3H
		panel[198] = McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Leave[2][1]; // 出発時刻3M
		panel[199] = McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Leave[2][2]; // 出発時刻3S

		//panel[159] = g_tims.HiddenLine[3] ? 0 : g_tims.Leave[3][0]; // 出発時刻4H
		panel[200] = McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Leave[3][1]; // 出発時刻4M
		panel[201] = McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Leave[3][2]; // 出発時刻4S
		//panel[162] = g_tims.HiddenLine[4] ? 0 : g_tims.Leave[4][0]; // 出発時刻5H
		panel[202] = McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Leave[4][1]; // 出発時刻5M
		panel[203] = McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Leave[4][2]; // 出発時刻5S

		panel[204] = McKey != 6 ? 0 : g_tims.HiddenLine[0] ? 0 : g_tims.Track[0]; // 次駅到着番線1
		panel[205] = McKey != 6 ? 0 : g_tims.HiddenLine[1] ? 0 : g_tims.Track[1]; // 次駅到着番線2
		panel[206] = McKey != 6 ? 0 : g_tims.HiddenLine[2] ? 0 : g_tims.Track[2]; // 次駅到着番線3
		panel[208] = McKey != 6 ? 0 : g_tims.HiddenLine[3] ? 0 : g_tims.Track[3]; // 次駅到着番線4
		panel[209] = McKey != 6 ? 0 : g_tims.HiddenLine[4] ? 0 : g_tims.Track[4]; // 次駅到着番線5
	//電列共通部分ここまで

	//列車スタフここから
	/*
		panel[170] = g_tims.HiddenLine[5] ? 0 : g_tims.Station[5]; // 駅名表示6
		panel[171] = g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][0]; // 到着時刻6H
		panel[172] = g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][1]; // 到着時刻6M
		panel[173] = g_tims.HiddenLine[5] ? 0 : g_tims.Arrive[5][2]; // 到着時刻6S
		panel[174] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][0]; // 出発時刻6H
		panel[175] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][1]; // 出発時刻6M
		panel[176] = g_tims.HiddenLine[5] ? 0 : g_tims.Leave[5][2]; // 出発時刻6S
		panel[177] = g_tims.HiddenLine[5] ? 0 : g_tims.Track[5]; // 次駅到着番線6

		panel[178] = g_tims.HiddenLine[0] ? 0 : g_tims.Span[0][0]; // 駅間走行時間12M
		panel[179] = g_tims.HiddenLine[0] ? 0 : g_tims.Span[0][1]; // 駅間走行時間12S
		panel[180] = g_tims.HiddenLine[1] ? 0 : g_tims.Span[1][0]; // 駅間走行時間23M
		panel[181] = g_tims.HiddenLine[1] ? 0 : g_tims.Span[1][1]; // 駅間走行時間23S
		panel[182] = g_tims.HiddenLine[2] ? 0 : g_tims.Span[2][0]; // 駅間走行時間34M
		panel[183] = g_tims.HiddenLine[2] ? 0 : g_tims.Span[2][1]; // 駅間走行時間34S
		panel[184] = g_tims.HiddenLine[3] ? 0 : g_tims.Span[3][0]; // 駅間走行時間45M
		panel[185] = g_tims.HiddenLine[3] ? 0 : g_tims.Span[3][1]; // 駅間走行時間45S
		panel[186] = g_tims.HiddenLine[4] ? 0 : g_tims.Span[4][0]; // 駅間走行時間56M
		panel[187] = g_tims.HiddenLine[4] ? 0 : g_tims.Span[4][1]; // 駅間走行時間56S

		panel[188] = g_tims.HiddenLine[0] ? 0 : g_tims.Limit[0]; // 制限速度1IN
		panel[189] = g_tims.HiddenLine[0] ? 0 : g_tims.Limit2[0]; // 制限速度1OUT
		panel[190] = g_tims.HiddenLine[1] ? 0 : g_tims.Limit[1]; // 制限速度2IN
		panel[191] = g_tims.HiddenLine[1] ? 0 : g_tims.Limit2[1]; // 制限速度2OUT
		panel[192] = g_tims.HiddenLine[2] ? 0 : g_tims.Limit[2]; // 制限速度3IN
		panel[193] = g_tims.HiddenLine[2] ? 0 : g_tims.Limit2[2]; // 制限速度3OUT
		panel[194] = g_tims.HiddenLine[3] ? 0 : g_tims.Limit[3]; // 制限速度4IN
		panel[195] = g_tims.HiddenLine[3] ? 0 : g_tims.Limit2[3]; // 制限速度4OUT
		panel[196] = g_tims.HiddenLine[4] ? 0 : g_tims.Limit[4]; // 制限速度5IN
		panel[197] = g_tims.HiddenLine[4] ? 0 : g_tims.Limit2[4]; // 制限速度5OUT
		panel[198] = g_tims.HiddenLine[5] ? 0 : g_tims.Limit[5]; // 制限速度6IN
		panel[199] = g_tims.HiddenLine[5] ? 0 : g_tims.Limit2[5]; // 制限速度6OUT
	//列車スタフここまで

	*/
	//電車スタフここから
		// うさプラ併用でオミット
		panel[163] = McKey != 6 ? 0 : g_tims.After; // 次採時駅表示
		panel[164] = McKey != 6 ? 0 : g_tims.AfterStationTimeA[0]; // 次採時駅到着時刻H
		panel[167] = McKey != 6 ? 0 : g_tims.AfterStationTimeA[1]; // 次採時駅到着時刻M
		panel[168] = McKey != 6 ? 0 : g_tims.AfterStationTimeA[2]; // 次採時駅到着時刻S
		//panel[174] = g_tims.AfterStationTimeL[0]; // 次採時駅出発時刻H
		panel[169] = McKey != 6 ? 0 : g_tims.AfterStationTimeL[1]; // 次採時駅出発時刻M
		panel[170] = McKey != 6 ? 0 : g_tims.AfterStationTimeL[2]; // 次採時駅出発時刻S
		panel[171] = McKey != 6 ? 0 : g_tims.AfterTrack; // 次駅到着番線

		panel[177] = McKey != 6 ? 0 : g_tims.Before; // 直前採時駅駅名表示
		panel[173] = McKey != 6 ? 0 : g_tims.BeforeStationTime[0]; // 直前採時駅到着時刻H
		panel[174] = McKey != 6 ? 0 : g_tims.BeforeStationTime[1]; // 直前採時駅到着時刻M
		panel[175] = McKey != 6 ? 0 : g_tims.BeforeStationTime[2]; // 直前採時駅到着時刻S
		panel[180] = McKey != 6 ? 0 : g_tims.BeforeTrack; // 直前採時駅駅名表示

		panel[210] = McKey != 6 ? 0 : g_tims.RelayName; // 降車駅駅名表示
		panel[211] = McKey != 6 ? 0 : g_tims.LastStopTime[0]; // 降車駅到着時刻H
		panel[212] = McKey != 6 ? 0 : g_tims.LastStopTime[1]; // 降車駅到着時刻M
		panel[213] = McKey != 6 ? 0 : g_tims.LastStopTime[2]; // 降車駅到着時刻S
		panel[214] = McKey != 6 ? 0 : g_tims.LastStopTimeL[0]; // 降車駅発車時刻H
		panel[215] = McKey != 6 ? 0 : g_tims.LastStopTimeL[1]; // 降車駅発車時刻M
		panel[216] = McKey != 6 ? 0 : g_tims.LastStopTimeL[2]; // 降車駅発車時刻S
		panel[217] = McKey != 6 ? 0 : g_tims.LastStopTrack; // 降車駅到着番線

		panel[218] = McKey != 6 ? 0 : g_tims.AfterKind; // 次行路列番
		panel[219] = McKey != 6 ? 0 : g_tims.AfterNumber[0]; // 次行路列番
		panel[220] = McKey != 6 ? 0 : g_tims.AfterNumber[1]; // 次行路列番
		panel[221] = McKey != 6 ? 0 : g_tims.AfterNumber[2]; // 次行路列番
		panel[222] = McKey != 6 ? 0 : g_tims.AfterNumber[3]; // 次行路列番
		panel[223] = McKey != 6 ? 0 : g_tims.AfterChara; // 次行路列番

		panel[224] = McKey != 6 ? 0 : g_tims.AfterTime[0] ? 0 : g_tims.AfterTime[0][0]; // 次行路到着時刻H
		panel[225] = McKey != 6 ? 0 : g_tims.AfterTime[0] ? 0 : g_tims.AfterTime[0][1]; // 次行路到着時刻M
		panel[226] = McKey != 6 ? 0 : g_tims.AfterTime[0] ? 0 : g_tims.AfterTime[0][2]; // 次行路到着時刻S
		panel[227] = McKey != 6 ? 0 : g_tims.AfterTime[1] ? 0 : g_tims.AfterTime[1][0]; // 次行路出発時刻H
		panel[228] = McKey != 6 ? 0 : g_tims.AfterTime[1] ? 0 : g_tims.AfterTime[1][1]; // 次行路出発時刻M
		panel[229] = McKey != 6 ? 0 : g_tims.AfterTime[1] ? 0 : g_tims.AfterTime[1][2]; // 次行路出発時刻S
	//*/
	//電車スタフ終り
	}


//電列共通部分ここから
	/* うさプラ併用でオミット
	panel[215] = g_tims.HiddenLine[3] ? 0 : (g_tims.Next * g_tims.NextBlink); // 駅名表示(次駅、点滅する)
	panel[216] = g_tims.HiddenLine[3] ? 0 : g_tims.NextTime[0]; // 到着時刻(次駅、時)
	panel[217] = g_tims.HiddenLine[3] ? 0 : g_tims.NextTime[1]; // 到着時刻(次駅、分)
	panel[218] = g_tims.HiddenLine[3] ? 0 : g_tims.NextTime[2]; // 到着時刻(次駅、秒)
	panel[219] = g_tims.HiddenLine[3] ? 0 : g_tims.NextTrack; // 次駅到着番線
	*/
	panel[61] = McKey != 6 ? 0 : g_tims.For; // 列車行先
//電列共通部分ここまで

//そのほか機能

	// panel[212] = g_date.Cooler; // 冷房状態


/* >>EXPORT
◆Panelインデックス
ats22 速度計
ats23 速度(100km/hの桁)
ats24 速度(10km/hの桁)
ats25 速度(1km/hの桁)
ats26 電流計[A]
ats30 ブレーキ段
ats31 非常ブレーキ
ats37 デジタル時
ats38 デジタル分
ats39 デジタル秒
ats41 ユニット表示灯1
ats42 ユニット表示灯2
ats43 ユニット表示灯3
ats44 ユニット表示灯4
ats110 ブレーキシリンダ指針(0-180kPa)
ats111 ブレーキシリンダ指針(200-380kPa)
ats112 ブレーキシリンダ指針(400-580kPa)
ats113 ブレーキシリンダ指針(600-780kPa)
ats114 元空気ダメ指針(750-795kPa)
ats115 元空気ダメ指針(800-845kPa)
ats116 元空気ダメ指針(850-895kPa)
ats145 TIMS速度計(100km/hの桁)
ats146 TIMS速度計(10km/hの桁)
ats147 TIMS速度計(1km/hの桁)
ats148 走行距離(kmの桁)
ats149 走行距離(100mの桁)
ats150 TIMSユニット表示1
ats151 TIMSユニット表示2
ats152 TIMSユニット表示3
ats153 TIMSユニット表示4
ats154 進行方向矢印

ats155 列車種別
→インデックス変更

ats157 列車番号(千の桁)
ats158 列車番号(百の桁)
ats159 列車番号(十の桁)
ats160 列車番号(一の桁)
ats161 列車番号(記号)
ats162 設定完了
ats163 通過設定
ats165 駅名表示1
ats166 駅名表示2
ats167 駅名表示3
ats168 到着時刻1H
ats169 到着時刻1M
ats170 到着時刻1S
ats171 到着時刻2H
ats172 到着時刻2M
ats173 到着時刻2S
ats174 到着時刻3H
ats175 到着時刻3M
ats176 到着時刻3S
ats177 出発時刻1H
ats178 出発時刻1M
ats179 出発時刻1S
ats180 出発時刻2H
ats181 出発時刻2M
ats182 出発時刻2S
ats183 出発時刻3H
ats184 出発時刻3M
ats185 出発時刻3S
ats186 駅間走行時間12M
ats187 駅間走行時間12S
ats188 駅間走行時間23M
ats189 駅間走行時間23S
ats190 次駅到着番線1
ats191 次駅到着番線2
ats192 次駅到着番線3
ats193 制限速度1IN
ats194 制限速度1OUT
ats195 制限速度2IN
ats196 制限速度2OUT
ats197 制限速度3IN
ats198 制限速度3OUT
ats199 駅名表示(次駅、点滅する)
ats200 到着時刻(次駅、時)
ats201 到着時刻(次駅、分)
ats202 到着時刻(次駅、秒)
ats203 次駅到着番線
ats206 200kPa警告
ats208 次駅停車表示灯
ats217 交流
ats218 直流
ats219 制御電圧異常
ats220 制御電圧(10位)
ats221 制御電圧(1位)
ats222 交流電圧異常
ats223 交流電圧(10000位)
ats224 交流電圧(1000位)
ats225 交流電圧(100位)
ats226 直流電圧異常
ats227 直流電圧(1000位)
ats228 直流電圧(100位)
ats229 直流電圧(10位)
ats230 制御指針
ats231 交流指針
ats232 直流指針
ats233 事故
ats234 三相
ats236 VCB(DigitalNumber、0無表示～1入～2切)
ats235 交直切替(点滅)

◇追加機能
ats155 TIMS列車種別(再掲)
※156→155に変更しました
ats156 TIMS行路表矢印
※155→156に変更しました

ats164 TIMS行先

ats213 TIMS運行パターン発駅
ats214 TIMS運行パターン着駅
*/

	// サウンド出力
	// sound[90] = g_sub.KeyOn;
	// sound[91] = g_sub.KeyOff;
	// sound[25+50] = g_tims.RelayD;
	sound[225] = g_tims.LbInit;
	sound[100] = g_tims.AirApply;
	sound[101] = g_tims.AirApplyEmg;
	sound[103] = g_tims.AirHigh;
	sound[105] = g_tims.EmgAnnounce;
	sound[106] = g_tims.UpdateInfo;
	// sound[106] = (sound[106] == ATS_SOUND_PLAYLOOPING) ? ATS_SOUND_CONTINUE : (g_date.Cooler == 1 ? ATS_SOUND_PLAYLOOPING : ATS_SOUND_STOP);
	sound[14+50] = McKey != 6 ? ATS_SOUND_STOP : g_spp.HaltChime3;
	sound[15+50] = McKey != 6 ? ATS_SOUND_STOP : g_spp.HaltChime;
	sound[16+50] = McKey != 6 ? ATS_SOUND_STOP : g_spp.PassAlarm;
	sound[17+46] = McKey != 6 ? ATS_SOUND_STOP : g_spp.HaltChime2;

/* >>EXPORT
◆Soundインデックス
ats15 誤通過防止 (停車、1回)
ats16 誤通過防止 (通過)
ats17 誤通過防止 (停車、ループ)
ats100 ブレーキ昇圧音
ats101 ブレーキ昇圧音(非常)
ats103 非常ブレーキ緩解音
ats105 非常ブレーキ放送
ats106 運行情報更新チャイム
ats225 HB(LbInit)
*/

	return g_output;
}

ATS_API void WINAPI SetPower(int notch)
{
	g_powerNotch = notch;
	g_tims.RandomizeSeed();
}

ATS_API void WINAPI SetBrake(int notch)
{
	g_tims.PlaySoundAirHigh(g_brakeNotch, notch);
	g_tims.PlaySoundEmgAnnounce(g_brakeNotch, notch);
	g_tims.PlaySoundAirApplyEmg(g_brakeNotch, notch);
	g_spp.NotchChanged();

	g_brakeNotch = notch;
	g_tims.RandomizeSeed();
}

ATS_API void WINAPI SetReverser(int pos)
{
	g_reverser = pos;
	g_tims.SetLbInit(pos, g_tims.event_lbInit);
}

ATS_API void WINAPI KeyDown(int atsKeyCode)
{
	/*
	switch(atsKeyCode)
	{
	case ATS_KEY_I: // マスコンキー抜取
		g_sub.KeyEvent(0);
		break;
	case ATS_KEY_J: // マスコンキー投入
		g_sub.KeyEvent(1);
		break;
	}
	*/
}

ATS_API void WINAPI KeyUp(int atsKeyCode)
{
}

/* >>EXPORT
◆キーアサイン
なし
*/

ATS_API void WINAPI HornBlow(int hornType)
{
}

ATS_API void WINAPI DoorOpen(void)
{
	g_pilotlamp = false;
	g_tims.DoorOpening();
	g_spp.StopChime(); // 停通のチャイムを止める
}

ATS_API void WINAPI DoorClose(void)
{
	g_pilotlamp = true;
	g_tims.DoorClosing();
}

ATS_API void WINAPI SetSignal(int signal)
{
	g_tims.CheckPath(signal);
}

/* >>EXPORT
◆Signalインデックス
なし
*/

ATS_API void WINAPI SetBeaconData(ATS_BEACONDATA beaconData)
{
	switch(beaconData.Type)
	{
	case ATS_BEACON_SPP: // 停車駅通過防止装置
		if(g_speed != 0){g_spp.Receive(beaconData.Optional % 10000);} // 駅ジャンプを除外する
		break;
	case 10: //メトロTIS駅名読み込み
		g_tims.SetSESta(beaconData.Optional);
		break;
	case 70: //小田急TIS駅名読み込み
		g_tims.SetSESta(beaconData.Optional / 100);
		break;
	case 89: //方向判定・系統設定（下り31～江ノ島線、51～多摩線、上り51～新宿行き）
		g_tims.SetSEDirection(beaconData.Optional % 2, beaconData.Optional);
		break;
	case 603: //駅名強制割込み
		g_tims.SetSEStaExtra(beaconData.Optional);
		break;
	case 604: //
	/*
	case ATS_BEACON_UPD: // TIMS更新宣言
		g_tims.UpdateView();
		break;
	*/
		/*20180824暫定撤去
	case ATS_BEACON_APP: // TIMS次駅接近
		if(g_speed != 0){g_spp.Receive(beaconData.Optional % 10000);} // 駅ジャンプを除外する
		g_tims.Receive(beaconData.Optional); // 駅ジャンプを除外しない
		break;
		*/
	//20180824暫定版
	case ATS_BEACON_APP: // TIMS次駅接近
		if(g_speed != 0){g_spp.Receive(beaconData.Optional % 10000);} // 駅ジャンプを除外する
		g_tims.Receive(beaconData.Optional % 10000000, beaconData.Optional / 10000000); // 駅ジャンプを除外しない
		break;

	case ATS_BEACON_NEXT: // TIMS次駅設定
		g_tims.SetNext(beaconData.Optional);
		break;
	case ATS_BEACON_NEXTTIME: // TIMS次駅到着時刻設定
		g_tims.SetNextTime(beaconData.Optional);
		break;
	case ATS_BEACON_NEXTTRL: // TIMS次駅到着番線設定
		g_tims.SetNextTrack(beaconData.Optional);
		break;
	case ATS_BEACON_KIND: // TIMS列車種別
		g_tims.SetKind(beaconData.Optional);
		break;
	case ATS_BEACON_NUM: // TIMS列車番号
		g_tims.SetNumber(beaconData.Optional / 100, beaconData.Optional % 100);
		break;
	case ATS_BEACON_STA: // TIMS駅名表示
		g_tims.InputLine(1, (beaconData.Optional / 1000) - 1, beaconData.Optional % 1000); //1000で割ってその-1を桁番号に、余りは駅番号
		break;
	case ATS_BEACON_ARV: // TIMS到着時刻
		g_tims.InputLine(2, (beaconData.Optional / 1000000) - 1, beaconData.Optional % 1000000); //1000000で割ってその-1を桁番号に、余りは時刻
		break;
	case ATS_BEACON_LEV: // TIMS出発時刻
		g_tims.InputLine(3, (beaconData.Optional / 1000000) - 1, beaconData.Optional % 1000000); //1000000で割ってその-1を桁番号に、余りは時刻
		break;
	case ATS_BEACON_TRL: // TIMS到着番線
		g_tims.InputLine(4, (beaconData.Optional / 100) - 1, (beaconData.Optional % 100)); // 到着番線、100で割ってその-1を桁番号に、余りは番線
		break;
	case ATS_BEACON_LIM1: // TIMS制限速度IN
		g_tims.InputLine(5, (beaconData.Optional / 100) - 1, (beaconData.Optional % 100)); // 制限速度IN、100で割ってその-1を桁番号に、余りは制限
		break;
	case ATS_BEACON_LIM2: // TIMS制限速度OUT
		g_tims.InputLine(6, (beaconData.Optional / 100) - 1, (beaconData.Optional % 100)); // 制限速度OUT、100で割ってその-1を桁番号に、余りは制限
		break;
	case ATS_BEACON_SPN: // TIMS駅間時間
		g_tims.InputLine(0, (beaconData.Optional / 10000) - 1, beaconData.Optional % 10000); //10000で割ってその-1を桁番号に、余りは駅間時間
		break;
	/*
	case ATS_BEACON_PATH: // TIMS開通情報
		g_tims.CheckPath(beaconData.Signal, beaconData.Distance, beaconData.Optional);
		break;
	case ATS_BEACON_CRWL: // TIMS徐行区間
		g_tims.SetCrawl(beaconData.Optional);
		break;
	*/
	case ATS_BEACON_LEG: // TIMS運行区間
		g_tims.SetLeg(beaconData.Optional);
		break;
	
	case ATS_BEACON_DIST: // TIMS距離加算設定
		g_tims.SetDistance(beaconData.Distance, beaconData.Optional);
		break;
	/*
	case ATS_BEACON_DISDEF: // TIMS列車位置補正
		g_tims.SetPositionDef(beaconData.Optional);
		break;
	*/

	case ATS_BEACON_SUBK: // TIMS次運用種別
		g_tims.SetAfteruent(3, beaconData.Optional, 0);
		break;
	
	case ATS_BEACON_SUBN: // TIMS次運用列番
		g_tims.SetAfteruent(0, beaconData.Optional / 100, beaconData.Optional % 100);
		break;
	
	case ATS_BEACON_SUBA: // TIMS次運用着時
		g_tims.SetAfteruent(1, beaconData.Optional, 0);
		break;
	case ATS_BEACON_SUBL: // TIMS次運用発時
		g_tims.SetAfteruent(2, beaconData.Optional, 0);
		break;
	
	case ATS_BEACON_DIR: // TIMS進行方向設定
		g_tims.SetDirection(beaconData.Optional);
		break;
	
	case ATS_BEACON_LSA: // TIMS終点到着時刻
		g_tims.SetLastStop(0, beaconData.Optional);
		break;
	case ATS_BEACON_LSL: // TIMS終点出発時刻
		g_tims.SetLastStop(1, beaconData.Optional);
		break;
	case ATS_BEACON_LST: // TIMS終点到着番線
		g_tims.SetLastStop(2, beaconData.Optional);
		break;
	case ATS_BEACON_LSN: // TIMS行路終点駅名
		g_tims.SetRelayStation(beaconData.Optional);
		break;
	
	case ATS_BEACON_ARW: // TIMS行路表矢印
		g_tims.SetArrowState(beaconData.Optional);
		break;

	case ATS_BEACON_FOR: // TIMS行路表行先
		g_tims.SetFor(beaconData.Optional);
		break;



	case ATS_BEACON_BTSL: // TIMS直前採時駅時刻
		g_tims.SetTimeStationTime(0, beaconData.Optional);
		break;
	case ATS_BEACON_ATSA: // TIMS次採時駅着時刻
		g_tims.SetTimeStationTime(1, beaconData.Optional);
		break;
	case ATS_BEACON_ATSL: // TIMS次採時駅発時刻
		g_tims.SetTimeStationTime(2, beaconData.Optional);
		break;
	case ATS_BEACON_BTST: // TIMS直前採時駅番線
		g_tims.SetTimeStationTime(3, beaconData.Optional);
		break;
	case ATS_BEACON_ATST: // TIMS次採時駅番線
		g_tims.SetTimeStationTime(4, beaconData.Optional);
		break;

	case ATS_BEACON_BTSN: // TIMS直前採時駅駅名
		g_tims.SetTimeStationName(0, beaconData.Optional);
		break;
	case ATS_BEACON_ATSN: // TIMS次採時駅駅名
		g_tims.SetTimeStationName(1, beaconData.Optional);
		break;

	
	case 120: // 切替お知らせ
		g_tims.AlartACDC(beaconData.Optional);
		break;
	case 121: // 交直切替
		g_tims.SetACDC(beaconData.Optional);
		break;
	}
}

/* >>EXPORT
◆Beaconインデックス
30 停車駅通過防止装置
sendData[m] に停止位置までの距離を設定、通過駅の場合は負の値

100 TIMS列車番号
sendData に列番表示器のシフト単位 (記号2桁+列番4桁) を設定

101 TIMS列車種別
sendData に種別表示器のシフト単位を設定
sendData+100 で通過設定

102 TIMS進行方向設定
sendData に列車の進行方向 (TIMS画面の左方向1、右方向2) を設定

103 TIMS距離加算設定
sendData に設定距離/10mの値を設定
前進して減るタイプは+10000する
(例) 23k05の場合、2305を設定

105 TIMS次駅接近
sendData[m] に停止位置までの距離を設定、SPP (#30) も同時に実行される
通過駅の場合は sendData を負の値にして設定する
sendData のオプションとして、 以下が使用できます (併用不可)
　一万の桁: ステップ更新する列数の設定、設定なし･デフォルトは1
　十万の桁: 一斉更新する列数の設定、更新する列数を設定
　百万の桁: 駅情報を更新しない設定、1を設定する

106 TIMS次駅設定
次駅名は sendData に駅番号 (自駅2桁+次駅2桁) を設定

107 TIMS次駅到着時刻設定
次駅時刻は HHmmss の形式で6桁を設定 (e.g. 21:36:15 → 213615)

108 TIMS次駅到着番線設定
次駅到着番線は sendData に到着番線画像シフト番号を設定、0を無表示とする

110 TIMS駅名表示
sendData は以下のように設定する
　一の桁: 駅番号
　十の桁: 〃
　百の桁以上: 設定行の番号 (1～10)
(例） 設定1行目、駅番号が12の場合、112を設定

111 TIMS到着時刻
sendData は以下のように設定する
　一の桁: 到着時刻(秒)の一の位
　十の桁: 到着時刻(秒)の十の位
　百の桁: 到着時刻(分)の一の位
　千の桁: 到着時刻(分)の十の位
　一万の桁: 到着時刻(時)の一の位
　十万の桁: 到着時刻(時)の十の位
　百万の桁以上: 設定行の番号 (1～10)
(例） 設定6行目、到着時刻が12:34:50の場合、6123450を設定
(例2) 設定3行目、到着時刻が無表示の場合、3246060を設定

112 TIMS出発時刻
sendData は以下のように設定する
　一の桁: 出発時刻(秒)の一の位
　十の桁: 出発時刻(秒)の十の位
　百の桁: 出発時刻(分)の一の位
　千の桁: 出発時刻(分)の十の位
　一万の桁: 出発時刻(時)の一の位
　十万の桁: 出発時刻(時)の十の位
　百万の桁以上: 設定行の番号 (1～10)
(例） 設定2行目、出発時刻が22:15:05の場合、2221505を設定
(例2) 設定1行目、出発時刻が無表示の場合、1246060を設定

113 TIMS到着番線
sendData は以下のように設定する
　一の桁: 到着番線の一の位
　十の桁: 到着番線の十の位
　百の桁: 設定行の番号 (1～10)
(例） 設定4行目、到着番線が12番線の場合、412を設定

114 TIMS制限速度IN
sendData は以下のように設定する
　一の桁: 制限速度の一の位 (5の倍数)
　十の桁: 制限速度の十の位
　百の桁: 設定行の番号 (1～10)
(例） 設定4行目、制限速度が35km/hの場合、435を設定

115 TIMS制限速度OUT
sendData は以下のように設定する
　一の桁: 制限速度の一の位 (5の倍数)
　十の桁: 制限速度の十の位
　百の桁: 設定行の番号 (1～10)

116 TIMS駅間時間
sendData は以下のように設定する
　一の桁: 駅間走行時間(秒)の一の位
　十の桁: 駅間走行時間(秒)の十の位
　百の桁: 駅間走行時間(分)の一の位
　千の桁: 駅間走行時間(分)の十の位
　一万の桁以上: 設定行の番号 (1～10)
(例） 設定1行目、駅間走行時間が3分45秒の場合、10345を設定

120 交直切替お知らせ(仮)
0で無効、1で有効

121 交直切替(仮)
0VCB切、1無電区間、2AC有電区間、3DC有電区間

◇追加機能
104 TIMS運行パターン
駅名は sendData に駅番号 (発駅2桁+着駅2桁) を設定

109 TIMS行先設定
駅名は sendData に駅番号 (行先2桁) を設定

117 TIMS行路表矢印
sendData に行路表矢印のシフト単位を設定

◇v0.04からの追加機能

118 TIMS次行路着時刻
sendData は以下のように設定する
　一の桁: 到着時刻(秒)の一の位
　十の桁: 到着時刻(秒)の十の位
　百の桁: 到着時刻(分)の一の位
　千の桁: 到着時刻(分)の十の位
　一万の桁: 到着時刻(時)の一の位
　十万の桁: 到着時刻(時)の十の位

119 TIMS次行路発時刻
sendData は以下のように設定する
　一の桁: 出発時刻(秒)の一の位
　十の桁: 出発時刻(秒)の十の位
　百の桁: 出発時刻(分)の一の位
　千の桁: 出発時刻(分)の十の位
　一万の桁: 出発時刻(時)の一の位
　十万の桁: 出発時刻(時)の十の位

*/
