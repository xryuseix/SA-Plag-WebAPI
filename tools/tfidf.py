# TF-IDFでテキストをベクトル化するモジュール
import pickle
import numpy as np

# グローバル変数 --- (*2)
word_dic = {'_id': 14, 'VAR': 0, 'FUNC': 1, 'SENTENCE': 2, 'TYPE': 3, 'OP': 4, 'NUM': 5, 'NAMESPACE': 6, 'STR': 7, 'TEMPLATE': 8, 'VALUE': 9, 'FORMULA': 10, 'CLASS': 11, 'ASSEMBLER': 12, 'CONTEXTUAL': 13}  # 単語辞書
dt_dic = {}  # 文書全体での単語の出現回数
files = []  # 全文書をIDで保存


def words_to_ids(words, auto_add=True):
    """単語一覧をIDの一覧に変換する"""  # --- (*4)
    result = []
    for w in words:
        if w in word_dic:
            result.append(word_dic[w])
            continue
    return result


def add_text(text):
    """テキストをIDリストに変換して追加"""  # --- (*5)
    ids = words_to_ids(text)
    files.append(ids)


def add_file(path):
    """テキストファイルを学習用に追加する"""  # --- (*6)
    with open(path, "r", encoding="utf-8") as f:
        s = f.read()
        add_text(s)


def calc_files():
    """追加したファイルを計算"""  # --- (*7)
    global dt_dic
    result = []
    doc_count = len(files)
    dt_dic = {}
    # 単語の出現頻度を数える --- (*8)
    for words in files:
        used_word = {}
        data = np.zeros(word_dic["_id"])
        for id in words:
            data[id] += 1
            used_word[id] = 1
        # 単語tが使われていればdt_dicを加算 --- (*9)
        for id in used_word:
            if not (id in dt_dic):
                dt_dic[id] = 0
            dt_dic[id] += 1
        # 出現回数を割合に直す --- (*10)
        data = data / len(words)
        result.append(data)
    # TF-IDFを計算 --- (*11)
    for i, doc in enumerate(result):
        for id, v in enumerate(doc):
            idf = np.log(doc_count / dt_dic[id]) + 1
            doc[id] = min([doc[id] * idf, 1.0])
        result[i] = doc
    return result


def save_dic(fname):
    """辞書をファイルへ保存する"""  # --- (*12)
    pickle.dump([word_dic, dt_dic, files], open(fname, "wb"))


def load_dic(fname):
    """辞書をファイルから読み込む"""  # --- (*13)
    global word_dic, dt_dic, files
    n = pickle.load(open(fname, "rb"))
    word_dic, dt_dic, files = n


def calc_text(text):
    """ 辞書を更新せずにベクトル変換する """  # --- (*14)
    data = np.zeros(word_dic["_id"])
    words = words_to_ids(text, False)
    for w in words:
        data[w] += 1
    data = data / len(words)
    for id, v in enumerate(data):
        if not dt_dic[id]:
            idf = 1
        else:
            idf = np.log(len(files) / dt_dic[id]) + 1
        data[id] = min([data[id] * idf, 1.0])
    return data


# モジュールのテスト --- (*15)
if __name__ == "__main__":
    add_text(["VAR", "OP", "NUM", "NUM", "SENTENCE"])
    add_text(["VAR", "OP", "NAMESPACE", "NAMESPACE", "VAR"])
    add_text(["OP", "NUM", "OP", "FUNC", "VAR"])
    print(calc_files())
    print(word_dic)

