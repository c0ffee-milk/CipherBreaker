#include "GA.h"

int main() {
    printf("����Ƶ�ʷ������Ŵ��㷨�ƽ���滻����\n\n");
    printf("ʹ��˵����\n�뽫�����ܵ�txt��ʽ���ı�����Ϊcode.txt����txt��ʽ�������ı���������Ϊdata.txt�����Ҷ������exe�ļ�ͬһĿ¼�£����ܺ��ı���������Ϊdecode.txt������ͬһĿ¼��\n");
    printf("��ע�⣺\n��ȷ�������ı��㹻������3000�����ʣ��������ı�����Խ��������Խ׼ȷ\n");
    while (1) {
        printf("�밴��1��ʼ�������룬����0�˳�����\n");
        int x;
        scanf("%d", &x);
        if (x == 1) {
            Genetic_Algorithm();
            printf("������ɣ���ʼ���ܣ�\n");
            if (!Decode()) {
                printf("������ɣ�\n");
            }
            else {
                printf("����ʧ�ܣ�\n");
            }
        }
        else if (x == 0) {
            break;
        }
        else {
            printf("�������\n");
        }
    }
    return 0;
}