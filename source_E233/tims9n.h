//tims9n.h
//マスコンキー連動など、うさプラ互換機能を設定します

#define TIMS_DECELERATION 19.5F //減速定数（減速度[km/h/s] * 7.2）
#define TIMS_OFFSET 0.5F //車上子オフセット

class tims9N
{
public:
	int McKey; //マスコンキー
	int TrainType; //種別
	int m_DepartSta; //乗車駅変換前
	int DepartSta; //乗車駅
	int ArrivalSta; //降車駅
	int SEArea; //社線用走行方面
	int m_Location; //自駅距離程
	int Location; //次駅距離程
	int Array;
	int NowSta; //現在駅段数（原則0）
	int SESta[10]; //社線駅名
	int DispSESta[10];
	int SEDirection; //社線進行方向
	//int SEArea; //社線用走行方面
	int HiddenLine[10]; //更新時に1行ずつ非表示にする

	//初期化
	void Init()
	{
		m_Location = 0;
		Location = 0;
		m_DepartSta = -1;

		m_dist = 0;
		m_array = 0;
		m_array2 = 0;

		m_pushUpFlag = -1;
		m_pushUpBeacon = 0;
		m_pushUpCount = 0;
		m_tmrVisible = g_time;
	}

	//毎フレーム実行
	void Execute()
	{
		float speed = fabsf(g_speed); //速度の絶対値[km/h]
		float def = speed / 3600 * g_deltaT; //1フレームで動いた距離（絶対値）[m]
		m_dist -= def; //残り距離を減算する
		m_array -= def;
		Location = m_array;
		if (m_array != 0 && m_array >= m_array2 * 2 / 3) //残距離2/3以上
			Array = 1;
		else if (m_array != 0 && m_array >= m_array2 / 3)
			Array = 2;
		else if (m_array > 0)
			Array = 3;
		else
			Array = 0;
		Array += max(NowSta, 0) * 4;
		if (Array > 16) { Array = 16; }

		//[TIMS9Nのみ]駅名の挿入
		for (int i = 0; i < 7; i++)
		{
			if (SEDirection == 1) //営団A線・小田急下り
			{
				SESta[i + 1] = SetSEStaA(SESta[i]);
			}
			else //営団B線・小田急上り
			{
				SESta[i + 1] = SetSEStaB(SESta[i]);
			}
			if (i <= 4)
			{
				DispSESta[i] = SESta[i];
			}
		}

		NowSta = -1;//1段目

		//降車駅がSESta[0]～SESta[3]と一致する場合
		for (int i = 0; i < 5; i++)
		{
			if (SESta[i] == ConvDest2Sta(ArrivalSta))
			{
				NowSta = 4 - i; //段数を入れる
				if (i < 4)
				{
					DispSESta[4] = SESta[i]; //強制的に5駅目とする
					for (int j = 4; j > 0; j--)//4～0の駅名をあてはめ
					{
						if (SEDirection == 1) //営団A線・小田急下り
						{
							DispSESta[j - 1] = SetSEStaB(DispSESta[j]);
						}
						else //営団B線・小田急上り
						{
							DispSESta[j - 1] = SetSEStaA(DispSESta[j]);
						}
					}
					for (int j = 4; j < 7; j++)//5～7の駅名をあてはめ
					{
						if (SEDirection == 1) //営団A線・小田急下り
						{
							DispSESta[j + 1] = SetSEStaA(DispSESta[j]);
						}
						else //営団B線・小田急上り
						{
							DispSESta[j + 1] = SetSEStaB(DispSESta[j]);
						}
					}
				}
				break;
			}
		}
		//次駅を点滅させる
		if (speed * speed / TIMS_DECELERATION >= m_dist - 50 && m_dist > 0) //速度照査（パターン)
		{
			//m_blinking = true;

			if (m_pushUpFlag == 1) //停車駅のとき
			{
				m_pushUpFlag = 2; //2 = 次2キロ割ったら更新
			}
		}

		//プッシュアップイベント
		if (g_speed != 0) //駅ジャンプを除外する
		{
			if ((m_pushUpFlag == 2 && m_tisFlag == 1 && (m_pushUpBeacon == 1 || g_speed < 2.0f)) || (m_pushUpFlag == -1 && m_tisFlag == 1 && m_dist <= 0)) //停通パターンにあたって2キロを割るか通過で距離程を割ると実行
			{
				m_pushUpFlag = 0;
				m_tisFlag = 0;

				//ステップ更新の回数だけループ
				for (; m_pushUpCount > 0; m_pushUpCount--)
				{
					if (m_pushUpCount > 1)
					{
						for (int i = 0; i < 10; i++)
						{
							//PushUp(i);
						}
					}
					else
					{
						m_tmrVisible = g_time;
						for (int i = 0; i < 10; i++)
						{
							//m_update[i] = 1;

							//3回目の時、次駅を最新にする、採時駅を最新にする、列番を更新する
							if (i == 3)
							{
								PushSESta();
							}
						}
					}
				}

				//ページめくりしない
				if (m_option > 0)
				{
					PushSESta();
				}
			}
		}
		//以後駅ジャンプ
		else if (m_pushUpFlag == 3 || m_pushUpBeacon == 2)
		{
			m_pushUpFlag = 0;
			PushSESta();
		}
		else if (m_pushUpFlag == -1) //起動時の初期化
		{
			PushSESta();
		}
		//ステップ更新の処理
		for (int i = 0; i < 10; i++)
		{
			if (g_time >= m_tmrVisible + (i * (LineUpdate * (2 / 3))) && g_time <= m_tmrVisible + LineUpdate * (i + 1) - (LineUpdate * (1 / 3)))
			{
				HiddenLine[i] = 1;
				/*
				if (m_update[i] == 1)
				{
					m_update[i] = 0;
					PushUp(i);
				}*/
			}
			else
			{
				HiddenLine[i] = 0;
			}
		}
	}

	//次駅接近SE（622）
	void RecieveSE(int data, int option)
	{
		m_pushUpFlag = data >= 0 ? 1 : -1;
		m_pushUpBeacon = 0;
		{
			m_pushUpCount = NowSta >= 0 ? 0 : 1;
			option = NowSta >= 0 ? 1 : 0;
		}
		m_option = abs(option) > 0 ? 1 : 0;

		m_dist = abs(data % 10000) - TIMS_OFFSET;
		m_blinking = false;

		if (g_speed == 0) //駅ジャンプを除外する
		{
			m_pushUpFlag = abs(data) < 100000 ? 3 : 0;
			m_pushUpCount = 0;
			m_dist = 0;
		}
	}

	//社線用駅名の設定（10，70，604）
	void SetSESta(int sta)
	{
		if (m_DepartSta == -1) {m_DepartSta = sta; }//104番地上子がない時、最初の駅名を入れる
		m_seSta = sta; //駅名データを入れる
		m_tisFlag = 1;
	}

	void SetSEDirection(int data, int area)
	{
		SEDirection = data == 0 ? -1 : 1; //上り列車では減算
		/*
		if (area >= 51)
			SEArea = 1;//下り多摩線・上り新宿行き
		if (area >= 31)
			SEArea = 2;//下り江ノ島線
		else
			SEArea = 0;//下り小田原線・上り千代田線
			*/
	}

	//毎フレーム実行降車駅
	void SetArrivalSta(int ats172)
	{
		ArrivalSta = ats172;
		if (McKey == 7)
		{
			//小田急キーでメトロ以遠の行先の際は代々木上原降車
			if ((ats172 >= 15 && ats172 <= 19) || (ats172 >= 42 && ats172 <= 48) || ats172 == 50 || ats172 == 51 || ats172 == 57)
			{
				ArrivalSta = 41;
			}
		}
		if (McKey == 1)
		{
			//メトロキーで小田急の行先の際は代々木上原降車
			if ((ats172 >= 1 && ats172 <= 8) || (ats172 >= 10 && ats172 <= 14) || (ats172 >= 21 && ats172 <= 35))
			{
				ArrivalSta = 41;
			}
			//メトロキーでJRの行先の際は綾瀬降車
			else if (ats172 >= 15 && ats172 <= 19)
			{
				ArrivalSta = 42;
			}
		}
		if (ats172 == 21)//新宿行きは1
			SEArea = 1;
		else if (ats172 == 24)//唐木田行きは3
			SEArea = 3;
		else if (ats172 == 14 || ats172 == 4 || ats172 == 5 || ats172 == 27 || ats172 == 28 || ats172 == 35)//大和・藤沢・江ノ島・長後行きは2
			SEArea = 2;
		else//他は0
			SEArea = 0;
	}

	//毎フレーム実行乗車駅
	void SetDepartSta()
	{
		DepartSta = ConvSta2Dest(m_DepartSta);
		if (McKey == 7)
		{
			//小田急キーでメトロ以遠の始発駅の場合は代々木上原乗車
			if ((DepartSta >= 15 && DepartSta <= 19) || (DepartSta >= 42 && DepartSta <= 48) || DepartSta == 50 || DepartSta == 51 || DepartSta == 57)
			{
				DepartSta = 41;
			}
		}
		if (McKey == 1)
		{
			//メトロキーで小田急の行先の際は代々木上原降車
			if ((DepartSta >= 1 && DepartSta <= 8) || (DepartSta >= 10 && DepartSta <= 14) || (DepartSta >= 21 && DepartSta <= 35))
			{
				DepartSta = 41;
			}
			//メトロキーでJRの行先の際は綾瀬降車
			else if (DepartSta >= 15 && DepartSta <= 19)
			{
				DepartSta = 42;
			}
		}
		if (ConvSta2Dest(m_DepartSta) == 21)//新宿始発は1
		{
			if (SEArea == 2 || SEArea == 3)
				SEArea += 2;//4or5
			else
				SEArea = 3;
		}
		else if (ConvSta2Dest(m_DepartSta) == 24)//唐木田始発は3
		{
			if (SEArea == 1)
				SEArea = 5;
			else
				SEArea = 2;
		}
		else if (ConvSta2Dest(m_DepartSta) == 14 || ConvSta2Dest(m_DepartSta) == 4 || ConvSta2Dest(m_DepartSta) == 5 || ConvSta2Dest(m_DepartSta) == 27 || ConvSta2Dest(m_DepartSta) == 28 || ConvSta2Dest(m_DepartSta) == 35)//大和・藤沢・江ノ島・長後始発は2
		{
			if (SEArea == 1)
				SEArea = 4;
			else
				SEArea = 1;
		}
		else//他は0
		{
			if (SEArea == 2 || SEArea == 3)
				SEArea -= 1;//1or2
			else if (SEArea == 1)
				SEArea = 3;
			else
				SEArea = 0;
		}

	}

	//A線・1駅増加
	int SetSEStaA(int sta0)
	{
		int sta1 = sta0;
		switch (sta0)
		{
			/*
		case 56: //北綾瀬
			sta1 = 101;
			break;
		case 101: //綾瀬奥取
			sta1 = 57;
			break;
		case 58: //北千住
			sta1 = 102;
			break;
		case 102: //千住折返
			sta1 = 59;
			break;
		case 62: //根津
			sta1 = 103;
			break;
		case 103: //湯島Y線
			sta1 = 63;
			break;
		case 64: //新御茶ノ水
			sta1 = 104;
			break;
		case 104: //大手折返
			sta1 = 65;
			break;
		case 67: //日比谷
			sta1 = 105;
			break;
		case 105: //霞関折返
			sta1 = 68;
			break;
		case 68: //霞ケ関
			sta1 = 106;
			break;
		case 106: //霞関Y線
			sta1 = 69;
			break;
		case 71: //乃木坂
			sta1 = 107;
			break;
		case 107: //参道折返
			sta1 = 72;
			break;
		case 73: //明治神宮
			sta1 = 108;
			break;
		case 108: //公園側線
			sta1 = 74;
			break;
			*/
		case 119: //代々木公園
			sta1 = 31;
			break;
		case 120: //代々木上原
			sta1 = 32;
			break;
		case 49: //新百合ヶ丘
		case 84:
			sta1 = SEArea % 3 == 2 ? 85 : 50;
			break;
			/*
		case 53: //町田
			sta1 = 13;
			break;
			*/
		case 13: //相模大野
		case 54: //相模大野
			sta1 = SEArea % 3 == 1 ? 12 : 55;
			break;
		case 12:
		case 11:
		case 10:
		case 9:
		case 8:
		case 7:
		case 6:
		case 5:
		case 4:
		case 3:
		case 2:
			sta1 = sta0 - 1; //江ノ島線通常
			break;
		case 1: //藤沢本町
			sta1 = 79; //藤沢
			break;
		case 0: //何もない時はそのままループさせておく
			sta1 = 0;
			break;
		default:
			sta1 = sta0 + 1;//通常時
			break;
		}
		return sta1;
	}

	//B線・1駅増加
	int SetSEStaB(int sta0)
	{
		int sta1 = sta0;
		switch (sta0)
		{
			/*
		case 57: //綾瀬
			sta1 = 101;
			break;
		case 101: //綾瀬奥取
			sta1 = 56;
			break;
		case 59: //町屋
			sta1 = 102;
			break;
		case 102: //千住折返
			sta1 = 58;
			break;
		case 63: //湯島
			sta1 = 103;
			break;
		case 103: //湯島Y線
			sta1 = 62;
			break;
		case 65: //大手町
			sta1 = 104;
			break;
		case 104: //大手折返
			sta1 = 64;
			break;
		case 68: //霞ケ関
			sta1 = 105;
			break;
		case 105: //霞関折返
			sta1 = 67;
			break;
		case 69: //国会議事堂前
			sta1 = 106;
			break;
		case 106: //霞関Y線
			sta1 = 68;
			break;
		case 72: //表参道
			sta1 = 107;
			break;
		case 107: //参道折返
			sta1 = 71;
			break;
		case 74: //代々木公園
			sta1 = 108;
			break;
		case 108: //公園側線
			sta1 = 73;
			break;
			*/
		case 31: //代々木上原
		case 120:
			sta1 = SEArea / 3 == 0 ? 119 : 30;
			break;
		case 85: //五月台
			sta1 = 49;
			break;
		case 84: //新百合
			sta1 = 48;
			break;
			/*
		case 75: //小田急相模原
			sta1 = 13;
			break;
			*/
		case 13: //相模大野
			sta1 = 53;
			break;
		case 12:
		case 11:
		case 10:
		case 9:
		case 8:
		case 7:
		case 6:
		case 5:
		case 4:
		case 3:
		case 2:
		case 1:
			sta1 = sta0 + 1; //江ノ島線通常
			break;
		case 79: //藤沢
			sta1 = 1;
			break;
		case 0: //何もない時はそのままループさせておく
			sta1 = 0;
			break;
		default:
			sta1 = sta0 - 1;//通常時
			break;
		}
		return sta1;
	}

	//駅番号を行先番号に変換
	int ConvSta2Dest(int sta)
	{
		int dest = sta;
		switch (sta)
		{
		case 3:
			dest = 2;
			break;
		case 13:
			dest = 1;
			break;
		case 60:
			dest = 6;
			break;
		case 73:
			dest = 7;
			break;
		case 45:
			dest = 8;
			break;
		case 31:
			dest = 9;
			break;
		case 37:
			dest = 10;
			break;
		case 58:
			dest = 11;
			break;
		case 62:
			dest = 12;
			break;
		case 65:
			dest = 13;
			break;
		case 8:
			dest = 14;
			break;
		case 95:
			dest = 42;
			break;
		case 93:
			dest = 43;
			break;
		case 119:
			dest = 44;
			break;
		case 96:
			dest = 45;
			break;
		case 117:
			dest = 46;
			break;
		case 110:
			dest = 47;
			break;
		case 103:
			dest = 48;
			break;
		case 116:
			dest = 50;
			break;
		case 106:
			dest = 51;
			break;
		case 101:
			dest = 57;
			break;
		default:
			dest = 0;
			break;
		}
		return dest;
	}

	//行先番号を駅番号に変換
	int ConvDest2Sta(int dest)
	{
		int sta = dest;
		switch (dest)
		{
		case 1:
		case 26:
			sta = 13;
			break;
		case 2:
		case 25:
			sta = 53;
			break;
		case 6:
		case 29:
			sta = 60;
			break;
		case 7:
		case 31:
			sta = 73;
			break;
		case 8:
		case 22:
			sta = 45;
			break;
		case 9:
		case 41:
			sta = 31;
			break;
		case 10:
			sta = 37;
			break;
		case 11:
			sta = 58;
			break;
		case 12:
			sta = 62;
			break;
		case 13:
			sta = 65;
			break;
		case 14:
			sta = 8;
			break;
		case 42:
			sta = 95;
			break;
		case 43:
			sta = 93;
			break;
		case 44:
			sta = 119;
			break;
		case 45:
			sta = 96;
			break;
		case 46:
			sta = 117;
			break;
		case 47:
			sta = 110;
			break;
		case 48:
			sta = 103;
			break;
		case 50:
			sta = 116;
			break;
		case 51:
			sta = 106;
			break;
		case 57:
			sta = 101;
			break;
		default:
			break;
		}
		return sta;
	}

	//うさプラ駅名を変換
	int ConvUsao2TIMS(int sta)
	{
		int nsta = sta;
		switch (sta)
		{
		case 13:
			return 95;
		case 56:
			return 93;
		case 57:
			return 95;
		case 58:
			return 96;
		case 59:
			return 98;
		case 60:
			return 99;
		case 61:
			return 100;
		case 62:
			return 101;
		case 63:
			return 103;
		case 64:
			return 104;
		case 65:
			return 106;
		case 66:
			return 107;
		case 67:
			return 108;
		case 68:
			return 110;
		case 69:
			return 112;
		case 70:
			return 113;
		case 71:
			return 114;
		case 72:
			return 116;
		case 73:
			return 117;
		case 74:
			return 119;
		case 75:
			return 120;
		default:
			return sta + 150;
		}
	}


	//東西線向け通過駅設定
	int SetTrainPass(int sta)
	{
#define PASS 62
#define STOP 63
		int ret = STOP;
		switch (TrainType)
		{
		case 5: //A快速
			if (sta == 36)
				ret = PASS;
		case 3: //B快速
		case 7:
			if (sta == 33 || sta == 34 || sta == 35)
				ret = PASS;
		case 4: //通勤快速
			if (sta == 37 || sta == 38 || sta == 39 || sta == 40)
				ret = PASS;
			if (TrainType != 7)//東葉快速以外
				break;
			if (sta == 42 || sta == 43 || sta == 45 || sta == 47 || sta == 48)
				ret = PASS;
			break;
		}
		return ret;
	}

	//行路表矢印
	void SetLocation(int loc)
	{
		//m_Location = Location;
		//Location = loc;
		if (g_speed != 0) {//駅ジャンプ以外
			m_array = loc;
			m_array2 = loc;
		}
	}

private:
	float m_dist; //停止予定点距離
	bool m_blinking; //次駅点滅かどうか

	float m_array;
	float m_array2;

	int m_pushUpFlag; //表示更新のフラグ
	int m_pushUpBeacon; //表示更新の地上子
	int m_pushUpCount; //表示更新の繰り返し数

	int m_tisFlag; //TIS表示更新のフラグ
	int m_seSta; //社線用駅名ストック

	int m_tmrVisible; //モニタのステップ更新
	int m_update[7]; //ステップ更新の状態
	int m_option; //次駅と採時駅のみ更新

	//社線用駅名を更新
	void PushSESta(void)
	{
		SESta[0] = m_seSta;
	}
};