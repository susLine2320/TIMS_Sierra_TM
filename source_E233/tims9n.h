//tims9n.h
//�}�X�R���L�[�A���ȂǁA�����v���݊��@�\��ݒ肵�܂�
class tims9N
{
public:
	int McKey; //�}�X�R���L�[
	int ArrivalSta; //�~�ԉw
	int SEArea; //�А��p���s����

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
		if (ats172 == 21 || ats172 == 24)//�V�h�E���ؓc�s����1
			SEArea = 1;
		else if (ats172 == 14 || ats172 == 4 || ats172 == 5 || ats172 == 27 || ats172 == 28 || ats172 == 35)//��a�E����E�]�m���E����s����2
			SEArea = 2;
		else//����0
			SEArea = 0;
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
			sta1 = SEArea == 1 ? 93 : 50;
			break;
		case 53: //���c
			sta1 = 13;
			break;
		case 13: //���͑��
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
			sta1 = SEArea == 0 ? 74 : 30;
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

	//void Execute()
};