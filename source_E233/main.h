//main.h
//���̃t�@�C���ł͔ėp�@�\���`���܂��B

int g_emgBrake; //���u���[�L
int g_svcBrake; //��p�ő�u���[�L
int g_brakeNotch; //�u���[�L�m�b�`
int g_powerNotch; //�͍s�m�b�`
int g_reverser; //���o�[�T�[
bool g_pilotLamp; //�^�]�m�m�点��
int g_time; //���ݎ���[ms]
float g_speed; //���x�v�̑��x[km/h]
int g_deltaT; //�O�t���[������̃t���[������[ms/f]

//�݊����[�h�֘A
int snp2Beacon;

//INI�t�@�C���֘A
int PLampindex; //�p�C���b�g�����v�C���f�b�N�X
int snp2Output; //�݊��p�l���o��
int DispType; //�^�]����ʂ�E/M
int EMeter; //���[�^�[�\���̗L����
int ETIMS; //TIMS�\���̗L����
int EVmeter; //�d���v�̗L����
int EUD; //���j�b�g�\�����L����
int D01ABnum; //�\������w��
int Row6; //6�i�ڂ̈���
int LVindex; //�ː��d���v
int HVindex; //�ː��d���v
int BCMRType; //BC/MR���v�d�l
int AMType; //�d���v�d�l
int LineUpdate; //1�s�X�V�̎���
int EbCut; //�u���[�L��������
int Lbinit; //����N�����͍s�x��

TIMS_SierraIni ini;
ATS_HANDLES g_output; // �o��