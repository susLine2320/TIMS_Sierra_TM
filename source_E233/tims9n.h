//tims9n.h
//ƒ}ƒXƒRƒ“ƒL[˜A“®‚È‚ÇA‚¤‚³ƒvƒ‰ŒİŠ·‹@”\‚ğİ’è‚µ‚Ü‚·
class tims9N
{
public:
	int McKey; //ƒ}ƒXƒRƒ“ƒL[
	int ArrivalSta; //~Ô‰w
	int SEArea; //Ğü—p‘–s•û–Ê

	//–ˆƒtƒŒ[ƒ€Às~Ô‰w
	void SetArrivalSta(int ats172)
	{
		ArrivalSta = ats172;
		if (McKey == 7)
		{
			//¬“c‹}ƒL[‚ÅƒƒgƒˆÈ‰“‚Ìsæ‚ÌÛ‚Í‘ãX–ØãŒ´~Ô
			if ((ats172 >= 15 && ats172 <= 19) || (ats172 >= 42 && ats172 <= 48) || ats172 == 50 || ats172 == 51 || ats172 == 57)
			{
				ArrivalSta = 41;
			}
		}
		if (McKey == 1)
		{
			//ƒƒgƒƒL[‚Å¬“c‹}‚Ìsæ‚ÌÛ‚Í‘ãX–ØãŒ´~Ô
			if ((ats172 >= 1 && ats172 <= 8) || (ats172 >= 10 && ats172 <= 14) || (ats172 >= 21 && ats172 <= 35))
			{
				ArrivalSta = 41;
			}
			//ƒƒgƒƒL[‚ÅJR‚Ìsæ‚ÌÛ‚Íˆ»£~Ô
			else if (ats172 >= 15 && ats172 <= 19)
			{
				ArrivalSta = 42;
			}
		}
		if (ats172 == 21 || ats172 == 24)//VhE“‚–Ø“cs‚«‚Í1
			SEArea = 1;
		else if (ats172 == 14 || ats172 == 4 || ats172 == 5 || ats172 == 27 || ats172 == 28 || ats172 == 35)//‘å˜aE“¡‘òE]ƒm“‡E’·Œãs‚«‚Í2
			SEArea = 2;
		else//‘¼‚Í0
			SEArea = 0;
	}

	//AüE1‰w‘‰Á
	int SetSEStaA(int sta0)
	{
		int sta1 = sta0;
		switch (sta0)
		{
		case 56: //–kˆ»£
			sta1 = 101;
			break;
		case 101: //ˆ»£‰œæ
			sta1 = 57;
			break;
		case 58: //–kçZ
			sta1 = 102;
			break;
		case 102: //çZÜ•Ô
			sta1 = 59;
			break;
		case 62: //ª’Ã
			sta1 = 103;
			break;
		case 103: //“’“‡Yü
			sta1 = 63;
			break;
		case 64: //VŒä’ƒƒm…
			sta1 = 104;
			break;
		case 104: //‘åèÜ•Ô
			sta1 = 65;
			break;
		case 67: //“ú”ä’J
			sta1 = 105;
			break;
		case 105: //‰àŠÖÜ•Ô
			sta1 = 68;
			break;
		case 68: //‰àƒPŠÖ
			sta1 = 106;
			break;
		case 106: //‰àŠÖYü
			sta1 = 69;
			break;
		case 71: //”T–Øâ
			sta1 = 107;
			break;
		case 107: //Q“¹Ü•Ô
			sta1 = 72;
			break;
		case 73: //–¾¡_‹{
			sta1 = 108;
			break;
		case 108: //Œö‰€‘¤ü
			sta1 = 74;
			break;
		case 74: //‘ãX–ØŒö‰€
			sta1 = 31;
			break;
		case 49: //V•S‡ƒ–‹u
			sta1 = SEArea == 1 ? 93 : 50;
			break;
		case 53: //’¬“c
			sta1 = 13;
			break;
		case 13: //‘Š–Í‘å–ì
			sta1 = SEArea == 2 ? 12 : 75;
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
			sta1 = sta0 - 1; //]ƒm“‡ü’Êí
			break;
		case 1: //“¡‘ò–{’¬
			sta1 = 91;
			break;
		default:
			sta1 = sta0 + 1;//’Êí
			break;
		}
		return sta1;
	}

	//BüE1‰w‘‰Á
	int SetSEStaB(int sta0)
	{
		int sta1 = sta0;
		switch (sta0)
		{
		case 57: //ˆ»£
			sta1 = 101;
			break;
		case 101: //ˆ»£‰œæ
			sta1 = 56;
			break;
		case 59: //’¬‰®
			sta1 = 102;
			break;
		case 102: //çZÜ•Ô
			sta1 = 58;
			break;
		case 63: //“’“‡
			sta1 = 103;
			break;
		case 103: //“’“‡Yü
			sta1 = 62;
			break;
		case 65: //‘åè’¬
			sta1 = 104;
			break;
		case 104: //‘åèÜ•Ô
			sta1 = 64;
			break;
		case 68: //‰àƒPŠÖ
			sta1 = 105;
			break;
		case 105: //‰àŠÖÜ•Ô
			sta1 = 67;
			break;
		case 69: //‘‰ï‹c–“°‘O
			sta1 = 106;
			break;
		case 106: //‰àŠÖYü
			sta1 = 68;
			break;
		case 72: //•\Q“¹
			sta1 = 107;
			break;
		case 107: //Q“¹Ü•Ô
			sta1 = 71;
			break;
		case 74: //‘ãX–ØŒö‰€
			sta1 = 108;
			break;
		case 108: //Œö‰€‘¤ü
			sta1 = 73;
			break;
		case 31: //‘ãX–ØãŒ´
			sta1 = SEArea == 0 ? 74 : 30;
			break;
		case 93: //ŒÜŒ‘ä
			sta1 = 49;
			break;
		case 75: //¬“c‹}‘Š–ÍŒ´
			sta1 = 13;
			break;
		case 13: //‘Š–Í‘å–ì
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
			sta1 = sta0 + 1; //]ƒm“‡ü’Êí
			break;
		case 91: //“¡‘ò
			sta1 = 1;
			break;
		default:
			sta1 = sta0 - 1;//’Êí
			break;
		}
		return sta1;
	}

	//sæ”Ô†‚ğ‰w”Ô†‚É•ÏŠ·
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

	//void Execute()
};