#include "GA.h"

int main() {
    printf("利用频率分析与遗传算法破解简单替换密码\n\n");
    printf("使用说明：\n请将待解密的txt格式的文本命名为code.txt，将txt格式的样本文本数据命名为data.txt，并且都存放在exe文件同一目录下，解密后文本将被命名为decode.txt保存在同一目录下\n");
    printf("请注意：\n请确保样本文本足够大（至少3000个单词），样本文本数量越大结果可能越准确\n");
    while (1) {
        printf("请按下1开始解析密码，按下0退出程序\n");
        int x;
        scanf("%d", &x);
        if (x == 1) {
            Genetic_Algorithm();
            printf("解析完成，开始解密！\n");
            if (!Decode()) {
                printf("解密完成！\n");
            }
            else {
                printf("解密失败！\n");
            }
        }
        else if (x == 0) {
            break;
        }
        else {
            printf("输入错误！\n");
        }
    }
    return 0;
}