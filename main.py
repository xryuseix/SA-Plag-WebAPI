from typing import List
from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from starlette.templating import Jinja2Templates
from starlette.requests import Request
from pydantic import BaseModel
from pathlib import Path
import tools.SA_Plag as plag
import re

app = FastAPI()

app.mount(
    "/static",
    StaticFiles(directory="./static"),
    name="static",
)


@app.post("/api")
def infer(code_pair: List[str]):
    # 最大30個まで受け付ける
    max_codes = 30
    if len(code_pair) > max_codes:
        code_pair = code_pair[:max_codes]
    # 最大10000文字で受け取る
    code_pair = [code[:10000] for code in code_pair]

    res_json = {"status": "OK", "result": []}

    for i, code1 in enumerate(code_pair):
        for j, code2 in enumerate(code_pair):
            if i >= j:
                continue
            # 1文字の場合はトークン化がうまくできない
            if len(code1) == 1 or len(code2) == 1:
                res_json["result"].append(
                    {"pair": [i, j], "pred": int(1), "prob": [0, 1]}
                )
                continue
            plag_class = plag.SA_Plag([code1, code2])
            pred, prob = plag_class.get_predict_result()
            res_json["result"].append(
                {"pair": [i, j], "pred": int(pred), "prob": list(prob)}
            )
    return res_json

# [[a,b], [c,d], ...]の形で受け取る場合
@app.post("/api/pair")
def infer_pair(code_pair: List[List[str]]):
    # 最大100個まで受け付ける
    max_codes = 100
    if len(code_pair) > max_codes:
        code_pair = code_pair[:max_codes]
    
    for pair in code_pair:
        if len(pair) != 2:
            return {"status" : "NG", "message" : "The size of the inner array must be 2."}
    
    # 最大10000文字で受け取る
    code_pair = [[code[0][:10000], code[1][:10000]] for code in code_pair]

    res_json = {"status": "OK", "result": []}

    for i, (code1, code2) in enumerate(code_pair):
        # 1文字の場合はトークン化がうまくできない
        if len(code1) == 1 or len(code2) == 1:
            res_json["result"].append(
                {"pair": [i, i], "pred": int(1), "prob": [0, 1]}
            )
            continue
        plag_class = plag.SA_Plag([code1, code2])
        pred, prob = plag_class.get_predict_result()
        res_json["result"].append(
            {"pair": [i, i], "pred": int(pred), "prob": list(prob)}
        )
    return res_json


templates = Jinja2Templates(directory="template")
jinja_env = templates.env  # Jinja2.Environment


@app.get("/")
def root(request: Request):
    return templates.TemplateResponse("index.html", {"request": request})


@app.get("/api")
def api(request: Request):
    return templates.TemplateResponse("api.html", {"request": request})

@app.get("/api_usage")
def usage(request: Request):
    return templates.TemplateResponse("api_usage.html", {"request": request})

@app.get("/sample_usage")
def sample(request: Request):
    def readfile(path):
        with open(path) as f:
            return f.read()
    return {"data" : [readfile('./static/cpp/sample1.cpp'), readfile('./static/cpp/sample2.cpp')]}

# uvicorn main:app --reload --workers 1 --host 0.0.0.0 --port 8080
# http://localhost:8080/plag
# https://sa-plag.herokuapp.com/

