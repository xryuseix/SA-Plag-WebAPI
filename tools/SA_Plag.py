from typing import List
import tools.code_management as mng


class SA_Plag:
    def __init__(self, code: List[str]):
        # ソースコード1の処理
        self.code1 = mng.Code(
            dic_path="./data/tfidf.dic", std_mean_path="./data/std_mean.csv"
        )
        self.code1.add_code(code[0])

        # ソースコード2の処理
        self.code2 = mng.Code(
            dic_path="./data/tfidf.dic", std_mean_path="./data/std_mean.csv"
        )
        self.code2.add_code(code[1])

        # 二つのソースコードの完全一致判定フラグ
        self.token_same_flag = sorted(self.code1.token_code) == sorted(
            self.code2.token_code
        )

    def get_predict_result(self):
        # Inferenceクラス生成
        Infer = mng.Inference(model_path="./data/model.sav")
        pred, prob = Infer.infer(
            self.code1.code_vec, self.code2.code_vec, self.token_same_flag
        )

        return (pred, prob)
