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
    # 最大10個まで受け付ける
    if len(code_pair) > 10:
        code_pair = code_pair[:10]
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


templates = Jinja2Templates(directory="template")
jinja_env = templates.env  # Jinja2.Environment


@app.get("/")
def test(request: Request):
    return templates.TemplateResponse("index.html", {"request": request})


@app.get("/api")
def index(request: Request):
    return templates.TemplateResponse("api.html", {"request": request})

@app.get("/api_usage")
def index(request: Request):
    return templates.TemplateResponse("api_usage.html", {"request": request})

# uvicorn main:app --reload --workers 1 --host 0.0.0.0 --port 8080
# http://localhost:8080/plag
# https://sa-plag.herokuapp.com/

