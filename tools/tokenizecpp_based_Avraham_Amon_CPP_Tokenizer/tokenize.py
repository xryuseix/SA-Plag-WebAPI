import subprocess
import platform
import string, random, os
import tokenizecpp_based_Avraham_Amon_CPP_Tokenizer.compatibility as com

# Avraham-Amon-CPP-Tokenizerを使用する
root = os.getcwd() + "/tokenizecpp_based_Avraham_Amon_CPP_Tokenizer/"


def avraham_amon_wrapper(filename: str):

    # format 
    cp = subprocess.run([root + "clang-format", "-style=file", "-i", filename], encoding="utf-8", stderr=subprocess.PIPE)
    # tokenize
    pf = platform.system()
    if pf == "Darwin":
        exec_filename = root + "tokenize.out"
    elif pf == "Linux":
        exec_filename = root + "tokenize.elf"
    cp = subprocess.run(
        [exec_filename, filename], encoding="utf-8", stdout=subprocess.PIPE
    )

    token_row = cp.stdout.split("\n")[:-1]

    def label_value_split(row: str):
        row_arr = row[7:].split(" ")
        return (" ".join(row_arr[:-1]), row_arr[-1])

    token = [label_value_split(row) for row in token_row]
    return token[:-1]


# ラベルの変換
def label_compression(aa_token):
    token = []
    for label, value in aa_token:
        label = label.split(" ")[0]
        if label in com.compatibility_table:
            token.append(com.compatibility_table[label])
    return token


# トークン化
def tokenize(code: str):
    filename = (
        root
        + "_"
        + "".join(random.choices(string.ascii_letters + string.digits, k=20))
        + ".cpp"
    )
    with open(filename, mode="w") as f:
        f.write(code)
    res = avraham_amon_wrapper(filename)
    token = label_compression(res)
    subprocess.run(["rm", filename])
    return token

