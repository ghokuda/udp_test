#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compareFiles(const char *file1, const char *file2) {
    FILE *f1, *f2;
    char ch1, ch2;

    // ファイルを開く
    f1 = fopen(file1, "r");
    f2 = fopen(file2, "r");

    // ファイルが開けない場合
    if (f1 == NULL || f2 == NULL) {
        perror("ファイルを開けません");
        return -1; // エラーコード
    }

    // ファイルの各文字を比較
    while (1) {

        ch1 = fgetc(f1);
        ch2 = fgetc(f2);

        // EOFチェック
        if (ch1 == EOF || ch2 == EOF) {
            // いずれかのファイルがEOFに達した場合
            break;
        }
        if (ch1 != ch2) {
            printf("文字が不一致 %c %c\n", ch1, ch2);
            fclose(f1);
            fclose(f2);
            return 0; // 一致しない
        }
    }

    // 片方のファイルが終わったが、もう片方がまだ残っている場合
    if ((ch1 != EOF) || (ch2 != EOF)) {
        printf("片方だけ終了 %c %c\n", ch1, ch2);
        fclose(f1);
        fclose(f2);
        return 0; // 一致しない
    }

    fclose(f1);
    fclose(f2);
    return 1; // 一致する
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("使用法: %s <testXX_expected.txt> <test_rx_XX.txt> <test_rx_XX_time.txt> <test_result_YYYYMMDD.txt>\n", argv[0]);
        return 1;
    }
    
    //////////////////////////////
    // 引数からテスト項目の名称（testXX）を取得
    const char *str_search = "_"; // アンダースコアを検索
    char *inputString = argv[1]; //testXX_expected.txtを取得
    char test_name[256]; // 255文字 + 終端文字のための1バイト

    size_t length = strcspn(inputString, str_search);
    int i;
    // テスト項目を変数に格納
    for (i = 0; i < length ; i++) {
        test_name[i] = inputString[i]; // 1文字ずつ格納
    }
    test_name[i] = '\0'; // 終端文字を追加

    //////////////////////////////
    // ファイル（[1]期待値と[2]受信データ）の比較
    int result = compareFiles(argv[1], argv[2]);

    //////////////////////////////
    // 受信日時のチェック TODO:
    // ・時刻の差分が想定通りか？

    //////////////////////////////
    // 結果ファイルに追記する
    char *filename = argv[4]; // 結果を格納するファイル名
    FILE *resultFile = fopen(filename, "a");
    if (resultFile == NULL) {
        perror("結果ファイルを開けませんでした");
        return 1;
    }

    if (result == 1) {
        fprintf(resultFile, "%s:\t OK ファイル一致。\n", test_name);
        printf("%s:\t OK ファイル一致。\n", test_name);
    } else if (result == 0) {
        fprintf(resultFile, "%s:\t NG1 ファイル不一致。\n", test_name);
        printf("%s:\t NG1 ファイル不一致。\n", test_name);
    } else {
        fprintf(resultFile, "%s:\t NG2 エラーが発生しました。\n", test_name);
        printf("%s:\t NG2 エラーが発生しました。\n", test_name);
    }

    return 0;
}
