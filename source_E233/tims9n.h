//tims9n.h
//マスコンキー連動など、うさプラ互換機能を設定します
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
	int NowSta; //現在駅段数（原則0）

	//初期化
	void Init()
	{
		m_Location = 0;
		Location = 0;
		m_DepartSta = -1;
	}

	//毎フレーム実行
	void Execute()
	{
		//m_Location
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
		case 74: //代々木公園
			sta1 = 31;
			break;
		case 49: //新百合ヶ丘
			sta1 = SEArea % 3 == 2 ? 93 : 50;
			break;
		case 53: //町田
			sta1 = 13;
			break;
		case 13: //相模大野
			sta1 = SEArea % 3 == 1 ? 12 : 75;
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
			sta1 = 91;
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
		case 31: //代々木上原
			sta1 = SEArea / 3 == 0 ? 74 : 30;
			break;
		case 93: //五月台
			sta1 = 49;
			break;
		case 75: //小田急相模原
			sta1 = 13;
			break;
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
		case 91: //藤沢
			sta1 = 1;
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
		case 80:
			dest = 6;
			break;
		case 89:
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
		case 78:
			dest = 11;
			break;
		case 82:
			dest = 12;
			break;
		case 85:
			dest = 13;
			break;
		case 8:
			dest = 14;
			break;
		case 87:
			dest = 42;
			break;
		case 56:
			dest = 43;
			break;
		case 74:
			dest = 44;
			break;
		case 58:
			dest = 45;
			break;
		case 73:
			dest = 46;
			break;
		case 68:
			dest = 47;
			break;
		case 63:
			dest = 48;
			break;
		case 72:
			dest = 50;
			break;
		case 65:
			dest = 51;
			break;
		case 62:
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
			sta = 3;
			break;
		case 6:
		case 29:
			sta = 80;
			break;
		case 7:
		case 31:
			sta = 89;
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
			sta = 78;
			break;
		case 12:
			sta = 82;
			break;
		case 13:
			sta = 85;
			break;
		case 14:
			sta = 8;
			break;
		case 42:
			sta = 57;
			break;
		case 43:
			sta = 56;
			break;
		case 44:
			sta = 74;
			break;
		case 45:
			sta = 58;
			break;
		case 46:
			sta = 73;
			break;
		case 47:
			sta = 68;
			break;
		case 48:
			sta = 63;
			break;
		case 50:
			sta = 72;
			break;
		case 51:
			sta = 65;
			break;
		case 57:
			sta = 62;
			break;
		default:
			break;
		}
		return sta;
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

	void SetLocation(int loc)
	{
		m_Location = Location;
		Location = loc;
	}

	//void Execute()
};