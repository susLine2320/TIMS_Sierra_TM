//tims9n.h
//�}�X�R���L�[�A���ȂǁA�����v���݊��@�\��ݒ肵�܂�
class tims9N
{
public:
	int McKey; //�}�X�R���L�[
	int TrainType; //���
	int m_DepartSta; //��ԉw�ϊ��O
	int DepartSta; //��ԉw
	int ArrivalSta; //�~�ԉw
	int SEArea; //�А��p���s����
	int m_Location; //���w������
	int Location; //���w������
	int NowSta; //���݉w�i���i����0�j

	//������
	void Init()
	{
		m_Location = 0;
		Location = 0;
		m_DepartSta = -1;
	}

	//���t���[�����s
	void Execute()
	{
		//m_Location
	}

	//���t���[�����s�~�ԉw
	void SetArrivalSta(int ats172)
	{
		ArrivalSta = ats172;
		if (McKey == 7)
		{
			//���c�}�L�[�Ń��g���ȉ��̍s��̍ۂ͑�X�؏㌴�~��
			if ((ats172 >= 15 && ats172 <= 19) || (ats172 >= 42 && ats172 <= 48) || ats172 == 50 || ats172 == 51 || ats172 == 57)
			{
				ArrivalSta = 41;
			}
		}
		if (McKey == 1)
		{
			//���g���L�[�ŏ��c�}�̍s��̍ۂ͑�X�؏㌴�~��
			if ((ats172 >= 1 && ats172 <= 8) || (ats172 >= 10 && ats172 <= 14) || (ats172 >= 21 && ats172 <= 35))
			{
				ArrivalSta = 41;
			}
			//���g���L�[��JR�̍s��̍ۂ͈����~��
			else if (ats172 >= 15 && ats172 <= 19)
			{
				ArrivalSta = 42;
			}
		}
		if (ats172 == 21)//�V�h�s����1
			SEArea = 1;
		else if (ats172 == 24)//���ؓc�s����3
			SEArea = 3;
		else if (ats172 == 14 || ats172 == 4 || ats172 == 5 || ats172 == 27 || ats172 == 28 || ats172 == 35)//��a�E����E�]�m���E����s����2
			SEArea = 2;
		else//����0
			SEArea = 0;
	}

	//���t���[�����s��ԉw
	void SetDepartSta()
	{
		DepartSta = ConvSta2Dest(m_DepartSta);
		if (McKey == 7)
		{
			//���c�}�L�[�Ń��g���ȉ��̎n���w�̏ꍇ�͑�X�؏㌴���
			if ((DepartSta >= 15 && DepartSta <= 19) || (DepartSta >= 42 && DepartSta <= 48) || DepartSta == 50 || DepartSta == 51 || DepartSta == 57)
			{
				DepartSta = 41;
			}
		}
		if (McKey == 1)
		{
			//���g���L�[�ŏ��c�}�̍s��̍ۂ͑�X�؏㌴�~��
			if ((DepartSta >= 1 && DepartSta <= 8) || (DepartSta >= 10 && DepartSta <= 14) || (DepartSta >= 21 && DepartSta <= 35))
			{
				DepartSta = 41;
			}
			//���g���L�[��JR�̍s��̍ۂ͈����~��
			else if (DepartSta >= 15 && DepartSta <= 19)
			{
				DepartSta = 42;
			}
		}
		if (ConvSta2Dest(m_DepartSta) == 21)//�V�h�n����1
		{
			if (SEArea == 2 || SEArea == 3)
				SEArea += 2;//4or5
			else
				SEArea = 3;
		}
		else if (ConvSta2Dest(m_DepartSta) == 24)//���ؓc�n����3
		{
			if (SEArea == 1)
				SEArea = 5;
			else
				SEArea = 2;
		}
		else if (ConvSta2Dest(m_DepartSta) == 14 || ConvSta2Dest(m_DepartSta) == 4 || ConvSta2Dest(m_DepartSta) == 5 || ConvSta2Dest(m_DepartSta) == 27 || ConvSta2Dest(m_DepartSta) == 28 || ConvSta2Dest(m_DepartSta) == 35)//��a�E����E�]�m���E����n����2
		{
			if (SEArea == 1)
				SEArea = 4;
			else
				SEArea = 1;
		}
		else//����0
		{
			if (SEArea == 2 || SEArea == 3)
				SEArea -= 1;//1or2
			else if (SEArea == 1)
				SEArea = 3;
			else
				SEArea = 0;
		}

	}

	//A���E1�w����
	int SetSEStaA(int sta0)
	{
		int sta1 = sta0;
		switch (sta0)
		{
		case 56: //�k����
			sta1 = 101;
			break;
		case 101: //��������
			sta1 = 57;
			break;
		case 58: //�k��Z
			sta1 = 102;
			break;
		case 102: //��Z�ܕ�
			sta1 = 59;
			break;
		case 62: //����
			sta1 = 103;
			break;
		case 103: //����Y��
			sta1 = 63;
			break;
		case 64: //�V�䒃�m��
			sta1 = 104;
			break;
		case 104: //���ܕ�
			sta1 = 65;
			break;
		case 67: //����J
			sta1 = 105;
			break;
		case 105: //���֐ܕ�
			sta1 = 68;
			break;
		case 68: //���P��
			sta1 = 106;
			break;
		case 106: //����Y��
			sta1 = 69;
			break;
		case 71: //�T�؍�
			sta1 = 107;
			break;
		case 107: //�Q���ܕ�
			sta1 = 72;
			break;
		case 73: //�����_�{
			sta1 = 108;
			break;
		case 108: //��������
			sta1 = 74;
			break;
		case 74: //��X�،���
			sta1 = 31;
			break;
		case 49: //�V�S�����u
			sta1 = SEArea % 3 == 2 ? 93 : 50;
			break;
		case 53: //���c
			sta1 = 13;
			break;
		case 13: //���͑��
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
			sta1 = sta0 - 1; //�]�m�����ʏ�
			break;
		case 1: //����{��
			sta1 = 91;
			break;
		default:
			sta1 = sta0 + 1;//�ʏ펞
			break;
		}
		return sta1;
	}

	//B���E1�w����
	int SetSEStaB(int sta0)
	{
		int sta1 = sta0;
		switch (sta0)
		{
		case 57: //����
			sta1 = 101;
			break;
		case 101: //��������
			sta1 = 56;
			break;
		case 59: //����
			sta1 = 102;
			break;
		case 102: //��Z�ܕ�
			sta1 = 58;
			break;
		case 63: //����
			sta1 = 103;
			break;
		case 103: //����Y��
			sta1 = 62;
			break;
		case 65: //��蒬
			sta1 = 104;
			break;
		case 104: //���ܕ�
			sta1 = 64;
			break;
		case 68: //���P��
			sta1 = 105;
			break;
		case 105: //���֐ܕ�
			sta1 = 67;
			break;
		case 69: //����c�����O
			sta1 = 106;
			break;
		case 106: //����Y��
			sta1 = 68;
			break;
		case 72: //�\�Q��
			sta1 = 107;
			break;
		case 107: //�Q���ܕ�
			sta1 = 71;
			break;
		case 74: //��X�،���
			sta1 = 108;
			break;
		case 108: //��������
			sta1 = 73;
			break;
		case 31: //��X�؏㌴
			sta1 = SEArea / 3 == 0 ? 74 : 30;
			break;
		case 93: //�܌���
			sta1 = 49;
			break;
		case 75: //���c�}���͌�
			sta1 = 13;
			break;
		case 13: //���͑��
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
			sta1 = sta0 + 1; //�]�m�����ʏ�
			break;
		case 91: //����
			sta1 = 1;
			break;
		default:
			sta1 = sta0 - 1;//�ʏ펞
			break;
		}
		return sta1;
	}

	//�w�ԍ����s��ԍ��ɕϊ�
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

	//�s��ԍ����w�ԍ��ɕϊ�
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

	//�����������ʉ߉w�ݒ�
	int SetTrainPass(int sta)
	{
#define PASS 62
#define STOP 63
		int ret = STOP;
		switch (TrainType)
		{
		case 5: //A����
			if (sta == 36)
				ret = PASS;
		case 3: //B����
		case 7:
			if (sta == 33 || sta == 34 || sta == 35)
				ret = PASS;
		case 4: //�ʋΉ���
			if (sta == 37 || sta == 38 || sta == 39 || sta == 40)
				ret = PASS;
			if (TrainType != 7)//���t�����ȊO
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