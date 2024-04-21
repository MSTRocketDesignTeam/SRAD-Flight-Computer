```
python -m venv .venv/ 
.venv/Scripts/activate
python -m pip install -r requirements.txt
pyinstaller -y --clean --onefile --name SRAD.exe main.py
```