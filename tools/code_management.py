import pickle
import numpy as np
import tools.tfidf as tfidf
import tokenizecpp as tcp
import subprocess
import string, random, platform

# ソースコード管理クラス
class Code:
    def __init__(self, dic_path, std_mean_path):
        tfidf.load_dic(dic_path)
        mean_std = np.loadtxt(std_mean_path, delimiter=",")
        (self.len_mean, self.len_std), (self.tok_mean, self.tok_std) = mean_std

    # Codeクラスにソースコードを追加
    def add_code(self, str_code: str, visible: bool = False):
        str_code = self.clang_format(str_code)  # 整形
        token_code, str_len = self.tokenize(str_code)  # トークン化
        self.token_code = self.to_tfidf(token_code)  # TF-IDF
        s_length, s_token = (
            # 文字数の測定
            self.standard_length(str_len),
            self.standard_token(len(token_code)),
        )
        # ベクトル作成
        self.code_vec = self.connect_tfidf_len_token(self.token_code, s_length, s_token)
        if visible:
            print(self.code_vec)

    # ソースコードを整形
    def clang_format(self, code: str):
        root = "./tools"
        # 一時保存するファイル名生成
        filename = "%s/format_tmp/_%s.cpp" % (
            root,
            "".join(random.choices(string.ascii_letters + string.digits, k=20)),
        )
        # ファイルの保存
        with open(filename, mode="w") as f:
            f.write(code)
        # clang-formatの実行
        subprocess.run(
            ["%s/bin/clang-format-%s" % (root, self.os_env()), "-style=file", "-i", filename],
            encoding="utf-8",
            stderr=subprocess.PIPE,
        )
        # ファイル読み込み
        with open(filename, mode="r") as f:
            code = f.read()
        subprocess.run(["rm", filename])
        return code
    
    # 現在使用中のOSを判定
    def os_env(self):
        pf = platform.system()
        if pf == "Darwin":
            return "mac"
        elif pf == "Linux":
            return "linux"

    # 文字列のソースコードを受け取って，コメントを除き，トークン化した配列と文字列の長さを返す
    def tokenize(self, code: str):
        rm_comment = tcp.remove_comment(code)
        return (tcp.to_tokenize(rm_comment).split(" "), len(rm_comment))

    # トークン化されたソースコード配列をtf-idfエンコーディングする
    def to_tfidf(self, token_code):
        return tfidf.calc_text(token_code)

    # ソースコード長を標準化する
    def standard_length(self, num):
        return (num - self.len_mean) / self.len_std

    # トークン長を標準化する
    def standard_token(self, num):
        return (num - self.tok_mean) / self.tok_std

    # ソースコード，ソースコード長，トークン長を結合する
    def connect_tfidf_len_token(self, tfidf_code, s_length, s_token):
        return np.concatenate([tfidf_code, np.array([s_length, s_token]),]).round(7)


# 類似度推論クラス
class Inference:
    def __init__(self, model_path):
        self.model = pickle.load(open(model_path, "rb"))

    # プログラムのベクトルの類似性を算出する
    def infer(self, code_vec1, code_vec2, token_same_flag):
        self.vec = np.concatenate([code_vec1, code_vec2]).reshape(1, -1)
        if token_same_flag:
            return (1, [0.0, 1.0])
        else:
            return (
                self.model.predict(self.vec)[0],
                self.model.predict_proba(self.vec)[0],
            )
